#pragma once
#ifndef STORAGEBUFFER_H
#define STORAGEBUFFER_H

#include <memory>

#include "buffer.h"
#include "bufferView.h"

#include "typeEnums.h"

namespace citrus {
	namespace graphics {

		class storageView : public typedBufferView {
		public:
			storageView(buffer* buf, size_t _start, size_t _size, glslType _type) : typedBufferView(buf, _start, _size, _type) {}
			storageView(buffer* buf, size_t _start, size_t _size, const void* _data, glslType _type) : typedBufferView(buf, _start, _size, _data, _type) {}
		};

		template<typename T>
		class storageViewT : public storageView {
			static_assert(isStorageType<T>::value, "T must be a program type (check typeEnums.h)");
		public:

			void bufferSubData(size_t localStart, const std::vector<T>& _data) const {
				bufferSubData(localStart, _data.size() * sizeof(T), _data.data());
			}
			std::vector<T> readbackSubData(size_t localStart, size_t count) const {
				std::vector<T> res;
				res.resize(count);
				readbackSubData(localStart, count * elementSize(), res.data());
			}

			storageViewT(buffer* buf) : storageView(buf, 0, count * sizeof(T), glslTypeTraits<T>::type) {}
			//_start = index in bytes into _ref
			//_count = number of elements in data
			storageViewT(buffer* buf, size_t _start, size_t _count, const T* data = nullptr) : storageView(buf, _start, _count * sizeof(T), data, glslTypeTraits<T>::type) {}
			storageViewT(buffer* buf, size_t _start, const std::vector<T> data) : storageView(buf, _start, _count * sizeof(T), data, glslTypeTraits<T>::type) {}
		};

		//these are logically valid, but have no corresponding
		//type in the GLSL, so they should not be used
		//if you were to use them, the memory would be intractably
		//out of alignment
		/*
		using storageView1ub = storageViewT<unsigned char>;
		using storageView2ub = storageViewT<glm::tvec2<unsigned char>>;
		using storageView3ub = storageViewT<glm::tvec3<unsigned char>>;
		using storageView4ub = storageViewT<glm::tvec4<unsigned char>>;

		using storageView1b = storageViewT<char>;
		using storageView2b = storageViewT<glm::tvec2<char>>;
		using storageView3b = storageViewT<glm::tvec3<char>>;
		using storageView4b = storageViewT<glm::tvec4<char>>;

		using storageView1us = storageViewT<unsigned short>;
		using storageView2us = storageViewT<glm::tvec2<unsigned short>>;
		using storageView3us = storageViewT<glm::tvec3<unsigned short>>;
		using storageView4us = storageViewT<glm::tvec4<unsigned short>>;

		using storageView1s = storageViewT<short>;
		using storageView2s = storageViewT<glm::tvec2<short>>;
		using storageView3s = storageViewT<glm::tvec3<short>>;
		using storageView4s = storageViewT<glm::tvec4<short>>;
		*/

		using storageView1ui = storageViewT<unsigned int>;
		using storageView2ui = storageViewT<glm::uvec2>;
		using storageView3ui = storageViewT<glm::uvec3>;
		using storageView4ui = storageViewT<glm::uvec4>;

		using storageView1i = storageViewT<int>;
		using storageView2i = storageViewT<glm::ivec2>;
		using storageView3i = storageViewT<glm::ivec3>;
		using storageView4i = storageViewT<glm::ivec4>;

		using storageView1f = storageViewT<float>;
		using storageView2f = storageViewT<glm::vec2>;
		using storageView3f = storageViewT<glm::vec3>;
		using storageView4f = storageViewT<glm::vec4>;

		using storageView1d = storageViewT<double>;
		using storageView2d = storageViewT<glm::dvec2>;
		using storageView3d = storageViewT<glm::dvec3>;
		using storageView4d = storageViewT<glm::dvec4>;

		using storageView_mat2 = storageViewT<glm::mat2>;
		using storageView_mat3 = storageViewT<glm::mat3>;
		using storageView_mat4 = storageViewT<glm::mat4>;

		using storageView_dmat2 = storageViewT<glm::dmat2>;
		using storageView_dmat3 = storageViewT<glm::dmat3>;
		using storageView_dmat4 = storageViewT<glm::dmat4>;
	}
}

#endif