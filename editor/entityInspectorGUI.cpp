#include "entityInspectorGUI.h"
namespace citrus {
	vector<weak_ptr<gui>> entityInspector::children() {
		vector<weak_ptr<gui>> res = floatingGui::children();

		res.push_back(refreshButton);

		for (int i = 0; i < eleNames.size(); i++) {
			res.emplace_back(eleNames[i].layout);
			res.emplace_back(eles[i]);
		}

		return res;
	}
	partial entityInspector::render() {
		partial res;

		res.appendDown(floatingGui::render());

		res.appendDown(refreshButton->render());

		if (ent) {
			res.appendDown(vec->render());
			res.appendDown(ori->render());
		}

		for (int i = 0; i < eles.size(); i++) {
			res.appendDown(eleNames[i].layout->render());
			res.appendDown(eles[i]->render());
		}

		return res;
	}
	void entityInspector::setEnt(entRef ent) {
		entRef oldEnt = this->ent;
		this->ent = ent;
		if(oldEnt != ent || !ent) refresh();
	}
	void entityInspector::refresh() {
		eleNames.clear();
		eles.clear();

		if (ent) {
			for (int i = 0; i < ent._raw()->eles.size(); i++) {
				element* ei = ent._raw()->eles[i];
				auto db = button::create(_ed, "del", [this, ei](button& b) {
					man->destroyElement(ei);
				});
				auto tf = std::make_shared<textField>(_ed);
				tf->setState(man->getInfo(ei->_raw()._type)->name);

				auto ll = std::make_shared<linearLayout>(_ed);
				ll->direction = linearLayout::right;
				ll->items = { db, tf };

				auto itf = std::make_shared<textField>(_ed);

				if (ei->_raw()._man) {
					itf->setState(ei->serialize().dump(2));
					itf->onChange = [ei](textField& tfp, string cp) {
						tfp.defocus();
						string old = tfp.state();
						json jp;
						try {
							jp = json::parse(cp);
						} catch (...) {
							std::cout << "invalid json\n";
							return;
						}
						ei->deserialize(jp);
					};
				} else {
					itf->setState("(Not Initialized)");
				}

				eleNames.push_back(eleNameBar{ ll, db, tf });
				eles.push_back(itf);
			}
		}
	}
	void entityInspector::update(ctEditor& ed) {
		if (ent) {
			vec->setState(vec4(ent.getGlobalTrans().getPosition(), 0.0f));
			auto qt = ent.getGlobalTrans().getOrientation();
			ori->setState(vec4(qt.x, qt.y, qt.z, qt.w));
		}
	}
	inline void entityInspector::addButtons() {
		floatingGui::addButtons();
		pinButton->info = "#";
		pinButton->onClick = [](button& b) {};
		shouldPin = true;
		exitButton.reset();
	}
	entityInspector::entityInspector(ctEditor &ed) : floatingGui(ed), man(nullptr) {
		refreshButton = std::make_shared<button>(ed);
		refreshButton->info = "Refresh";
		refreshButton->onClick = [this, &ed](button& b) {
			refresh();
		};

		vec = std::make_shared<vecField>(ed, 3);

		ori = std::make_shared<vecField>(ed, 4);

		shouldPin = true;

		if (pinButton) {
			pinButton->onClick = [](button&) {};
			pinButton->info = "*";
		}

		title = "Entity Inspector";
	}
}