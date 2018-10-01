#pragma once

#ifndef WORLD_H
#define WORLD_H

#include <memory>
#include <mutex>
#include <set>

#include <btBulletDynamicsCommon.h>

#include "rigidBody.h"

namespace citrus {
	namespace dynamics {
		class /*za*/ world {
			friend class rigidBody;

			private:
			std::shared_ptr<btCollisionConfiguration> _collisionConfiguration;
			std::shared_ptr<btDispatcher> _dispatcher;
			std::shared_ptr<btBroadphaseInterface> _broadphaseInterface;
			std::shared_ptr<btConstraintSolver> _constraintSolver;
			std::shared_ptr<btDynamicsWorld> _world;

			struct bodyUpdateInfo {
				rigidBody* body;
				glm::vec3 newPosition;
				glm::quat newOrientation;
			};
			std::set<rigidBody*> _bodies;
			std::vector<bodyUpdateInfo> _toBeUpdated;
			std::mutex _toBeUpdatedMut;

			std::set<rigidBody*> _toBeAdded;
			std::set<rigidBody*> _toBeRemoved;
			std::mutex _toBeAddedMut;
			std::mutex _toBeRemovedMut;

			void addBody(rigidBody* body);
			void removeBody(rigidBody* body);

			public:
			void flushAddRemove();
			
			void step();

			void updateBodyInfo();

			world();
		};
	}
}


#endif // !WORLD_H
