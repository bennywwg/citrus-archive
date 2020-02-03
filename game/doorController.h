#pragma once

#include "../mankern/element.h"
#include "../mankern/elementRef.h"
#include "../builtin/freeCam.h"
#include "../builtin/modelEle.h"
#include "../builtin/sensorEle.h"
#include "../builtin/collisionEle.h"
#include "roomController.h"

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

		eleRef<roomController> posRoom, negRoom;

		float ds = 0.0f;
		bool opening = false;

		void openDoor();
		void closeDoor();

		doorState getState();

		void action();

		doorController(entRef const& ent, manager& man, void* usr);
	};
}
