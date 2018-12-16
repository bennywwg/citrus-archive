#pragma once

#include <engine/element.h>
#include <dynamics/physics/world.h>

namespace citrus {
	namespace engine {
		class worldManager : public element {
			public:

			dynamics::world* w;

			void preRender();

			worldManager(entityRef e) : element(e, typeid(worldManager)), w(new dynamics::world()) {
			}
		};
	}
}
