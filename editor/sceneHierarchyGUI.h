#pragma once

#include "gui.h"
#include "../mankern/manager.h"
#include "../mankern/entityRef.h"
#include "editor.h"

namespace citrus {
	struct sceneHierarchy : public floatingGui {
	private:
		struct entBar {
			shared_ptr<button> delButton, selButton;
			shared_ptr<textField> nameField;
			shared_ptr<linearLayout> layout;
			entRef ent;
		};

		shared_ptr<button> createButton;
		shared_ptr<button> refreshButton;
		shared_ptr<button> loadButton;
		vector<entBar> items;

		void addEnt(ctEditor& ed, entRef ent, int level);
	public:
		manager* man;

		vector<weak_ptr<gui>> children();
		partial render();
		void refresh();

		void update(ctEditor& ed);

		void addButtons();

		sceneHierarchy(ctEditor& ed);
	};
}