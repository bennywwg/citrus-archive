#pragma once

#include <glm/ext.hpp>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <stdexcept>

namespace citrus::editor {
	using glm::vec2;
	using glm::ivec2;
	using glm::vec3;
	using glm::vec4;
	using std::string;
	using std::vector;
	using std::shared_ptr;
	using std::weak_ptr;
	using std::make_unique;
	using std::make_shared;
	using std::unique_ptr;

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
		std::weak_ptr<gui> owner;
		viewType type;
	};

	struct gui : std::enable_shared_from_this<gui> {
		weak_ptr<gui> parent;
		std::function<void(ctEditor&)> updateFunc;

		weak_ptr<gui> topLevelParent();
		virtual void mouseDown(ivec2 cursor, ivec2 myPos) { }
		virtual void mouseDragged(ivec2 cursor, ivec2 myPos) { }
		virtual void mouseUp(ivec2 cursor, ivec2 myPos) { }
		virtual vector<weak_ptr<gui>> children() { throw std::runtime_error("gui::children"); }
		inline void update(ctEditor& ed) { if (updateFunc) updateFunc(ed); for (auto& child : children()) if (!child.expired()) child.lock()->update(ed); }
		virtual ivec2 dimensions() { throw std::runtime_error("gui::dimensions"); }
		virtual void render(ivec2 pos, vector<view>& views, float depth) { throw std::runtime_error("gui::render"); }
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

	struct button : public guiLeaf {
		string info;
		std::function<void(button&)> onClick;

		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
		void mouseDown(ivec2 cursor, ivec2 myPos);

		static shared_ptr<button> create(string const& info, std::function<void(button&)> click);
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
		std::function<void(toggle&, bool)> onChange;

		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
	};

	struct slider : public guiLeaf {
		string info;
		double min = 4, max = 5;
		double state = 4.5;
		std::function<void(slider&, double)> onChange;

		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
	};

	struct textField : public guiLeaf {
		string info;
		string state;
		std::function<void(textField&, string)> onChange;

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