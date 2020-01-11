#include "doorController.h"
#include "../builtin/rigidEle.h"
#include "../mankern/manager.inl"
#include "gameState.h"

namespace citrus {
	void doorController::openDoor() {
		barrier->deactivate();
		opening = true;
	}
	void doorController::closeDoor() {
		barrier->activate();
		opening = false;
	}
	void doorController::action() {
		entRef p = ((gameState*)usr())->playerNavBody;

		if (frontSensor->touching(p) || backSensor->touching(p)) {
			openDoor();
		} else {
			closeDoor();
		}

		if (opening) {
			ds += 0.005f;
			if (ds > 1.0f) ds = 1.0f;
		} else {
			ds -= 0.005f;
			if (ds < 0.0f) ds = 0.0f;
		}

		doorModel->setAnimationState(0, ds);
	}
	doorController::doorController(entRef const& ent, manager& man, void* usr) : element(ent, man, usr, typeid(doorController)) {
		barrier = ent.getChild("barrier").getEle<collisionEle>();
		barrierSensor = ent.getChild("barrierSensor").getEle<sensorEle>();
		frontSensor = ent.getChild("frontSensor").getEle<sensorEle>();
		backSensor = ent.getChild("backSensor").getEle<sensorEle>();
		doorModel = ent.getEle<modelEle>();
	}
}