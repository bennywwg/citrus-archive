#pragma once

#include "../mankern/entityRef.h"
#include "../graphkern/window.h"

namespace citrus {
	class gameState {
	public:
		int frameNum;
		float dt;
		window* win;

		entRef playerEnt;
		entRef playerNavBody;
	};
}