#include <engine/elements/meshFilter.h>

#include <engine/elements/renderManager.h>

#include <engine/manager.inl>

#include <engine/elementRef.inl>

namespace citrus::engine {
	void meshFilter::setState(int m, int t, int s, int a) {
		auto g = eng()->getAllOfType<renderManager>()[0];
		if(s != -1) {
			g->addDrawable(eleRef<meshFilter>(this), m, t, s);
		} else {
			g->removeDrawable(eleRef<meshFilter>(this), _model, _tex, _sh);
		}
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
		return eng()->time() - _aniStart;
	}

	void meshFilter::startAnimation(int ani, geom::behavior mode) {
		_aniStart = eng()->time();
		_ani = ani;
		_mode = mode;
	}
	void meshFilter::load(const json& js) {
	}

	meshFilter::meshFilter(entityRef ent) : element(ent, typeid(meshFilter)) {
	}
	meshFilter::~meshFilter() {
		if(_sh != -1) {
			auto g = eng()->getAllOfType<renderManager>()[0];
			g->removeDrawable(eleRef<meshFilter>(this), _model, _tex, _sh);
		}
	}
}