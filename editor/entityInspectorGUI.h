#pragma once

#include "gui.h"
#include "../mankern/manager.h"
#include "../mankern/entityRef.h"

namespace citrus {
	struct entityInspector : public floatingGui {
	private:
		manager* man;

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
		
		shared_ptr<vecField> vec, ori;



		vector<weak_ptr<gui>> children() {
			vector<weak_ptr<gui>> res;
			if (pinButton) res.emplace_back(pinButton);
			if (exitButton) res.emplace_back(exitButton);
			for (int i = 0; i < items.size(); i++) {
				res.emplace_back(items[i]);
			}
			return res;
		}
		ivec2 dimensions() {
			int h = textHeight + margin;
			int w = 

			for (int i = 0; i < eles.size(); i++) {
				h += eleNames[i].layout->dimensions().y;
				h += eles[i]->dimensions().y;
			}


		}
		void render(ivec2 pos, vector<view>& views, float depth) {
			int h = 0;
			//render title
			floatingGui::render(pos, views, depth);
			{
				views.emplace_back();
				view& v = views.back();
				v.text = "Entity Inspector";
				v.loc = pos + ivec2(pinButton ? (margin + textWidth + margin) * 2 : 0, margin);
				v.size = dimensions();
				v.color = vec3(0.8f);
				v.depth = depth;
				v.owner = shared_from_this();
				h = textHeight + margin;
			}

			for (int i = 0; i < eles.size(); i++) {
				eleNames[i].layout->render(pos + ivec2(0, h), views, depth + 1.0f);
				h += eleNames[i].layout->dimensions().y;
				eles[i]->render(pos + ivec2(0, h), views, depth + 1.0f);
				h += eles[i]->dimensions().y;
			}
		}

		void setEnt(entRef ent) {
			bool needToUpdate = (this->ent == ent);
			this->ent = ent;
			
		}

		entityInspector() {
			auto vec = std::make_shared<vecField>();

			auto ori = std::make_shared<vecField>();
			ori->numComponents = 4;
		}
	};
}