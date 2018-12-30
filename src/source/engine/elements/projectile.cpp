#include <engine/elements/projectile.h>

namespace citrus::engine {
	void projectile::preRender() {
		if(eng()->time() - startTime >= maxTime) {
			eng()->man->destroy(ent());
		}
		ent().setLocalPosition(ent().getLocalPosition() + float(eng()->dt()) * velocity);
	}
	projectile::projectile(entityRef ent) : element(ent, typeid(projectile)) { }
}