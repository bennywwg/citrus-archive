#pragma once

#include "gui.h"
#include "../mankern/manager.h"
#include "../mankern/entityRef.h"

namespace citrus {
	struct entityInspector : public floatingGui {
	private:

		struct eleNameBar {
			shared_ptr<linearLayout> layout;
			shared_ptr<button> destroyButton;
			shared_ptr<textField> name;
		};

		vector<eleNameBar> eleNames;
		vector<shared_ptr<textField>> eles;
		entRef ent;

		void regenElements() {
			eleNames.clear();
			eles.clear();

			if (ent) {
				for (int i = 0; i < ent._raw()->eles.size(); i++) {
					element* ei = ent._raw()->eles[i];
					auto db = button::create("del", [this, ei](button& b) {
						man->destroyElement(ei);
					});
					auto tf = std::make_shared<textField>();
					auto ll = std::make_shared<linearLayout>();
					ll->direction = linearLayout::right;
					ll->items = { db, tf };

					auto itf = std::make_shared<textField>();

					if (ei->_raw()._man) {
						tf->setState(man->getInfo(ei->_raw()._type)->name + "\n" + ei->serialize().dump(2));
						itf->setState(ei->serialize().dump(2));
					} else {
						tf->setState(man->getInfo(ei->_raw()._type)->name + "\n(Not Initialized)");
						itf->setState("(Not Initialized)");
					}

					eleNames.push_back(eleNameBar{ ll, db, tf });
					eles.push_back(itf);
				}
			}
		}
	public:
		manager* man;
		
		shared_ptr<vecField> vec, ori;

		vector<weak_ptr<gui>> children() {
			vector<weak_ptr<gui>> res = floatingGui::children();

			for (int i = 0; i < eleNames.size(); i++) {
				res.emplace_back(eleNames[i].layout);
				res.emplace_back(eles[i]);
			}

			return res;
		}
		partial render() {
			partial res;

			res.appendDown(floatingGui::render());

			for (int i = 0; i < eles.size(); i++) {
				res.appendDown(eleNames[i].layout->render());
				res.appendDown(eles[i]->render());
			}

			return res;
		}

		void setEnt(entRef ent) {
			bool needToUpdate = (this->ent == ent);
			this->ent = ent;
			if(needToUpdate) regenElements();
		}

		entityInspector() {
			auto vec = std::make_shared<vecField>();

			auto ori = std::make_shared<vecField>();
			ori->numComponents = 4;
		}
	};
}