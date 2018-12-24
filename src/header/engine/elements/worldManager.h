#pragma once

#include <engine/element.h>
#include <dynamics/physics/world.h>
#include <graphics/framebuffer/standaloneFramebuffer.h>
#include <graphics/shader/shader.h>

namespace citrus {
	namespace engine {
		class worldManager : public element {
			public:

			graphics::simpleFrameBuffer* fbo = nullptr;
			std::unique_ptr<graphics::shader> debugShader;

			dynamics::world* w;

			void enableDebugDraw();
			void disableDebugDraw();

			void preRender();
			void render();

			worldManager(entityRef e) : element(e, typeid(worldManager)), w(new dynamics::world()) {
			}
		};
	}
}
