#include "playerController.h"
#include "../builtin/rigidEle.h"
#include "../mankern/manager.inl"

namespace citrus {
	void playerController::cameraStuff() {
		//do camera stuff
		float dt = 0.01;
		y += -dt * ySpeed * ((win->getKey(windowInput::arrowLeft) ? 1 : 0 + win->getKey(windowInput::arrowRight) ? -1 : 0) + win->controllerValue(windowInput::ctr_rstick_x));
		float distMag = ((win->getKey(windowInput::arrowUp) ? 1 : 0 + win->getKey(windowInput::arrowDown) ? -1 : 0) - win->controllerValue(windowInput::ctr_rstick_y));
		//dist += -eng()->dt() * distSpeed * (abs(distMag) > 0.8f ? distMag : 0.0f);
		x -= distMag * 0.2f;

		if(y >= 360.0f) y -= 360.0f;
		if(y < 0) y += 360.0f;

		transform t =
			glm::translate(glm::vec3(ent().getLocalPos())) *
			glm::rotate(y / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(-x / 180.0f * glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::translate(glm::vec3(0.0f, 0.0f, dist));

		cam->ent().setLocalTrans(t);
	}
	void playerController::movementStuff() {
		//do movement
		glm::vec2 movement = glm::vec2(win->controllerValue(windowInput::ctr_lstick_x), -win->controllerValue(windowInput::ctr_lstick_y));
		movement += glm::vec2((win->getKey(windowInput::d) ? 1.0f : 0.0f) + (win->getKey(windowInput::a) ? -1.0f : 0.0f), (win->getKey(windowInput::w) ? 1.0f : 0.0f) + (win->getKey(windowInput::s) ? -1.0f : 0.0f));
		if(glm::length(movement) > 0.125f) {
			movement = glm::vec2(glm::rotate(y / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(movement, 0.0f, 0.0f));

			float movementAngle = glm::atan(movement.y, movement.x) + glm::pi<float>() * 0.5f;
			body->setOri(glm::rotate(movementAngle, glm::vec3(0.0f, 1.0f, 0.0f)));
		}
		if (glm::length(movement) > 1.0f) movement = glm::normalize(movement);

		if (legSensor->numTouching() >= 2) {
			if (win->controllerButton(windowInput::ctr_south) || win->getKey(windowInput::space)) {
				body->applyImpulse(vec3(0.0f, jumpStrength, 0.0f));
			}
			vec3 target = vec3(movement.x, 0.0f, -movement.y) * targetSpeed;
			vec3 vel = body->getVelocity();
			vel.y = 0;
			vec3 diff = target - vel;
			body->applyImpulse(diff * accelFactor);
		}
	}
	void playerController::actionStuff() {
		//glm::vec3 playerPos = ent().getGlobalTransform().getPosition();
		glm::vec3 playerDir = glm::toMat4(ent().getLocalOri()) * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
		glm::quat playerOri = glm::toQuat(glm::toMat4(ent().getGlobalTrans().getOrientation()) * glm::rotate(3.141596f * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));


		/*if(win->controllerButton(windowInput::ctr_rtrigger)) {
			if(!fired) {
				glm::vec3 projStart = ent().getGlobalTrans().getMat() * glm::translate(glm::vec3(0.0f, 0.0f, 1.5f)) * glm::vec4(0, 0, 0, 1);
				fired = true;
				auto er = man().create("shot");
				
				
				, {
					eleInit<projectile>::run([e, playerDir, playerOri, projStart](projectile& p) {
						p.startTime = (float)e->time();
						p.maxTime = (float) 1.0f;
						p.velocity = playerDir * 15.0f;
						p.ent().setLocalPosition(projStart);
						p.ent().setLocalOrientation(playerOri);
						p.ent().setLocalScale(glm::vec3(0.1f, 0.1f, 0.1f));
					}),
					eleInit<meshFilter>::run([e](meshFilter& m) {
						m.setState(0, 1, 3);
						m.color = glm::vec3(1.0f, 0.5f, 0.5f);
					}),
					eleInit<sensorEle>::run([e](sensorEle& s) {
						s.sense->setRadius(0.1f);
					})
					}, util::nextID());
			}
		} else {
			fired = false;
		}*/
	}
	void playerController::action() {
		//close if needed
		cameraStuff();
		movementStuff();
		actionStuff();
	}
	void playerController::deserialize(json const& j) {
		dist = j["dist"];
		jumpStrength = j["jumpStrength"];
		targetSpeed = j["targetSpeed"];
		accelFactor = j["accelFactor"];
	}
	playerController::playerController(entRef const& ent, manager& man, void* usr) : element(ent, man, usr, typeid(playerController)), win((window*)usr) {
		cam = man.ofType<freeCam>()[0];
		body = ent.getEle<rigidEle>();
		legSensor = ent.getChild("legSensor").getEle<sensorEle>();
	}
}