#pragma once

#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H

#include <engine/element.h>
#include <dynamics/physics/rigidBody.h>

namespace citrus {
	namespace engine {
		class rigidBodyComponent : public element {
			public:
			std::unique_ptr<dynamics::rigidBody> body;

			void load(const nlohmann::json& parsed) {

			}
			void preRender() {
				ent->trans = body->getTransform();
			}

			rigidBodyComponent(entity* owner) : element(owner) { }
		};
	}
}

#endif