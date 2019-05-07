#include "citrus/engine/meshFilter.h"

#include "citrus/engine/renderManager.h"

#include "citrus/engine/manager.inl"

#include "citrus/engine/elementRef.inl"

namespace citrus::engine {
	void meshFilter::setState(int m, int t, int a) {
		auto g = eng()->getAllOfType<renderManager>()[0];
		if(t != -1) {
			g->addItem(eleRef<meshFilter>(this), m, t);
		} else {
			g->removeItem(eleRef<meshFilter>(this), _model);
		}
		_model = m;
		_tex = t;
		_ani = a;
		if(a != -1) _mode = graphics::behavior::repeat;
		util::sout("setState\n");
	}
	int meshFilter::model() const {
		return _model;
	}
	int meshFilter::tex() const {
		return _tex;
	}
	int meshFilter::ani() const {
		return _ani;
	}
	graphics::behavior meshFilter::mode() const {
		return _mode;
	}
	double meshFilter::aniTime() const {
		return eng()->time() - _aniStart;
	}

	void meshFilter::startAnimation(int ani, graphics::behavior mode) {
		_aniStart = eng()->time();
		_ani = ani;
		_mode = mode;
	}
	std::unique_ptr<editor::gui> meshFilter::renderGUI() {
		editor::container* c = new	editor::container();
		c->title = "Mesh Filter";

		{
			editor::button* b = new editor::button();
			b->info = "Broken";
			//b->info = "Mesh: " + ((_model == -1) ? string("(None)") : eng()->getAllOfType<meshManager>()[0]->getMesh(_model).name);
			c->items.emplace_back(b);
		}

		return std::unique_ptr<editor::gui>(c);
	}
	void meshFilter::load(const json& js) {
	}

	meshFilter::meshFilter(entityRef ent) : element(ent, typeid(meshFilter)) {
	}
	meshFilter::~meshFilter() {
		if(_tex != -1) {
			auto g = eng()->getAllOfType<renderManager>()[0];
			g->removeItem(eleRef<meshFilter>(this), _model);
		}
	}
}