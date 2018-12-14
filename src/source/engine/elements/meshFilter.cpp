#include <engine/elements/meshFilter.h>

#include <engine/elements/renderManager.h>

#include <engine/manager.inl>

#include <engine/elementRef.inl>

namespace citrus::engine {

	void meshFilter::setState(int m, int t, int s) {
		_model = m;
		_tex = t;
		_sh = s;
	}
	int meshFilter::model() {
		return _model;
	}
	int meshFilter::tex() {
		return _tex;
	}
	int meshFilter::sh() {
		return _sh;
	}

	void meshFilter::load(const json& js) {
		auto g = e->getAllOfType<renderManager>()[0];
		g->addDrawable(eleRef<meshFilter>(this), _model);
	}

	meshFilter::meshFilter(entityRef ent) : element(ent, typeid(meshFilter)) {
	}
}