#pragma once

#ifndef RENDERMANAGERCOMPONENT_H
#define RENDERMANAGERCOMPONENT_H

#include <citrus/engine/freeCam.h>
#include <citrus/graphics/tilemapFont.h>
#include <citrus/engine/meshFilter.h>
#include <citrus/util.h>

#include <citrus/graphics/model.h>


namespace citrus::engine {
	class renderManager : public element {
		std::mutex _drawableMut;
		
		struct modelList {
			graphics::model* _model;
			vector<eleRef<meshFilter>> _eles;
		};
		
		vector<modelList> _items;
		
	public:
		eleRef<freeCam> camRef;
		
		void addItem(eleRef<meshFilter> mf, int modelIndex, int textureIndex, int shaderIndex);
		void removeItem(eleRef<meshFilter> mf, int modelIndex, int textureIndex, int shaderIndex);

		void loadMesh(string loc, int index);

		void load(const nlohmann::json& parsed);
		json save() const;
		void onCreate();
		void render();
		void onDestroy();

		renderManager(entityRef ent);
	};
}

#endif
