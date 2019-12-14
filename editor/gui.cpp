#include <algorithm>
#include "gui.h"
#include "editor.h"

namespace citrus {
	partial button::render() {
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

	/*

	partial toggle::render() {
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

	partial slider::render() {
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

	*/

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
	partial textField::render() {
		partial res;
		string curState = state();
		res.views.emplace_back();
		view& v = res.views.back();
		v.text = curState;
		v.sizeFromText();
		v.color = color;
		v.owner = shared_from_this();
		return res;
	}

	vector<weak_ptr<gui>> horiBar::children() {
		vector<weak_ptr<gui>> res;
		for (int i = 0; i < buttons.size(); i++) {
			res.emplace_back(buttons[i]);
		}
		return res;
	}
	partial horiBar::render() {
		partial res;
		for(auto b : buttons) res.appendRight(b->render());
		view & v = *res.views.emplace(res.views.begin());
		v.size = ivec2(_globalEd->win->framebufferSize().x, res.dimensions().y);
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
	partial dropDown::render() {
		partial res;
		if(pinButton) res.appendRight(pinButton->render());
		if(exitButton) res.appendRight(exitButton->render());
		if (!title.empty()) {
			view &v = res.views.emplace_back();
			v.color = vec3(0.0f, 0.8f, 0.0f);
			v.loc = ivec2(res.dimensions().x, 0);
			v.text = title;
			v.sizeFromText();
			v.owner = shared_from_this();
			v.type = viewType::handleType;
		}
		for (auto& button : buttons) {
			res.appendDown(button->render());
		}
		res.translate(pos);
		return res;
	}
	shared_ptr<dropDown> dropDown::create(string const& title, vector<shared_ptr<button>> const& buts) {
		auto res = ::std::make_shared<dropDown>();
		res->addButtons();
		res->buttons = buts;
		res->title = title;
		return res;
	}

	partial vecField::render() {
		partial res;
		for (int i = 0; i < 4; i++) {
			res.views.emplace_back();
			view& v = res.views.back();
			v.text = i < numComponents ? formatFloat(vec[i]) : string(charsPerFloat, 'X');
			v.sizeFromText();
			v.color = vec3(1.0f, 1.0f, 1.0f);
			v.loc = ivec2(charsPerFloat * textWidth * i, 0);
			v.owner = shared_from_this();
		}
		return res;
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

	vector<weak_ptr<gui>> floatingContainer::children() {
		vector<weak_ptr<gui>> res;
		if (pinButton) res.emplace_back(pinButton);
		if (exitButton) res.emplace_back(exitButton);
		for (int i = 0; i < items.size(); i++) {
			res.emplace_back(items[i]);
		}
		return res;
	}
	partial floatingContainer::render() {
		partial res;
		if (pinButton) res.appendRight(pinButton->render());
		if (exitButton) res.appendRight(exitButton->render());
		if (!title.empty()) {
			view &v = res.views.emplace_back();
			v.text = title;
			v.sizeFromText();
			v.loc = ivec2(res.dimensions().x + (pinButton ? (margin + textWidth + margin) * 2 : 0), 0);
			v.color = vec3(0.8f);
			v.owner = shared_from_this();
		}
		for (auto item : items) {
			res.appendDown(item->render());
		}
		res.translate(pos);
		return res;
	}

	vector<weak_ptr<gui>> linearLayout::children() {
		vector<weak_ptr<gui>> res(items.size());
		for (int i = 0; i < items.size(); i++) {
			res[i] = items[i];
		}
		return res;
	}
	partial linearLayout::render() {
		ivec2 offset(0, 0);

		partial res;

		for (int i = 0; i < items.size(); i++) {
			partial pt = items[i]->render();
			res.append(pt, offset);
			ivec2 dims = pt.dimensions();
			offset += (direction == down) ? ivec2(0, dims.y) : ivec2(dims.x, 0);
		}

		return res;
	}
}
