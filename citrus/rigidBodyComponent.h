#pragma once

#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H

#include "entityComponent.h"
#include "entity.h"
#include "transformComponent.h"

#include "rigidBody.h"

namespace citrus {
	namespace engine {
		class rigidBodyComponent : public entityComponent {
			public:
			std::weak_ptr<dynamics::rigidBody> body;

			void postPhysics() {
				if(getOwner()->hasComponent<transformComponent>()) {
					getOwner()->getComponent<transformComponent>().lock()->setTransform(body.lock()->getTransform());
				}
			}

			rigidBodyComponent(engine* engine, entity* owner) : entityComponent(engine, owner) { }
		};
	}
}

#endif // ! RIGIDBODYCOMPONENT_H
