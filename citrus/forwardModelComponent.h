#pragma once

#ifndef FORWARDMODELCOMPONENT_H
#define FORWARDMODELCOMPONENT_H

#include "entity.h"
#include "entityComponent.h"
#include "transformComponent.h"

#include "vertexarray.h"
#include "texture.h"
#include "shader.h"
#include "framebuffer.h"

namespace citrus {
	namespace engine {
		class forwardModelComponent : public entityComponent {
			public:
			std::weak_ptr<geom::simpleModel> vao;
			std::weak_ptr<graphics::texture> tex;
			std::weak_ptr<graphics::shader> sh;

			GLint drawMode = GL_TRIANGLES;

			void draw() const {
				glm::mat4 modelMat = getOwner()->hasComponent<transformComponent>() ?
					modelMat = getOwner()->getComponent<transformComponent>().lock()->getTransform().getMat() :
					glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));

				tex.lock()->bind();
				sh.lock()->setUniform("modelViewProjectionMat", modelMat);
				vao.lock()->draw();

				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
			}

			forwardModelComponent(engine* engine, entity* owner) : entityComponent(engine, owner) { }
		};
	}
}

#endif