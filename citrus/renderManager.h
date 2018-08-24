#pragma once

#ifndef RENDERMANAGERCOMPONENT_H
#define RENDERMANAGERCOMPONENT_H

#include "element.h"

#include "stdUtil.h"
#include "camera.h"
#include "shader.h"
#include "vertexarray.h"
#include "tilemapFont.h"
#include "standaloneFramebuffer.h"

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

			tilemapFont font;
			std::unique_ptr<graphics::simpleFrameBuffer> standardFBO;
			std::unique_ptr<graphics::simpleFrameBuffer> textFBO;
			std::unique_ptr<graphics::shader> passthrough;
			std::unique_ptr<graphics::shader> composite;

			vector<shared_ptr<shaderInfo>> _shaders;
			mutex _shadersMut;
			public:
			camera cam;

			weak_ptr<shaderInfo> loadShader(string shaderName, string vertFile, string geomFile, string fragFile);
			weak_ptr<shaderInfo> getShader(string name);
			void flushShaders();

			void resizeBuffer(unsigned int width, unsigned int height);

			void onCreate();
			void render();
			void onDestroy();

			renderManager(entity* ent);
		};
	}
}

#endif