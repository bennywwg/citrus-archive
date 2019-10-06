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
		p.items[foundSlot].normalTexIndex = 0;
		p.items[foundSlot].pos = ent().getGlobalTransform().getPosition();
		p.items[foundSlot].ori = ent().getGlobalTransform().getOrientation();

		this->itemIndex = foundSlot;
	}
	void meshFilter::setNormalMap(int texIndex) {
		if (materialIndex != -1) eng()->sys->meshPasses[materialIndex]->items[itemIndex].normalTexIndex = texIndex;
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
	float meshFilter::aniTime() const {
		return eng()->time() - _aniStart;
	}

	void meshFilter::startAnimation(int ani, graphics::behavior mode) {
		_aniStart = eng()->time();
		_mode = mode;
		eng()->sys->meshPasses[materialIndex]->items[itemIndex].animationIndex = ani;
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
			graphics::meshPass::itemInfo& inf = eng()->sys->meshPasses[materialIndex]->items[itemIndex];
			inf.pos		= ent().getGlobalTransform().getPosition();
			inf.ori		= ent().getGlobalTransform().getOrientation();
			inf.aniTime = float(eng()->time() - _aniStart);
		}
	}
	void meshFilter::load(const citrus::json& js) {
		reset();
		if (!js.empty()) {
			setState(js[0], js[1], js[2]);
			setNormalMap(js[3]);
		}
	}
	citrus::json meshFilter::save() {
		if (materialIndex != -1) {
			graphics::meshPass& p = *eng()->sys->meshPasses[materialIndex];
			return {
				materialIndex,
				p.items[itemIndex].modelIndex,
				p.items[itemIndex].texIndex,
				p.items[itemIndex].normalTexIndex
			};
		} else {
			return json();
		}
	}

	string meshFilter::name() const {
		return "Mesh Filter";
	}

	meshFilter::meshFilter(entityRef ent) : element(ent, typeid(meshFilter)) {
	}
	meshFilter::~meshFilter() {
		reset();
	}
}