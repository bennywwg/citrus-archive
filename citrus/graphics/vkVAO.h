#pragma once

#include <memory"
#include <vector"


#include <util/graphics_h.h"
#include <util/glmUtil.h"
#include <graphics/buffer/buffer.inl"

namespace citrus::graphics {
	struct staticVertex {
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 uv;
	};

	struct riggedVertex {
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 uv;
		int bone0;
		int bone1;
		float boneWeight0;
		float boneWeight1;
	};

	struct vertexAttribute {
		GLint location;
		bool normalized;
		std::unique_ptr<vertexView> buf;
		vertexAttribute(GLint loc, bool normalized, const vertexView& buf) : location(loc), normalized(normalized), buf(new vertexView(buf)) {}
		vertexAttribute(const vertexAttribute& other) : location(other.location), normalized(other.normalized), buf(new vertexView(*other.buf)) {}
	};

	class vertexArray {
	public:
		GLuint ptr() {
			return _ptr;
		}
		std::unique_ptr<indexView> getIndexBuffer() {
			return std::unique_ptr<indexView>(new indexView(*_indexbuffer));
		}
		std::vector<vertexAttribute> getAttributeBuffers() {
			return _attributebuffers;
		}

		void drawAll(GLint mode = GL_TRIANGLES) const {
			glBindVertexArray(_ptr);
			if (_indexbuffer) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexbuffer->ptr());
				size_t start = _indexbuffer->start();
				glDrawElements(mode, (GLsizei)_indexbuffer->count(), _indexbuffer->glType(), reinterpret_cast<void*>((size_t) _indexbuffer->start()));
			} else {
				glDrawArrays(mode, (GLint)_attributebuffers[0].buf->start(), (GLsizei)_attributebuffers[0].buf->count());
			}
			glBindVertexArray(0);
		}

		static void drawOne() {
			vertexArray ar;
			glBindVertexArray(ar._ptr);
			glDrawArrays(GL_POINTS, 0, 1);
			glBindVertexArray(0);
		}

		vertexArray() {
			glGenVertexArrays(1, &_ptr);
		}
		vertexArray(const std::vector<vertexAttribute>& attributes) : vertexArray() {
			//_attributebuffers = attributes;

			//C++ you've failed me!
			_attributebuffers.clear();
			for(const vertexAttribute& attrib : attributes) {
				vertexAttribute at(attrib);
				_attributebuffers.emplace_back(attrib);
			}

			glBindVertexArray(_ptr);

			for(vertexAttribute atc : _attributebuffers) {
				glEnableVertexAttribArray(atc.location);
				glBindBuffer(GL_ARRAY_BUFFER, atc.buf->ptr());

				GLint type = gpuGLType(atc.buf->type());
				if(type == GL_FLOAT) {
					glVertexAttribPointer(
						atc.location,
						atc.buf->typeComponents(),
						GL_FLOAT,
						atc.normalized ? GL_TRUE : GL_FALSE,
						0,
						reinterpret_cast<GLvoid*>((size_t) atc.buf->start())
					);
				} else if(type == GL_DOUBLE) {
					glVertexAttribLPointer(
						atc.location,
						atc.buf->typeComponents(),
						GL_DOUBLE,
						0,
						reinterpret_cast<GLvoid*>((size_t) atc.buf->start())
					);
				} else {
					glVertexAttribIPointer(
						atc.location,
						atc.buf->typeComponents(),
						type,
						0,
						reinterpret_cast<GLvoid*>((size_t) atc.buf->start())
					);
				}

				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			glBindVertexArray(0);

		}
		vertexArray(std::vector<vertexAttribute> attributes, const indexView& indices) : vertexArray(attributes) {
			
		}
		~vertexArray() {
			
		}
	private:
		vertexArray(const vertexArray& other) = delete;
		vertexArray& operator=(const vertexArray& other) = delete;
	};
}