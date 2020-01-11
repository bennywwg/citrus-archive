#pragma once

#include "world.h"
#include "../mankern/elementRef.inl"
#include "../mankern/entityRef.h"
#include "shapeEle.h"
#include "util.h"
#include "bulletObject.h"

namespace citrus {
	class sensorEle : public bulletObject, public element {
		worldShape* _shape;

		btGhostObject* _body;

		world* _world;
	public:
		userData data;

		void action();

		bool touchingAny() const;

		int numTouching() const;

		bool touching(entRef const& e) const;

		std::vector<eleRef<shapeEle>> touching() const;

		sensorEle(entRef const& ent, manager& man, void* usr);
		~sensorEle();
	};
}
