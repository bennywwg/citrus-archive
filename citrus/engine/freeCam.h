#pragma once

#include "citrus/graphics/camera.h"
#include "citrus/util.h"
#include "citrus/engine/element.h"

namespace citrus {
	namespace engine {
		class freeCam : public element {
			public:
			graphics::camera cam;

			void preRender();
			void render();

			string name() const;

			freeCam(entityRef ent);
		};
	}
}