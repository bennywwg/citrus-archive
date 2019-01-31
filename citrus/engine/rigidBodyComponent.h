#pragma once

#include <engine/element.h>
#include <dynamics/physics/rigidBody.h>
#include <dynamics/physics/collisionShape.h>
#include <dynamics/transform.h>

namespace citrus::engine {
	class rigidBodyComponent : public element {

		public:
		enum shapeType {
			sphere,
			box,
			hull,
			none
		};
		shapeType type;

		dynamics::world *w;
		std::unique_ptr<dynamics::collisionShape> shape;
		std::unique_ptr<dynamics::rigidBody> body;

		bool isSphere();
		bool isBox();

		void setToSphere(float radius);
		void setToBox(glm::vec3 boxSize);
		void setToHull();

		void setPosition(const glm::vec3& pos);
		void setOrientation(const glm::quat& ori);
		void setTransform(const transform& tr);
		glm::vec3 getPosition();
		glm::quat getOrientation();
		transform getTransform();

		void setDynamic(bool d);

		rigidBodyComponent(entityRef owner);
	};
}