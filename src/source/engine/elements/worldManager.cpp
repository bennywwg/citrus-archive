#include <engine/elements/worldManager.h>
#include <engine/elements/rigidBodyComponent.h>
#include <engine/engine.h>
#include <engine/manager.inl>
#include <engine/elementRef.inl>

#include <graphics/framebuffer/standaloneFramebuffer.h>
#include <engine/elements/renderManager.h>
#include <engine/elements/meshManager.h>
#include <memory>

namespace citrus::engine {
	void worldManager::enableDebugDraw() {
		fbo = e->getAllOfType<renderManager>()[0]->meshFBO.get();
		debugShader.reset(new graphics::shader(
			util::loadEntireFile("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\shaders\\wireframe.vert"),
			util::loadEntireFile("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\shaders\\wireframe.frag"))
		);
	}

	void worldManager::disableDebugDraw() {
		fbo = nullptr;
	}

	void worldManager::preRender() {
		if(e->getKey(graphics::windowInput::equals)) {
			if(fbo == nullptr) enableDebugDraw();
		}

		if(e->getKey(graphics::windowInput::minus)) {
			disableDebugDraw();
		}


		auto list = e->getAllOfType<rigidBodyComponent>();
		for(auto& rbc : list) {
			if(rbc->ent.getGlobalTransform() != rbc->body->getTransform()) {
				auto et = rbc->ent.getGlobalTransform();
				auto bt = rbc->body->getTransform();
				rbc->body->setTransform(rbc->ent.getGlobalTransform());
			}
		}

		w->step();

		for(auto& rbc : list) {
			rbc->ent.setLocalTransform(rbc->body->getTransform());
		}
	}
	void worldManager::render() {
		if(fbo != nullptr) {
			auto man = e->getAllOfType<renderManager>()[0];
			const auto& sphere = e->getAllOfType<meshManager>()[0]->getModel(1);
			const auto& cube = e->getAllOfType<meshManager>()[0]->getModel(2);
			const auto& list = e->getAllOfType<rigidBodyComponent>();
			debugShader->use();
			debugShader->setUniform("lineColor", glm::vec3(1.0f, 0.0f, 0.0f));
			fbo->bind();
			for(auto& ele : list) {
				glm::mat4 model = ele->ent.getGlobalTransform().getMat();
				if(ele->isBox()) {
					auto size = ((btBoxShape*)ele->shape->ptr())->getHalfExtentsWithoutMargin();
					model = model * glm::scale(2.0f * glm::vec3(size.getX(), size.getY(), size.getZ()));
				}
				debugShader->setUniform("modelViewProjectionMat", man->camRef->cam.getViewProjectionMatrix() * model);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				if(ele->isSphere()) {
					sphere.drawAll();
				} else if(ele->isBox()) {
					cube.drawAll();
				}
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
	}
}
