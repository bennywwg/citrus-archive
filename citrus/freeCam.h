#pragma once

#ifndef FREECAMCOMPONENT_H
#define FREECAMCOMPONENT_H

#include "engine.h"
#include "camera.h"
#include "util.h"
#include "element.h"

namespace citrus {
	namespace engine {
		class freeCam : public element {
			public:
			camera cam;

			void preRender();
			void render();

			freeCam(entity* ent);
		};
	}
}

#endif