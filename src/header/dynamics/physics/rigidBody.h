#pragma once

#include <memory>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <glm/ext.hpp>

#include <util/util.h>
#include <dynamics/transform.h>

namespace citrus {
	namespace dynamics {
		class world;
		class collisionShape;

		class rigidBody {
			friend class world;

		private:
			world* _world;

			btRigidBody* _body;
			btMotionState* _state;

			bool _updatedTrans = false;
			transform _trans = transform();

		public:
			bool dynamic = true;
			double mass = 1.0;

			btRigidBody* ptr() const;

			glm::vec3 getPosition() const;
			glm::quat getOrientation() const;
			void setPosition(glm::vec3 pos);
			void setOrientation(glm::quat ori);
			transform getTransform() const;
			void setTransform(transform tr);

			rigidBody(collisionShape* shape, world* _world);
			~rigidBody();
		};
	}
}
