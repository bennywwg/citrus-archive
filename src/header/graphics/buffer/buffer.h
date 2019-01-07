#pragma once

#include <glm/ext.hpp>

#include <util/graphics_h.h>
#include <util/typeEnums.h>
#include <vector>

namespace citrus::graphics {
	using uint = unsigned int;

	class buffer {
	private:
		GLuint _ptr;

		const uint _size;
	public:
		GLuint ptr() const;
		uint size() const;
		void bufferSubData(uint start, uint size, const void* data);
		void readbackSubData(uint start, uint size, void* data);
		buffer(uint _size, const void* _data = nullptr);
		~buffer();
	private:
		buffer(const buffer& other) = delete;
		buffer& operator=(const buffer& other) = delete;
	};

	//a pointer to a buffer and a range of values
	class rawBufferView {
		private:
		buffer* const _buf;

		const uint _start;
		const uint _size;
		public:
		buffer* buf() const {
			return _buf;
		}
		GLuint ptr() const;

		uint start() const;
		uint size() const;
		uint end() const;

		void bufferSubData(uint localStart, uint dataSize, const void* _data) const;
		void readbackSubData(uint localStart, uint size, void* data) const;

		template<typename T>
		void unsafe_bufferSubData(uint localStart, const std::vector<T>& _data) const;
		template<typename T>
		std::vector<T> unsafe_readbackSubData(uint localStart, uint size) const;

		rawBufferView(buffer* buf, uint _start, uint _size);
		rawBufferView(buffer* buf, uint _start, uint _size, const void* _data);
		rawBufferView(buffer* buf);
	};

	class typedBufferView : public rawBufferView {
		private:
		const glslType _type;

		public:
		glslType type() const;
		GLenum glType() const;
		unsigned int typeComponents() const;
		uint elementSize() const;
		uint count() const;

		typedBufferView(buffer* buf, uint _start, uint _size, glslType _type);
		typedBufferView(buffer* buf, uint _start, uint _size, const void* _data, glslType _type);
	};

	class vertexView : public typedBufferView {
		public:
		vertexView(buffer* buf, uint _start, uint _size, glslType _type);
		vertexView(buffer* buf, uint _start, uint _size, const void* _data, glslType _type);
	};

	template<typename T>
	class vertexViewT : public vertexView {
		static_assert(isVertexType<T>::value, "T must be a vertex type (check typeEnums.h)");
		public:

		void bufferSubData(uint localStart, const std::vector<T>& _data) const;
		std::vector<T> readbackSubData(uint localStart, uint count) const;

		vertexViewT(buffer* buf);
		//_start = index in bytes into _ref
		//_count = number of elements in data
		vertexViewT(buffer* buf, uint _start, uint _count, const T* data);
		vertexViewT(buffer* buf, uint _start, const std::vector<T>& data);
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

	class indexView : public typedBufferView {
		public:
		indexView(buffer* buf, uint _start, uint _size, glslType _type);
		indexView(buffer* buf, uint _start, uint _size, const void* _data, glslType _type);
	};

	template<typename T>
	class indexViewT : public indexView {
		static_assert(isIndexType<T>::value, "T must be an index type (check typeEnums.h)");
		public:

		void bufferSubData(uint localStart, const std::vector<T>& _data) const;
		std::vector<T> readbackSubData(uint localStart, uint count) const;

		indexViewT(buffer* buf);
		//_start = index in bytes into _ref
		//_count = number of elements in data
		indexViewT(buffer* buf, uint _start, uint _count, const T* data);
		indexViewT(buffer* buf, uint _start, const std::vector<T>& data);
	};

	using indexViewub = indexViewT<unsigned char>;
	using indexViewus = indexViewT<unsigned short>;
	using indexViewui = indexViewT<unsigned int>;

	class storageView : public typedBufferView {
		public:
		storageView(buffer* buf, uint _start, uint _size, glslType _type);
		storageView(buffer* buf, uint _start, uint _size, const void* _data, glslType _type);
	};

	template<typename T>
	class storageViewT : public storageView {
		static_assert(isStorageType<T>::value, "T must be a program type (check typeEnums.h)");
		public:

		void bufferSubData(uint localStart, const std::vector<T>& _data) const;
		std::vector<T> readbackSubData(uint localStart, uint count) const;

		storageViewT(buffer* buf);
		//_start = index in bytes into _ref
		//_count = number of elements in data
		storageViewT(buffer* buf, uint _start, uint _count, const T* data = nullptr);
		storageViewT(buffer* buf, uint _start, const std::vector<T> data);
	};

	//these are logically valid, but have no corresponding
	//type in the GLSL, so they should not be used
	//if you were to use them, the memory would be
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