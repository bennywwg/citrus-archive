#pragma once

//#include <BulletCollision/CollisionDispatch/btGhostObject.h"
class btGhostObject;

#include "citrus/dynamics/collisionObject.h"
#include "citrus/util.h"
#include <vector>

namespace citrus {
	class transform;
}

namespace citrus::dynamics {
	class collisionShape;
	class rigidBody;
	class world;

	class sensor : public collisionObject {
		btGhostObject* _ghost; //is the same as underlying();

	public:
		btGhostObject* ptr() const;
		vec3 getPosition() const;
		quat getOrientation() const;
		void setPosition(vec3 pos) const;
		void setOrientation(quat ori) const;
		transform getTransform() const;
		void setTransform(transform tr) const;
		void setRadius(float rad) const;
		bool touchingAny() const;
		bool isTouching(rigidBody& other) const;
		bool isTouching(sensor& other) const;
		void allTouching(std::vector<collisionObject*>& fill) const;

		sensor(collisionShape* shape, world* world);
		~sensor();
	};
}
