#pragma once

#include <memory>
#include <set>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <btBulletDynamicsCommon.h>

#include "shape.h"
#include "util.h"

#include "../graphkern/immediatePass.h"

namespace citrus {
	class ctBulletDebugDraw;

	class worldShape {
	public:
		btCollisionShape *wdshape; // don't mess with
		btScalar *verts;
		int *indices;

		worldShape(shape const& s);
		~worldShape();
	};

	class /*za*/ world /*o*/ {
		friend class rigidBody;
		friend class sensor;

	public:
		ctBulletDebugDraw* dbdraw;

		std::unique_ptr<btCollisionConfiguration> _collisionConfiguration;
		std::unique_ptr<btDispatcher> _dispatcher;
		std::unique_ptr<btBroadphaseInterface> _broadphaseInterface;
		std::unique_ptr<btConstraintSolver> _constraintSolver;
		std::unique_ptr<btDynamicsWorld> _world;

		std::set<btRigidBody*> bodies;
		std::set<btCollisionObject*> objects;
		std::set<btGhostObject*> ghosts;

		void addBody(btRigidBody* body);
		void removeBody(btRigidBody* body);
		void addSensor(btGhostObject* se);
		void removeSensor(btGhostObject* se);
		void addObject(btCollisionObject* object);
		void removeObject(btCollisionObject* object);
		void debugDraw();

		void step();

		world(immediatePass& pass);
	};
}