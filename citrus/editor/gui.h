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
		bool focused = false;
		weak_ptr<gui> topLevelParent();
		virtual void mouseDown(ivec2 cursor, ivec2 myPos) { }
		virtual void mouseDragged(ivec2 cursor, ivec2 myPos) { }
		virtual void mouseUp(ivec2 cursor, ivec2 myPos) { }
		virtual vector<weak_ptr<gui>> children() { throw std::runtime_error("gui::children"); }
		virtual ivec2 dimensions() { throw std::runtime_error("gui::dimensions"); }
		virtual void render(ivec2 pos, vector<view>& views, float depth) { throw std::runtime_error("gui::render"); }
		virtual ~gui() = default;
	};

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

	struct horiBar : public guiLeaf {
		vector<shared_ptr<button>> buttons;

		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
	};

	struct dropDown : public gui {
		string title;
		shared_ptr<button> pinButton;
		shared_ptr<button> exitButton;
		vector<shared_ptr<button>> buttons;
		bool shouldClose = false;
		bool shouldPin = false;

		vector<weak_ptr<gui>> children();
		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
		void addButtons();
	};
}