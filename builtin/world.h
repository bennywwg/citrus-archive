#pragma once

#include <memory>
#include <set>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <btBulletDynamicsCommon.h>

#include "shape.h"
#include "util.h"

namespace citrus {
	class worldShape {
	public:
		btCollisionShape *wdshape; // don't mess with

		inline worldShape(shape const& s) {
			if (s.type == shapeType::sphere) {
				wdshape = new btSphereShape(s.state.r);
			} else if (s.type == shapeType::box) {
				wdshape = new btBoxShape(glmToBt(s.state));
			} else if (s.type == shapeType::hull) {
				std::vector<btVector3> btVerts;
				btVerts.resize(s.points.size());
				for (size_t i = 0; i < s.points.size(); i++) {
					btVerts[i] = btVector3(s.points[i].x, s.points[i].y, s.points[i].z);
				}
				wdshape = new btConvexHullShape((btScalar*)btVerts.data(), btVerts.size(), sizeof(btVector3));
			} else {
				wdshape = nullptr;
			}
		}
		inline ~worldShape() {
			if(wdshape) delete wdshape;
		}
	};

	class /*za*/ world /*o*/ {
		friend class rigidBody;
		friend class sensor;

	public:

		std::unique_ptr<btCollisionConfiguration> _collisionConfiguration;
		std::unique_ptr<btDispatcher> _dispatcher;
		std::unique_ptr<btBroadphaseInterface> _broadphaseInterface;
		std::unique_ptr<btConstraintSolver> _constraintSolver;
		std::unique_ptr<btDynamicsWorld> _world;

		std::set<btRigidBody*> bodies;
		std::set< btGhostObject*> ghosts;

		void addBody(btRigidBody* body) {
			_world->addRigidBody(body);
			_world->computeOverlappingPairs();
			bodies.insert(body);
		}
		void removeBody(btRigidBody* body) {
			_world->removeRigidBody(body);
			_world->computeOverlappingPairs();
			bodies.erase(body);
		}
		void addSensor(btGhostObject* se) {
			_world->addCollisionObject(se);
			_world->computeOverlappingPairs();
			ghosts.insert(se);
		}
		void removeSensor(btGhostObject* se) {
			_world->removeCollisionObject(se);
			_world->computeOverlappingPairs();
			ghosts.erase(se);
		}

		inline void step() {
			_world->stepSimulation(0.01f, 1, 0.01f);
		}

		inline world() :
			_collisionConfiguration(new btDefaultCollisionConfiguration()),
			_dispatcher(new btCollisionDispatcher(_collisionConfiguration.get())),
			_broadphaseInterface(new btDbvtBroadphase()),
			_constraintSolver(new btSequentialImpulseConstraintSolver()),
			_world(new btDiscreteDynamicsWorld(_dispatcher.get(), _broadphaseInterface.get(), _constraintSolver.get(), _collisionConfiguration.get()))
		{
				_world->setGravity(btVector3(0.0, -9.81, 0.0));
				_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
		}
	};
}