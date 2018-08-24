#ifndef BUFFER_H
#define BUFFER_H

#include <glm\ext.hpp>

#include "graphics_h.h"

namespace citrus {
	namespace graphics {
		class buffer {
		private:
			GLuint _ptr;

			const size_t _size;
		public:
			GLuint ptr() const {
				return _ptr;
			}

			size_t size() const {
				return _size;
			}

			void bufferSubData(size_t start, size_t size, const void* data) {
				glBindBuffer(GL_ARRAY_BUFFER, _ptr);
				glBufferSubData(GL_ARRAY_BUFFER, start, size, data);
			}
			void readbackSubData(size_t start, size_t size, void* data) {
				glBindBuffer(GL_ARRAY_BUFFER, _ptr);
				glGetBufferSubData(GL_ARRAY_BUFFER, start, size, data);
			}

			buffer(size_t _size, const void* _data = nullptr) : _size(_size) {
				glGenBuffers(1, &_ptr);
				glBindBuffer(GL_ARRAY_BUFFER, _ptr);
				glBufferData(GL_ARRAY_BUFFER, _size, _data, GL_STATIC_DRAW);
			}
			~buffer() {
				glDeleteBuffers(1, &_ptr);
			}
		private:
			buffer(const buffer& other) = delete;
			buffer& operator=(const buffer& other) = delete;
		};
	}
}



#endif