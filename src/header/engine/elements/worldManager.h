#pragma once

#ifndef WORLDMANAGER_H
#define WORLDMANAGER_H

#include <engine/element.h>
#include <dynamics/physics/world.h>

namespace citrus {
	namespace engine {
		class worldManager : public element {
			public:

			dynamics::world w;

			void load(const nlohmann::json& parsed) {
				//parsed[.get<int>()
			}
			void preRender() {
				w.flushAddRemove();
				w.step();
				w.updateBodyInfo();
			}

			worldManager(entity* e) : element(e), w() {
				
			}
		};
	}
}

#endif