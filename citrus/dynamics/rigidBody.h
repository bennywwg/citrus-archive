#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "citrus/util.h"
#include "citrus/dynamics/collisionObject.h"

namespace citrus {
	namespace dynamics {
		class world;
		class collisionShape;

		class rigidBody : public collisionObject {
			friend class world;

		private:
			btRigidBody* _body;
			btMotionState* _state;
			collisionShape* const _shape;

		public:
			bool dynamic = true;
			double mass = 1.0;

			btRigidBody* ptr() const;
			collisionShape* getShape() const;

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
