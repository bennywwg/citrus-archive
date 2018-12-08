#include <engine/engine.h>
#include <engine/elements/renderManager.h>
#include <engine/elementRef.inl>
#include <engine/manager.inl>

//#include <openvr/openvr.h>

namespace citrus {
	namespace engine {
		weak_ptr<renderManager::shaderInfo> renderManager::loadShader(string name, string vertFile, string geomFile, string fragFile) {
			shared_ptr<renderManager::shaderInfo> info = std::make_shared<renderManager::shaderInfo>();
			info->name = name;
			info->vertFile = vertFile;
			info->geomFile = geomFile;
			info->fragFile = fragFile;
			info->sh = nullptr;
			_shaders.push_back(info);
			return info;
		}
		weak_ptr<renderManager::shaderInfo> renderManager::getShader(string name) {
			for(int i = 0; i < (int)_shaders.size(); i++) {
				if(_shaders[i]->name == name) return _shaders[i];
			}
			return _invalid;
		}
		
		void renderManager::flushShaders() {
			std::lock_guard<mutex> lock(_shadersMut);
			for(int i = 0; i < (int)_shaders.size(); i++) {
				shared_ptr<shaderInfo> info = _shaders[i];
				if(info->sh == nullptr) {
					if(info->geomFile == "")
						info->sh = new graphics::shader(info->vertFile, info->fragFile);
					else
						info->sh = new graphics::shader(info->vertFile, info->geomFile, info->fragFile);
					if(!info->sh->good()) {
						e->Log(info->sh->log());
					}
				}
			}
		}

		void renderManager::resizeBuffer(unsigned int width, unsigned int height) {
			if(width == 0 || height == 0) return;

			standardFBO.reset();
			standardFBO = std::make_unique<graphics::simpleFrameBuffer>(width, height);

			textFBO.reset();
			textFBO = std::make_unique<graphics::simpleFrameBuffer>(width, height);
		}

		void renderManager::load(const nlohmann::json& parsed) {
			text = parsed["text"].get<std::string>();
			camRef = e->man->dereferenceElement<freeCam>(parsed["cam"]);
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
			flushShaders();

			auto win = e->getWindow();
			auto size = win->framebufferSize();
			if(standardFBO->width() != size.x || standardFBO->height() != size.y) {
				resizeBuffer(size.x, size.y);
			}

			standardFBO->bind();
			standardFBO->clearAll();
			_shaders[0]->sh->use();
			glm::mat4 projectionViewMat = (*camRef).cam.getViewProjectionMatrix();
			for(auto ent : e->man->allEntities()) {
				_shaders[0]->sh->setUniform("modelViewProjectionMat", projectionViewMat * ent.getGlobalTransform().getMat());
				graphics::vertexArray::drawOne();
			}
			standardFBO->unbind();


			textFBO->bind();
			textFBO->clearAll();
			auto str = save().dump(2);
			font.streamText(this->save().dump(2), (*camRef).cam.getViewProjectionMatrix() * glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)));
			textFBO->unbind();

			graphics::frameBuffer screen(win);
			screen.bind();
			screen.clearAll();
			composite->use();
			composite->setSampler("bottomColor", *(standardFBO->getColors()[0].tex));
			composite->setSampler("bottomDepth", *(standardFBO->getDepth()));
			composite->setSampler("topColor", *(textFBO->getColors()[0].tex));
			composite->setSampler("topDepth", *(textFBO->getDepth()));
			graphics::vertexArray::drawOne();
			composite->unuse();

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
			font("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\textures\\consolas1024x1024.png", 16, 16),
			element(ent, std::type_index(typeid(renderManager))) {

			auto win = e->getWindow();
			auto size = win->framebufferSize();
			standardFBO = std::make_unique<graphics::simpleFrameBuffer>(size.x, size.y);
			textFBO = std::make_unique<graphics::simpleFrameBuffer>(size.x, size.y);

			_invalid = std::make_shared<shaderInfo>();
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
				"  int count = sampleNear(fUV, topColor);\n"
				"  color = vec4(mix(color.xyz, vec3(1.0, 1.0, 1.0), count / 28.0), 1.0);\n"
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
			loadShader("TransformShader",
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
