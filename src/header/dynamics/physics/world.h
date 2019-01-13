#pragma once

#include <memory>
#include <mutex>
#include <set>

#include <btBulletDynamicsCommon.h>

#include "rigidBody.h"

namespace citrus::dynamics {
	class /*za*/ world /*o*/ {
		friend class rigidBody;
		friend class sensor;

		private:
		std::unique_ptr<btCollisionConfiguration> _collisionConfiguration;
		std::unique_ptr<btDispatcher> _dispatcher;
		std::unique_ptr<btBroadphaseInterface> _broadphaseInterface;
		std::unique_ptr<btConstraintSolver> _constraintSolver;
		std::unique_ptr<btDynamicsWorld> _world;

		std::set<rigidBody*> _bodies;
		std::set<sensor*> _sensors;
/*
		std::vector<rigidBody*> _toBeAdded;
		std::vector<rigidBody*> _toBeRemoved;
		std::mutex _toBeAddedMut;
		std::mutex _toBeRemovedMut;*/

		void addBody(rigidBody* body);
		void removeBody(rigidBody* body);
		void addSensor(sensor* sense);
		void removeSensor(sensor* sense);

		public:
			
		void step();

		world();
	};
}