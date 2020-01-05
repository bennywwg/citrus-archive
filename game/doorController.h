#pragma once

#include "../mankern/element.h"
#include "../mankern/elementRef.h"
#include "../builtin/freeCam.h"
#include "../builtin/modelEle.h"
#include "../builtin/sensorEle.h"
#include "../builtin/collisionEle.h"

namespace citrus {
	class doorController : public element {
	public:
		enum doorState {
			door_closed,
			door_opening,
			door_open,
			door_closing
		};

		eleRef<collisionEle> barrier;
		eleRef<sensorEle> barrierSensor, frontSensor, backSensor;
		eleRef<modelEle> doorModel;

		void openDoor();
		void closeDoor();

		doorState getState();

		void action();

		doorController(entRef const& ent, manager& man, void* usr);
	};
}
