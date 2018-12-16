#include <dynamics/physics/rigidBody.h>
#include <dynamics/physics/world.h>
#include <dynamics/physics/collisionShape.h>
#include <util/util.h>

namespace citrus::dynamics {
	btRigidBody* rigidBody::ptr() const {
		return _body;
	}

	glm::vec3 rigidBody::getPosition() const {
		return _trans.getPosition();
	}
	glm::quat rigidBody::getOrientation() const {
		return _trans.getOrientation();
	}
	void rigidBody::setPosition(glm::vec3 pos) {
		_trans.setPosition(pos);
		_updatedTrans = true;
	}
	void rigidBody::setOrientation(glm::quat ori) {
		_trans.setOrientation(ori);
		_updatedTrans = true;
	}
	transform rigidBody::getTransform() const {
		return transform(getPosition(), getOrientation());
	}

	void rigidBody::setTransform(transform trans) {
		_trans = trans;
		_updatedTrans = true;
	}

	rigidBody::rigidBody(collisionShape* shape, world* world) : _world(world) {
		_state = new btDefaultMotionState();
		btRigidBody::btRigidBodyConstructionInfo ci = btRigidBody::btRigidBodyConstructionInfo(mass, _state, shape->ptr(), btVector3(1.0f, 1.0f, 1.0f));
		ci.m_friction = 1.0f;
		ci.m_restitution = 1.0f;
		_body = new btRigidBody(ci);
		_world->addBody(this);
	}
	rigidBody::~rigidBody() {
		_world->removeBody(this);
		delete _state;
		delete _body;
	}
}