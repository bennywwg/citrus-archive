#include "citrus/engine/playerController.h"

#include "citrus/engine/projectile.h"
#include "citrus/engine/meshFilter.h"
#include "citrus/engine/sensorEle.h"

#include "citrus/engine/manager.inl"
#include "citrus/engine/elementRef.inl"
#include "citrus/engine/entityRef.inl"

#include "citrus/graphics/mesh/mesh.h"

namespace citrus::engine {
	void playerController::cameraStuff() {
		//do camera stuff
		y += -eng()->dt() * ySpeed * ((eng()->getKey(button::arrowLeft) ? 1 : 0 + eng()->getKey(button::arrowRight) ? -1 : 0) + eng()->controllerValue(analog::ctr_rstick_x));
		float distMag = ((eng()->getKey(button::arrowUp) ? 1 : 0 + eng()->getKey(button::arrowDown) ? -1 : 0) - eng()->controllerValue(analog::ctr_rstick_y));
		//dist += -eng()->dt() * distSpeed * (abs(distMag) > 0.8f ? distMag : 0.0f);
		x -= distMag * 0.2f;
		dist = glm::clamp(dist, minDist, maxDist);
		dist = 16.0f;

		if(y < 0) y += 360.0f;
		if(y >= 360.0f) y -= 360.0f;

		if(eng()->controllerButton(button::ctr_south)) {
			eng()->getAllOfType<rigidBodyComponent>()[0]->body->ptr()->setLinearVelocity(btVector3(0.0, 5.0, 0.0));
		}

		transform t =
			glm::translate(glm::vec3(ent().getLocalPosition())) *
			glm::rotate(y / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(-x / 180.0f * glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::translate(glm::vec3(0.0f, 0.0f, dist));

		cam->ent().setLocalTransform(t);
	}
	void playerController::movementStuff() {
		//do movement
		glm::vec2 movement = glm::vec2(eng()->controllerValue(analog::ctr_lstick_x), -eng()->controllerValue(analog::ctr_lstick_y));
		movement += glm::vec2((eng()->getKey(button::d) ? 1.0f : 0.0f) + (eng()->getKey(button::a) ? -1.0f : 0.0f), (eng()->getKey(button::w) ? 1.0f : 0.0f) + (eng()->getKey(button::s) ? -1.0f : 0.0f));
		if(glm::length(movement) > 0.25f) {
			movement = glm::vec2(glm::rotate(y / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(movement, 0.0f, 0.0f));
			movement *= eng()->dt() * navSpeed;
			ent().setLocalPosition(ent().getLocalPosition() + glm::vec3(movement.x, 0.0f, -movement.y));

			float movementAngle = glm::atan(movement.y, movement.x) + glm::pi<float>() * 0.5f;
			ent().setLocalOrientation(glm::rotate(movementAngle, glm::vec3(0.0f, 1.0f, 0.0f)));

			if(!walking) {
				walking = true;
				m->startAnimation(1, graphics::behavior::repeat);
			}
		} else {
			if(walking) {
				walking = false;
				m->startAnimation(0, graphics::behavior::repeat);
			}
		}
	}
	void playerController::actionStuff() {
		//glm::vec3 playerPos = ent().getGlobalTransform().getPosition();
		glm::vec3 playerDir = glm::toMat4(ent().getLocalOrientation()) * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
		glm::quat playerOri = glm::toQuat(glm::toMat4(ent().getGlobalTransform().getOrientation()) * glm::rotate(3.141596f * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));


		if(eng()->controllerButton(ctr_rtrigger)) {
			if(!fired) {
				glm::vec3 projStart = ent().getGlobalTransform().getMat() * glm::translate(glm::vec3(0.0f, 0.0f, 1.5f)) * glm::vec4(0, 0, 0, 1);
				fired = true;
				engine* e = this->eng();
				e->man->create("shot", {
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
		}
	}
	void playerController::preRender() {
		//close if needed
		if(eng()->getKey(button::escape)) eng()->stop();

		cameraStuff();
		movementStuff();
		actionStuff();
	}
	string playerController::name() const {
		return "Player Controller";
	}
	playerController::playerController(entityRef ent) : element(ent, typeid(playerController)) {
		cam = eng()->getAllOfType<freeCam>()[0];
		m = ent.getChildren()[0].getElement<meshFilter>();
	}
}