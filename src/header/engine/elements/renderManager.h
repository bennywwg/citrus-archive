#pragma once

#ifndef RENDERMANAGERCOMPONENT_H
#define RENDERMANAGERCOMPONENT_H

#include <engine/elements/freeCam.h>

#include <graphics/framebuffer/standaloneFramebuffer.h>
#include <graphics/geometry/vertexarray.h>
#include <highlevel/tilemapFont.h>
#include <engine/elements/meshFilter.h>


namespace citrus {
	namespace engine {
		class renderManager : public element {
			std::mutex _drawableMut;

		public:
			tilemapFont font;

			std::unique_ptr<graphics::simpleFrameBuffer> meshFBO;
			std::unique_ptr<graphics::simpleFrameBuffer> textFBO;
			std::unique_ptr<graphics::shader> passthrough;
			std::unique_ptr<graphics::shader> composite;
			std::unique_ptr<graphics::shader> bones;
			std::unique_ptr<graphics::shader> transSh;
			
			struct shaderInfo {
				std::unique_ptr<graphics::shader> sh;
				std::vector<eleRef<meshFilter>> eles;
			};
			std::vector<shaderInfo> drawable;
			void addDrawable(eleRef<meshFilter> me, int model);

			eleRef<freeCam> camRef;
			public:

			void resizeBuffers(unsigned int width, unsigned int height);

			void load(const nlohmann::json& parsed);
			nlohmann::json save() const;
			void onCreate();
			void render();
			void onDestroy();

			renderManager(entityRef ent);
		};
	}
}

#endif