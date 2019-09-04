#include "citrus/editor/editor.h"
#include "citrus/graphics/camera.h"
#include "citrus/engine/engine.h"
#include "citrus/graphics/system/immediatePass.h"
#include "citrus/engine/meshFilter.h"
#include "citrus/engine/manager.inl"

namespace citrus::editor {
	void ctEditor::click(ivec2 cursor) {
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

		for (int i = 0; i < floating.size(); i++) {
			gui* cur = topView ? topView->owner : nullptr;
			auto& fl = floating[i];
			if (fl.persistent) continue;
			bool preserve = false;
			while (cur) {
				if (&(*fl.ele) == cur) {
					preserve = true;
				}
				cur = cur->parent;
			}
			if (!preserve) {
				floating.erase(floating.begin() + i);
				i--;
			}
		}

		if (topView && topView->owner) {
			topView->owner->mouseDown(cursor, topView->loc);
		}

		currentViews.clear();
		topBar->render(ivec2(0, 0), currentViews, 0);
		
		for (auto& fl : floating) {
			fl.ele->render(fl.pos, currentViews, fl.depth);
		}

		if (!topView) {
			selected = hovered;
		}
	}

	void ctEditor::update(uint16_t const& selectedIndex) {
		auto const& items = eng->man->ofType(typeid(engine::meshFilter));
		hovered = engine::entityRef();
		for (auto const& eler : items) {
			auto const& ele = (engine::meshFilter*)eler;
			if(eng->sys->meshPasses[ele->materialIndex]->initialIndex + ele->itemIndex == selectedIndex) {
				hovered = ele->ent();
			}
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
		horiBar* c = new horiBar();

		button* fileButton = new button();
		fileButton->info = "File";
		fileButton->parent = c;
		fileButton->onClick = [this](button& b) {
			this->floating.emplace_back();
			this->floating.back().depth = 100;
			this->floating.back().pos = this->startDragPx;
			this->floating.back().persistent = false;
			this->floating.back().ele.reset(new dropDown());
			auto& dd = *(dropDown*)&(*this->floating.back().ele);
			dd.title = "- File -";
			
			{
				dd.buttons.emplace_back(new button());
				dd.buttons.back()->info = "Save Scene";
				dd.buttons.back()->onClick = [this](button& but) {
					std::cout << "Save Scene = " << util::selectFile("cts") << "\n";
				};
				dd.buttons.back()->parent = &dd;
			} {
				dd.buttons.emplace_back(new button());
				dd.buttons.back()->info = "Load Scene";
				dd.buttons.back()->onClick = [this](button& but) {
					std::cout << "Load Scene = " << util::selectFile("cts") << "\n";
				};
				dd.buttons.back()->parent = &dd;
			} {
				dd.buttons.emplace_back(new button());
				dd.buttons.back()->info = "Exit";
				dd.buttons.back()->onClick = [this](button& but) {
					eng->stop();
				};
				dd.buttons.back()->parent = &dd;
			}
		};
		c->buttons.emplace_back(fileButton);

		button* editButton = new button();
		editButton->info = "Edit";
		editButton->parent = c;
		editButton->onClick = [this](button& b) {
			this->floating.emplace_back();
			this->floating.back().depth = 100;
			this->floating.back().pos = this->startDragPx;
			this->floating.back().persistent = false;
			this->floating.back().ele.reset(new dropDown());
			auto& dd = *(dropDown*)&(*this->floating.back().ele);
			dd.title = "- Edit -";

			{
				dd.buttons.emplace_back(new button());
				dd.buttons.back()->info = "Pause";
				dd.buttons.back()->onClick = [this](button& but) {
					this->playing = false;
				};
				dd.buttons.back()->parent = &dd;
			} {
				dd.buttons.emplace_back(new button());
				dd.buttons.back()->info = "Play";
				dd.buttons.back()->onClick = [this](button& but) {
					this->playing = true;
				};
				dd.buttons.back()->parent = &dd;
			} {
				dd.buttons.emplace_back(new button());
				dd.buttons.back()->info = "One Frame";
				dd.buttons.back()->onClick = [this](button& but) {
					this->doFrame = true;
				};
				dd.buttons.back()->parent = &dd;
			}
		};
		c->buttons.emplace_back(editButton);

		button* toolsButton = new button();
		toolsButton->info = "Tools";
		toolsButton->parent = c;
		toolsButton->onClick = [this](button& b) {
			this->floating.emplace_back();
			auto& ddCont = this->floating.back();
			ddCont.depth = 100;
			ddCont.pos = this->startDragPx;
			ddCont.persistent = false;
			ddCont.ele.reset(new dropDown());
			auto& dd = *(dropDown*)&(*ddCont.ele);
			dd.title = "- Tools -";

			{
				dd.buttons.emplace_back(new button());
				dd.buttons.back()->info = "Entity List";
				dd.buttons.back()->onClick = [this](button& but) {
					this->floating.emplace_back();
					auto& ddCont = this->floating.back();
					ddCont.depth = 50.0f;
					ddCont.pos = this->startDragPx;
					ddCont.persistent = true;
					ddCont.ele.reset(new dropDown());
					auto& dd = *(dropDown*)&(*ddCont.ele);
				};
				dd.buttons.back()->parent = &dd;
			} {
				dd.buttons.emplace_back(new button());
				dd.buttons.back()->info = "Inspector";
				dd.buttons.back()->onClick = [this](button& but) {
					
				};
				dd.buttons.back()->parent = &dd;
			} {
				dd.buttons.emplace_back(new button());
				dd.buttons.back()->info = "Profiler";
				dd.buttons.back()->onClick = [this](button& but) {
					util::sout("Nothing happened");
				};
				dd.buttons.back()->parent = &dd;
			}
		};
		c->buttons.emplace_back(toolsButton);

		button* aboutButton = new button();
		aboutButton->info = "About";
		aboutButton->parent = c;
		c->buttons.emplace_back(aboutButton);

		topBar.reset(c);
		topBar->render(ivec2(0, 0), currentViews, 0);

		playing = true;
	}
}
