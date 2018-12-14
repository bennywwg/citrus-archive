#pragma once

#include <engine/element.h>
#include <graphics/geometry/vertexarray.h>
#include <graphics/texture/colorTexture.h>
#include <graphics/shader/shader.h>
#include <engine/engine.h>

namespace citrus::engine {
	class meshFilter : public element {
	private:
		int _model = -1;
		int _tex = -1;
		int _sh = -1;

	public:
		void setState(int m, int t, int s);
		int model();
		int tex();
		int sh();

		void load(const json&);

		meshFilter(entityRef ent);
	};
}