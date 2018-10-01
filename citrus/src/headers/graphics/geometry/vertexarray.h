#pragma once

#ifndef VERTEX_ARRAY_OBJECT_H
#define VERTEX_ARRAY_OBJECT_H

#include <memory>
#include <vector>

#include <graphics/buffer/indexView.h>
#include <graphics/buffer/vertexView.h>
#include <util/glmUtil.h>

namespace citrus {
	namespace graphics {
		struct vertexAttribute {
			GLint location;
			bool normalized;
			std::unique_ptr<vertexView> buf;
			vertexAttribute(GLint loc, bool normalized, const vertexView& buf) : location(loc), normalized(normalized), buf(new vertexView(buf)) {}
			vertexAttribute(const vertexAttribute& other) : location(other.location), normalized(other.normalized), buf(new vertexView(*other.buf)) {}
		};

		class vertexArray {
		private:
			GLuint _ptr;

			std::unique_ptr<indexView> _indexbuffer;
			std::vector<vertexAttribute> _attributebuffers;

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

			void drawAll(GLint mode = GL_TRIANGLES) {
				glBindVertexArray(_ptr);
				if (_indexbuffer) {
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexbuffer->ptr());
					size_t start = _indexbuffer->start();
					glDrawElements(mode, (GLsizei)_indexbuffer->count(), _indexbuffer->glType(), (void*)_indexbuffer->start());
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
							(void*)atc.buf->start()
						);
					} else if(type == GL_DOUBLE) {
						glVertexAttribLPointer(
							atc.location,
							atc.buf->typeComponents(),
							GL_DOUBLE,
							0,
							(void*)atc.buf->start()
						);
					} else {
						glVertexAttribIPointer(
							atc.location,
							atc.buf->typeComponents(),
							type,
							0,
							(void*)atc.buf->start()
						);
					}

					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}

				glBindVertexArray(0);

			}
			vertexArray(std::vector<vertexAttribute> attributes, const indexView& indices) : vertexArray(attributes) {
				if (attributes.size() == 0) throw std::exception("Must have at least one vertex attribute buffer");

				_indexbuffer = std::make_unique<indexView>(indices);
			}
			~vertexArray() {
				glDeleteVertexArrays(1, &_ptr);
			}
		private:
			vertexArray(const vertexArray& other) = delete;
			vertexArray& operator=(const vertexArray& other) = delete;
		};
	}
}

#endif