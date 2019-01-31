#include <dynamics/physics/collisionObject.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

namespace citrus::dynamics {
	btCollisionObject* collisionObject::underlying() const {
		return _ptr;
	}
	collisionObject::collisionObject(world * world, btRigidBody * ptr) : _world(world), _ptr(ptr), _type(rigidBodyType) {
	}
	collisionObject::collisionObject(world * world, btGhostObject * ptr) : _world(world), _ptr(ptr), _type(sensorType) {
	}
}