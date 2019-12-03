#include "../mankern/entityRef.h"
#include "modelEle.h"
#include "../mankern/manager.h"

namespace citrus {
	void modelEle::setState(int matIndex, int modelIndex, int texIndex) {
		reset();

		this->materialIndex = matIndex;

		meshPass& p = *sys.meshPasses[materialIndex];
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
		p.items[foundSlot].pos = ent().getGlobalTrans().getPosition();
		p.items[foundSlot].ori = ent().getGlobalTrans().getOrientation();

		this->itemIndex = foundSlot;
	}
	void modelEle::setNormalMap(int texIndex) {
		if (materialIndex != -1) sys.meshPasses[materialIndex]->items[itemIndex].normalTexIndex = texIndex;
	}
	void modelEle::reset() {
		if (materialIndex != -1) {
			meshPass& p = *sys.meshPasses[materialIndex];
			p.items[itemIndex].enabled = false;
			materialIndex = -1;
		}
	}
	int modelEle::model() const {
		return (materialIndex == -1) ? -1 : sys.meshPasses[materialIndex]->items[itemIndex].modelIndex;
	}
	int modelEle::tex() const {
		return (materialIndex == -1) ? -1 : sys.meshPasses[materialIndex]->items[itemIndex].texIndex;
	}
	int modelEle::ani() const {
		return 0;
	}
	behavior modelEle::mode() const {
		return _mode;
	}
	float modelEle::aniTime() const {
		return man().time() - _aniStart;
	}

	void modelEle::startAnimation(int ani, behavior mode) {
		_aniStart = man().time();
		_mode = mode;
		sys.meshPasses[materialIndex]->items[itemIndex].animationIndex = ani;
	}

	//std::unique_ptr<editor::gui> modelEle::renderGUI() {
	//	editor::container* c = new	editor::container();
	//	c->title = "Mesh Filter";
	//	{
	//		editor::button* b = new editor::button();
	//		b->info = "Broken";
	//		//b->info = "Mesh: " + ((_model == -1) ? string("(None)") : eng()->getAllOfType<meshManager>()[0]->getMesh(_model).name);
	//		c->items.emplace_back(b);
	//	}
	//	return std::unique_ptr<editor::gui>(c);
	//}

	void modelEle::deserialize(const citrus::json& js) {
		setState(js[0], js[1], js[2]);
		setNormalMap(js[3]);
	}
	citrus::json modelEle::serialize() {
		if (materialIndex != -1) {
			meshPass& p = *sys.meshPasses[materialIndex];
			return {
				materialIndex,
				p.items[itemIndex].modelIndex,
				p.items[itemIndex].texIndex,
				p.items[itemIndex].normalTexIndex
			};
		}
		else {
			return json();
		}
	}

	void modelEle::action() {
		if (materialIndex != -1) {
			meshPass::itemInfo& inf = sys.meshPasses[materialIndex]->items[itemIndex];
			inf.pos = ent().getGlobalTrans().getPosition();
			inf.ori = ent().getGlobalTrans().getOrientation();
			inf.aniTime = float(time - _aniStart);
		}
	}
	modelEle::modelEle(entRef const& ent, manager & man, void* usr) :
		element(ent, man, usr, typeid(modelEle)),
		sys(*((modelEleStruct*)usr)->sys),
		time(*((modelEleStruct*)usr)->time)
	{
		setState(0, 0, 0);
	}
	modelEle::~modelEle() {
		reset();
	}
}