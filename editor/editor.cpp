#include <stdlib.h>

#include <nfd.h>

#include "editor.h"
#include "../graphkern/camera.h"
#include "../graphkern/immediatePass.h"
#include "../mankern/manager.inl"
#include "../builtin/modelEle.h"

namespace citrus {
	std::string selectFile(std::string ext) {
		nfdchar_t* outPath = nullptr;
		nfdresult_t result = NFD_OpenDialog(ext.empty() ? nullptr : ext.c_str(), nullptr, &outPath);

		if (result == NFD_OKAY) {
			string res = outPath;
			free(outPath);
			return res;
		} else if (result == NFD_CANCEL) {
			return "";
		} else {
			return "";
		}
	}

	view* ctEditor::getHoveredView(ivec2 cursor) {
		view* topView = nullptr;
		float topViewDepth = -999.0f;
		for (auto& view : currentViews) {
			if (cursor.x >= view.loc.x && cursor.y >= view.loc.y && cursor.x <= (view.loc.x + view.size.x) && cursor.y <= (view.loc.y + view.size.y)) {
				if (topView == nullptr || view.depth > topViewDepth) {
					topView = &view;
					topViewDepth = view.depth;
				}
			}
		}

		return topView;
	}
	weak_ptr<floatingGui> ctEditor::getHoveredFloating(ivec2 cursor) {
		view* v = getHoveredView(cursor);
		if (!v) return weak_ptr<floatingGui>();
		for (int i = 0; i < floating.size(); i++) {
			if (floating[i] == v->owner.lock()) {
				return weak_ptr<floatingGui>(floating[i]);
			}
		}
		return weak_ptr<floatingGui>();
	}
	void ctEditor::clearFloating(view* hovered) {
		weak_ptr<gui> owner = hovered ? hovered->owner : weak_ptr<gui>();
		shared_ptr<floatingGui> hoveredFloating = owner.expired() ? shared_ptr<floatingGui>() : std::dynamic_pointer_cast<floatingGui>(owner.lock()->topLevelParent().lock());
		for (int i = 0; i < floating.size(); i++) {
			if (floating[i]->justCreated) {
				floating[i]->justCreated = false;
				continue;
			}
			if (floating[i]->shouldClose || (!floating[i]->shouldPin && floating[i] != hoveredFloating)) {
				floating.erase(floating.begin() + i);
				i--;
			}
		}
	}
	void ctEditor::renderAllGui() {
		currentViews.clear();
		topBar->render(ivec2(0, 0), currentViews, 0);
		for (auto& fl : floating) {
			fl->render(fl->pos, currentViews, 10.0f);
		}
	}
	void ctEditor::mouseDown(uint16_t const& selectedIndex) {
		view* topView = getHoveredView(cursorPx);

		draggedGui = getHoveredFloating(cursorPx);
		if (!draggedGui.expired()) {
			draggedGuiStart = draggedGui.lock()->pos;
		}

		if (topView) {
			if (!topView->owner.expired()) {
				topView->owner.lock()->mouseDown(cursorPx, topView->loc);
			}
		}

		clearFloating(topView);

		renderAllGui();

		// starting to drag entity
		if (transMap.find(selectedIndex) != transMap.end()) {
			draggingEntity = true;
			entityStartLocal = selected.getLocalPos();

			vec3 firstWorld = transMap[selectedIndex] * vec4(0, 0, 0, 1);
			vec3 secondWorld = transMap[selectedIndex] * vec4(1, 0, 0, 1);
			localTransDir = secondWorld - firstWorld;
			localTransDir = (selected.getParent() ? glm::inverse(selected.getParent().getGlobalTrans().getMat()) : glm::identity<glm::mat4>()) * vec4(localTransDir, 0.0f);
			localTransDir = glm::normalize(localTransDir);

			vec2 first = sys->frameCam.worldToScreen(firstWorld);
			vec2 second = sys->frameCam.worldToScreen(secondWorld);
			transDir = glm::normalize(second - first);
		}
	}
	void ctEditor::mouseUp(uint16_t const& selectedIndex) {
		view* topView = getHoveredView(cursorPx);

		draggedGui.reset();

		if (cursorPx == startDragPx && !topView) {
			selected = hovered;
		}

		draggingEntity = false;
	}

	bool ctEditor::anyCapturing() {
		if (topBar->captureInput()) return true;
		for (int i = 0; i < floating.size(); i++)
			if (floating[i]->captureInput()) return true;
		return false;
	}
	void ctEditor::keyDown(windowInput::button b) {
		for (int i = 0; i < floating.size(); i++) {
			if(floating[i]->captureInput()) floating[i]->keyDown(b);
		}
	}

	void ctEditor::updateCam() {
		if (!anyCapturing()) {

			float navSpeed = 0.1f;
			float rotSpeed = 0.01f;
			vec3 dir = vec3();

			// translation
			if (win->getKey(windowInput::w)) {
				dir.z -= navSpeed;
			}
			if (win->getKey(windowInput::a)) {
				dir.x -= navSpeed;
			}
			if (win->getKey(windowInput::s)) {
				dir.z += navSpeed;
			}
			if (win->getKey(windowInput::d)) {
				dir.x += navSpeed;
			}
			if (win->getKey(windowInput::q)) {
				dir.y -= navSpeed;
			}
			if (win->getKey(windowInput::e)) {
				dir.y += navSpeed;
			}

			// rotation
			if (win->getKey(windowInput::p)) {
				rx += rotSpeed;
			}
			if (win->getKey(windowInput::l)) {
				ry += rotSpeed;
			}
			if (win->getKey(windowInput::semicolon)) {
				rx -= rotSpeed;
			}
			if (win->getKey(windowInput::apostrophe)) {
				ry -= rotSpeed;
			}
			cam.ori = glm::toQuat(glm::rotate(ry, vec3(0.f, 1.f, 0.f)) * glm::rotate(rx, vec3(1.f, 0.f, 0.f)));
		
			vec3 camDir = vec3(glm::toMat4(cam.ori) * vec4(dir, 0.0f));
			cam.pos += camDir;
		}
	}
	void ctEditor::update(immediatePass& ipass, uint16_t const& selectedIndex) {
		auto const& items = man->ofType<modelEle>();
		hovered = entRef();
		for (auto const& ele : items) {
			if (sys->meshPasses[ele->materialIndex]->initialIndex + ele->itemIndex == selectedIndex) {
				hovered = ele->ent();
			}
		}

		if (!draggedGui.expired()) {
			draggedGui.lock()->pos = draggedGuiStart + (cursorPx - startDragPx);
			renderAllGui();
		}

		topBar->update(*this);
		for (auto& dd : floating) {
			dd->update(*this);
		}

		renderAllGui();

		/*std::lock_guard<std::mutex> lock(toExecMut);
		for (int i = 0; i < toExec.size(); i++) {
			try {
				auto res = lua.script(toExec[i]);
			}
			catch (std::runtime_error const& ex) {
				std::cout << ex.what() << "\n";
			}
		}
		toExec.clear();*/

		if (draggingEntity) {
			float firstDot = glm::dot(transDir, startDrag);
			float secondDot = glm::dot(transDir, cursor);
			selected.setLocalPos(entityStartLocal + (secondDot - firstDot) * localTransDir);
		}

		updateCam();

		sys->frameCam = cam;
	}

	void ctEditor::render(immediatePass& ipass) {
		ipass.groupings.clear();

		if (hovered) {
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addCube(vec3(0.1f, 0.1f, 0.1f));
				gp.tr = hovered.getGlobalTrans().getMatNoScale();
				gp.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
		if (selected) {
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addTorus(0.6f, 0.02f, 24, 6);
				gp.tr = selected.getGlobalTrans().getMatNoScale() * glm::rotate(glm::pi<float>() * 0.5f, vec3(0.0f, 1.0f, 0.0f));
				gp.color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
				ipass.groupings.size();
			}
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addTorus(0.6f, 0.02f, 24, 6);
				gp.tr = selected.getGlobalTrans().getMatNoScale() * glm::rotate(glm::pi<float>() * 0.5f, vec3(1.0f, 0.0f, 0.0f));
				gp.color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
			}
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addTorus(0.6f, 0.02f, 24, 6);
				gp.tr = selected.getGlobalTrans().getMatNoScale();
				gp.color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
			}

			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addArrow(0.025f, 1.0f, 8);
				gp.tr = selected.getGlobalTrans().getMatNoScale();// *glm::rotate(glm::pi<float>() * 0.5f, vec3(0.0f, 1.0f, 0.0f));
				gp.color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
				transMap[ipass.indexBits | (ipass.groupings.size() - 1)] = gp.tr;
			}
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addArrow(0.025f, 1.0f, 8);
				gp.tr = selected.getGlobalTrans().getMatNoScale() * glm::rotate(glm::pi<float>() * 0.5f, vec3(0.0f, 0.0f, 1.0f));
				gp.color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
				transMap[ipass.indexBits | (ipass.groupings.size() - 1)] = gp.tr;
			}
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addArrow(0.025f, 1.0f, 8);
				gp.tr = selected.getGlobalTrans().getMatNoScale() * glm::rotate(glm::pi<float>() * 0.5f, vec3(0.0f, 1.0f, 0.0f));
				gp.color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
				transMap[ipass.indexBits | (ipass.groupings.size() - 1)] = gp.tr;
			}
		} else {
			transMap.clear();
		}

		for (auto const& view : currentViews) {
			immediatePass::grouping gp = { };
			gp.color = view.color;
			gp.pixelspace = true;
			gp.tr = glm::identity<mat4>();

			gp.addText(view.text, 16, view.loc);

			ipass.groupings.emplace_back(gp);
		}
	}

	void ctEditor::addToScenceHierarchy(weak_ptr<floatingContainer> weakRef, entRef ent, int level) {
		auto bar = std::make_shared<linearLayout>();
		bar->direction = linearLayout::right;

		auto delButton = std::make_shared<button>();
		delButton->parent = bar;
		delButton->info = "del";
		delButton->onClick = [this, ent](button& b) {
			man->destroy(ent);
		};

		auto selectButton = std::make_shared<button>();
		selectButton->parent = bar;
		selectButton->info = "sel" + std::string(level * 2, '-');
		selectButton->onClick = [this, ent](button& b) {
			selected = ent;
		};

		auto nameField = std::make_shared<textField>();
		nameField->parent = bar;
		nameField->setState(ent.name());
		if (ent == selected) {
			nameField->color = vec3(1.0f, 0.5f, 0.5f);
		}

		bar->items = { delButton, selectButton, nameField };

		weakRef.lock()->items.push_back(bar);

		for (auto const& child : ent.getChildren()) {
			addToScenceHierarchy(weakRef, child, level + 1);
		}
	}
	void ctEditor::renderSceneHierarchy(weak_ptr<floatingContainer> weakRef) {
		weakRef.lock()->items.clear();

		auto createButton = std::make_shared<button>();
		createButton->parent = weakRef;
		createButton->info = "Create Entity";
		createButton->onClick = [this](button& b) {
			selected = man->create("Unnamed");
		};
		weakRef.lock()->items.push_back(createButton);

		auto ents = man->allEnts();
		for (entRef const& ent : ents) {
			if (!ent.getParent()) {
				addToScenceHierarchy(weakRef, ent, 0);
			}
		}
	}

	void ctEditor::renderEntityInspector(weak_ptr<floatingContainer> weakRef) {
		auto inspectorCont = weakRef.lock();

		std::shared_ptr<gui> name;
		if (inspectorCont->items.empty()) {
			auto name2 = std::make_shared<textField>();
			name2->parent = inspectorCont;
			name2->setState(selected ? selected.name() : "(No Selection)");
			name = name2;
		} else {
			name = inspectorCont->items[0];
		}

		inspectorCont->items.clear();

		auto vec = std::make_shared<vecField>();
		vec->parent = inspectorCont;

		auto ori = std::make_shared<vecField>();
		ori->parent = inspectorCont;
		ori->numComponents = 4;

		auto eleCreate = std::make_shared<button>();
		eleCreate->parent = inspectorCont;
		eleCreate->info = selected ? "Add Element" : "(No Selection)";
		eleCreate->onClick = [this](button& b) {
			if (selected) {
				vector<shared_ptr<button>> eles;
				for (int i = 0; i < man->_data.size(); i++) {
					auto *ei = man->_data[i].info;
					string str = ei->name;
					eles.push_back(button::create(str, [this, ei](button& b) {
						man->addElement(selected, ei);
					}));
				}

				auto eleDD = dropDown::create("Choose Element ... ", eles);

				floating.push_back(eleDD);
			}
		};

		inspectorCont->items.emplace_back(name);
		inspectorCont->items.emplace_back(eleCreate);
		inspectorCont->items.emplace_back(vec);
		inspectorCont->items.emplace_back(ori);

		if(selected) {
			vec->vec = vec4(selected.getGlobalTrans().getPosition(), 0.f);
			auto quat = selected.getGlobalTrans().getOrientation();
			ori->vec = vec4(quat.x, quat.y, quat.z, quat.w);
		} else {
			vec->vec = vec4(0, 0, 0, 0);
			ori->vec = vec4(0, 0, 0, 0);
		}

		if (selected) {	
			for (int i = 0; i < selected._ptr->eles.size(); i++) {
				auto* ei = selected._ptr->eles[i];
				auto tf = std::make_shared<textField>();
				tf->parent = inspectorCont;

				if (ei->_man) {
					tf->setState(man->getInfo(ei->_type)->name + "\n" + ei->serialize().dump(2));
				} else {
					tf->setState(man->getInfo(ei->_type)->name + "\n(Not Initialized)");
				}

				inspectorCont->items.emplace_back(tf);
			}
		}

	}

	/*S
	void ctEditor::setupShell() {
		lua.open_libraries(sol::lib::base, sol::lib::package);

		// clear terminal (windows only)
		lua["clear"] = []() {
			system("cls");
		};

		// load prefab
		lua["load"] = [this](string str) {
			auto st = loadEntireFile(str);
			auto js = json::parse(st);
			man->loadPrefab(js);
		};

		// save prefab
		lua["save"] = [this](string id, string str) {
			auto e = eng->man->findByID(id);
			if (e) {
				auto js = eng->man->savePrefab(e);
				auto st = js.dump();
				util::saveEntireFile(str, st);
			} else {
				util::sout("couldn't find id");
			}
		};

		// return selected entity id
		lua["selected"] = [this]() {
			return std::to_string(selected.id());
		};

		// select entity by id
		lua["select"] = [this](string id) {
			selected = eng->man->findByID(id);
		};

		// find by name
		lua["find"] = [this](string name) {
			return std::to_string(eng->man->findByName(name).id());
		};

		lua["parent"] = [this](string id) {
			auto er = eng->man->findByID(id);
			return std::to_string(er ? er.getParent().id() : entityRef::nullID);
		};

		lua["children"] = [this](string id) {
			auto er = eng->man->findByID(id);
			if (er) {
				auto vec = er.getChildren();
				std::vector<string> res; res.resize(vec.size());
				for (int i = 0; i < vec.size(); i++) {
					res[i] = std::to_string(vec[i].id());
				}
				return res;
			} else {
				return std::vector<string>();
			}
		};

		lua["exit"] = [this]() {
			eng->stop();
		};

		lua["setpos"] = [this](string id, float x, float y, float z) {
			auto er = eng->man->findByID(id);
			if (er) {
				er.setLocalPosition(vec3(x, y, z));
			}
		};
		
		lua["getpos"] = [this](string id) {
			auto er = eng->man->findByID(id);
			if (er) {
				auto vec = er.getLocalPosition();
				return vector<float> { vec.x, vec.y, vec.z };
			}
		};

		lua["destroy"] = [this](string id) {
			eng->man->destroy(eng->man->findByID(id));
		};
		
		lua["pause"] = [this]() {
			playing = false;
		};

		lua["play"] = [this]() {
			playing = true;
		};

		lua["step"] = [this]() {
			doFrame = true;
		};

		lua["elements"] = [this](string id) {
			auto ent = eng->man->findByID(id);
			for (auto const& ele : ent.raw().lock()->getElements()) {
				util::sout(ele->name() + "\n");
			}
			doFrame = true;
		};
	}

	void ctEditor::shell(std::string const& ex) {
		std::lock_guard<std::mutex> lock(toExecMut);
		toExec.push_back(ex);
	}
	*/
	ctEditor::ctEditor(manager *man, renderSystem *sys, window *win) : man(man), sys(sys), win(win) {
		//setupShell();

		topBar = std::make_shared<horiBar>();

		shared_ptr<button> fileButton = std::make_shared<button>();
		fileButton->info = "File";
		fileButton->parent = topBar;
		fileButton->onClick = [this](button& b) {
			auto saveButton = button::create("Save Scene", [](button& but) { std::cout << "Save Scene = " << selectFile("cts") << "\n"; } );
			auto loadButton = button::create("Load Scene", [](button& but) { std::cout << "Load Scene = " << selectFile("cts") << "\n"; } );
			auto exitButton = button::create("Exit", [this](button& but) { this->man->stop(); } );
			auto fileDropDown = dropDown::create("File->", { saveButton, loadButton, exitButton });
			floating.emplace_back(fileDropDown);
		};
		topBar->buttons.emplace_back(fileButton);

		shared_ptr<button> editButton = std::make_shared<button>();
		editButton->info = "Edit";
		editButton->parent = topBar;
		editButton->onClick = [this](button& b) {
			auto playButton = button::create("Play", [this](button& but) { playing = true; });
			auto pauseButton = button::create("Pause", [this](button& but) { playing = false; });
			auto editDropDown = dropDown::create("Edit->", { playButton, pauseButton });
			floating.emplace_back(editDropDown);
		};
		topBar->buttons.emplace_back(editButton);
		
		shared_ptr<button> toolsButton = std::make_shared<button>();
		toolsButton->info = "Tools";
		toolsButton->parent = topBar;
		toolsButton->onClick = [this](button& b) {
			auto inspectorButton = button::create("Inspector", [this](button& but) {
				auto inspectorCont = std::make_shared<floatingContainer>();
				inspectorCont->addButtons();
				inspectorCont->title = "Entity Inspector";
				inspectorCont->pos = this->cursorPx;

				inspectorCont->updateFunc = [this, inspectorCont](ctEditor& ed) {
					renderEntityInspector(inspectorCont);
				};

				floating.emplace_back(inspectorCont);
			});
			auto hierarchyButton = button::create("Scene Hierarchy", [this](button& but) {
				auto hierarchyCont = std::make_shared<floatingContainer>();
				weak_ptr<floatingContainer> weakRef = hierarchyCont;
				hierarchyCont->addButtons();
				hierarchyCont->title = "Scene Hierarchy";
				hierarchyCont->pos = this->cursorPx;

				hierarchyCont->updateFunc = [this, weakRef](ctEditor& ed) {
					renderSceneHierarchy(weakRef);
				};

				floating.emplace_back(hierarchyCont);
			});
			auto editDropDown = dropDown::create("Tools->", { inspectorButton, hierarchyButton });
			floating.emplace_back(editDropDown);
		};
		topBar->buttons.emplace_back(toolsButton);

		topBar->render(ivec2(0, 0), currentViews, 0);

		playing = true;
	}
}
