#include <algorithm>
#include "gui.h"

namespace citrus {
	void container::click(vec2 point, vector<view>& view) {
		for(int i = 0; i < items.size(); i++) {

		}
	}
	vector<weak_ptr<gui>> container::children() {
		vector<weak_ptr<gui>> res(items.size());
		for (int i = 0; i < items.size(); i++) {
			res[i] = items[i];
		}
		return res;
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
			views.back().owner = shared_from_this();
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
		v.owner = shared_from_this();
	}
	void button::mouseDown(ivec2 cursor, ivec2 myPos) {
		if(onClick) onClick(*this);
	}
	shared_ptr<button> button::create(string const& info, std::function<void(button&)> click) {
		auto res = ::std::make_shared<button>();
		res->info = info;
		res->onClick = click;
		return res;
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
			v.owner = shared_from_this();
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
			v.owner = shared_from_this();
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
			v.owner = shared_from_this();
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
			v.owner = shared_from_this();
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
			v.owner = shared_from_this();
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
			v.owner = shared_from_this();
		}
	}

	// shifts cursor maximally to left and returns number of characters passed along the way

	int textField::home() {
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

	int textField::end() {
		if (stringIndex == -1) return -1;
		int fromRight = 0;
		while (stringIndex != _state.size() && _state[stringIndex] != '\n') {
			stringIndex++;
			fromRight++;
		}
		return fromRight;
	}
	void textField::shiftLine(int lines) {
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
	void textField::focus(ivec2 cursorPx) {
		stringIndex = 0;
		color = vec3(1, 1, 1);
		focused = true;
	}
	void textField::defocus() {
		stringIndex = -1;
		color = vec3(1, 0, 0);
		focused = false;
		if (updateFunc) updateFunc(_state);
	}
	void textField::edit(windowInput::button b) {
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
	void textField::setState(string st) {
		defocus();
		_state = st;
	}
	void textField::mouseDown(ivec2 cursor, ivec2 myPos) {
		focus(cursor - myPos);
	}
	void textField::keyDown(windowInput::button bu) {
		if (focused) edit(bu);
	}
	bool textField::captureInput() {
		return focused;
	}
	ivec2 textField::dimensions() {
		string curState = state();
		return ivec2(250, margin + textHeight * (1 + std::count(curState.begin(), curState.end(), '\n')) + margin);
	}
	void textField::render(ivec2 pos, vector<view>& views, float depth) {
		string curState = state();
		views.emplace_back();
		view& v = views.back();
		v.text = curState;
		v.color = color;
		v.loc = pos;
		v.border = true;
		v.size = ivec2(250, margin + textHeight * (1 + std::count(curState.begin(), curState.end(), '\n')) + margin);
		v.depth = depth;
		v.owner = shared_from_this();
	}
	vector<weak_ptr<gui>> horiBar::children() {
		vector<weak_ptr<gui>> res;
		for (int i = 0; i < buttons.size(); i++) {
			res.emplace_back(buttons[i]);
		}
		return res;
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
	vector<weak_ptr<gui>> guiLeaf::children() {
		return vector<weak_ptr<gui>>();
	}
	
	vector<weak_ptr<gui>> dropDown::children() {
		vector<weak_ptr<gui>> res(buttons.size());
		for (int i = 0; i < buttons.size(); i++) {
			res[i] = buttons[i];
		}
		return res;
	}
	ivec2 dropDown::dimensions() {
		int height = title.empty() ? 0 : (margin + textHeight + margin);
		int largestWidth = title.empty() ? glm::max((int)(margin + title.length() * textWidth + margin), 400) : 400;
		for (auto& button : buttons) {
			height += button->dimensions().y;
			if (button->dimensions().x > largestWidth) largestWidth = button->dimensions().x;
		}
		return ivec2(largestWidth, height);
	}
	void dropDown::render(ivec2 pos, vector<view>& views, float depth) {
		int yOffset = 0;
		if(pinButton) pinButton->render(pos, views, depth + 0.1f);
		if(exitButton) exitButton->render(pos + ivec2(margin + textWidth + margin, 0), views, depth + 0.1f);
		if (!title.empty()) {
			views.emplace_back();
			views.back().color = vec3(0.0f, 0.8f, 0.0f);
			views.back().depth = depth + 0.1;
			views.back().loc = pos + ivec2((margin + textWidth + margin) * 2, 0);
			views.back().text = title;
			views.back().size = ivec2(margin + title.length() * textWidth + margin, margin + textHeight + margin);
			views.back().owner = shared_from_this();
			views.back().type = viewType::handleType;
			yOffset = margin + textHeight + margin;
		}
		for (auto& button : buttons) {
			button->render(pos + ivec2(0, yOffset), views, depth + 0.1f);
			yOffset += button->dimensions().y;
		}
	}
	void floatingGui::addButtons() {
		pinButton = std::make_shared<button>();
		pinButton->info = ".";
		pinButton->onClick = [this](button& b) {
			this->shouldPin = !this->shouldPin;
			this->pinButton->info = this->shouldPin ? "!" : ".";
		};
		exitButton = std::make_shared<button>();
		exitButton->info = "x";
		exitButton->onClick = [this](button& b) {
			this->shouldClose = true;
		};
	}
	shared_ptr<dropDown> dropDown::create(string const& title, vector<shared_ptr<button>> const& buts) {
		auto res = ::std::make_shared<dropDown>();
		res->addButtons();
		res->buttons = buts;
		res->title = title;
		return res;
	}
	ivec2 vecField::dimensions() {
		return ivec2(textWidth * charsPerFloat * 4 + margin * 5, margin * 2 + textHeight);
	}
	void vecField::render(ivec2 pos, vector<view>& views, float depth) {
		for (int i = 0; i < 4; i++) {
			views.emplace_back();
			view& v = views.back();
			v.text = i < numComponents ? formatFloat(vec[i]) : string(charsPerFloat, 'X');
			v.color = vec3(1.0f, 1.0f, 1.0f);
			v.loc = pos + ivec2(charsPerFloat * textWidth * i, 0);
			v.border = true;
			v.size = ivec2(textWidth * charsPerFloat, textHeight);
			v.depth = depth;
			v.owner = shared_from_this();
		}
	}
	vector<weak_ptr<gui>> floatingContainer::children() {
		vector<weak_ptr<gui>> res;
		if (pinButton) res.emplace_back(pinButton);
		if (exitButton) res.emplace_back(exitButton);
		for (int i = 0; i < items.size(); i++) {
			res.emplace_back(items[i]);
		}
		return res;
	}
	ivec2 floatingContainer::dimensions() {
		ivec2 res = ivec2(250, title.empty() ? 0 : (margin + textHeight + margin));
		for (int i = 0; i < items.size(); i++) {
			res.y += items[i]->dimensions().y;
		}
		return res;
	}
	void floatingContainer::render(ivec2 pos, vector<view>& views, float depth) {
		int h = pos.y;
		if (pinButton) pinButton->render(pos, views, depth + 0.1f);
		if (exitButton) exitButton->render(pos + ivec2(margin + textWidth + margin, 0), views, depth + 0.1f);
		if (!title.empty()) {
			views.emplace_back();
			views.back().text = title;
			views.back().loc = pos + ivec2(pinButton ? (margin + textWidth + margin) * 2 : 0, margin);
			views.back().size = dimensions();
			views.back().color = vec3(0.8f);
			views.back().depth = depth;
			views.back().owner = shared_from_this();
			h = textHeight + margin;
		}
		for (auto item : items) {
			item->render(pos + ivec2(0, h), views, depth + 1.0f);
			h += item->dimensions().y;
		}
	}

	vector<weak_ptr<gui>> linearLayout::children() {
		vector<weak_ptr<gui>> res(items.size());
		for (int i = 0; i < items.size(); i++) {
			res[i] = items[i];
		}
		return res;
	}
	ivec2 linearLayout::dimensions() {
		if (direction == down) {
			ivec2 res = ivec2(0, 0);
			for (int i = 0; i < items.size(); i++) {
				res.y += items[i]->dimensions().y;
				if (items[i]->dimensions().x > res.x) res.x = items[i]->dimensions().x;
			}
			return res;
		} else if (direction == right) {
			ivec2 res = ivec2(0, 0);
			for (int i = 0; i < items.size(); i++) {
				res.x += items[i]->dimensions().x;
				if (items[i]->dimensions().y > res.y) res.y = items[i]->dimensions().y;
			}
			return res;
		}
	}
	void linearLayout::render(ivec2 pos, vector<view>& views, float depth) {
		int offset = 0;
		
		if (direction == down) {
			for (int i = 0; i < items.size(); i++) {
				items[i]->render(pos + ivec2(0, offset), views, depth + 1.0f);
				offset += items[i]->dimensions().y;
			}
		} else if (direction == right) {
			for (int i = 0; i < items.size(); i++) {
				items[i]->render(pos + ivec2(offset, 0), views, depth + 1.0f);
				offset += items[i]->dimensions().x;
			}
		}
	}
}
