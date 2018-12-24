#pragma once

#include <engine/element.h>
#include <engine/engine.h>
#include <engine/elements/freeCam.h>

namespace citrus::engine {
	using namespace graphics::windowInput;

	class playerController : public element {
	public:
		float minDist = 3.0f, maxDist = 160.0f;
		float minX = 45.0f, maxX = 45.0f;
		float y = 45.0f, x = 30.0f, dist = 5.0f;
		float ySpeed = 90.0f, xSpeed = 90.0f, distSpeed = 2.0f;
		eleRef<freeCam> cam;

		float navSpeed = 2.0f;

		inline void preRender() {
			//do movement
			glm::vec2 movement = glm::vec2(e->controllerValue(analog::ctr_lstick_x), e->controllerValue(analog::ctr_lstick_y));
			if(glm::length(movement) > 0.05f) {
				movement = glm::vec2(glm::rotate(y / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(movement, 0.0f, 0.0f));
				movement *= e->dt() * navSpeed;
				ent.setLocalPosition(ent.getLocalPosition() + glm::vec3(movement.x, 0.0f, -movement.y));

				float movementAngle = glm::atan(movement.y, movement.x) + glm::pi<float>() * 0.5f;
				ent.setLocalOrientation(glm::rotate(movementAngle, glm::vec3(0.0f, 1.0f, 0.0f)));
			}
			
			//close if needed
			if(e->getKey(button::escape)) e->stop();


			//do camera stuff
			y += -e->dt() * ySpeed * ((e->getKey(button::arrowLeft) ? 1 : 0 + e->getKey(button::arrowRight) ? -1 : 0) + e->controllerValue(analog::ctr_rstick_x));
			dist += -e->dt() * distSpeed * ((e->getKey(button::arrowUp) ? 1 : 0 + e->getKey(button::arrowDown) ? -1 : 0) + e->controllerValue(analog::ctr_rstick_y));
			dist = glm::clamp(dist, minDist, maxDist);

			if(y < 0) y += 360.0f;
			if(y >= 360.0f) y -= 360.0f;


			transform t =
			glm::translate(glm::vec3(ent.getLocalPosition())) *
			glm::rotate(y / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(-x / 180.0f * glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::translate(glm::vec3(0.0f, 0.0f, dist));

			cam->ent.setLocalTransform(t);
		}
		
		inline playerController(entityRef ent) : element(ent, typeid(playerController)) {
			cam = e->getAllOfType<freeCam>()[0];
		}
	};
}