#include "citrus/editor/editor.h"

#include "citrus/engine/renderManager.h"

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

	void ctEditor::update(renderManager& rm, camera& cam) {
		bool clickedThisFrame = false;
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

		if(clickedThisFrame) selected = hovered;
	}

	void ctEditor::render(renderManager& rm, camera& cam) {
		//graphics::shader& colorSh = *rm.drawable[2].sh;
		//graphics::vertexArray& sphereVao = rm.eng()->getAllOfType<meshManager>()[0]->getModel(6);

		//vec2 ssCursor = (vec2)rm.eng()->getWindow()->getCursorPos() / (vec2)rm.eng()->getWindow()->framebufferSize() * 2.0f - 1.0f;
		//auto line = cam.getRayFromScreenSpace(vec2(ssCursor.x, -ssCursor.y));

		//std::vector<entityRef> entities = rm.eng()->man->allEntities();

		//rm.textFBO->bind();
		//rm.textFBO->clearDepth();
		//

		//colorSh.use();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glm::mat4 viewProjection = cam.getViewProjectionMatrix();
		//for(int i = 0; i < entities.size()/* && i < 1*/; i++) {
		//	entityRef& r = entities[i];

		//	colorSh.setUniform("modelViewProjectionMat", viewProjection * glm::translate(r.getGlobalTransform().getPosition()) * glm::scale(glm::vec3(0.1f)));
		//	if(r == selected) {
		//		colorSh.setUniform("drawColor", glm::vec3(1.0f, 1.0f, 1.0f));
		//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//		sphereVao.drawAll();
		//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//	} else {
		//		colorSh.setUniform("drawColor", glm::vec3(0.4f, 0.4f, 0.6f));
		//		sphereVao.drawAll();
		//	}
		//}
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//auto& arrowVao = rm.eng()->getAllOfType<meshManager>()[0]->getModel(7);
		//if(selected) {
		//	glm::mat4 preTrans =
		//		viewProjection *
		//		glm::translate(selected.getGlobalTransform().getPosition()) *
		//		glm::toMat4(selected.getGlobalTransform().getOrientation());
		//	colorSh.setUniform("drawColor", glm::vec3(1.0f, 0.0f, 0.0f));
		//	colorSh.setUniform("modelViewProjectionMat",
		//		preTrans *
		//		glm::rotate(3.14159f / 2, glm::vec3(0.0f, 1.0f, 0.0f)));
		//	arrowVao.drawAll();

		//	colorSh.setUniform("drawColor", glm::vec3(0.0f, 1.0f, 0.0f));
		//	colorSh.setUniform("modelViewProjectionMat",
		//		preTrans *
		//		glm::rotate(-3.14159f / 2, glm::vec3(1.0f, 0.0f, 0.0f)));
		//	arrowVao.drawAll();

		//	colorSh.setUniform("drawColor", glm::vec3(0.0f, 0.0f, 1.0f));
		//	colorSh.setUniform("modelViewProjectionMat",
		//		preTrans *
		//		glm::rotate(3.14159f / 2, glm::vec3(0.0f, 0.0f, 1.0f)));
		//	arrowVao.drawAll();
		//}


		//rm.textFBO->bind();

		//rm.rectshader->use();
		//rm.rectshader->setUniform("screen", rm.eng()->getWindow()->framebufferSize());
		//
		///*if(selected) {
		//	auto gui = selected.renderGUI();
		//	gui->render(ivec2(0, topBar->dimensions().y), views, 0);
		//}*/

		//glDisable(GL_DEPTH_TEST);

		//for(editor::view& v : currentViews) {
		//	rm.rectshader->setUniform("position", v.loc);
		//	rm.rectshader->setUniform("size", v.size);
		//	rm.rectshader->setUniform("drawColor", v.color);
		//	rm.rectshader->setUniform("depth", v.depth);
		//	graphics::vertexArray::drawOne();
		//	rm.font.streamText(v.text, v.loc + ivec2(margin, 0), glm::vec3(0, 0, 0), v.depth + 0.05f, rm.eng()->getWindow()->framebufferSize());
		//}
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
	}
}
