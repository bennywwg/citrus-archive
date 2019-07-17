#include "citrus/engine/meshFilter.h"

#include "citrus/engine/manager.inl"

#include "citrus/engine/elementRef.inl"

namespace citrus::engine {
	void meshFilter::setState(int matIndex, int modelIndex, int texIndex) {
		reset();

		this->materialIndex = matIndex;
		
		graphics::meshPass& p = *eng()->sys->meshPasses[materialIndex];
		int foundSlot = -1;
		for (int i = 0; i < p.items.size(); i++) {
			if (!p.items[i].enabled) {
				foundSlot = i;
				break;
			}
		}
		if (foundSlot == -1) {
			foundSlot = p.items.size();
			p.items.push_back({ });
		}
		p.items[foundSlot].enabled = true;
		p.items[foundSlot].modelIndex = modelIndex;
		p.items[foundSlot].texIndex = texIndex;
		p.items[foundSlot].pos = ent().getGlobalTransform().getPosition();
		p.items[foundSlot].ori = ent().getGlobalTransform().getOrientation();

		this->itemIndex = foundSlot;
	}
	void meshFilter::reset() {
		if (materialIndex != -1) {
			graphics::meshPass& p = *eng()->sys->meshPasses[materialIndex];
			p.items[itemIndex].enabled = false;
			materialIndex = -1;
		}
	}
	int meshFilter::model() const {
		return (materialIndex == -1) ? -1 : eng()->sys->meshPasses[materialIndex]->items[itemIndex].modelIndex;
	}
	int meshFilter::tex() const {
		return (materialIndex == -1) ? -1 : eng()->sys->meshPasses[materialIndex]->items[itemIndex].texIndex;
	}
	int meshFilter::ani() const {
		return 0;
	}
	graphics::behavior meshFilter::mode() const {
		return _mode;
	}
	double meshFilter::aniTime() const {
		return eng()->time() - _aniStart;
	}

	void meshFilter::startAnimation(int ani, graphics::behavior mode) {
		_aniStart = eng()->time();
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
	void meshFilter::preRender() {
		if (materialIndex != -1) {
			eng()->sys->meshPasses[materialIndex]->items[itemIndex].pos = ent().getGlobalTransform().getPosition();
			eng()->sys->meshPasses[materialIndex]->items[itemIndex].ori = ent().getGlobalTransform().getOrientation();
		}
	}
	void meshFilter::load(const json& js) {
	}

	meshFilter::meshFilter(entityRef ent) : element(ent, typeid(meshFilter)) {
	}
	meshFilter::~meshFilter() {
		reset();
	}
}