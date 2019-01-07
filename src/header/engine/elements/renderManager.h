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
			std::unique_ptr<graphics::shader> rectshader;
			
			std::vector<graphics::texture3b*> textures;

			struct shaderInfo {
				struct textureInfo {
					std::vector<eleRef<meshFilter>> eles;
				};
				std::unique_ptr<graphics::shader> sh;
				std::vector<textureInfo> groups;
			};
			std::vector<shaderInfo> drawable;
			void addDrawable(eleRef<meshFilter> me, int m, int t, int s);
			void removeDrawable(eleRef<meshFilter> me, int m, int t, int s);

			graphics::texture3b& getTexture(int index);

			void loadPNG(std::string loc, int index);

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