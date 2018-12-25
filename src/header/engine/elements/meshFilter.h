#pragma once

#include <engine/element.h>
#include <graphics/geometry/vertexarray.h>
#include <graphics/texture/colorTexture.h>
#include <graphics/shader/shader.h>
#include <engine/engine.h>

#include <graphics/geometry/animesh.h>

namespace citrus::engine {
	class meshFilter : public element {
	private:
		int _model = -1;
		int _tex = -1;
		int _sh = -1;
		int _ani = -1;
		geom::behavior _mode;
		double _aniStart = 0.0;

	public:
		void setState(int m, int t, int s, int a = -1);
		int model() const;
		int tex() const;
		int sh() const;
		int ani() const;
		double aniStart() const;
		void startAnimation(int ani, geom::behavior mode = geom::nearest);

		void load(const json&);

		meshFilter(entityRef ent);
	};
}