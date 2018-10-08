#pragma once
#ifndef BUFFERVIEW_H
#define BUFFERVIEW_H

#include <type_traits>
#include <memory>
#include <vector>

#include <util/graphics_h.h>
#include <graphics/buffer/buffer.h>
#include <util/typeEnums.h>

namespace citrus {
	namespace graphics {

		//a pointer to a buffer and a range of values
		class rawBufferView {
		private:
			buffer* const _buf;

			const size_t _start;
			const size_t _size;
		public:
			buffer* buf() const {
				return _buf;
			}
			GLuint ptr() const {
				return _buf->ptr();
			}

			size_t start() const {
				return _start;
			}
			size_t size() const {
				return _size;
			}
			size_t end() const {
				return _start + _size;
			}

			void bufferSubData(size_t localStart, size_t dataSize, const void* _data) const {
				_buf->bufferSubData(start() + localStart, dataSize, _data);
			}
			void readbackSubData(size_t localStart, size_t size, void* data) const {
				_buf->readbackSubData(_start + localStart, size, data);
			}

			template<typename T>
			void unsafe_bufferSubData(size_t localStart, const std::vector<T>& _data) const {
				bufferSubData(start() + localStart, _data.size() * sizeof(T), _data.data());
			}
			template<typename T>
			std::vector<T> unsafe_readbackSubData(size_t localStart, size_t size) const {
				if (size % sizeof(T) != 0) throw std::runtime_error("rawBufferView unsafe readback failed, incompatible element and request size");
				std::vector<T> res;
				res.resize(size / sizeof(T));
				readbackSubData(localStart, size, res.data());
			}

			rawBufferView(buffer* buf, size_t _start, size_t _size) : _buf(buf), _start(_start), _size(_size) {
				if (end() > _buf->size()) throw std::runtime_error("rawBufferView range is larger than referenced buffer");
			}
			rawBufferView(buffer* buf, size_t _start, size_t _size, const void* _data) : rawBufferView(buf, _start, _size) {
				bufferSubData(0, size(), _data);
			}
			rawBufferView(buffer* buf) : rawBufferView(buf, 0, buf->size()) {}
		};

		class typedBufferView : public rawBufferView {
		private:
			const glslType _type;

		public:
			glslType type() const {
				return _type;
			}
			GLenum glType() const {
				return gpuGLType(_type);
			}
			unsigned int typeComponents() const {
				return glslTypeComponents(_type);
			}
			size_t elementSize() const {
				return glslTypeSize(_type);
			}
			size_t count() const {
				return size() / elementSize();
			}

			typedBufferView(buffer* buf, size_t _start, size_t _size, glslType _type)
				: _type(_type), rawBufferView(buf, _start, _size) {}
			typedBufferView(buffer* buf, size_t _start, size_t _size, const void* _data, glslType _type)
				: _type(_type), rawBufferView(buf, _start, _size, _data) {}
		};
	}
}

#endif
