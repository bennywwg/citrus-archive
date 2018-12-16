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
		dynamics::world *w;
		std::unique_ptr<dynamics::collisionShape> shape;
		std::unique_ptr<dynamics::rigidBody> body;

		void preRender() {
			ent.setLocalTransform(body->getTransform());
		}

		rigidBodyComponent(entityRef owner) : element(owner, typeid(rigidBodyComponent)) {
			w = (e->getAllOfType<worldManager>()[0]->w);
			shape.reset(new dynamics::collisionShape(std::vector<glm::vec3>{
				glm::vec3(-1.0f, -1.0f, -1.0f),
					glm::vec3(1.0f, -1.0f, -1.0f),
					glm::vec3(1.0f, -1.0f, 1.0f),
					glm::vec3(-1.0f, -1.0f, 1.0f),

					glm::vec3(-1.0f, 1.0f, -1.0f),
					glm::vec3(1.0f, 1.0f, -1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(-1.0f, 1.0f, 1.0f)
			}));
			body.reset(new dynamics::rigidBody(shape.get(), w));
		}
	};
}

#endif
