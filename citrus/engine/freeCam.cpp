#include <citrus/engine/freeCam.h>
#include <citrus/engine/engine.h>
#include <citrus/engine/renderManager.h>
#include <citrus/graphics/window.h>

#include <citrus/util.h>

namespace citrus::engine {
	using namespace graphics;

	void freeCam::preRender() {
/*
		vec3 yPlaneMovement(
			(e->getKey(windowInput::a) ? -1.0f : 0.0f) + (e->getKey(windowInput::d) ? 1.0f : 0.0f),
			(e->getKey(windowInput::q) ? -1.0f : 0.0f) + (e->getKey(windowInput::e) ? 1.0f : 0.0f),
			(e->getKey(windowInput::w) ? -1.0f : 0.0f) + (e->getKey(windowInput::s) ? 1.0f : 0.0f)
		);
		yPlaneMovement += vec3(
			e->controllerValue(windowInput::analog::ctr_lstick_x),
			0.0f,
			-e->controllerValue(windowInput::analog::ctr_lstick_y)
		);
		yPlaneMovement *= 0.015f;

		if(e->getKey(windowInput::escape) || e->controllerButton(windowInput::ctr_start))
			e->stop();

		glm::vec3 pos = ent.getGlobalTransform().getPosition() + yPlaneMovement;

		ent.setLocalPosition(pos);*/

		//transformComponent& trans = *getOwner()->getComponent<transformComponent>().lock();
		//trans.setTransform(trans.getTransform().getTranslated(yPlaneMovement));

	}
	void freeCam::render() {
		cam.trans = ent().getGlobalTransform();
	}
	freeCam::freeCam(entityRef ent) : element(ent, typeid(freeCam)) { }
}