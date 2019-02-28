#pragma once

#ifndef FREECAMCOMPONENT_H
#define FREECAMCOMPONENT_H

#include <citrus/graphics/camera.h>
#include <citrus/util.h>
#include <citrus/engine/element.h>

namespace citrus {
	namespace engine {
		class freeCam : public element {
			public:
			camera cam;

			void preRender();
			void render();

			freeCam(entityRef ent);
		};
	}
}

#endif
