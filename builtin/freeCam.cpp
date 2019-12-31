#include "freeCam.h"
#include "../mankern/entityRef.h"

namespace citrus {
	void freeCam::action() {
		cam.pos = ent().getGlobalTrans().getPosition();
		cam.ori = ent().getGlobalTrans().getOrientation();
		if (enabled) ((renderSystem*)usr())->frameCam = cam;
	}
	freeCam::freeCam(entRef const& ent, manager& man, void* usr) : element(ent, man, usr, typeid(freeCam)) {
	}
}