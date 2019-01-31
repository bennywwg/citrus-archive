#include <engine/elements/meshFilter.h>

#include <engine/elements/renderManager.h>
#include <engine/elements/meshManager.h>

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
	std::unique_ptr<editor::gui> meshFilter::renderGUI() {
		editor::container* c = new	editor::container();
		c->title = "Mesh Filter";

		{
			editor::button* b = new editor::button();
			b->info = "Mesh: " + ((_model == -1) ? string("(None)") : eng()->getAllOfType<meshManager>()[0]->getMesh(_model).name);
			c->items.emplace_back(b);
		}

		return std::unique_ptr<editor::gui>(c);
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