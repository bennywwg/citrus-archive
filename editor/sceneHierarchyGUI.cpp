#include "sceneHierarchyGUI.h"
#include "util.h"
#include "../graphkern/util.h"
#include "../basicUtil/basicUtil.h"

namespace citrus {
	void sceneHierarchy::addEnt(ctEditor& ed, entRef ent, int level) {
		auto bar = std::make_shared<linearLayout>(ed);
		bar->direction = linearLayout::right;

		auto delButton = std::make_shared<button>(ed);
		delButton->info = "del";
		delButton->onClick = [this, ent, &ed](button& b) {
			man->destroy(ent);
			man->flushToDestroy();
			refresh();
		};

		auto selectButton = std::make_shared<button>(ed);
		selectButton->info = "sel" + std::string(level * 2, '-');
		selectButton->onClick = [this, &ed, ent](button& b) {
			ed.selected = ent;
		};

		auto nameField = std::make_shared<textField>(ed);
		nameField->setState(ent.name());
		nameField->onChange = [ent](textField&tf, string val) {
			if (ent) ent.setName(val);
		};

		bar->items = { delButton, selectButton, nameField };

		entBar b;
		b.delButton = delButton;
		b.selButton = selectButton;
		b.layout = bar;
		b.nameField = nameField;
		b.ent = ent;

		items.push_back(b);

		for (auto const& child : ent.getChildren()) {
			addEnt(ed, child, level + 1);
		}
	}
	vector<weak_ptr<gui>> sceneHierarchy::children() {
		vector<weak_ptr<gui>> res = floatingGui::children();
		res.push_back(createButton);
		res.push_back(refreshButton);
		res.push_back(loadButton);
		for (auto& item : items) res.push_back(item.layout);
		return res;
	}
	partial sceneHierarchy::render() {
		partial res = floatingGui::render();
		res.appendDown(createButton->render());
		res.appendDown(refreshButton->render());
		res.appendDown(loadButton->render());
		for (auto& item : items) res.appendDown(item.layout->render());
		return res;
	}
	void sceneHierarchy::refresh() {
		items.clear();

		auto ents = man->allEnts();
		for (entRef const& ent : ents) {
			if (!ent.getParent()) {
				addEnt(_ed, ent, 0);
			}
		}
	}
	void sceneHierarchy::update(ctEditor& ed) {
		floatingGui::update(ed);
		for (int i = 0; i < items.size(); i++) {
			if (items[i].ent == ed.selected) {
				items[i].nameField->color = vec3(0.8f, 1.0f, 0.8f);
			} else {
				items[i].nameField->color = vec3(0.8f, 0.8f, 0.8f);
			}
		}
	}
	void sceneHierarchy::addButtons() {
		title = "Scene Hierarchy";
		shouldPin = true;
	}
	sceneHierarchy::sceneHierarchy(ctEditor& ed) : floatingGui(ed) {
		createButton = std::make_shared<button>(ed);
		createButton->info = "Create Entity";
		createButton->onClick = [this, &ed](button& b) {
			ed.selected = man->create("Unnamed");
			man->flushToCreate();
			refresh();
		};
		refreshButton = std::make_shared<button>(ed);
		refreshButton->info = "Refresh";
		refreshButton->onClick = [this, &ed](button& b) {
			refresh();
		};
		loadButton = std::make_shared<button>(ed);
		loadButton->info = "Load <-";
		loadButton->onClick = [this, &ed](button& b) {
			std::string toLoad = dialogOpenFile("cts");
			if (toLoad.empty()) return;
			std::string loaded;
			try {
				loaded = loadEntireFile(toLoad);
			} catch (std::runtime_error const& er) {
				std::cout << "failed to load prefab\n";
				return;
			}
			json parsed;
			try {
				parsed = json::parse(loaded);
			} catch (...) {
				std::cout << "failed to parse prefab (json)\n";
				return;
			}
			try {
				ed.man->deserializeTree(parsed);
			} catch (managerException const& er) {
				std::cout << "failed to parse prefab (hierarchy)\n";
			}
			refresh();
		};
	}
}