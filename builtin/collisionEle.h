#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "world.h"
#include "../mankern/element.h"
#include "shapeEle.h"
#include "util.h"
#include "bulletObject.h"

namespace citrus {
	class collisionEle : public bulletObject, public element {
		float _friction;
		float _restitution;

		worldShape* _shape;

		btCollisionObject* _object;

		world* _world;
	public:
		userData data;

		void action();

		bool active();
		void activate();
		void deactivate();

		collisionEle(entRef const& ent, manager& man, void* usr);
		~collisionEle();
	};
}
