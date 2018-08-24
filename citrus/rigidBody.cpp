#include "rigidBody.h"
#include "world.h"
#include "collisionShape.h"
#include "util.h"

using namespace citrus::dynamics;

btRigidBody* rigidBody::ptr() const {
	return _body;
}

glm::vec3 rigidBody::getPosition() const {
	return _position;
}
glm::quat rigidBody::getOrientation() const {
	return _orientation;
}
citrus::engine::transform rigidBody::getTransform() const {
	return engine::transform(getPosition(), getOrientation());
}

rigidBody::rigidBody(const collisionShape& shape, world* _world) :
	_body(new btRigidBody(1.0f, nullptr, shape.ptr().lock().get())) {
	_world->addBody(this);
}
rigidBody::~rigidBody() {
	_world->removeBody(this);
}