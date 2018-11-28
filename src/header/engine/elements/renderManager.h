#pragma once

#ifndef RENDERMANAGERCOMPONENT_H
#define RENDERMANAGERCOMPONENT_H

#include <engine/elements/freeCam.h>

#include <graphics/framebuffer/standaloneFramebuffer.h>
#include <graphics/geometry/vertexarray.h>
#include <highlevel/tilemapFont.h>

namespace citrus {
	namespace engine {
		class renderManager : public element {
			struct shaderInfo {
				string name = "";
				string vertFile = "";
				string geomFile = "";
				string fragFile = "";
				graphics::shader* sh = nullptr;
				inline ~shaderInfo() { if(sh != nullptr) delete sh; }
			};
			shared_ptr<shaderInfo> _invalid;

		public:

			std::string text;

			tilemapFont font;
			std::unique_ptr<graphics::simpleFrameBuffer> standardFBO;
			std::unique_ptr<graphics::simpleFrameBuffer> textFBO;
			std::unique_ptr<graphics::shader> passthrough;
			std::unique_ptr<graphics::shader> composite;

			vector<shared_ptr<shaderInfo>> _shaders;
			mutex _shadersMut;
			eleRef<freeCam> camRef;
			public:

			weak_ptr<shaderInfo> loadShader(string shaderName, string vertFile, string geomFile, string fragFile);
			weak_ptr<shaderInfo> getShader(string name);
			void flushShaders();

			void resizeBuffer(unsigned int width, unsigned int height);

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