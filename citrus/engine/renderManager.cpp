#include <citrus/engine/engine.h>
#include <citrus/engine/renderManager.h>
#include <citrus/engine/entityRef.inl>
#include <citrus/engine/elementRef.inl>
#include <citrus/engine/manager.inl>

//#include <openvr/openvr.h>

namespace citrus {
	namespace engine {
		void renderManager::addItem(eleRef<meshFilter> me, int m, int t, int s) {
			std::lock_guard<std::mutex> lock(_drawableMut);
			auto& eles = _items[m]._eles;
			for(int i = 0; i < eles.size(); i++) {
				if(eles[i].null()) {
					eles[i] = me;
					return;
				}
			}
			eles.push_back(me);
		}
		void renderManager::removeItem(eleRef<meshFilter> me, int oldM, int oldT, int oldS) {
			std::lock_guard<std::mutex> lock(_drawableMut);
			auto& eles = _items[oldM]._eles;
			for(int i = 0; i < eles.size(); i++) {
				if(eles[i] == me) {
					eles[i] = nullptr;
				}
			}
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

			camera cam = camRef->cam;

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
	}
}
