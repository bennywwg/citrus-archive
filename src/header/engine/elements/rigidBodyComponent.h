#pragma once

#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H

#include <engine/element.h>
#include <dynamics/physics/rigidBody.h>
#include <dynamics/physics/collisionShape.h>

namespace citrus {
	namespace engine {
		class rigidBodyComponent : public element {
			public:
			dynamics::world *w;
			std::unique_ptr<dynamics::collisionShape> shape;
			std::unique_ptr<dynamics::rigidBody> body;

			void load(const nlohmann::json& parsed) {
				w = &e->man->dereferenceElement<worldManager>(parsed["world"])->w;
				shape.reset(new dynamics::collisionShape(std::vector<glm::vec3>{
					glm::vec3(-1.0f, -1.0f, -1.0f),
						glm::vec3(1.0f, -1.0f, -1.0f),
						glm::vec3(1.0f, -1.0f, 1.0f),
						glm::vec3(-1.0f, -1.0f, 1.0f),

						glm::vec3(-1.0f, 1.0f, -1.0f),
						glm::vec3(1.0f, 1.0f, -1.0f),
						glm::vec3(1.0f, 1.0f, 1.0f),
						glm::vec3(-1.0f, 1.0f, 1.0f),
				}));
				body.reset(new dynamics::rigidBody(shape.get(), w));
			}
			void preRender() {
				ent->getGlobalTransform() = body->getTransform();
			}

			rigidBodyComponent(entityRef owner) : element(owner) { }
		};
	}
}

#endif
