#include <citrus/dynamics/sensor.h>
#include <citrus/dynamics/collisionShape.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <citrus/dynamics/rigidBody.h>
#include <citrus/util.h>
#include <citrus/dynamics/world.h>

namespace citrus::dynamics {
	btGhostObject* sensor::ptr() const {
		return _ghost;
	}

	glm::vec3 sensor::getPosition() const {
		return util::btToGlm(_ghost->getWorldTransform().getOrigin());
	}
	glm::quat sensor::getOrientation() const {
		return util::btToGlm(_ghost->getWorldTransform().getRotation());
	}
	void sensor::setPosition(glm::vec3 pos) const {
		_ghost->getWorldTransform().setOrigin(util::glmToBt(pos));
	}
	void sensor::setOrientation(glm::quat ori) const {
		_ghost->getWorldTransform().setRotation(util::glmToBt(ori));
	}
	transform sensor::getTransform() const {
		return transform(getPosition(), getOrientation());
	}
	void sensor::setTransform(transform trans) const {
		_ghost->setWorldTransform(util::glmToBt(trans));
	}

	void sensor::setRadius(float rad) const {
		((btSphereShape*)_ghost->getCollisionShape())->setUnscaledRadius(rad);
	}

	bool sensor::touchingAny() const {
		return _ghost->getNumOverlappingObjects() != 0;
	}

	bool sensor::isTouching(rigidBody& other) const {
		return _ghost->checkCollideWith(other.ptr());
	}
	bool sensor::isTouching(sensor& other) const {
		return _ghost->checkCollideWith(other.ptr());
	}

	void sensor::allTouching(std::vector<collisionObject*>& fill) const {
		auto overlapping = _ghost->getOverlappingPairs();
		fill.resize(overlapping.size());
		for (int i = 0; i < overlapping.size(); i++) {
			fill[i] = (collisionObject*)overlapping[i]->getUserPointer();
		}
	}

	sensor::sensor(collisionShape* shape, world* world) : collisionObject(world, (btGhostObject*)nullptr) {
		_ghost = new btGhostObject();
		_ghost->setUserPointer(this);
		_ptr = _ghost;
		_ghost->setCollisionShape(shape->ptr());
		_ghost->setCollisionFlags(_ghost->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		_world->addSensor(this);
	}
	sensor::~sensor() {
		_world->removeSensor(this);
	}
}