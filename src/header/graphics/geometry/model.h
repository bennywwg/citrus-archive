#pragma once

#ifndef LOGICALMODEL_H
#define LOGICALMODEL_H

#include <memory>

#include "vertexarray.h"
#include "colorTexture.h"

namespace citrus {
	namespace graphics {
		struct textureAttachment {
			std::shared_ptr<colorTexture> tex;
			GLint unit;
			textureAttachment() = default;
			textureAttachment(std::shared_ptr<colorTexture> tex, GLint unit) : tex(tex), unit(unit) {}
		};

		class model {
			std::vector<textureAttachment> _textures;
			std::shared_ptr<vertexArray> _vao;
		public:
			void draw(GLint drawMode = GL_TRIANGLES) {
				for (size_t i = 0; i < _textures.size(); i++) _textures[i].tex->bind(_textures[i].unit); 
				_vao->drawAll(drawMode);
			}

			model(std::vector<textureAttachment> textures, std::shared_ptr<vertexArray> vao) : _textures(textures), _vao(vao) {}
		};
	}
}

#endif