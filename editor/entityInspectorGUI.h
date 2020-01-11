#pragma once

#include "gui.h"
#include "../mankern/manager.h"
#include "../mankern/entityRef.h"

namespace citrus {
	class ctEditor;

	struct entityInspector : public floatingGui {
	private:
		struct eleNameBar {
			shared_ptr<linearLayout> layout;
			shared_ptr<button> destroyButton;
			shared_ptr<textField> name;
		};

		shared_ptr<button> refreshButton;
		vector<eleNameBar> eleNames;
		vector<shared_ptr<textField>> eles;
		entRef ent;

	public:
		manager* man;
		
		shared_ptr<vecField> vec, ori;

		vector<weak_ptr<gui>> children();
		partial render();

		void setEnt(entRef ent);
		void refresh();
		
		void update(ctEditor& ed);

		void addButtons();

		entityInspector(ctEditor &ed);
	};
}