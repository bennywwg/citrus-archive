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
		bool border;
		float depth;
		weak_ptr<gui> owner;
		viewType type;
	};

	struct gui : ::std::enable_shared_from_this<gui> {
		weak_ptr<gui> parent;
		::std::function<void(ctEditor&)> updateFunc;

		weak_ptr<gui> topLevelParent();
		virtual void mouseDown(ivec2 cursor, ivec2 myPos) { }
		virtual void mouseDragged(ivec2 cursor, ivec2 myPos) { }
		virtual void mouseUp(ivec2 cursor, ivec2 myPos) { }
		virtual void keyDown(windowInput::button b) { for (auto child : children()) child.lock()->keyDown(b); }
		virtual bool captureInput() { for (auto child : children()) if (child.lock()->captureInput()) return true; return false; }
		virtual vector<weak_ptr<gui>> children() { throw ::std::runtime_error("gui::children"); }
		inline void update(ctEditor& ed) { if (updateFunc) updateFunc(ed); for (auto& child : children()) if (!child.expired()) child.lock()->update(ed); }
		virtual ivec2 dimensions() { throw ::std::runtime_error("gui::dimensions"); }
		virtual void render(ivec2 pos, vector<view>& views, float depth) { throw ::std::runtime_error("gui::render"); }
		virtual ~gui() = default;
	};

	// just a gui with no children
	struct guiLeaf : public gui {
		vector<weak_ptr<gui>> children();
	};

	struct container : public gui {
		vec3 color;
		string title;

		vector<shared_ptr<gui>> items;

		void click(vec2 point, vector<view>& view);

		vector<weak_ptr<gui>> children();
		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
	};

	struct linearLayout : public gui {
		enum layoutDirect {
			down,
			right
		} direction = right;

		vector<shared_ptr<gui>> items;

		vector<weak_ptr<gui>> children();
		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
	};

	struct button : public guiLeaf {
		string info;
		function<void(button&)> onClick;

		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
		void mouseDown(ivec2 cursor, ivec2 myPos);

		static shared_ptr<button> create(string const& info, function<void(button&)> click);
	};

	struct floatingGui : public gui {
		ivec2 pos;
		shared_ptr<button> pinButton;
		shared_ptr<button> exitButton;
		bool shouldClose = false;
		bool shouldPin = false;
		bool justCreated = true;
		void addButtons();
	};

	struct floatingContainer : public floatingGui {
		string title;
		vector<shared_ptr<gui>> items;

		vector<weak_ptr<gui>> children();
		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
	};

	struct toggle : public guiLeaf {
		string info;
		bool state;
		function<void(toggle&, bool)> onChange;

		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
	};

	struct slider : public guiLeaf {
		string info;
		double min = 4, max = 5;
		double state = 4.5;
		function<void(slider&, double)> onChange;

		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
	};

	struct textField : public guiLeaf {
	private:
		string _state;
	public:

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
		int home() {
			// A  B  C  \n D  B  \n \0
			// 0  1  2  3  4  5  6  7

			if (stringIndex == -1) return -1;
			int fromLeft = 0;
			while (stringIndex != 0 && _state[stringIndex - 1] != '\n') {
				stringIndex--;
				fromLeft++;
			}
			return fromLeft;
		}
		// shifts cursor maximally to right and returns number of characters passed along the way
		int end() {
			if (stringIndex == -1) return -1;
			int fromRight = 0;
			while (stringIndex != _state.size() && _state[stringIndex] != '\n') {
				stringIndex++;
				fromRight++;
			}
			return fromRight;
		}
		void shiftLine(int lines) {
			if (stringIndex == -1 || _state.empty() || lines == 0) return;
			int fromLeft = home();
			while (lines != 0) {
				if (lines > 0) {
					end();
					if (stringIndex == _state.size()) break;
					stringIndex++;
					lines--;
				} else {
					home();
					if (stringIndex == 0) break;
					stringIndex--;
					lines++;
				}
			}

			while (stringIndex != _state.size() && _state[stringIndex] != '\n') {
				stringIndex++;
			}
		}
		void focus(ivec2 cursorPx) {
			stringIndex = 0;
			color = vec3(1, 1, 1);
			focused = true;
		}
		void defocus() {
			stringIndex = -1;
			color = vec3(1, 0, 0);
			focused = false;
		}
		void edit(windowInput::button b) {
			if (stringIndex == -1) return;
			if (b == windowInput::arrowUp) {
				shiftLine(-1);
			} else if (b == windowInput::arrowDown) {
				shiftLine(1);
			} else if (b == windowInput::arrowLeft) {
				if (stringIndex != 0) stringIndex--;
			} else if (b == windowInput::arrowRight) {
				if (stringIndex != _state.size()) stringIndex++;
			} else if (b == windowInput::home) {
				home();
			} else if (b == windowInput::end) {
				end();
			} else if (windowInput::toChar(b, false)) {
				_state.insert(_state.begin() + stringIndex, windowInput::toChar(b, false));
				stringIndex++;
			} else if (b == windowInput::back) {
				if (!_state.empty() && stringIndex != 0) {
					_state.erase(_state.begin() + (stringIndex - 1));
					stringIndex--;
				}
			} else if (b == windowInput::del) {
				if (!_state.empty() && stringIndex != _state.size()) {
					_state.erase(_state.begin() + stringIndex);
				}
			} else if (b == windowInput::escape) {
				defocus();
			}
		}

		void setState(string st) {
			defocus();
			_state = st;
		}

		void mouseDown(ivec2 cursor, ivec2 myPos) {
			focus(cursor - myPos);
		}
		void keyDown(windowInput::button bu) {
			if (focused) edit(bu);
		}

		bool captureInput() {
			return focused;
		}

		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
	};

	struct vecField : public guiLeaf {
		int charsPerFloat = 10;
		int numComponents = 3;
		vec4 vec;

		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
	};

	struct horiBar : public gui {
		vector<shared_ptr<button>> buttons;

		vector<weak_ptr<gui>> children();
		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
	};


	struct dropDown : public floatingGui {
		string title;
		vector<shared_ptr<button>> buttons;

		vector<weak_ptr<gui>> children();
		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);

		static shared_ptr<dropDown> create(string const& title, vector<shared_ptr<button>> const& buts);
	};
}