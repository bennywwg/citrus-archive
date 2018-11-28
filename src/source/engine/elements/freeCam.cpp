#include <engine/elements/freeCam.h>
#include <engine/engine.h>
#include <engine/elements/renderManager.h>
#include <graphics/window/window.h>

#include <util/glmUtil.h>

namespace citrus {
	namespace engine {
		using namespace graphics;



		void freeCam::preRender() {
			vec3 yPlaneMovement(
				(e->getKey(windowInput::a) ? -1.0f : 0.0f) + (e->getKey(windowInput::d) ? 1.0f : 0.0f),
				(e->getKey(windowInput::q) ? -1.0f : 0.0f) + (e->getKey(windowInput::e) ? 1.0f : 0.0f),
				(e->getKey(windowInput::w) ? -1.0f : 0.0f) + (e->getKey(windowInput::s) ? 1.0f : 0.0f)
			);
			yPlaneMovement *= 0.005f;

			if(e->getKey(windowInput::escape))
				e->stop();

			glm::vec3 pos = ent.getGlobalTransform().getPosition() + yPlaneMovement;

			ent.setLocalPosition(pos);
			// util::sout(std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));

			//transformComponent& trans = *getOwner()->getComponent<transformComponent>().lock();
			//trans.setTransform(trans.getTransform().getTranslated(yPlaneMovement));

			cam.trans.setPosition(pos);
		}
		void freeCam::render() {

		}
		freeCam::freeCam(entityRef ent) : element(ent, typeid(freeCam)) { }
	}
}