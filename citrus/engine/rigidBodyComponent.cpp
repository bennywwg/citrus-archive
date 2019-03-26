#include "citrus/engine/rigidBodyComponent.h"
#include "citrus/engine/engine.h"
#include "citrus/engine/manager.inl"
#include "citrus/engine/elementRef.inl"
#include "citrus/engine/worldManager.h"

namespace citrus::engine {
	bool rigidBodyComponent::isSphere() {
		return shape->ptr()->getShapeType() == SPHERE_SHAPE_PROXYTYPE;
	}

	bool rigidBodyComponent::isBox() {
		return shape->ptr()->getShapeType() == BOX_SHAPE_PROXYTYPE;
	}

	void rigidBodyComponent::setToSphere(float radius) {
		type = sphere;
		body.reset();
		shape.reset(new dynamics::collisionShape(radius));
		body.reset(new dynamics::rigidBody(shape.get(), w));
	}

	void rigidBodyComponent::setToBox(glm::vec3 boxSize) {
		type = box;
		body.reset();
		shape.reset(new dynamics::collisionShape(boxSize.x, boxSize.y, boxSize.z));
		body.reset(new dynamics::rigidBody(shape.get(), w));
	}

	void rigidBodyComponent::setToHull() {
		throw std::runtime_error("Not implemented");
	}

	void rigidBodyComponent::setPosition(const glm::vec3 & pos) {
		body->setPosition(pos);
		ent().setLocalPosition(pos);
	}

	void rigidBodyComponent::setOrientation(const glm::quat & ori) {
		body->setOrientation(ori);
		ent().setLocalOrientation(ori);
	}

	void rigidBodyComponent::setTransform(const transform & tr) {
		body->setTransform(tr);
		ent().setLocalTransform(tr);
	}

	glm::vec3 rigidBodyComponent::getPosition() {
		return body->getPosition();
	}

	glm::quat rigidBodyComponent::getOrientation() {
		return body->getOrientation();
	}

	transform rigidBodyComponent::getTransform() {
		return body->getTransform();
	}

	void rigidBodyComponent::setDynamic(bool d) {
		body->dynamic = d;
	}

	rigidBodyComponent::rigidBodyComponent(entityRef owner) : element(owner, typeid(rigidBodyComponent)) {
		w = eng()->getAllOfType<worldManager>()[0]->w;
	}
}