#include "citrus/editor/editor.h"
#include "citrus/graphics/camera.h"
#include "citrus/engine/engine.h"
#include "citrus/graphics/system/immediatePass.h"
#include "citrus/engine/meshFilter.h"
#include "citrus/engine/manager.inl"

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
	weak_ptr<guiFloating> ctEditor::getHoveredFloating(ivec2 cursor) {
		view* v = getHoveredView(cursor);
		if (!v) return weak_ptr<guiFloating>();
		for (int i = 0; i < floating.size(); i++) {
			if (floating[i]->ele == v->owner.lock()) {
				return weak_ptr<guiFloating>(floating[i]);
			}
		}
		return weak_ptr<guiFloating>();
	}
	void ctEditor::clearFloating(view* hovered) {
		for (int i = 0; i < floating.size(); i++) {
			weak_ptr<gui> cur = hovered ? hovered->owner : weak_ptr<gui>();
			weak_ptr<dropDown> dd = cur.expired() ? weak_ptr<dropDown>() : (cur.lock()->topLevelParent().expired() ? weak_ptr<dropDown>() : std::dynamic_pointer_cast<dropDown>(cur.lock()->topLevelParent().lock()));
			auto& fl = floating[i];
			bool preserve = fl->persistent;
			while (!cur.expired()) {
				if (fl->ele == cur.lock()) {
					preserve = true;
				}
				cur = cur.lock()->parent;
			}
			if (!fl->justCreated && (!preserve || (!dd.expired() && dd.lock()->shouldClose))) {
				floating.erase(floating.begin() + i);
				i--;
			}
			fl->justCreated = false;
		}
	}
	void ctEditor::renderAllGui() {
		currentViews.clear();
		topBar->render(ivec2(0, 0), currentViews, 0);
		for (auto& fl : floating) {
			fl->ele->render(fl->pos, currentViews, fl->depth);
			auto ddc = std::dynamic_pointer_cast<dropDown>(fl->ele);
			if (ddc) {
				fl->persistent = ddc->shouldPin;
			}
		}
	}
	void ctEditor::mouseDown(ivec2 cursor) {
		view* topView = getHoveredView(cursor);
		
		draggedGui = getHoveredFloating(cursor);
		if (!draggedGui.expired()) {
			draggedGuiStart = draggedGui.lock()->pos;
		}

		if (topView) {
			if (!topView->owner.expired()) {
				topView->owner.lock()->mouseDown(cursor, topView->loc);
			}
		}

		clearFloating(topView);

		renderAllGui();
	}
	void ctEditor::mouseUp(ivec2 cursor) {
		view* topView = getHoveredView(cursor);

		draggedGui.reset();

		if (cursor == startDragPx && !topView) {
			selected = hovered;
		}
	}

	void ctEditor::update(ivec2 cursor, uint16_t const& selectedIndex) {
		auto const& items = eng->man->ofType(typeid(engine::meshFilter));
		hovered = engine::entityRef();
		for (auto const& eler : items) {
			auto const& ele = (engine::meshFilter*)eler;
			if(eng->sys->meshPasses[ele->materialIndex]->initialIndex + ele->itemIndex == selectedIndex) {
				hovered = ele->ent();
			}
		}

		if (!draggedGui.expired()) {
			draggedGui.lock()->pos = draggedGuiStart + (cursor - startDragPx);
			renderAllGui();
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
			}
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addArrow(0.025f, 1.0f, 8);
				gp.tr = selected.getGlobalTransform().getMatNoScale() *glm::rotate(glm::pi<float>() * 0.5f, vec3(0.0f, 0.0f, 1.0f));
				gp.color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
			}
			{
				ipass.groupings.push_back({});
				auto& gp = ipass.groupings.back();
				gp.addArrow(0.025f, 1.0f, 8);
				gp.tr = selected.getGlobalTransform().getMatNoScale() *glm::rotate(glm::pi<float>() * 0.5f, vec3(0.0f, 1.0f, 0.0f));
				gp.color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
			}
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

	ctEditor::ctEditor() {
		shared_ptr<horiBar> c(make_shared<horiBar>());

		shared_ptr<button> fileButton = make_shared<button>();
		fileButton->info = "File";
		fileButton->parent = c;
		fileButton->onClick = [this](button& b) {
			this->floating.emplace_back(std::make_shared<guiFloating>());
			this->floating.back()->depth = 100;
			this->floating.back()->pos = this->startDragPx;
			this->floating.back()->persistent = false;
			shared_ptr<dropDown> dd = make_shared<dropDown>();
			this->floating.back()->ele = dd;
			dd->title = "- File -";
			dd->addButtons();
			
			{
				dd->buttons.emplace_back(make_shared<button>());
				dd->buttons.back()->info = "Save Scene";
				dd->buttons.back()->onClick = [this](button& but) {
					std::cout << "Save Scene = " << util::selectFile("cts") << "\n";
				};
				dd->buttons.back()->parent = dd;
			} {
				dd->buttons.emplace_back(make_shared<button>());
				dd->buttons.back()->info = "Load Scene";
				dd->buttons.back()->onClick = [this](button& but) {
					std::cout << "Load Scene = " << util::selectFile("cts") << "\n";
				};
				dd->buttons.back()->parent = dd;
			} {
				dd->buttons.emplace_back(make_shared<button>());
				dd->buttons.back()->info = "Exit";
				dd->buttons.back()->onClick = [this](button& but) {
					eng->stop();
				};
				dd->buttons.back()->parent = dd;
			}
		};
		c->buttons.emplace_back(fileButton);

		shared_ptr<button> editButton = make_shared<button>();
		editButton->info = "Edit";
		editButton->parent = c;
		editButton->onClick = [this](button& b) {
			this->floating.emplace_back(std::make_shared<guiFloating>());
			this->floating.back()->depth = 100;
			this->floating.back()->pos = this->startDragPx;
			this->floating.back()->persistent = false;
			shared_ptr<dropDown> dd = make_shared<dropDown>();
			this->floating.back()->ele = dd;
			dd->title = "- Edit -";
			dd->addButtons();

			{
				dd->buttons.emplace_back(new button());
				dd->buttons.back()->info = "Pause";
				dd->buttons.back()->onClick = [this](button& but) {
					this->playing = false;
				};
				dd->buttons.back()->parent = dd;
			} {
				dd->buttons.emplace_back(new button());
				dd->buttons.back()->info = "Play";
				dd->buttons.back()->onClick = [this](button& but) {
					this->playing = true;
				};
				dd->buttons.back()->parent = dd;
			} {
				dd->buttons.emplace_back(new button());
				dd->buttons.back()->info = "One Frame";
				dd->buttons.back()->onClick = [this](button& but) {
					this->doFrame = true;
				};
				dd->buttons.back()->parent = dd;
			}
		};
		c->buttons.emplace_back(editButton);

		shared_ptr<button> toolsButton = make_shared<button>();
		toolsButton->info = "Tools";
		toolsButton->parent = c;
		toolsButton->onClick = [this](button& b) {
			this->floating.emplace_back(std::make_shared<guiFloating>());
			auto& ddCont = this->floating.back();
			ddCont->depth = 100;
			ddCont->pos = this->startDragPx;
			ddCont->persistent = false;
			shared_ptr<dropDown> dd = make_shared<dropDown>();
			ddCont->ele = dd;
			dd->title = "- Tools -";
			dd->addButtons();
			{
				dd->buttons.emplace_back(make_shared<button>());
				dd->buttons.back()->info = "Entity List";
				dd->buttons.back()->onClick = [this](button& but) {
					this->floating.emplace_back(std::make_shared<guiFloating>());
					auto& ddCont = this->floating.back();
					ddCont->depth = 50.0f;
					ddCont->pos = this->startDragPx;
					ddCont->persistent = true;
					ddCont->ele.reset(new dropDown());
					auto& dd = *(dropDown*)&(*ddCont->ele);
				};
				dd->buttons.back()->parent = dd;
			} {
				dd->buttons.emplace_back(make_shared<button>());
				dd->buttons.back()->info = "Inspector";
				dd->buttons.back()->onClick = [this](button& but) {
					
				};
				dd->buttons.back()->parent = dd;
			} {
				dd->buttons.emplace_back(make_shared<button>());
				dd->buttons.back()->info = "Profiler";
				dd->buttons.back()->onClick = [this](button& but) {
					util::sout("Nothing happened");
				};
				dd->buttons.back()->parent = dd;
			}
		};
		c->buttons.emplace_back(toolsButton);

		shared_ptr<button> aboutButton = make_shared<button>();
		aboutButton->info = "About";
		aboutButton->parent = c;
		c->buttons.emplace_back(aboutButton);

		topBar = c;
		topBar->render(ivec2(0, 0), currentViews, 0);

		playing = true;
	}
}
