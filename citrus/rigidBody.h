#pragma once
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <memory>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <glm\ext.hpp>

#include "util.h"
#include "transform.h"

namespace citrus {
	namespace dynamics {
		class world;
		class collisionShape;

		class rigidBody {
			friend class world;

		private:
			world* _world;

			btRigidBody* _body;

			glm::vec3 _position;
			glm::quat _orientation;

		public:
			btRigidBody* ptr() const;

			glm::vec3 getPosition() const;
			glm::quat getOrientation() const;
			engine::transform getTransform() const;

			rigidBody(const collisionShape& shape, world* _world);
			~rigidBody();
		};
	}
}





#endif
