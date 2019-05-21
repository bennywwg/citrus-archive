#include "citrus/engine/engine.h"
#include "citrus/engine/renderManager.h"
#include "citrus/engine/entityRef.inl"
#include "citrus/engine/elementRef.inl"
#include "citrus/engine/manager.inl"

#include <fstream>
#include "citrus/graphics/mesh.h"
#include "citrus/graphics/finalPassShader.h"
//#include <openvr/openvr.h"

namespace citrus::engine {
	void renderManager::addStatic(eleRef<meshFilter> const& me, int m, int t) {
		std::lock_guard<std::mutex> lock(_drawableMut);
		if (m >= sys->aniModels.size()) throw std::runtime_error("model index too large");
		if (t >= sys->textures.size()) throw std::runtime_error("shader index too large");
		for (int i = 0; i < sys->staticItems[m].size(); i++) {
			if (!sys->staticItems[m][i].enabled) {
				me->systemIndex = i;
				sys->staticItems[m][i].pos = me->ent().getGlobalTransform().getPosition();
				sys->staticItems[m][i].ori = me->ent().getGlobalTransform().getOrientation();
				sys->staticItems[m][i].texIndex = t;
				sys->staticItems[m][i].enabled = true;
				return;
			}
		}
		graphics::system::itemInfo info;
		info.pos = me->ent().getGlobalTransform().getPosition();
		info.ori = me->ent().getGlobalTransform().getOrientation();
		info.texIndex = t;
		info.enabled = true;
		me->systemIndex = sys->staticItems[m].size();
		sys->staticItems[m].push_back(info);
	}
	void renderManager::removeStatic(eleRef<meshFilter> const& me, int oldM) {
		std::lock_guard<std::mutex> lock(_drawableMut);
		sys->staticItems[oldM][me->systemIndex].enabled = false;
		me->systemIndex = -1;
	}
	void renderManager::addDynamic(eleRef<meshFilter> const& me, int m, int t, int a) {
		std::lock_guard<std::mutex> lock(_drawableMut);
		if (m >= sys->aniModels.size()) throw std::runtime_error("model index too large");
		if (t >= sys->textures.size()) throw std::runtime_error("shader index too large");
		for (int i = 0; i < sys->aniItems[m].size(); i++) {
			if (!sys->aniItems[m][i].enabled) {
				me->systemIndex = i;
				sys->aniItems[m][i].pos = me->ent().getGlobalTransform().getPosition();
				sys->aniItems[m][i].ori = me->ent().getGlobalTransform().getOrientation();
				sys->aniItems[m][i].texIndex = t;
				sys->aniItems[m][i].enabled = true;
				return;
			}
		}
		graphics::system::itemInfo info;
		info.ori = me->ent().getGlobalTransform().getOrientation();
		info.pos = me->ent().getGlobalTransform().getPosition();
		info.texIndex = t;
		info.enabled = true;
		me->systemIndex = sys->aniItems[m].size();
		sys->aniItems[m].push_back(info);
	}
	void renderManager::removeDynamic(eleRef<meshFilter> const& me, int oldM) {
		std::lock_guard<std::mutex> lock(_drawableMut);
		sys->aniItems[oldM][me->systemIndex].enabled = false;
		me->systemIndex = -1;
	}

	void renderManager::initSystem(string vs, string fs, vector<string> textures, vector<string> staticModels, vector<string> aniModels, vector<string> animations) {
		sys = new graphics::system(*inst(), vs, fs, textures, staticModels, aniModels, animations);
	}

	void renderManager::bindAllAvailableAnimations() {
		/*int totalBound = 0;
		for (int i = 0; i < _meshes.size(); i++) {
			if (_meshes[i]._me) {
				for (int j = 0; j < _animations.size(); j++) {
					if (_meshes[i]._me->bindAnimation(*_animations[j])) totalBound++;
				}
			}
		}
		util::sout("matched " + std::to_string(totalBound) + " animations\n");*/
	}

	void renderManager::load(const nlohmann::json& parsed) {
		if(!parsed.empty()) {
			camRef = eng()->man->dereferenceElement<freeCam>(parsed["cam"]);
		}

		_currentFrame = 0;
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
		std::chrono::time_point ts = std::chrono::high_resolution_clock::now();
		if (win()->getKey(graphics::windowInput::escape)) eng()->stop();

		sys->render(_currentFrame, mainSem, camRef->cam);
		sys->postProcess(
			_currentFrame,
			eng()->currentFrameIndex(),
			{ mainSem, eng()->getImageReadySemaphore() },
			eng()->getPresentSemaphore()
		);

		_currentFrame = (_currentFrame + 1) % 2;

		std::chrono::time_point te = std::chrono::high_resolution_clock::now();

		util::sout("Frame Took: " + std::to_string((te - ts).count()) + " ns or " + std::to_string((te - ts).count() / 1000) + "ms\n");

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
		mainSem = inst()->createSemaphore();
	}
	renderManager::~renderManager() {

	}
}