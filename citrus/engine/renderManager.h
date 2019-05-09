#pragma once

#include "citrus/engine/freeCam.h"
#include "citrus/graphics/tilemapFont.h"
#include "citrus/engine/meshFilter.h"
#include "citrus/util.h"
#include "citrus/graphics/instance.h"
#include "citrus/graphics/image.h"

#include "citrus/graphics/model.h"
#include "citrus/graphics/renderSystem.h"

namespace citrus::engine {
	class renderManager : public element {
		std::mutex _drawableMut;

		uint32_t _currentFrame;
		
		graphics::system* sys;
		VkSemaphore mainSem;
	public:
		eleRef<freeCam> camRef;
		
		void addItem(eleRef<meshFilter> mf, int modelIndex, int textureIndex);
		void removeItem(eleRef<meshFilter> mf, int modelIndex);

		void bindAllAvailableAnimations();

		void initSystem(string vs, string fs, vector<string> textures, vector<string> models, vector<string> animations);

		void load(const nlohmann::json& parsed);
		json save() const;
		void onCreate();
		void render();
		void onDestroy();

		renderManager(entityRef ent);
		~renderManager();
	};
}