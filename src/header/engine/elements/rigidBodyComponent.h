#pragma once

#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H

#include <engine/element.h>
#include <dynamics/physics/rigidBody.h>
#include <dynamics/physics/collisionShape.h>
#include <engine/engine.h>

namespace citrus::engine {
	class rigidBodyComponent : public element {

		public:
		enum shapeType {
			sphere,
			box,
			hull,
			none
		};
		shapeType type;

		dynamics::world *w;
		std::unique_ptr<dynamics::collisionShape> shape;
		std::unique_ptr<dynamics::rigidBody> body;

		bool isSphere() {
			return shape->ptr()->getShapeType() == SPHERE_SHAPE_PROXYTYPE;
		}
		bool isBox() {
			return shape->ptr()->getShapeType() == BOX_SHAPE_PROXYTYPE;
		}

		void setToSphere(float radius) {
			type = sphere;
			body.reset();
			shape.reset(new dynamics::collisionShape(radius));
			body.reset(new dynamics::rigidBody(shape.get(), w));
		}
		void setToBox(glm::vec3 boxSize) {
			type = box;
			body.reset();
			shape.reset(new dynamics::collisionShape(boxSize.x, boxSize.y, boxSize.z));
			body.reset(new dynamics::rigidBody(shape.get(), w));
		}
		void setToHull() {
			throw std::runtime_error("Not implemented");
		}

		void setPosition(const glm::vec3& pos) {
			body->setPosition(pos);
			ent.setLocalPosition(pos);
		}
		void setOrientation(const glm::quat& ori) {
			body->setOrientation(ori);
			ent.setLocalOrientation(ori);
		}
		void setTransform(const transform& tr) {
			body->setTransform(tr);
			ent.setLocalTransform(tr);
		}
		glm::vec3 getPosition() {
			return body->getPosition();
		}
		glm::quat getOrientation() {
			return body->getOrientation();
		}
		transform getTransform() {
			return body->getTransform();
		}

		void setDynamic(bool d) {
			body->dynamic = d;
		}

		rigidBodyComponent(entityRef owner) : element(owner, typeid(rigidBodyComponent)) {
			w = (e->getAllOfType<worldManager>()[0]->w);
		}
	};
}

#endif
