#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <stdexcept>

#include "../mankern/util.h"
#include "../graphkern/window.h"

namespace citrus {
	using ::std::shared_ptr;
	using ::std::weak_ptr;
	using ::std::vector;
	using ::std::string;
	using ::std::function;

	const int itemHeight = 24;
	const int margin = 2;
	const int textWidth = 8;
	const int textHeight = 16;


	enum struct viewType {
		noType,
		handleType,
		buttonType,
		fieldType,
		dropTarget
	};

	struct gui;

	class ctEditor;
	
	struct view {
		ivec2 loc;
		ivec2 size;
		string text;
		vec3 color;
		weak_ptr<gui> owner;
		viewType type = viewType::noType;
		void sizeFromText(bool useMargin = false) {
			int numCharsWidth = 0;
			int maxNumCharsWidth = 0;
			int numCharsHeight = 1;
			for (char c : text) {
				if (c == '\n') {
					numCharsHeight++;
					if (numCharsWidth > maxNumCharsWidth) maxNumCharsWidth = numCharsWidth;
					numCharsWidth = 0;
				}
				else numCharsWidth++;
			}
			if (numCharsWidth > maxNumCharsWidth) maxNumCharsWidth = numCharsWidth;
			size = ivec2((useMargin ? margin * 2 : 0) + maxNumCharsWidth * textWidth, (useMargin ? margin * 2 : 0) + numCharsHeight * textHeight);
		}
	};

	struct partial {
		vector<view> views;
		ivec2 dimensions() {
			ivec2 res(0, 0);
			for (view const& v : views) {
				if (v.loc.x + v.size.x > res.x) res.x = v.loc.x + v.size.x;
				if (v.loc.y + v.size.y > res.y) res.y = v.loc.y + v.size.y;
			}
			return res;
		}
		view* getSelected(ivec2 cursor) {
			for (int i = views.size() - 1; i >= 0; i--) {
				view& v = views[i];
				if (cursor.x >= v.loc.x && cursor.x < (v.loc.x + v.size.x) && cursor.y >= v.loc.y && cursor.y < (v.loc.y + v.size.y)) {
					return &v;
				}
			}
			return nullptr;
		}
		void append(partial const& other, ivec2 pos) {
			for (view v : other.views) {
				v.loc += pos;
				views.push_back(v);
			}
		}
		void appendRight(partial const& other) {
			append(other, ivec2(dimensions().x, 0));
		}
		void appendDown(partial const& other) {
			append(other, ivec2(0, dimensions().y));
		}
		partial& translate(ivec2 trans) {
			for (view& v : views) {
				v.loc += trans;
			}
			return *this;
		}
		partial() = default;
		partial(view const& v) : views({ v }) { }
	};

	struct gui : std::enable_shared_from_this<gui> {
		void propMouseDown(ctEditor& ed, ivec2 cursor, view* selectedView);
		void propMouseUp(ctEditor& ed, ivec2 cursor, view* selectedView);
		void propKeyDown(ctEditor& ed, windowInput::button b);
		void propKeyUp(ctEditor& ed, windowInput::button b);
		void propUpdate(ctEditor& ed);
		vector<weak_ptr<gui>> allSubChildren();

		virtual void mouseDown(ctEditor& ed, ivec2 cursor, view* selectedView);
		virtual void mouseUp(ctEditor& ed, ivec2 cursor, view* selectedView);
		virtual void keyDown(ctEditor& ed, windowInput::button b);
		virtual void keyUp(ctEditor& ed, windowInput::button b);
		virtual void update(ctEditor& ed);

		virtual vector<weak_ptr<gui>> children();

		virtual partial render();

		ctEditor &_ed;

		gui(ctEditor& ed);
		virtual ~gui() = default;
	};

	// just a gui with no children
	struct guiLeaf : public gui {
		vector<weak_ptr<gui>> children();
		guiLeaf(ctEditor& ed);
	};

	struct linearLayout : public gui {
		enum layoutDirect {
			down,
			right
		} direction = right;

		vector<shared_ptr<gui>> items;

		vector<weak_ptr<gui>> children();
		partial render();

		linearLayout(ctEditor& ed);
	};

	struct button : public guiLeaf {
		string info;
		function<void(button&)> onClick;

		partial render();
		void mouseDown(ctEditor& ed, ivec2 cursor, view *selectedView);

		button(ctEditor& ed);

		static shared_ptr<button> create(ctEditor &ed, string const& info, function<void(button&)> click);
	};

	struct floatingGui : public gui {
		ivec2 pos = ivec2(0, 0);
		ivec2 dragPosStart = ivec2(0, 0);
		ivec2 dragStart = ivec2(0, 0);
		bool beingDragged = false;
		shared_ptr<button> pinButton;
		shared_ptr<button> exitButton;
		string title;
		bool shouldClose = false;
		bool shouldPin = false;
		bool justCreated = true;
		void mouseDown(ctEditor& ed, ivec2 cursor, view* selectedView);
		void mouseUp(ctEditor& ed, ivec2 cursor, view* selectedView);
		void update(ctEditor& ed);
		vector<weak_ptr<gui>> children();
		virtual void addButtons();
		partial render();
		floatingGui(ctEditor& ed);
	};

	struct floatingContainer : public floatingGui {
		vector<shared_ptr<gui>> items;

		vector<weak_ptr<gui>> children();
		partial render();

		floatingContainer(ctEditor& ed);
	};

	/*struct toggle : public guiLeaf {
		string info;
		bool state;
		function<void(toggle&, bool)> onChange;

		partial render();
	};

	struct slider : public guiLeaf {
		string info;
		double min = 4, max = 5;
		double state = 4.5;
		function<void(slider&, double)> onChange;

		partial render();
	};*/

	struct textField : public guiLeaf {
	private:
		string _state;
	public:

		string info;
		string state();
		vec3 color = vec3(1.0f, 1.0f, 1.0f);
		function<void(textField&, string)> onChange;

		int stringIndex = -1; // ∈ [0, state.size()]
		bool focused = false;
		bool editable = false;
		bool modified = false;

		// shifts cursor maximally to left and returns number of characters passed along the way
		int home();
		// shifts cursor maximally to right and returns number of characters passed along the way
		int end();
		void shiftLine(int lines);
		void focus(ivec2 cursorPx);
		void defocus();
		void edit(windowInput::button b);

		void setState(string st);

		void mouseDown(ctEditor& ed, ivec2 cursor, view *selectedView);
		void keyDown(ctEditor& ed, windowInput::button bu);
		void keyUp(ctEditor& ed, windowInput::button bu);

		partial render();

		textField(ctEditor& ed);
		~textField();
	};

	struct vecField : public gui {
	private:
		int charsPerFloat = 10;
		int numComponents = 3;
		vec4 _vec;
		vector<shared_ptr<textField>> items;
		shared_ptr<linearLayout> fields;
	public:

		vector<weak_ptr<gui>> children();
		partial render();

		void setState(vec4 vec);
		vec4 getState();

		vecField(ctEditor& ed, int numComponents);
	};

	struct horiBar : public gui {
		vector<shared_ptr<button>> buttons;

		vector<weak_ptr<gui>> children();
		partial render();

		horiBar(ctEditor& ed);
	};


	struct dropDown : public floatingGui {
		vector<shared_ptr<button>> buttons;

		vector<weak_ptr<gui>> children();
		partial render();

		dropDown(ctEditor& ed);

		static shared_ptr<dropDown> create(ctEditor& ed, string const& title, vector<shared_ptr<button>> const& buts);
	};
}