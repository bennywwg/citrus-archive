

namespace citrus {
	bool rigidBodyComponent::isSphere() {
		return shape->ptr()->getShapeType() == SPHERE_SHAPE_PROXYTYPE;
	}

	bool rigidBodyComponent::isBox() {
		return shape->ptr()->getShapeType() == BOX_SHAPE_PROXYTYPE;
	}

	void rigidBodyComponent::setToSphere(float radius) {
		type = shapeType::sphere;
		body.reset();
		shape.reset(new dynamics::collisionShape(radius));
		body.reset(new dynamics::rigidBody(shape.get(), w));
	}

	void rigidBodyComponent::setToBox(glm::vec3 boxSize) {
		type = shapeType::box;
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

	void rigidBodyComponent::load(citrus::json const& data) {
		if (data.empty()) return;
		auto type = (shapeType)data["type"].get<int>();
		switch (type) {
		case citrus::engine::rigidBodyComponent::shapeType::sphere:
			setToSphere(data["radius"]);
			break;
		case citrus::engine::rigidBodyComponent::shapeType::box:
			setToBox(util::loadVec3(data["dims"]));
			break;
		default:
			break;
		}
		if (body) {
			body->dynamic = data["dynamic"];
			body->mass = data["mass"];
		}
	}

	citrus::json rigidBodyComponent::save() {
		json res;
		res["type"] = (int)type;
		res["dynamic"] = body->dynamic;
		res["mass"] = body->mass;
		if (type == shapeType::sphere) {
			res["radius"] = (float)((btSphereShape*)shape->ptr())->getRadius();
		} else if(type == shapeType::box) {
			auto vec = util::btToGlm(((btBoxShape*)shape->ptr())->getHalfExtentsWithoutMargin());
			res["dims"] = util::save(vec);
		}
		return res;
	}

	string rigidBodyComponent::name() const {
		return "Rigid Body";
	}

	rigidBodyComponent::rigidBodyComponent(entityRef owner) : element(owner, typeid(rigidBodyComponent)) {
		w = eng()->getAllOfType<worldManager>()[0]->w;
	}
}