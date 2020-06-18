#include <algorithm>
#include "gui.h"
#include "editor.h"

namespace citrus {
	partial button::render() {
		partial res;
		view& v = res.views.emplace_back();
		v.text = info;
		v.color = vec3(0.8f, 1.0f, 0.8f);
		v.sizeFromText();
		v.owner = shared_from_this();
		v.type = viewType::buttonType;
		return res;
	}
	void button::mouseDown(ctEditor& ed, ivec2 cursor, view* selectedView) {
		if(onClick && selectedView && !selectedView->owner.expired() && (selectedView->owner.lock() == shared_from_this())) onClick(*this); // jesus
	}
	button::button(ctEditor& ed) : guiLeaf(ed) { }
	shared_ptr<button> button::create(ctEditor& ed, string const& info, std::function<void(button&)> click) {
		auto res = ::std::make_shared<button>(ed);
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

	string textField::state() {
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
		int targetChar = glm::max(0, (cursorPx.x + textWidth / 2) / textWidth);
		int targetLine = glm::max(0, cursorPx.y / textHeight);

		stringIndex = 0;
		while (targetLine != 0 && stringIndex < _state.length() && targetLine != 0) {
			if (_state[stringIndex] == '\n') targetLine--;
			stringIndex++;
		}
		stringIndex = glm::min((int)_state.length(), stringIndex + targetChar);

		if (!focused) {
			_ed.numFocused++;
			focused = true;
		}
	}
	void textField::defocus() {
		if (focused) {
			_ed.numFocused--;
			stringIndex = -1;
			focused = false;
			if (onChange) onChange(*this, _state);
		}
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
		} else if (windowInput::toChar(b, modified)) {
			_state.insert(_state.begin() + stringIndex, windowInput::toChar(b, modified));
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
	void textField::mouseDown(ctEditor& ed, ivec2 cursor, view *selectedView) {
		if (selectedView && selectedView->owner.lock() == shared_from_this()) {
			focus(cursor - selectedView->loc);
		} else {
			defocus();
		}
	}
	void textField::keyDown(ctEditor& ed, windowInput::button bu) {
		if (bu == windowInput::lshift || bu == windowInput::rshift) modified = true;
		if (focused) edit(bu);
	}
	void textField::keyUp(ctEditor& ed, windowInput::button bu) {
		if (bu == windowInput::lshift || bu == windowInput::rshift) modified = false;
	}
	partial textField::render() {
		view v;
		v.loc = ivec2(0, 0);
		v.text = state();
		v.sizeFromText();
		v.color = color;
		v.owner = shared_from_this();
		v.type = viewType::fieldType;
		return v;
	}

	textField::textField(ctEditor& ed) : guiLeaf(ed) { }

	textField::~textField() {
		defocus();
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
		//v.size = ivec2(_globalEd->win->framebufferSize().x, res.dimensions().y);
		return res;
	}

	horiBar::horiBar(ctEditor& ed) : gui(ed) { }

	vector<weak_ptr<gui>> guiLeaf::children() {
		return vector<weak_ptr<gui>>();
	}

	guiLeaf::guiLeaf(ctEditor& ed) : gui(ed) { }
	
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
	dropDown::dropDown(ctEditor& ed) : floatingGui(ed) { }
	shared_ptr<dropDown> dropDown::create(ctEditor& ed, string const& title, vector<shared_ptr<button>> const& buts) {
		auto res = std::make_shared<dropDown>(ed);
		res->addButtons();
		res->buttons = buts;
		res->title = title;
		return res;
	}

	vector<weak_ptr<gui>> vecField::children() {
		return { weak_ptr<gui>(fields) };
	}

	partial vecField::render() {
		return fields->render();
	}

	void vecField::setState(vec4 vec) {
		_vec = vec;
		for (int i = 0; i < numComponents; i++) {
			items[i]->setState(formatFloat(_vec[i]));
		}
		for (int i = numComponents; i < 4; i++) {
			items[i]->setState(string(charsPerFloat, 'X'));
		}
	}

	vecField::vecField(ctEditor& ed, int numComponent) : gui(ed), numComponents(numComponent) {
		numComponents = glm::max(glm::min(numComponents, 4), 1);
		fields = std::make_shared<linearLayout>(ed);
		
		for (int i = 0; i < 4; i++) {
			auto field = std::make_shared<textField>(ed);
			items.push_back(field);
			field->editable = i < numComponents;
			field->onChange = [this, &field, i](textField& tf, string str) {
				try {
					_vec[i] = std::stof(str);
				} catch (std::invalid_argument const& arg) {
					field->setState(formatFloat(_vec[i]));
				}
			};
			fields->items.push_back(field);
		}

		setState(vec4(0.0f, 0.0f, 0.0f, 0.0f));
	}
	vec4 vecField::getState() {
		return _vec;
	}

	void floatingGui::mouseDown(ctEditor& ed, ivec2 cursor, view* selectedView) {
		bool clickedThis = selectedView != nullptr;
		if (clickedThis) {
			auto allChildren = allSubChildren();
			auto viewOwner = selectedView->owner.lock();
			if (viewOwner != shared_from_this()) {
				bool found = false;
				for (weak_ptr<gui> subChild : allChildren) {
					if (subChild.lock() == viewOwner) {
						found = true;
						break;
					}
				}
				if (!found) clickedThis = false;
			}
		}
		if (!clickedThis && !shouldPin) {
			shouldClose = true;
		}
		if (clickedThis) {
			beingDragged = true;
			dragPosStart = pos;
			dragStart = cursor;
		}
	}
	void floatingGui::mouseUp(ctEditor& ed, ivec2 cursor, view* selectedView) {
		if (beingDragged) {
			beingDragged = false;
			pos = cursor - dragStart + dragPosStart;
		}
	}
	void floatingGui::update(ctEditor& ed) {
		if (beingDragged) {
			pos = (ed.cursorPx - dragStart) + dragPosStart;
		}
	}

	vector<weak_ptr<gui>> floatingGui::children() {
		vector<weak_ptr<gui>> res;
		if (pinButton) res.push_back(pinButton);
		if (exitButton) res.push_back(exitButton);
		return res;
	}

	void floatingGui::addButtons() {
		pinButton = std::make_shared<button>(_ed);
		pinButton->info = ".";
		pinButton->onClick = [this](button& b) {
			this->shouldPin = !this->shouldPin;
			this->pinButton->info = this->shouldPin ? "!" : ".";
		};
		exitButton = std::make_shared<button>(_ed);
		exitButton->info = "x";
		exitButton->onClick = [this](button& b) {
			this->shouldClose = true;
		};
	}

	partial floatingGui::render() {
		partial res;
		if (pinButton) res.appendRight(pinButton->render());
		if (exitButton) res.appendRight(exitButton->render());
		if (!title.empty()) {
			view v;
			v.text = "Entity Inspector";
			v.loc = ivec2(0, 0);
			v.sizeFromText();
			v.color = vec3(0.8f);
			v.owner = shared_from_this();
			res.appendRight(v);
		}
		return res;
	}

	floatingGui::floatingGui(ctEditor& ed) : gui(ed) { }

	vector<weak_ptr<gui>> floatingContainer::children() {
		vector<weak_ptr<gui>> res = floatingGui::children();
		for (int i = 0; i < items.size(); i++) res.emplace_back(items[i]);
		return res;
	}
	partial floatingContainer::render() {
		partial res = floatingGui::render();
		for (auto item : items) res.appendDown(item->render());
		return res;
	}

	floatingContainer::floatingContainer(ctEditor& ed) : floatingGui(ed) { }

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
	linearLayout::linearLayout(ctEditor& ed) : gui(ed) { }
	void gui::propMouseDown(ctEditor& ed, ivec2 cursor, view* selectedView) {
		mouseDown(ed, cursor, selectedView);
		for (auto & child : children()) {
			if(!child.expired()) child.lock()->propMouseDown(ed, cursor, selectedView);
		}
	}
	void gui::propMouseUp(ctEditor& ed, ivec2 cursor, view* selectedView) {
		mouseUp(ed, cursor, selectedView);
		for (auto & child : children()) {
			if (!child.expired()) child.lock()->propMouseUp(ed, cursor, selectedView);
		}
	}
	void gui::propKeyDown(ctEditor& ed, windowInput::button b) {
		keyDown(ed, b);
		for (auto& child : children()) {
			if (!child.expired()) child.lock()->propKeyDown(ed, b);
		}
	}

	void gui::propKeyUp(ctEditor& ed, windowInput::button b) {
		keyUp(ed, b);
		for (auto& child : children()) {
			if (!child.expired()) child.lock()->propKeyUp(ed, b);
		}
	}
	void gui::propUpdate(ctEditor& ed) {
		update(ed);
		for (auto & child : children()) {
			if (!child.expired()) child.lock()->propUpdate(ed);
		}
	}
	void gui::mouseDown(ctEditor& ed, ivec2 cursor, view* selectedView) { }
	void gui::mouseUp(ctEditor& ed, ivec2 cursor, view* selectedView) { }
	void gui::keyDown(ctEditor& ed, windowInput::button b) { }
	void gui::keyUp(ctEditor& ed, windowInput::button b) { }
	vector<weak_ptr<gui>> gui::children() { throw ::std::runtime_error("gui::children"); }
	vector<weak_ptr<gui>> gui::allSubChildren() {
		vector<weak_ptr<gui>> myChildren = children();
		vector<weak_ptr<gui>> res = myChildren;
		for (auto child : myChildren) {
			auto childChildren = child.lock()->allSubChildren();
			res.insert(res.end(), childChildren.begin(), childChildren.end());
		}
		return res;
	}
	void gui::update(ctEditor& ed) { }
	partial gui::render() { return {}; }
	gui::gui(ctEditor& ed) : _ed(ed) { }
}
