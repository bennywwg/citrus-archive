#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "world.h"
#include "../mankern/element.h"
#include "shapeEle.h"
#include "util.h"

namespace citrus {
	class collisionEle : public element {
		float _friction;
		float _restitution;

		worldShape* _shape;

		btCollisionObject* _object;

		world* _world;
	public:
		void action();

		collisionEle(entRef const& ent, manager& man, void* usr);
		~collisionEle();
	};
}
