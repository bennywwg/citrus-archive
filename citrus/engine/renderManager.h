#pragma once

#ifndef RENDERMANAGERCOMPONENT_H
#define RENDERMANAGERCOMPONENT_H

#include <citrus/engine/freeCam.h>
#include <citrus/graphics/tilemapFont.h>
#include <citrus/engine/meshFilter.h>
#include <citrus/util.h>


namespace citrus {
	namespace engine {
		class renderManager : public element {
			std::mutex _drawableMut;

		public:
			tilemapFont font;

			eleRef<freeCam> camRef;
			

			void resizeBuffers(unsigned int width, unsigned int height);

			void load(const nlohmann::json& parsed);
			json save() const;
			void onCreate();
			void render();
			void onDestroy();

			renderManager(entityRef ent);
		};
	}
}

#endif
