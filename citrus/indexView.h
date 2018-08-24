#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

#include "bufferView.h"

namespace citrus {
	namespace graphics {
		class indexView : public typedBufferView {
		public:
			indexView(buffer* buf, size_t _start, size_t _size, glslType _type) : typedBufferView(buf, _start, _size, _type) {}
			indexView(buffer* buf, size_t _start, size_t _size, const void* _data, glslType _type) : typedBufferView(buf, _start, _size, _data, _type) {}
		};

		template<typename T>
		class indexViewT : public indexView {
			static_assert(isIndexType<T>::value, "T must be an index type (check typeEnums.h)");
		public:

			void bufferSubData(size_t localStart, const std::vector<T>& _data) const {
				bufferSubData(localStart, _data.size() * sizeof(T), _data.data());
			}
			std::vector<T> readbackSubData(size_t localStart, size_t count) const {
				std::vector<T> res;
				res.resize(count);
				readbackSubData(localStart, count * elementSize(), res.data());
			}

			indexViewT(buffer* buf) : indexView(buf, 0, count * sizeof(T), glslTypeTraits<T>::type) {}
			//_start = index in bytes into _ref
			//_count = number of elements in data
			indexViewT(buffer* buf, size_t _start, size_t _count, const T* data) : indexView(buf, _start, _count * sizeof(T), data, glslTypeTraits<T>::type) {}
			indexViewT(buffer* buf, size_t _start, const std::vector<T>& data) : indexView(buf, _start, data.size() * sizeof(T), data.data(), glslTypeTraits<T>::type) {}
		};

		using indexViewub = indexViewT<unsigned char>;
		using indexViewus = indexViewT<unsigned short>;
		using indexViewui = indexViewT<unsigned int>;
	}
}

#endif