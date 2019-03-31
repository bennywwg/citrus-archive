#pragma once

#ifndef RENDERMANAGERCOMPONENT_H
#define RENDERMANAGERCOMPONENT_H

#include "citrus/engine/freeCam.h"
#include "citrus/graphics/tilemapFont.h"
#include "citrus/engine/meshFilter.h"
#include "citrus/util.h"
#include "citrus/graphics/instance.h"

#include "citrus/graphics/model.h"
#include "citrus/graphics/vkShader.h"

namespace citrus::engine {
	class renderManager : public element {
		std::mutex _drawableMut;
		
		struct modelData {
			graphics::model* _model = nullptr;
			graphics::mesh* _me = nullptr;
		};

		struct modelSet {
			uint64_t modelIndex;
			uint64_t nonEmptyCount;
			vector<eleRef<meshFilter>> _eles;
		};

		struct shaderList {
			graphics::dynamicOffsetMeshShader* _sh = nullptr;
			vector<modelSet> _sets;
		};
		
		vector<shaderList> _shaderTypes;
		vector<modelData> _meshTypes;
		vector<graphics::animation*> _animations;

		vector<graphics::ctTexture> _frames;
		vector<VkFence> _fences;
		VkSemaphore _renderDoneSem;
		uint32_t _currentFrame;
	public:

		eleRef<freeCam> camRef;
		
		void addItem(eleRef<meshFilter> mf, int modelIndex, int textureIndex, int shaderIndex);
		void removeItem(eleRef<meshFilter> mf, int modelIndex, int textureIndex, int shaderIndex);

		void loadShader(string vertLoc, string fragLoc, int index);
		void loadAnimation(string loc, int index);
		void loadMesh(string loc, int index);
		void bindAllAvailableAnimations();

		void load(const nlohmann::json& parsed);
		json save() const;
		void onCreate();
		void render();
		void onDestroy();

		renderManager(entityRef ent);
		~renderManager();
	};
}

#endif
