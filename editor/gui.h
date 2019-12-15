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
		fieldType
	};

	struct gui;

	class ctEditor;
	
	struct view {
		ivec2 loc;
		ivec2 size;
		string text;
		vec3 color;
		weak_ptr<gui> owner;
		viewType type;
		void sizeFromText(bool useMargin = false) {
			size = ivec2((useMargin ? margin * 2 : 0) + text.length() * textWidth, (useMargin ? margin * 2 : 0) + textHeight);
			for (char c : text) if (c == '\n') size.y += textHeight;
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
		void translate(ivec2 trans) {
			for (view& v : views) {
				v.loc += trans;
			}
		}
	};

	struct gui : std::enable_shared_from_this<gui> {
		std::function<void(ctEditor&)> updateFunc;

		virtual void mouseDown(ivec2 cursor, ivec2 myPos) { }
		virtual void mouseDragged(ivec2 cursor, ivec2 myPos) { }
		virtual void mouseUp(ivec2 cursor, ivec2 myPos) { }
		virtual void keyDown(windowInput::button b) { for (auto child : children()) child.lock()->keyDown(b); }
		virtual bool captureInput() { for (auto child : children()) if (child.lock()->captureInput()) return true; return false; }
		virtual vector<weak_ptr<gui>> children() { throw ::std::runtime_error("gui::children"); }
		inline void update(ctEditor& ed) { if (updateFunc) updateFunc(ed); for (auto& child : children()) if (!child.expired()) child.lock()->update(ed); }
		virtual partial render() { throw ::std::runtime_error("gui::render"); }
		virtual ~gui() = default;
	};

	// just a gui with no children
	struct guiLeaf : public gui {
		vector<weak_ptr<gui>> children();
	};

	struct linearLayout : public gui {
		enum layoutDirect {
			down,
			right
		} direction = right;

		vector<shared_ptr<gui>> items;

		vector<weak_ptr<gui>> children();
		partial render();
	};

	struct button : public guiLeaf {
		string info;
		function<void(button&)> onClick;

		partial render();
		void mouseDown(ivec2 cursor, ivec2 myPos);

		static shared_ptr<button> create(string const& info, function<void(button&)> click);
	};

	struct floatingGui : public gui {
		ivec2 pos;
		shared_ptr<button> pinButton;
		shared_ptr<button> exitButton;
		string title;
		bool shouldClose = false;
		bool shouldPin = false;
		bool justCreated = true;
		vector<weak_ptr<gui>> children();
		void addButtons();
		partial render();
	};

	struct floatingContainer : public floatingGui {
		vector<shared_ptr<gui>> items;

		vector<weak_ptr<gui>> children();
		partial render();
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

		std::function<void(string)> updateFunc;

		string info;
		string state() {
			if (stringIndex == -1) return _state;
			string res = _state;
			res += "_";
			if (res[stringIndex] == '\n') {
				res.insert(res.begin() + stringIndex, 'X');
			} else {
				res[stringIndex] = 'X';
			}

			return res;
		}
		vec3 color = vec3(1.0f, 1.0f, 1.0f);
		function<void(textField&, string)> onChange;

		int stringIndex = -1; // ∈ [0, state.size()]
		bool focused = false;
		bool editable = false;

		// shifts cursor maximally to left and returns number of characters passed along the way
		int home();
		// shifts cursor maximally to right and returns number of characters passed along the way
		int end();
		void shiftLine(int lines);
		void focus(ivec2 cursorPx);
		void defocus();
		void edit(windowInput::button b);

		void setState(string st);

		void mouseDown(ivec2 cursor, ivec2 myPos);
		void keyDown(windowInput::button bu);

		bool captureInput();

		partial render();
	};

	struct vecField : public guiLeaf {
		int charsPerFloat = 10;
		int numComponents = 3;
		vec4 vec;

		partial render();
	};

	struct horiBar : public gui {
		vector<shared_ptr<button>> buttons;

		vector<weak_ptr<gui>> children();
		partial render();
	};


	struct dropDown : public floatingGui {
		vector<shared_ptr<button>> buttons;

		vector<weak_ptr<gui>> children();
		partial render();

		static shared_ptr<dropDown> create(string const& title, vector<shared_ptr<button>> const& buts);
	};
}