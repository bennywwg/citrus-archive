#include <dynamics/physics/rigidBody.h>
#include <dynamics/physics/world.h>
#include <dynamics/physics/collisionShape.h>
#include <util/util.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

namespace citrus::dynamics {
	btRigidBody* rigidBody::ptr() const {
		return _body;
	}

	collisionShape * rigidBody::getShape() const {
		return _shape;
	}

	glm::vec3 rigidBody::getPosition() const {
		return util::btToGlm(_body->getWorldTransform().getOrigin());
	}
	glm::quat rigidBody::getOrientation() const {
		return util::btToGlm(_body->getWorldTransform().getRotation());
	}
	void rigidBody::setPosition(glm::vec3 pos) {
		_body->getWorldTransform().setOrigin(util::glmToBt(pos));
		_body->activate(false);
	}
	void rigidBody::setOrientation(glm::quat ori) {
		_body->getWorldTransform().setRotation(util::glmToBt(ori));
		_body->activate(false);
	}
	transform rigidBody::getTransform() const {
		return transform(getPosition(), getOrientation());
	}

	void rigidBody::setTransform(transform trans) {
		_body->setWorldTransform(util::glmToBt(trans));
		_body->activate(false);
	}

	rigidBody::rigidBody(collisionShape* shape, world* world) : collisionObject(world, (btRigidBody*)nullptr), _shape(shape) {
		//_state = new btDefaultMotionState();
		btRigidBody::btRigidBodyConstructionInfo ci = btRigidBody::btRigidBodyConstructionInfo(mass, nullptr, shape->ptr(), btVector3(0.0f, 0.0f, 0.0f));
		ci.m_friction = 1.0f;
		ci.m_restitution = 0.1f;
		_body = new btRigidBody(ci);
		_body->setUserPointer(this);
		_ptr = _body;
		_world->addBody(this);
		_body->setActivationState(DISABLE_DEACTIVATION);
		setTransform(transform());
	}
	rigidBody::~rigidBody() {
		_world->removeBody(this);
		//delete _state;
		delete _body;
	}
}