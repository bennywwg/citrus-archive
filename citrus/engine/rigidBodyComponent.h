#pragma once

#include "citrus/engine/element.h"
#include "citrus/dynamics/rigidBody.h"
#include "citrus/dynamics/collisionShape.h"
#include "citrus/dynamics/transform.h"

namespace citrus::engine {
	class rigidBodyComponent : public element {
		public:
		enum class shapeType : int {
			none = 0,
			sphere = 1,
			box = 2,
			hull = 3
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

		void load(citrus::json const& data);
		citrus::json save();

		rigidBodyComponent(entityRef owner);
	};
}
