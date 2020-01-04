#include "doorController.h"
#include "../builtin/rigidEle.h"
#include "../mankern/manager.inl"

namespace citrus {
	void doorController::openDoor() {
		barrier->deactivate();
	}
	void doorController::closeDoor() {
		barrier->activate();
	}
	void doorController::action() {
		
	}
	doorController::doorController(entRef const& ent, manager& man, void* usr) : element(ent, man, usr, typeid(doorController)) {
		barrier = ent.getChild("barrier").getEle<collisionEle>();
		barrierSensor = ent.getChild("barrierSensor").getEle<sensorEle>();
		frontSensor = ent.getChild("frontSensor").getEle<sensorEle>();
		backSensor = ent.getChild("backSensor").getEle<sensorEle>();
	}
}