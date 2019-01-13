#include <editor/gui.h>

namespace citrus::editor {
	void container::click(vec2 point, vector<view>& view) {
		for(int i = 0; i < items.size(); i++) {

		}
	}
	ivec2 container::dimensions() {
		ivec2 res = ivec2(250, title.empty() ? 0 : (margin + textHeight + margin));
		for(int i = 0; i < items.size(); i++) {
			res.y += items[i]->dimensions().y;
		}
		return res;
	}
	void container::render(ivec2 pos, vector<view>& views, float depth) {
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
	ivec2 button::dimensions() {
		return ivec2(margin + textWidth * info.length() + margin, margin + textHeight + margin);
	}
	void button::render(ivec2 pos, vector<view>& views, float depth) {
		views.emplace_back();
		view& v = views.back();
		v.text = info;
		v.color = vec3(0.8f, 1.0f, 0.8f);
		v.size = ivec2(margin + textWidth * info.length() + margin, margin + textHeight + margin);
		v.loc = pos;
		v.border = true;
		v.depth = depth;
	}
	ivec2 toggle::dimensions() {
		return ivec2(margin + textWidth * info.length() + margin + margin + textWidth + margin, margin + textHeight + margin);
	}
	void toggle::render(ivec2 pos, vector<view>& views, float depth) {
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
	ivec2 slider::dimensions() {
		return ivec2(250, margin + textHeight + margin);
	}
	void slider::render(ivec2 pos, vector<view>& views, float depth) {
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
			v.loc = pos + ivec2(margin + textWidth * info.length() + margin + (int)(250 * (state - min) / (max - min)), (margin + textHeight + margin) / 2 - 8);
			v.text = info;
			v.size = ivec2(16, 16);
			v.depth = depth;
		}
	}
	ivec2 textField::dimensions() {
		return ivec2(250, margin + textHeight * (1 + std::count(state.begin(), state.end(), '\n')) + margin);
	}
	void textField::render(ivec2 pos, vector<view>& views, float depth) {
		views.emplace_back();
		view& v = views.back();
		v.text = state;
		v.color = vec3(1.0f, 1.0f, 1.0f);
		v.loc = pos;
		v.border = true;
		v.size = ivec2(250, margin + textHeight + margin + textHeight * (1 + std::count(state.begin(), state.end(), '\n')) + margin);
		v.depth = depth;
	}
	ivec2 horiBar::dimensions() {
		int widthSum = 0;
		int maxHeight = 0;
		for(int i = 0; i < buttons.size(); i++) {
			auto& b = buttons[i];
			widthSum += b->dimensions().x + ((i != buttons.size() - 1) ? margin : 0);
			if(b->dimensions().y > maxHeight) maxHeight = b->dimensions().y;
		}
		return ivec2(1920 * 4, maxHeight);
	}
	void horiBar::render(ivec2 pos, vector<view>& views, float depth) {
		int xAdd = 0;
		views.emplace_back();
		views.back().color = vec3(1);
		views.back().size = dimensions();
		for(int i = 0; i < buttons.size(); i++) {
			auto& b = buttons[i];
			b->render(pos + ivec2(xAdd, 0), views, depth + 0.1f);
			xAdd += b->dimensions().x + ((i != buttons.size() - 1) ? margin : 0);
		}
	}
}