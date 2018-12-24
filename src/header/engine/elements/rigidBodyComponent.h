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
			hull
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

		void preRender() {
			ent.setLocalTransform(body->getTransform());
			/*util::sout(util::toString(ent.getLocalPosition()) + "\n");*/
		}

		void load(const json&) {
			if(type == sphere) {

			}
		}

		rigidBodyComponent(entityRef owner) : element(owner, typeid(rigidBodyComponent)) {
			w = (e->getAllOfType<worldManager>()[0]->w);
			shape.reset(new dynamics::collisionShape(1.0f));
			body.reset(new dynamics::rigidBody(shape.get(), w));
		}
	};
}

#endif
