#include "citrus/engine/worldManager.h"
#include "citrus/engine/rigidBodyComponent.h"
#include "citrus/engine/sensorEle.h"
#include "citrus/engine/engine.h"
#include "citrus/engine/manager.inl"
#include "citrus/engine/elementRef.inl"

#include <memory>

namespace citrus::engine {
	/*void worldManager::enableDebugDraw() {
		fbo = eng()->getAllOfType<renderManager>()[0]->meshFBO.get();
		debugShader.reset(new graphics::shader(
			util::loadEntireFile("/home/benny/Desktop/folder/citrus/res/shaders/wireframe.vert"),
			util::loadEntireFile("/home/benny/Desktop/folder/citrus/res/shaders/wireframe.frag"))
		);
	}*/

	/*void worldManager::disableDebugDraw() {
		fbo = nullptr;
	}*/

	void worldManager::preRender() {
		if(eng()->getKey(graphics::windowInput::equals)) {
			//if(fbo == nullptr) enableDebugDraw();
		}

		if(eng()->getKey(graphics::windowInput::minus)) {
			//disableDebugDraw();
		}


		auto list = eng()->getAllOfType<rigidBodyComponent>();
		for (auto& rbc : list) {
			if (rbc->ent().getGlobalTransform() != rbc->body->getTransform()) {
				rbc->body->setTransform(rbc->ent().getGlobalTransform());
			}
		}

		auto sensors = eng()->getAllOfType<sensorEle>();
		for (auto& sen : sensors) {
			sen->sense->setTransform(sen->ent().getGlobalTransform());
		}

		w->step();

		for(auto& rbc : list) {
			rbc->ent().setLocalTransform(rbc->body->getTransform());
		}
	}
	void worldManager::render() {
		/*if(fbo != nullptr) {
			auto man = eng()->getAllOfType<renderManager>()[0];
			const auto& sphere = eng()->getAllOfType<meshManager>()[0]->getModel(1);
			const auto& cube = eng()->getAllOfType<meshManager>()[0]->getModel(2);
			const auto& list = eng()->getAllOfType<rigidBodyComponent>();
			const auto& sensors = eng()->getAllOfType<sensorEle>();
			glEnable(GL_DEPTH_TEST);
			debugShader->use();
			debugShader->setUniform("lineColor", glm::vec3(1.0f, 0.0f, 0.0f));
			fbo->bind();
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			for (auto& ele : list) {
				glm::mat4 model = ele->ent().getGlobalTransform().getMat();
				if (ele->isBox()) {
					auto size = ((btBoxShape*)ele->shape->ptr())->getHalfExtentsWithoutMargin();
					model = model * glm::scale(2.0f * glm::vec3(size.getX(), size.getY(), size.getZ()));
				}
				debugShader->setUniform("modelViewProjectionMat", man->camRef->cam.getViewProjectionMatrix() * model);
				if (ele->isSphere()) {
					sphere.drawAll();
				}
				else if (ele->isBox()) {
					cube.drawAll();
				}
			}

			debugShader->setUniform("lineColor", glm::vec3(0.0f, 1.0f, 0.0f));
			for (auto& ele : sensors) {
				glm::mat4 model = ele->ent().getGlobalTransform().getMat();
				if (ele->isBox()) {
					auto size = ((btBoxShape*)ele->shape->ptr())->getHalfExtentsWithoutMargin();
					model = model * glm::scale(2.0f * glm::vec3(size.getX(), size.getY(), size.getZ()));
				}
				debugShader->setUniform("modelViewProjectionMat", man->camRef->cam.getViewProjectionMatrix() * model);
				if (ele->isSphere()) {
					sphere.drawAll();
				}
				else if (ele->isBox()) {
					cube.drawAll();
				}
			}

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}*/
	}
	
	worldManager::worldManager(entityRef e) : element(e, typeid(worldManager)), w(new dynamics::world()) { }
}
