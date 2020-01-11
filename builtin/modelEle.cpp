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
		return animationIndex;
	}
	
	void modelEle::setAnimationState(int aniIndex, float time) {
		animationIndex = aniIndex;
		if (materialIndex == -1) return;
		meshPass& p = *sys.meshPasses[materialIndex];
		p.items[itemIndex].animationIndex = aniIndex;
		p.items[itemIndex].aniTime = time;
	}

	void modelEle::action() {
		if (materialIndex != -1) {
			meshPass::itemInfo& inf = sys.meshPasses[materialIndex]->items[itemIndex];
			inf.pos = ent().getGlobalTrans().getPosition();
			inf.ori = ent().getGlobalTrans().getOrientation();
		}
	}

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

	modelEle::modelEle(entRef const& ent, manager & man, void* usr) :
		element(ent, man, usr, typeid(modelEle)),
		sys(*((modelEleStruct*)usr)->sys)
	{
		setState(0, 0, 0);
	}
	modelEle::~modelEle() {
		reset();
	}
}