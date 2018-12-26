#include <engine/elements/meshFilter.h>

#include <engine/elements/renderManager.h>

#include <engine/manager.inl>

#include <engine/elementRef.inl>

namespace citrus::engine {
	void meshFilter::setState(int m, int t, int s, int a) {
		_model = m;
		_tex = t;
		_sh = s;
		_ani = a;
		if(a != -1) _mode = geom::repeat;
	}
	int meshFilter::model() const {
		return _model;
	}
	int meshFilter::tex() const {
		return _tex;
	}
	int meshFilter::sh() const {
		return _sh;
	}
	int meshFilter::ani() const {
		return _ani;
	}
	geom::behavior meshFilter::mode() const {
		return _mode;
	}
	double meshFilter::aniTime() const {
		return e->time() - _aniStart;
	}

	void meshFilter::startAnimation(int ani, geom::behavior mode) {
		_aniStart = e->time();
		_ani = ani;
		_mode = mode;
	}
	void meshFilter::load(const json& js) {
		auto g = e->getAllOfType<renderManager>()[0];
		g->addDrawable(eleRef<meshFilter>(this), _model, _tex, _sh);
	}

	meshFilter::meshFilter(entityRef ent) : element(ent, typeid(meshFilter)) {
	}
}