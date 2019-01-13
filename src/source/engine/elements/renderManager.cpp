#include <engine/engine.h>
#include <engine/elements/renderManager.h>
#include <engine/elementRef.inl>
#include <engine/manager.inl>

#include <engine/elements/meshManager.h>
#include <editor/editor.h>

//#include <openvr/openvr.h>

namespace citrus {
	namespace engine {
		void renderManager::addDrawable(eleRef<meshFilter> me, int m, int t, int s) {
			std::lock_guard<std::mutex> lock(_drawableMut);
			auto& eles = drawable[s].groups[t].eles;
			for(int i = 0; i < eles.size(); i++) {
				if(eles[i].null()) {
					eles[i] = me;
					return;
				}
			}
			eles.push_back(me);
		}
		void renderManager::removeDrawable(eleRef<meshFilter> me, int oldM, int oldT, int oldS) {
			std::lock_guard<std::mutex> lock(_drawableMut);
			auto& eles = drawable[oldS].groups[oldT].eles;
			for(int i = 0; i < eles.size(); i++) {
				if(eles[i] == me) {
					eles[i] = nullptr;
				}
			}
		}
		graphics::texture3b & renderManager::getTexture(int index) {
			if(index < 0) throw std::runtime_error("Texture access index negative");
			if(index >= textures.size() || !textures[index]) throw std::runtime_error("Texture at index does not exist");
			return *textures[index];
		}
		void renderManager::loadPNG(std::string loc, int index) {
			if(index < 0) throw std::runtime_error("Texture creation index negative");
			if(index >= textures.size()) textures.resize(index + 1, nullptr);
			for(int i = 0; i < drawable.size(); i++) {
				if(index >= drawable[i].groups.size()) drawable[i].groups.resize(index + 1, shaderInfo::textureInfo());
			}
			if(textures[index]) throw std::runtime_error("Texture already exists at index " + std::to_string(index));
			try {
				textures[index] = new graphics::texture3b(graphics::image3b(loc));
				eng()->Log("Loaded texture " + std::to_string(index) + ": \"" + loc + "\"");
			} catch(const std::runtime_error& er) {
				eng()->Log("Failed to load texture " + std::to_string(index) + " \"" + loc + "\": " + er.what());
			} catch(...) {
				eng()->Log("Failed to load texture " + std::to_string(index) + " \"" + loc + "\": (Whack error)");
			}
		}
		void renderManager::resizeBuffers(unsigned int width, unsigned int height) {
			if(width == 0 || height == 0) return;

			meshFBO.reset();
			meshFBO = std::make_unique<graphics::simpleFrameBuffer>(width, height);

			textFBO.reset();
			textFBO = std::make_unique<graphics::simpleFrameBuffer>(width, height, true);

			camRef->cam.aspectRatio = width / (float) height;
		}

		void renderManager::load(const nlohmann::json& parsed) {
			if(!parsed.empty()) {
				camRef = eng()->man->dereferenceElement<freeCam>(parsed["cam"]);
			}
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
			auto win = eng()->getWindow();
			auto size = win->framebufferSize();
			if(meshFBO->width() != size.x || meshFBO->height() != size.y) {
				resizeBuffers(size.x, size.y);
			}

			camera cam = camRef->cam;
			eleRef<meshManager> models = eng()->getAllOfType<meshManager>()[0];

			meshFBO->bind();
			//meshFBO->clearAll();

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);

			{
				shaderInfo& info = drawable[0];

				info.sh->use();
				info.sh->setUniform("projectionMat", cam.getProjectionMatrix());
				info.sh->setUniform("viewMat", cam.getViewMatrix());

				for(int i = 0; i < info.groups.size(); i++) {
					if(info.groups[i].eles.empty()) continue;
					textures[i]->bind(GL_TEXTURE0);
					info.sh->setUniform("tex", 0);

					for(int j = 0; j < info.groups[i].eles.size(); j++) {
						auto& ref = info.groups[i].eles[j];
						if(ref.null() || !ref->visible) continue;

						glm::mat4 modelMat = ref->ent().getGlobalTransform().getMat();

						info.sh->setUniform("modelMat", modelMat);
						info.sh->setUniform("modelViewProjectionMat", cam.getViewProjectionMatrix() * modelMat);

						int model = ref->model();


						models->getModel(model).drawAll();
					}
				}
			}

			{
				shaderInfo& info = drawable[1];

				info.sh->use();
				info.sh->setUniform("projectionMat", cam.getProjectionMatrix());
				info.sh->setUniform("viewMat", cam.getViewMatrix());
				info.sh->setUniform("viewBone", 0);

				for(int i = 0; i < info.groups.size(); i++) {
					if(info.groups[i].eles.empty()) continue;
					textures[i]->bind(GL_TEXTURE0);
					info.sh->setUniform("tex", 0);

					for(int j = 0; j < info.groups[i].eles.size(); j++) {
						auto& ref = info.groups[i].eles[j];
						if(ref.null() || !ref->visible) continue;

						glm::mat4 modelMat = ref->ent().getGlobalTransform().getMat();

						info.sh->setUniform("modelMat", modelMat);
						info.sh->setUniform("modelViewProjectionMat", cam.getViewProjectionMatrix() * modelMat);

						int model = ref->model();
						std::vector<glm::mat4> trData(64, glm::translate(glm::vec3(0.f, 0.f, 0.f)));
						int ani = ref->ani();
						if(ani != -1) {
							double actualTime = ref->aniTime();
							models->getMesh(model).calculateAnimationTransforms(ani, trData, actualTime, geom::repeat);
						}
						info.sh->setUniform("boneData", trData.data(), 64);


						models->getModel(model).drawAll();
					}
				}
			}

			{
				shaderInfo& info = drawable[2];

				info.sh->use();
				info.sh->setUniform("projectionMat", cam.getProjectionMatrix());
				info.sh->setUniform("viewMat", cam.getViewMatrix());

				for(int i = 0; i < info.groups.size(); i++) {
					if(info.groups[i].eles.empty()) continue;
					for(int j = 0; j < info.groups[i].eles.size(); j++) {
						auto& ref = info.groups[i].eles[j];
						if(ref.null() || !ref->visible) continue;

						glm::mat4 modelMat = ref->ent().getGlobalTransform().getMat();

						info.sh->setUniform("drawColor", ref->color);
						info.sh->setUniform("modelMat", modelMat);
						info.sh->setUniform("modelViewProjectionMat", cam.getViewProjectionMatrix() * modelMat);

						int model = ref->model();
						models->getModel(model).drawAll();
					}
				}
			}

			/*glm::mat4 projectionViewMat = camRef->cam.getViewProjectionMatrix();
			transSh->use();
			for(int i = 0; i < drawable.size(); i++) {
				shaderInfo& info = drawable[i];

				for(int j = 0; j < info.eles.size(); j++) {
					auto& ref = info.eles[j];

					auto& me = models->getMesh(ref->model());
						glm::mat4 modelMat = ref->ent.getGlobalTransform().getMat();
					for(int k = 0; k < me.boneBase.allBones().size(); k++) {
						transSh->setUniform("modelViewProjectionMat", projectionViewMat * me.boneBase.allBones()[k].accumulatedTransform * modelMat * glm::scale(glm::vec3(0.01f, 0.01f, 0.01f)));
						graphics::vertexArray::drawOne();
					}
				}
			}*/


			/*for(auto& ent : e->man->allEntities()) {
				transSh->setUniform("modelViewProjectionMat", projectionViewMat * ent.getGlobalTransform().getMat());
				graphics::vertexArray::drawOne();
			}*/
			meshFBO->unbind();

			textFBO->clearColor(0, 0, 0, 0);
			if(eng()->ed) {
				eng()->ed->update(*this, cam);
				eng()->ed->render(*this, cam);
			}
			//textFBO->clearAll(1.0f, 1.0f, 1.0f, 1.0f);
			/*textFBO->bind();
			font.streamText("Render Manager", camRef->cam.getViewProjectionMatrix() * glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)));
			textFBO->unbind();*/

			graphics::frameBuffer screen(win);
			screen.bind();
			screen.clearAll();
			composite->use();
			composite->setSampler("bottomColor", *(meshFBO->getColors()[0].tex));
			composite->setSampler("bottomDepth", *(meshFBO->getDepth()));
			composite->setSampler("topColor", *(textFBO->getColors()[0].tex));
			composite->setSampler("topDepth", *(textFBO->getDepth()));
			graphics::vertexArray::drawOne();
			composite->unuse();

			meshFBO->clearAll();
			//textFBO->clearAll();

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

		renderManager::renderManager(entityRef ent) :
			font("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\textures\\consolas256x256.png", 16, 16),
			element(ent, std::type_index(typeid(renderManager))) {

			auto win = eng()->getWindow();
			auto size = win->framebufferSize();
			meshFBO = std::make_unique<graphics::simpleFrameBuffer>(size.x, size.y);
			textFBO = std::make_unique<graphics::simpleFrameBuffer>(size.x, size.y, true);

			{
				std::string simpleVert = util::loadEntireFile("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\shaders\\simple.vert");
				std::string simpleFrag = util::loadEntireFile("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\shaders\\simple.frag");
				shaderInfo inf;
				inf.sh = std::make_unique<graphics::shader>(simpleVert, simpleFrag);
				drawable.push_back(std::move(inf));
			}
			{
				std::string bonesVert = util::loadEntireFile("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\shaders\\bones.vert");
				std::string bonesFrag = util::loadEntireFile("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\shaders\\bones.frag");
				shaderInfo inf;
				inf.sh = std::make_unique<graphics::shader>(bonesVert, bonesFrag);
				drawable.push_back(std::move(inf));
			}
			{
				std::string colorVert = util::loadEntireFile("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\shaders\\color.vert");
				std::string colorFrag = util::loadEntireFile("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\shaders\\color.frag");
				shaderInfo inf;
				inf.sh = std::make_unique<graphics::shader>(colorVert, colorFrag);
				drawable.push_back(std::move(inf));
			}
			
			rectshader = std::make_unique<graphics::shader>(
				util::loadEntireFile("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\shaders\\rect.vert"),
				util::loadEntireFile("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\shaders\\rect.geom"),
				util::loadEntireFile("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\shaders\\rect.frag"));

			composite = std::make_unique<graphics::shader>(
				"#version 450\n"
				""
				"void main() {\n"
				"  gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
				"}\n",
				"#version 450\n"
				""
				"layout(points) in;\n"
				"layout(triangle_strip, max_vertices = 6) out;\n"
				""
				"out vec2 fUV;\n"
				""
				"void main() {\n"
				"  gl_Position = vec4(1.0, 1.0, 0.0, 1.0);\n"
				"  fUV = vec2(1.0, 1.0);\n"
				"  EmitVertex();\n"
				"  gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);\n"
				"  fUV = vec2(0.0, 1.0);\n"
				"  EmitVertex();\n"
				"  gl_Position = vec4(1.0, -1.0, 0.0, 1.0);\n"
				"  fUV = vec2(1.0, 0.0);\n"
				"  EmitVertex();\n"
				"  EndPrimitive();\n"
				""
				"  gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);\n"
				"  fUV = vec2(0.0, 1.0);\n"
				"  EmitVertex();\n"
				"  gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);\n"
				"  fUV = vec2(0.0, 0.0);\n"
				"  EmitVertex();\n"
				"  gl_Position = vec4(1.0, -1.0, 0.0, 1.0);\n"
				"  fUV = vec2(1.0, 0.0);\n"
				"  EmitVertex();\n"
				"  EndPrimitive();\n"
				"}\n",
				"#version 450\n"
				"in vec2 fUV;\n"
				"uniform sampler2D bottomColor;\n"
				"uniform sampler2D bottomDepth;\n"
				"uniform sampler2D topColor;\n"
				"uniform sampler2D topDepth;\n"
				"layout(location = 0) out vec4 color;\n"
				""
				"int sampleNear(vec2 uv, sampler2D sampler) {\n"
				"  float pixelWidth = 1.0 / float(textureSize(sampler, 0).x);\n"
				"  float pixelHeight = 1.0 / float(textureSize(sampler, 0).y);\n"
				"  int count = 0;\n"
				"  for(int i = -3; i <= 3; i++) {\n"
				"    for(int u = -3; u <= 3; u++) {\n"
				"      if(i*i + u*u > 9) continue;\n"
				"      if(texture(sampler, uv + vec2(pixelWidth * i, pixelHeight * u)).x > 0.5) {\n"
				"        count++;\n"
				"      }\n"
				"    }\n"
				"  }\n"
				"  return count;\n"
				"}\n"
				"void main() {\n"
				"  color = texture(bottomColor, fUV);\n"
				"  vec4 guiLayer = texture(topColor, fUV);\n"
				"  color = vec4(mix(color.rgb, guiLayer.rgb, guiLayer.a), 1.0);\n"
				"}\n"
			);
			passthrough = std::make_unique<graphics::shader>(
				"#version 450\n"
				""
				"void main() {\n"
				"  gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
				"}\n",
				"#version 450\n"
				""
				"layout(points) in;\n"
				"layout(triangle_strip, max_vertices = 6) out;\n"
				""
				"out vec2 fUV;\n"
				""
				"void main() {\n"
				"  gl_Position = vec4(1.0, 1.0, 0.0, 1.0);\n"
				"  fUV = vec2(1.0, 1.0);\n"
				"  EmitVertex();\n"
				"  gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);\n"
				"  fUV = vec2(0.0, 1.0);\n"
				"  EmitVertex();\n"
				"  gl_Position = vec4(1.0, -1.0, 0.0, 1.0);\n"
				"  fUV = vec2(1.0, 0.0);\n"
				"  EmitVertex();\n"
				"  EndPrimitive();\n"
				""
				"  gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);\n"
				"  fUV = vec2(0.0, 1.0);\n"
				"  EmitVertex();\n"
				"  gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);\n"
				"  fUV = vec2(0.0, 0.0);\n"
				"  EmitVertex();\n"
				"  gl_Position = vec4(1.0, -1.0, 0.0, 1.0);\n"
				"  fUV = vec2(1.0, 0.0);\n"
				"  EmitVertex();\n"
				"  EndPrimitive();\n"
				"}\n",
				"#version 450\n"
				"in vec2 fUV;\n"
				"uniform sampler2D tex;\n"
				"layout(location = 0) out vec4 color;\n"
				"void main() {\n"
				"  color = texture(tex, fUV);\n"
				"}\n"
				);
			transSh = std::make_unique<graphics::shader>(
				"#version 450\n"
				""
				"void main() {\n"
				"  gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
				"}\n",

				"#version 450\n"
				""
				"layout(points) in;\n"
				"layout(line_strip, max_vertices = 6) out;\n"
				""
				"uniform mat4 modelViewProjectionMat;\n"
				""
				"out vec3 inter_color;\n"
				""
				"void main() {\n"
				"  vec4 basePos = modelViewProjectionMat * vec4(0.0, 0.0, 0.0, 1.0);\n"
				""
				"  gl_Position = basePos;\n"
				"  inter_color = vec3(1.0, 1.0, 1.0);\n"
				"  EmitVertex();\n"
				"  gl_Position = modelViewProjectionMat * vec4(1.0, 0.0, 0.0, 1.0);\n"
				"  inter_color = vec3(1.0, 0.0, 0.0);\n"
				"  EmitVertex();\n"
				"  EndPrimitive();\n"
				""
				"  gl_Position = basePos;\n"
				"  inter_color = vec3(1.0, 1.0, 1.0);\n"
				"  EmitVertex();\n"
				"  gl_Position = modelViewProjectionMat * vec4(0.0, 1.0, 0.0, 1.0);\n"
				"  inter_color = vec3(0.0, 1.0, 0.0);\n"
				"  EmitVertex();\n"
				"  EndPrimitive();\n"
				""
				"  gl_Position = basePos;\n"
				"  inter_color = vec3(1.0, 1.0, 1.0);\n"
				"  EmitVertex();\n"
				"  gl_Position = modelViewProjectionMat * vec4(0.0, 0.0, 1.0, 1.0);\n"
				"  inter_color = vec3(0.0, 0.0, 1.0);\n"
				"  EmitVertex();\n"
				"  EndPrimitive();\n"
				"}\n",
				"#version 450\n"
				"in vec3 inter_color;\n"
				"out vec4 color;\n"
				"void main() {\n"
				"  color = vec4(inter_color, 1.0);\n"
				"}\n"
			);
		}
	}
}
