#pragma once

class btCollisionObject;
class btRigidBody;
class btGhostObject;

namespace citrus::dynamics {
	class world;

	class collisionObject {
	protected:
		enum collisionObjectType {
			rigidBodyType,
			sensorType
		} const _type;
		btCollisionObject* _ptr;
		world* const _world;
	public:
		btCollisionObject* underlying() const;

		collisionObject(world* world, btRigidBody* ptr);
		collisionObject(world* world, btGhostObject* ptr);
	};
}