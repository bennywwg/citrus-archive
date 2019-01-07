#pragma once

#include <editor/gui.h>
#include <memory>
#include <engine/entityRef.h>
#include <vector>

namespace citrus::editor {

	using std::vector;
	using std::string;
	using std::unique_ptr;

	struct multiFieldEditor {
		engine::entityRef ref;

		vector<unique_ptr<viewBase>> items;

		string title;
		multiFieldEditor& addField(int* val) {


			return *this;
		}
		multiFieldEditor(engine::entityRef ref) : ref(ref) {

		}
	};

	struct viewBase {
		multiFieldEditor* owner;
		virtual unique_ptr<gui> render() = 0;
	};
	
	struct intView : public viewBase {
		int* ptr;
	};
}