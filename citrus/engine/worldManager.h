#pragma once

#include "citrus/engine/element.h"
#include "citrus/dynamics/world.h"

namespace citrus {
	namespace engine {
		class worldManager : public element {
			public:

			//graphics::simpleFrameBuffer* fbo = nullptr;
			//std::unique_ptr<graphics::shader> debugShader;

			dynamics::world* w;

			//void enableDebugDraw();
			//void disableDebugDraw();

			void preRender();
			void render();

			string name() const;

			worldManager(entityRef e);
		};
	}
}
