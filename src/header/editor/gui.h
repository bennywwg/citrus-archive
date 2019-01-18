#pragma once

#include <glm/ext.hpp>
#include <string>
#include <vector>
#include <functional>

namespace citrus::editor {
	using glm::vec2;
	using glm::ivec2;
	using glm::vec3;
	using glm::vec4;
	using std::string;
	using std::vector;
	using std::unique_ptr;

	const int itemHeight = 24;
	const int margin = 2;
	const int textWidth = 8;
	const int textHeight = 16;

	struct gui;
	
	struct view {
		ivec2 loc;
		ivec2 size;
		string text;
		vec3 color;
		bool border;
		float depth;
		gui* owner;
	};

	struct gui {
		gui* parent = nullptr;
		bool focused = false;
		virtual void mouseDown(ivec2 cursor, ivec2 myPos) { }
		virtual void mouseDragged(ivec2 cursor, ivec2 myPos) { }
		virtual void mouseUp(ivec2 cursor, ivec2 myPos) { }
		virtual vector<gui*> children() { throw std::runtime_error("gui::children"); }
		virtual ivec2 dimensions() { throw std::runtime_error("gui"); }
		virtual void render(ivec2 pos, vector<view>& views, float depth) { throw std::runtime_error("gui"); }
		virtual ~gui() = default;
	};

	struct guiLeaf : public gui {
		vector<gui*> children();
	};


	struct container : public gui {
		vec3 color;
		string title;

		vector<unique_ptr<gui>> items;

		void click(vec2 point, vector<view>& view);

		vector<gui*> children();
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
		vector<unique_ptr<button>> buttons;

		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
	};

	struct dropDown : public gui {
		string title;
		vector<unique_ptr<button>> buttons;

		vector<gui*> children();
		ivec2 dimensions();
		void render(ivec2 pos, vector<view>& views, float depth);
	};
}