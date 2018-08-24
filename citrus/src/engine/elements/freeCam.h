#pragma once

#ifndef FREECAMCOMPONENT_H
#define FREECAMCOMPONENT_H

#include <graphics/camera/camera.h>
#include <util/util.h>
#include <engine/element.h>

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