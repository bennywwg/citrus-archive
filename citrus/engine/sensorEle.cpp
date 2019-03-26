#include "citrus/engine/sensorEle.h"

#include "citrus/engine/engine.h"
#include "citrus/engine/manager.inl"
#include "citrus/engine/elementRef.inl"
#include "citrus/engine/worldManager.h"

#include <BulletCollision/CollisionDispatch/btGhostObject.h>

namespace citrus::engine {

	bool sensorEle::touchingAny() const {
		return sense->touchingAny();
	}
	bool sensorEle::isSphere() {
		return true;
	}
	bool sensorEle::isBox() {
		return false;
	}
	void sensorEle::setToSphere(float radius) {
		
	}
	void sensorEle::setToBox(glm::vec3 boxSize) {
	}
	void sensorEle::setToHull() {
		throw std::runtime_error("setToHull() not implemented");
	}
	void sensorEle::preRender() {
		if (!sense) {
			const auto& wms = eng()->getAllOfType<worldManager>();
			if (!wms.empty()) sense.reset(new dynamics::sensor(new dynamics::collisionShape(0.1f), wms[0]->w));
		} else {
			transform tr = ent().getGlobalTransform();
			tr.setScale(vec3(1, 1, 1));
			sense->setTransform(tr);
		}
	}
	sensorEle::sensorEle(entityRef owner) : element(owner, typeid(sensorEle)) {
		const auto& wms = eng()->getAllOfType<worldManager>();
		if (!wms.empty()) sense.reset(new dynamics::sensor(new dynamics::collisionShape(0.1f), wms[0]->w));
	}
	sensorEle::~sensorEle() {
		delete sense->ptr()->getCollisionShape();
	}
}