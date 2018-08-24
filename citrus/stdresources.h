#pragma once
#ifndef STDRESOURCES_H
#define STDRESOURCES_H

#include <memory>
#include <vector>

#include "vertexarray.h"
#include "vertexView.h"
#include "indexView.h"
#include "buffer.h"

namespace citrus {
	namespace graphics {
		class standardResourceContainer {
		public:
			std::shared_ptr<graphics::buffer> stdbuffer;

			graphics::vertexView3f quadVertices;
			graphics::vertexView2f quadUVs;
			graphics::indexViewui quadIndices;

			graphics::vertexArray quadVAO;

			standardResourceContainer() :
				stdbuffer(new graphics::buffer(2 * 1024 * 1024)),
				quadVertices(stdbuffer, 0, std::vector<glm::vec3>{
				glm::vec3(1.0f, 1.0f, 0.0f),
					glm::vec3(-1.0f, 1.0f, 0.0f),
					glm::vec3(-1.0f, -1.0f, 0.0f),
					glm::vec3(1.0f, -1.0f, 0.0f)
			}),
			quadIndices(stdbuffer, 1024, std::vector<unsigned int> {
				0, 1, 2,
					0, 2, 3
			}),
			quadUVs(stdbuffer, 2048, std::vector<glm::vec2> {
				glm::vec2(1.0f, 1.0f),
					glm::vec2(0.0f, 1.0f),
					glm::vec2(0.0f, 0.0f),
					glm::vec2(1.0f, 0.0f)
			}),
			quadVAO(std::vector < graphics::vertexAttribute>({
				graphics::vertexAttribute(0, false, quadVertices),
				graphics::vertexAttribute(2, false, quadUVs)
				}), quadIndices) {}
		};
	}
}

#endif