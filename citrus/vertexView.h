#pragma once
#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <memory>

#include "buffer.h"
#include "bufferView.h"

#include "typeEnums.h"

namespace citrus {
	namespace graphics {
		class vertexView : public typedBufferView {
		public:
			vertexView(buffer* buf, size_t _start, size_t _size, glslType _type) : typedBufferView(buf, _start, _size, _type) {}
			vertexView(buffer* buf, size_t _start, size_t _size, const void* _data, glslType _type) : typedBufferView(buf, _start, _size, _data, _type) {}
		};

		template<typename T>
		class vertexViewT : public vertexView {
			static_assert(isVertexType<T>::value, "T must be a vertex type (check typeEnums.h)");
		public:

			void bufferSubData(size_t localStart, const std::vector<T>& _data) const {
				bufferSubData(localStart, _data.size() * sizeof(T), _data.data());
			}
			std::vector<T> readbackSubData(size_t localStart, size_t count) const {
				std::vector<T> res;
				res.resize(count);
				readbackSubData(localStart, count * elementSize(), res.data());
			}

			vertexViewT(buffer* buf) : vertexView(buf, 0, count * sizeof(T), glslTypeTraits<T>::type) {}
			//_start = index in bytes into _ref
			//_count = number of elements in data
			vertexViewT(buffer* buf, size_t _start, size_t _count, const T* data) : vertexView(buf, _start, _count * sizeof(T), data, glslTypeTraits<T>::type) {}
			vertexViewT(buffer* buf, size_t _start, const std::vector<T>& data) : vertexView(buf, _start, data.size() * sizeof(T), data.data(), glslTypeTraits<T>::type) {}
		};

		using vertexView1ui = vertexViewT<unsigned int>;
		using vertexView2ui = vertexViewT<glm::uvec2>;
		using vertexView3ui = vertexViewT<glm::uvec3>;
		using vertexView4ui = vertexViewT<glm::uvec4>;

		using vertexView1i = vertexViewT<int>;
		using vertexView2i = vertexViewT<glm::ivec2>;
		using vertexView3i = vertexViewT<glm::ivec3>;
		using vertexView4i = vertexViewT<glm::ivec4>;

		using vertexView1f = vertexViewT<float>;
		using vertexView2f = vertexViewT<glm::vec2>;
		using vertexView3f = vertexViewT<glm::vec3>;
		using vertexView4f = vertexViewT<glm::vec4>;

		using vertexView1d = vertexViewT<double>;
		using vertexView2d = vertexViewT<glm::dvec2>;
		using vertexView3d = vertexViewT<glm::dvec3>;
		using vertexView4d = vertexViewT<glm::dvec4>;
	}
}

#endif