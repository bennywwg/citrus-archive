#pragma once

#include "world.h"
#include "../mankern/elementRef.inl"
#include "../mankern/entityRef.h"
#include "shapeEle.h"
#include "util.h"

namespace citrus {
	class sensorEle : public element {
		btMotionState* _state;

		worldShape* _shape;

		btGhostObject* _body;

		world* _world;
	public:
		inline void action() {
			_body->setWorldTransform(glmToBt(ent().getLocalTrans()));
		}

		inline bool touchingAny() const {
			return false;
		}

		inline std::vector<eleRef<shapeEle>> touching() const {
			return { };
		}

		inline sensorEle(entRef const&ent, manager& man, void* usr) :
			element(ent, man, usr, typeid(sensorEle)),
			_world((world*)usr)
		{
			eleRef<shapeEle> const& myShape = ent.getEle<shapeEle>();
			if (!myShape.id()) throw std::runtime_error("sensorEle needs shapeEle");

			// make shape in world
			_shape = new worldShape(myShape->getShape());

			// make body
			_body = new btGhostObject();
			_body->setUserPointer(this);

			_world->addSensor(_body);
		}
		inline ~sensorEle() {
			_world->removeSensor(_body);
			delete _body;
			delete _shape;
			delete _state;
		}
	};
}
