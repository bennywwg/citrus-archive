#include "citrus/engine/engine.h"
#include "citrus/engine/renderManager.h"
#include "citrus/engine/entityRef.inl"
#include "citrus/engine/elementRef.inl"
#include "citrus/engine/manager.inl"

#include <fstream>
#include "citrus/graphics/mesh.h"
#include "citrus/graphics/finalPassShader.h"
#include "citrus/graphics/vkShader.h"
//#include <openvr/openvr.h"

namespace citrus {
	namespace engine {
		void renderManager::addItem(eleRef<meshFilter> me, int m, int t, int s) {
			std::lock_guard<std::mutex> lock(_drawableMut);
			if (s >= _shaderTypes.size() || (_shaderTypes[s]._sh == nullptr)) throw std::runtime_error("shader index too large or unloaded");
			if (m >= _meshTypes.size() || (_meshTypes[m]._me == nullptr)) throw std::runtime_error("shader index too large or unloaded");
			auto& eles = _shaderTypes[s]._sets[m]._eles;
			for (int i = 0; i < eles.size(); i++) {
				if (eles[i].null()) {
					eles[i] = me;
					return;
				}
			}
			eles.push_back(me);
		}
		void renderManager::removeItem(eleRef<meshFilter> me, int oldM, int oldT, int oldS) {
			std::lock_guard<std::mutex> lock(_drawableMut);
			auto & eles = _shaderTypes[oldS]._sets[oldM]._eles;
			for (int i = 0; i < eles.size(); i++) {
				if (eles[i].null()) {
					eles[i] = nullptr;
					return;
				}
			}
		}

		void renderManager::loadShader(string vertLoc, string fragLoc, int index) {
			if (_shaderTypes.size() <= index) _shaderTypes.resize(index + 1);
			if (_shaderTypes[index]._sh) throw std::runtime_error("shader index already in use");

			_shaderTypes[index]._sh = new graphics::dynamicOffsetMeshShader(
				*inst(),
				graphics::meshDescription::getLit(true),
				{ _frames[0].view, _frames[1].view },
				win()->framebufferSize().x,
				win()->framebufferSize().y,
				false,
				100, sizeof(glm::mat4),
				vertLoc, "", fragLoc);
			for (int i = 0; i < _meshTypes.size(); i++) {
				_shaderTypes[index]._sets.emplace_back();
				_shaderTypes[index]._sets.back().modelIndex = i;
			}
		}

		void renderManager::loadAnimation(string loc, int index) {
			if (_animations.size() <= index) _animations.resize(index + 1, nullptr);
			if (_animations[index]) throw std::runtime_error("mesh index already in use");

			std::ifstream fs(loc, std::ios::binary);
			if (!fs.is_open()) throw std::runtime_error("can't open animation file");
			_animations[index] = new graphics::animation();
			_animations[index]->read(fs);
		}

		void renderManager::loadMesh(string loc, int index) {
			if (_meshTypes.size() <= index) {
				_meshTypes.resize(index + 1);
				for (int i = 0; i < _shaderTypes.size(); i++) {
					int oldSize = _shaderTypes[i]._sets.size();
					_shaderTypes[i]._sets.resize(index + 1);
					for (int j = oldSize; j < _shaderTypes[i]._sets.size(); j++) {
						_shaderTypes[i]._sets[j].modelIndex = j;
					}
				}
			}
			if (_meshTypes[index]._model) throw std::runtime_error("mesh index already in use");
			
			_meshTypes[index]._me = new graphics::mesh(loc);

			_meshTypes[index]._model = new graphics::model(*eng()->getWindow()->inst(), *_meshTypes[index]._me);
		}

		void renderManager::bindAllAvailableAnimations() {
			int totalBound = 0;
			for (int i = 0; i < _meshTypes.size(); i++) {
				if (_meshTypes[i]._me) {
					for (int j = 0; j < _animations.size(); j++) {
						if (_meshTypes[i]._me->bindAnimation(*_animations[j])) totalBound++;
					}
				}
			}
			util::sout("matched " + std::to_string(totalBound) + " animations\n");
		}
		

		void renderManager::load(const nlohmann::json& parsed) {
			if(!parsed.empty()) {
				camRef = eng()->man->dereferenceElement<freeCam>(parsed["cam"]);
			}

			ivec2 dispSize = eng()->getWindow()->framebufferSize();
			_frames = {
				eng()->getWindow()->inst()->createTexture4b(dispSize.x, dispSize.y, true, nullptr),
				eng()->getWindow()->inst()->createTexture4b(dispSize.x, dispSize.y, true, nullptr)
			};
			_fences = {
				inst()->createFence(true),
				inst()->createFence(true)
			};
			_currentFrame = 0;
			_renderDoneSem = VK_NULL_HANDLE;
			const uint64_t maxDynamicObjects = 10000;
			const uint64_t dynamicObjectDataSize = glm::max(inst()->minUniformBufferOffsetAlignment(), sizeof(mat4));
			const uint64_t dynamicSize = maxDynamicObjects * dynamicObjectDataSize;
		}
		nlohmann::json renderManager::save() const {
			return nlohmann::json();
		}

		void renderManager::onCreate() {
			//vr::EVRInitError er;
			//vr::EVRApplicationType t = vr::EVRApplicationType::VRApplication_Scene;
			//vr::VR_Init(&er, t);
			//int x = 5;
		}
		void renderManager::render() {
			if (_renderDoneSem != VK_NULL_HANDLE) inst()->destroySemaphore(_renderDoneSem);
			inst()->waitForFence(_fences[_currentFrame]);
			inst()->resetFence(_fences[_currentFrame]);

			VkCommandBuffer buf = inst()->createCommandBuffer();

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			if (vkBeginCommandBuffer(buf, &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			for (int i = 0; i < _shaderTypes.size(); i++) {
				if (_shaderTypes[i]._sh) {
					
					//fill dynamic buffer with data
					graphics::dynamicOffsetMeshShader& sh = *_shaderTypes[i]._sh;
					uint64_t dynamicOffset = 0;
					uint64_t dynamicIncrement = sh.realItemSize;
					vector<modelSet>& sets = _shaderTypes[i]._sets;
					for (int j = 0; j < sets.size(); j++) {
						vector<eleRef<meshFilter>> eles = sets[j]._eles;
						for (int k = 0; k < eles.size(); k++) {
							if (eles[k].null()) continue;
							glm::mat4 mvp = eles[k] ->ent().getGlobalTransform().getMat();
							memcpy(((uint8_t*)sh.targets[_currentFrame].buf.mapped) + dynamicOffset, &mvp, sizeof(glm::mat4));
							dynamicOffset += dynamicIncrement;
						}
					}
					
					//upload and reset data
					inst()->flushDynamicOffsetBufferRange(sh.targets[_currentFrame].buf, 0, dynamicOffset);
					dynamicOffset = 0;

					sh.beginShader(_currentFrame, buf);

					for (int j = 0; j < sets.size(); j++) {

						_meshTypes[sets[j].modelIndex]._model->bindVertexAndIndexBuffers(buf);



						vector<eleRef<meshFilter>> eles = sets[j]._eles;
						for (int k = 0; k < eles.size(); k++) {
							if (eles[k].null()) continue;

							uint32_t dynamicOffsetu32 = dynamicOffset;
							vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, sh.pipelineLayout, 0, 1, &sh.targets[_currentFrame].set, 1, &dynamicOffsetu32);
							_meshTypes[eles[k]->model()]._model->cmdDraw(buf);
							dynamicOffset += dynamicIncrement;
						}
					}

					sh.endShader(_currentFrame, buf);
				}
			}

			if (vkEndCommandBuffer(buf) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}

			_renderDoneSem = inst()->createSemaphore();

			VkSubmitInfo info = { };
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			info.commandBufferCount = 1;
			info.pCommandBuffers = &buf;
			info.signalSemaphoreCount = 1;
			info.pSignalSemaphores = &_renderDoneSem;
			vkQueueSubmit(inst()->_graphicsQueue, 1, &info, _fences[_currentFrame]);

			inst()->_finalPass->fillCommandBuffer(eng()->currentFrameIndex(), _frames[_currentFrame]);
			inst()->_finalPass->submit(eng()->currentFrameIndex(), { eng()->getImageReadySemaphore(), _renderDoneSem }, eng()->getPresentSemaphore());

			//vr::Texture_t t;
			//t.eColorSpace = vr::ColorSpace_Gamma;
			//t.eType = vr::TextureType_OpenGL;
			//t.handle = (void*)standardFBO->getColors()[0].tex->ptr();
			//vr::VRCompositor()->Submit(vr::EVREye::Eye_Left, &t);
			//vr::VRCompositor()->Submit(vr::EVREye::Eye_Right, &t);
			//glFlush();
		}
		void renderManager::onDestroy() {
			//vr::VR_Shutdown();
			
		}

		renderManager::renderManager(entityRef ent) : element(ent, std::type_index(typeid(renderManager))) {
		}
		renderManager::~renderManager() {
			for (int i = 0; i < _frames.size(); i++) {
				inst()->destroyTexture(_frames[i]);
			}
			if(_renderDoneSem != VK_NULL_HANDLE) inst()->destroySemaphore(_renderDoneSem);
			for (int i = 0; i < _fences.size(); i++) {
				inst()->destroyFence(_fences[i]);
			}
			for (int i = 0; i < _shaderTypes.size(); i++) {
				delete _shaderTypes[i]._sh;
			}
		}
	}
}
