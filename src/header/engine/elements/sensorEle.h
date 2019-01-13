#pragma once

#include <engine/element.h>
#include <dynamics/physics/sensor.h>
#include <dynamics/physics/world.h>
#include <dynamics/physics/collisionShape.h>
#include <dynamics/transform.h>

namespace citrus::engine {
	class sensorEle : public element {
	public:
		dynamics::world *w;
		std::unique_ptr<dynamics::collisionShape> shape;
		std::unique_ptr<dynamics::sensor> sense;

		bool touchingAny() const;

		bool isSphere();
		bool isBox();

		void setToSphere(float radius);
		void setToBox(glm::vec3 boxSize);
		void setToHull();

		void preRender();

		sensorEle(entityRef owner);
		~sensorEle();
	};
}