#include <citrus/engine/projectile.h>
#include <citrus/engine/sensorEle.h>
#include <citrus/engine/entityRef.inl>
#include <citrus/engine/elementRef.inl>

namespace citrus::engine {
	void projectile::preRender() {
		if(eng()->time() - startTime >= maxTime) {
			eng()->man->destroy(ent());
		}
		ent().setLocalPosition(ent().getLocalPosition() + float(eng()->dt()) * velocity);
		if (ent().getElement<sensorEle>()->touchingAny())
			eng()->man->destroy(ent());
	}
	projectile::projectile(entityRef ent) : element(ent, typeid(projectile)) { }
}