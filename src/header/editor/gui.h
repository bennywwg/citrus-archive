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
	
	struct view {
		ivec2 loc;
		ivec2 size;
		string text;
		vec3 color;
		bool border;
		float depth;
	};

	struct gui {
		bool focused = false;
		virtual ivec2 dimensions() { throw std::runtime_error("gui"); };
		virtual void render(ivec2 pos, vector<view>& views, float depth) { throw std::runtime_error("gui"); };
		virtual ~gui() = default;
	};


	struct container : public gui {
		ivec2 pos;
		vec3 color;
		string title;

		vector<unique_ptr<gui>> items;

		void click(vec2 point, vector<view>& view) {
			for(int i = 0; i < items.size(); i++) {
				
			}
		}

		ivec2 dimensions() {
			ivec2 res = ivec2(250, title.empty() ? 0 : (margin + textHeight + margin));
			for(int i = 0; i < items.size(); i++) {
				res.y += items[i]->dimensions().y;
			}
			return res;
		}
		void render(ivec2 pos, vector<view>& views, float depth) {
			int h = pos.y;
			if(!title.empty()) {
				views.emplace_back();
				views.back().text = title;
				views.back().loc = pos + ivec2(0, margin);
				views.back().size = dimensions();
				views.back().color = vec3(0.8f);
				views.back().depth = depth;
				h = textHeight + margin;
			}
			for(int i = 0; i < items.size(); i++) {
				items[i]->render(pos + ivec2(0, h), views, depth + 1.0f);
				h += items[i]->dimensions().y;
			}
		}
	};

	struct button : public gui {
		string info;
		std::function<void(button&)> onClick;

		ivec2 dimensions() {
			return ivec2(margin + textWidth * info.length() + margin, margin + textHeight + margin);
		}
		void render(ivec2 pos, vector<view>& views, float depth) {
			views.emplace_back();
			view& v = views.back();
			v.text = info;
			v.color = vec3(0.8f, 1.0f, 0.8f);
			v.size = ivec2(margin + textWidth * info.length() + margin, margin + textHeight + margin);
			v.loc = pos;
			v.border = true;
			v.depth = depth;
		}
	};

	struct toggle : public gui {
		string info;
		bool state;
		std::function<void(toggle&, bool)> onChange;

		ivec2 dimensions() {
			return ivec2(margin + textWidth * info.length() + margin + margin + textWidth + margin, margin + textHeight + margin);
		}
		void render(ivec2 pos, vector<view>& views, float depth) {
			{
				views.emplace_back();
				view& v = views.back();
				v.text = "";
				v.color = vec3(0.8f, 0.8f, 0.8f);
				v.size = ivec2(margin + textWidth * info.length() + margin + margin + textWidth * 1 + margin);
				v.loc = pos;
				v.border = false;
				v.depth = depth;
			}
			{
				views.emplace_back();
				view& v = views.back();
				v.text = info;
				v.color = vec3(1.0f, 1.8f, 1.0f);
				v.size = ivec2(margin + textWidth * info.length() + margin);
				v.loc = pos + ivec2(0, margin);
				v.border = false;
				v.depth = depth;
			}
			{
				views.emplace_back();
				view& v = views.back();
				v.text = state ? "X" : "_";
				v.color = vec3(0.8f, 1.0f, 0.8f);
				v.size = ivec2(margin + textWidth * info.length() + margin + margin + textWidth * 1 + margin);
				v.loc = pos + ivec2(margin + textWidth * info.length() + margin, 0);
				v.border = true;
				v.depth = depth;
			}
		}
	};

	struct slider : public gui {
		string info;
		double min = 4, max = 5;
		double state = 4.5;
		std::function<void(slider&, double)> onChange;

		ivec2 dimensions() {
			return ivec2(250, margin + textHeight + margin);
		}
		void render(ivec2 pos, vector<view>& views, float depth) {
			{
				views.emplace_back();
				view& v = views.back();
				v.border = false;
				v.color = vec3(1.0f, 1.0f, 1.0f);
				v.loc = pos;
				v.text = info;
				v.size = ivec2(margin + textWidth * info.length() + margin, margin + textHeight + margin);
				v.depth = depth;
			}
			{
				views.emplace_back();
				view& v = views.back();
				v.border = false;
				v.color = vec3(0.0f, 0.0f, 0.0f);
				v.loc = pos + ivec2(margin + textWidth * info.length() + margin, (margin + textHeight + margin) / 2 - 4);
				v.text = info;
				v.size = ivec2(250 - (margin + textWidth * info.length() + margin) - margin, 8);
				v.depth = depth;
			}
			{
				views.emplace_back();
				view& v = views.back();
				v.border = true;
				v.color = vec3(1.0f, 1.0f, 1.0f);
				v.loc = pos + ivec2(margin + textWidth * info.length() + margin + (int) (250 * (state - min) / (max - min)), (margin + textHeight + margin) / 2 - 8);
				v.text = info;
				v.size = ivec2(16, 16);
				v.depth = depth;
			}
		}
	};

	struct textField : public gui {
		string info;
		string state;
		std::function<void(textField&, string)> onChange;

		ivec2 dimensions() {
			return ivec2(250, margin + textHeight * (1 + std::count(state.begin(), state.end(), '\n')) + margin);
		}
		void render(ivec2 pos, vector<view>& views, float depth) {
			views.emplace_back();
			view& v = views.back();
			v.text = state;
			v.color = vec3(1.0f, 1.0f, 1.0f);
			v.loc = pos;
			v.border = true;
			v.size = ivec2(250, margin + textHeight + margin + textHeight * (1 + std::count(state.begin(), state.end(), '\n')) + margin);
			v.depth = depth;
		}
	};


	struct horiBar : public gui {
		vector<unique_ptr<button>> buttons;

		ivec2 dimensions() {
			int widthSum = 0;
			int maxHeight = 0;
			for(int i = 0; i < buttons.size(); i++) {
				auto& b = buttons[i];
				widthSum += b->dimensions().x + ((i != buttons.size() - 1) ? margin : 0);
				if(b->dimensions().y > maxHeight) maxHeight = b->dimensions().y;
			}
			return ivec2(widthSum, maxHeight);
		}
		void render(ivec2 pos, vector<view>& views, float depth) {
			int xAdd = 0;
			for(auto& b : buttons) {
				b->render(pos + ivec2(xAdd, 0), views, depth + 0.1f);
				xAdd += b->dimensions().x;
			}
		}
	};
}