#pragma once

#include <graphics/buffer/buffer.h>

namespace citrus::graphics {
	template<typename T>
	void rawBufferView::unsafe_bufferSubData(uint localStart, const std::vector<T>& _data) const {
		bufferSubData(start() + localStart, _data.size() * sizeof(T), _data.data());
	}
	template<typename T>
	std::vector<T> rawBufferView::unsafe_readbackSubData(uint localStart, uint size) const {
		if(size % sizeof(T) != 0) throw std::runtime_error("rawBufferView unsafe readback failed, incompatible element and request size");
		std::vector<T> res;
		res.resize(size / sizeof(T));
		readbackSubData(localStart, size, res.data());
	}


	template<typename T>
	void vertexViewT<T>::bufferSubData(uint localStart, const std::vector<T>& _data) const {
		bufferSubData(localStart, _data.size() * sizeof(T), _data.data());
	}
	template<typename T>
	std::vector<T> vertexViewT<T>::readbackSubData(uint localStart, uint count) const {
		std::vector<T> res;
		res.resize(count);
		readbackSubData(localStart, count * elementSize(), res.data());
	}
	template<typename T>
	vertexViewT<T>::vertexViewT(buffer* buf) : vertexView(buf, 0, count * sizeof(T), glslTypeTraits<T>::type) { }
	//_start = index in bytes into _ref
	//_count = number of elements in data
	template<typename T>
	vertexViewT<T>::vertexViewT(buffer* buf, uint _start, uint _count, const T* data) : vertexView(buf, _start, _count * sizeof(T), data, glslTypeTraits<T>::type) { }
	template<typename T>
	vertexViewT<T>::vertexViewT(buffer* buf, uint _start, const std::vector<T>& data) : vertexView(buf, _start, data.size() * sizeof(T), data.data(), glslTypeTraits<T>::type) { }


	template<typename T>
	void indexViewT<T>::bufferSubData(uint localStart, const std::vector<T>& _data) const {
		bufferSubData(localStart, _data.size() * sizeof(T), _data.data());
	}
	template<typename T>
	std::vector<T> indexViewT<T>::readbackSubData(uint localStart, uint count) const {
		std::vector<T> res;
		res.resize(count);
		readbackSubData(localStart, count * elementSize(), res.data());
	}
	template<typename T>
	indexViewT<T>::indexViewT(buffer* buf) : indexView(buf, 0, count * sizeof(T), glslTypeTraits<T>::type) { }
	//_start = index in bytes into _ref
	//_count = number of elements in data
	template<typename T>
	indexViewT<T>::indexViewT(buffer* buf, uint _start, uint _count, const T* data) : indexView(buf, _start, _count * sizeof(T), data, glslTypeTraits<T>::type) { }
	template<typename T>
	indexViewT<T>::indexViewT(buffer* buf, uint _start, const std::vector<T>& data) : indexView(buf, _start, data.size() * sizeof(T), data.data(), glslTypeTraits<T>::type) { }


	template<typename T>
	void storageViewT<T>::bufferSubData(uint localStart, const std::vector<T>& _data) const {
		bufferSubData(localStart, _data.size() * sizeof(T), _data.data());
	}
	template<typename T>
	std::vector<T> storageViewT<T>::readbackSubData(uint localStart, uint count) const {
		std::vector<T> res;
		res.resize(count);
		readbackSubData(localStart, count * elementSize(), res.data());
	}
	template<typename T>
	storageViewT<T>::storageViewT(buffer* buf) : storageView(buf, 0, count * sizeof(T), glslTypeTraits<T>::type) { }
	//_start = index in bytes into _ref
	//_count = number of elements in data
	template<typename T>
	storageViewT<T>::storageViewT(buffer* buf, uint _start, uint _count, const T* data) : storageView(buf, _start, _count * sizeof(T), data, glslTypeTraits<T>::type) { }
	template<typename T>
	storageViewT<T>::storageViewT(buffer* buf, uint _start, const std::vector<T> data) : storageView(buf, _start, _count * sizeof(T), data, glslTypeTraits<T>::type) { }
}