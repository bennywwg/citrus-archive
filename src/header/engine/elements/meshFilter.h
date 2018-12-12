#pragma once

#include <engine/element.h>
#include <graphics/geometry/vertexarray.h>
#include <graphics/texture/colorTexture.h>
#include <graphics/shader/shader.h>
#include <engine/engine.h>

namespace citrus::engine {
	class meshFilter : public element {
		graphics::vertexArray*		model;
		graphics::texture3b*		tex;
		graphics::shader*			sh;

	public:
		void setMeshByName(std::string name);
		void setTextureByName(std::string name);
		void setShaderByName(std::string name);

		void render();

		public:
		meshFilter(entityRef ent);
	};
}