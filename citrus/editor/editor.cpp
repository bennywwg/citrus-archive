#include "citrus/editor/editor.h"
#include "citrus/graphics/camera.h"
#include "citrus/engine/engine.h"
#include "citrus/graphics/system/immediatePass.h"
#include "citrus/engine/meshFilter.h"
#include "citrus/engine/manager.inl"
#include <stdlib.h>

namespace citrus::editor {
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
			entityStartLocal = selected.getLocalPosition();

			vec3 firstWorld = transMap[selectedIndex] * vec4(0, 0, 0, 1);
			vec3 secondWorld = transMap[selectedIndex] * vec4(1, 0, 0, 1);
			localTransDir = secondWorld - firstWorld;
			localTransDir = (selected.getParent() ? glm::inverse(selected.getParent().getGlobalTransform().getMat()) : glm::identity<glm::mat4>()) * vec4(localTransDir, 0.0f);
			localTransDir = glm::normalize(localTransDir);

			vec2 first = eng->sys->frameCam.worldToScreen(firstWorld);
			vec2 second = eng->sys->frameCam.worldToScreen(secondWorld);
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

	void ctEditor::update(graphics::immediatePass& ipass, uint16_t const& selectedIndex) {
		auto const& items = eng->man->ofType(typeid(engine::meshFilter));
		hovered = engine::entityRef();
		for (auto const& eler : items) {
			auto const& ele = (engine::meshFilter*)eler;
			if(eng->sys->meshPasses[ele->materialIndex]->initialIndex + ele->itemIndex == selectedIndex) {
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
		
		std::lock_guard<std::mutex> lock(toExecMut);
		for (int i = 0; i < toExec.size(); i++) {
			try {
				auto res = lua.script(toExec[i]);
			}
			catch (std::runtime_error const& ex) {
				util::sout(ex.what());
			}
		}
		toExec.clear();

		if (draggingEntity) {
			float firstDot = glm::dot(transDir, startDrag);
			float secondDot = glm::dot(transDir, cursor);
			selected.setLocalPosition(entityStartLocal + (secondDot - firstDot) * localTransDir);
		}
	}

	void ctEditor::render(graphics::immediatePass & ipass) {
		ipass.groupings.clear();

		if (hovered) {
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addCube(vec3(0.1f, 0.1f, 0.1f));
				gp.tr = hovered.getGlobalTransform().getMatNoScale();
				gp.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
		if(selected) {
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addTorus(0.6f, 0.02f, 24, 6);
				gp.tr = selected.getGlobalTransform().getMatNoScale() * glm::rotate(glm::pi<float>() * 0.5f, vec3(0.0f, 1.0f, 0.0f));
				gp.color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
				ipass.groupings.size();
			}
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addTorus(0.6f, 0.02f, 24, 6);
				gp.tr = selected.getGlobalTransform().getMatNoScale() * glm::rotate(glm::pi<float>() * 0.5f, vec3(1.0f, 0.0f, 0.0f));
				gp.color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
			}
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addTorus(0.6f, 0.02f, 24, 6);
				gp.tr = selected.getGlobalTransform().getMatNoScale();
				gp.color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
			}
			
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addArrow(0.025f, 1.0f, 8);
				gp.tr = selected.getGlobalTransform().getMatNoScale();// *glm::rotate(glm::pi<float>() * 0.5f, vec3(0.0f, 1.0f, 0.0f));
				gp.color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
				transMap[ipass.indexBits | (ipass.groupings.size() - 1)] = gp.tr;
			}
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addArrow(0.025f, 1.0f, 8);
				gp.tr = selected.getGlobalTransform().getMatNoScale() *glm::rotate(glm::pi<float>() * 0.5f, vec3(0.0f, 0.0f, 1.0f));
				gp.color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
				transMap[ipass.indexBits | (ipass.groupings.size() - 1)] = gp.tr;
			}
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addArrow(0.025f, 1.0f, 8);
				gp.tr = selected.getGlobalTransform().getMatNoScale() *glm::rotate(glm::pi<float>() * 0.5f, vec3(0.0f, 1.0f, 0.0f));
				gp.color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
				transMap[ipass.indexBits | (ipass.groupings.size() - 1)] = gp.tr;
			}
		} else {
			transMap.clear();
		}

		for (auto const& view : currentViews) {
			graphics::immediatePass::grouping gp = { };
			gp.color = view.color;
			gp.pixelspace = true;
			gp.tr = glm::identity<mat4>();

			gp.addText(view.text, 16, view.loc);

			ipass.groupings.emplace_back(gp);
		}
	}

	void ctEditor::setupShell() {
		lua.open_libraries(sol::lib::base, sol::lib::package);

		// clear terminal (windows only)
		lua["clear"] = []() {
			system("cls");
		};

		// load prefab
		lua["load"] = [this](string str) {
			auto st = util::loadEntireFile(str);
			auto js = json::parse(st);
			eng->man->loadPrefab(js);
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

	ctEditor::ctEditor() {
		setupShell();

		topBar = make_shared<horiBar>();

		shared_ptr<button> fileButton = make_shared<button>();
		fileButton->info = "File";
		fileButton->parent = topBar;
		fileButton->onClick = [this](button& b) {
			auto saveButton = button::create("Save Scene", [](button& but) { std::cout << "Save Scene = " << util::selectFile("cts") << "\n"; } );
			auto loadButton = button::create("Load Scene", [](button& but) { std::cout << "Load Scene = " << util::selectFile("cts") << "\n"; } );
			auto exitButton = button::create("Exit", [this](button& but) { eng->stop(); } );
			auto fileDropDown = dropDown::create("File->", { saveButton, loadButton, exitButton });
			floating.emplace_back(fileDropDown);
		};
		topBar->buttons.emplace_back(fileButton);

		shared_ptr<button> editButton = make_shared<button>();
		editButton->info = "Edit";
		editButton->parent = topBar;
		editButton->onClick = [this](button& b) {
			auto playButton = button::create("Play", [this](button& but) { playing = true; });
			auto pauseButton = button::create("Pause", [this](button& but) { playing = false; });
			auto editDropDown = dropDown::create("Edit->", { playButton, pauseButton });
			floating.emplace_back(editDropDown);
		};
		topBar->buttons.emplace_back(editButton);
		
		shared_ptr<button> toolsButton = make_shared<button>();
		toolsButton->info = "Tools";
		toolsButton->parent = topBar;
		toolsButton->onClick = [this](button& b) {
			auto inspectorButton = button::create("Inspector", [this](button& but) {
				auto inspectorCont = make_shared<floatingContainer>();
				inspectorCont->addButtons();
				inspectorCont->title = "Entity Inspector";

				auto name = make_shared<textField>();
				name->parent = inspectorCont;

				auto vec = make_shared<vecField>();
				vec->parent = inspectorCont;

				auto ori = make_shared<vecField>();
				ori->parent = inspectorCont;
				ori->numComponents = 4;

				inspectorCont->items.emplace_back(name);
				inspectorCont->items.emplace_back(vec);
				inspectorCont->items.emplace_back(ori);

				floating.emplace_back(inspectorCont);

				inspectorCont->updateFunc = [name, vec, ori](ctEditor& ed) {
					if (ed.selected) {
						name->info = ed.selected.name();
						vec->vec = vec4(ed.selected.getGlobalTransform().getPosition(), 0.f);
						auto quat = ed.selected.getGlobalTransform().getOrientation();
						ori->vec = vec4(quat.x, quat.y, quat.z, quat.w);
					}
					else {
						name->info = "(No Selection)";
						vec->vec = vec4(0, 0, 0, 0);
						ori->vec = vec4(0, 0, 0, 0);
					}

				};
			});
			auto hierarchyButton = button::create("Inspector", [this](button& but) {
				auto hierarchyCont = make_shared<floatingContainer>();
				weak_ptr<floatingContainer> weakRef = hierarchyCont;
				hierarchyCont->addButtons();
				hierarchyCont->title = "Scene Hierarchy";

				hierarchyCont->updateFunc = [this, weakRef](ctEditor& ed) {
					weakRef.lock()->items.clear();
					auto txt = make_shared<textField>();
					weakRef.lock()->items.push_back(txt);
					txt->info = "abc";
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
