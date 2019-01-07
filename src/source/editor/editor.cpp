#include <editor/editor.h>

#include <engine/elements/renderManager.h>
#include <engine/elements/meshManager.h>

namespace citrus::editor {

	using ::citrus::engine::meshManager;

	void ctEditor::update(renderManager& rm, camera& cam) {
		bool clickedThisFrame = false;
		if(rm.eng()->getKey(graphics::windowInput::leftMouse)) {
			if(!dragged) {
				clickedThisFrame = true;
				dragged = true;
				startDragPx = rm.eng()->getWindow()->getCursorPos();
				startDrag = (vec2)startDragPx / (vec2)rm.eng()->getWindow()->framebufferSize() * 2.0f - 1.0f;
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
		graphics::shader& colorSh = *rm.drawable[2].sh;
		graphics::vertexArray& sphereVao = rm.eng()->getAllOfType<meshManager>()[0]->getModel(6);

		vec2 ssCursor = (vec2)rm.eng()->getWindow()->getCursorPos() / (vec2)rm.eng()->getWindow()->framebufferSize() * 2.0f - 1.0f;
		auto line = cam.getRayFromScreenSpace(vec2(ssCursor.x, -ssCursor.y));

		std::vector<entityRef> entities = rm.eng()->man->allEntities();

		rm.textFBO->bind();
		rm.textFBO->clearDepth();
		

		colorSh.use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glm::mat4 viewProjection = cam.getViewProjectionMatrix();
		for(int i = 0; i < entities.size()/* && i < 1*/; i++) {
			entityRef& r = entities[i];

			colorSh.setUniform("modelViewProjectionMat", viewProjection * glm::translate(r.getGlobalTransform().getPosition()) * glm::scale(glm::vec3(0.1f)));
			if(r == selected) {
				colorSh.setUniform("drawColor", glm::vec3(1.0f, 1.0f, 1.0f));
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				sphereVao.drawAll();
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} else {
				colorSh.setUniform("drawColor", glm::vec3(0.4f, 0.4f, 0.6f));
				sphereVao.drawAll();
			}
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		auto& arrowVao = rm.eng()->getAllOfType<meshManager>()[0]->getModel(7);
		if(selected) {
			glm::mat4 preTrans =
				viewProjection *
				glm::translate(selected.getGlobalTransform().getPosition()) *
				glm::toMat4(selected.getGlobalTransform().getOrientation());
			colorSh.setUniform("drawColor", glm::vec3(1.0f, 0.0f, 0.0f));
			colorSh.setUniform("modelViewProjectionMat",
				preTrans *
				glm::rotate(3.14159f / 2, glm::vec3(0.0f, 1.0f, 0.0f)));
			arrowVao.drawAll();

			colorSh.setUniform("drawColor", glm::vec3(0.0f, 1.0f, 0.0f));
			colorSh.setUniform("modelViewProjectionMat",
				preTrans *
				glm::rotate(-3.14159f / 2, glm::vec3(1.0f, 0.0f, 0.0f)));
			arrowVao.drawAll();

			colorSh.setUniform("drawColor", glm::vec3(0.0f, 0.0f, 1.0f));
			colorSh.setUniform("modelViewProjectionMat",
				preTrans *
				glm::rotate(3.14159f / 2, glm::vec3(0.0f, 0.0f, 1.0f)));
			arrowVao.drawAll();
		}


		rm.textFBO->bind();

		rm.rectshader->use();
		rm.rectshader->setUniform("screen", rm.eng()->getWindow()->framebufferSize());

		
		std::vector<editor::view> views;
		topBar->render(ivec2(0, 0), views, 0);

		if(selected) {
			auto gui = selected.renderGUI();
			gui->render(ivec2(0, topBar->dimensions().y), views, 0);
		}

		glDisable(GL_DEPTH_TEST);

		for(editor::view& v : views) {
			rm.rectshader->setUniform("position", v.loc);
			rm.rectshader->setUniform("size", v.size);
			rm.rectshader->setUniform("drawColor", v.color);
			rm.rectshader->setUniform("depth", v.depth);
			graphics::vertexArray::drawOne();
			rm.font.streamText(v.text, v.loc, glm::vec3(0, 0, 0), v.depth + 0.05f, rm.eng()->getWindow()->framebufferSize());
		}
	}

	ctEditor::ctEditor() {
		horiBar* c = new horiBar();

		button* fileButton = new button();
		fileButton->info = "File";
		c->buttons.emplace_back(fileButton);

		button* editButton = new button();
		editButton->info = "Edit";
		c->buttons.emplace_back(editButton);

		button* toolsButton = new button();
		toolsButton->info = "Tools";
		c->buttons.emplace_back(toolsButton);

		button* aboutButton = new button();
		aboutButton->info = "About";
		c->buttons.emplace_back(aboutButton);

		topBar.reset(c);
	}
}