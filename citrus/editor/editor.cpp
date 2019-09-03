#include "citrus/editor/editor.h"
#include "citrus/graphics/camera.h"
#include "citrus/engine/engine.h"
#include "citrus/graphics/system/immediatePass.h"

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
	}

	void ctEditor::update(graphics::camera& cam) {
		/*bool clickedThisFrame = false;
		if(rm.eng()->getKey(graphics::windowInput::leftMouse)) {
			if(!dragged) {
				clickedThisFrame = true;
				dragged = true;
				startDragPx = rm.eng()->getWindow()->getCursorPos();
				startDrag = (vec2)startDragPx / (vec2)rm.eng()->getWindow()->framebufferSize() * 2.0f - 1.0f;

				click(rm.eng()->getWindow()->getCursorPos());
			}
		} else {
			dragged = false;
		}

		auto line = cam.getRayFromScreenSpace(vec2(startDrag.x, -startDrag.y));

		std::vector<entityRef> entities = rm.eng()->man->allEntities();

		entityRef hovered;
		float nearest = std::numeric_limits<float>::max();

		for(int i = 0; i < entities.size(); i++) {
			entityRef& r = entities[i];

			geom::sphere s;
			s.p = r.getGlobalTransform().getPosition();
			s.r = 0.1f;

			geom::ray ray;
			ray.p = line.p0;
			ray.d = line.p1 - line.p0;

			geom::hit res = s.intersects(ray);

			if(res && res.dist() <= nearest) {
				hovered = r;
				nearest = res.dist();
			}
		}

		if(clickedThisFrame) selected = hovered;*/
	}

	void ctEditor::render(graphics::immediatePass & ipass) {
		ipass.groupings.clear();

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
