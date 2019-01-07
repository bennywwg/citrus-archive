#include <graphics/buffer/buffer.h>

namespace citrus::graphics {
	GLuint buffer::ptr() const {
		return _ptr;
	}
	uint buffer::size() const {
		return _size;
	}
	void buffer::bufferSubData(uint start, uint size, const void* data) {
		glBindBuffer(GL_ARRAY_BUFFER, _ptr);
		glBufferSubData(GL_ARRAY_BUFFER, start, size, data);
	}
	void buffer::readbackSubData(uint start, uint size, void* data) {
		glBindBuffer(GL_ARRAY_BUFFER, _ptr);
		glGetBufferSubData(GL_ARRAY_BUFFER, start, size, data);
	}
	buffer::buffer(uint _size, const void* _data) : _size(_size) {
		glGenBuffers(1, &_ptr);
		glBindBuffer(GL_ARRAY_BUFFER, _ptr);
		glBufferData(GL_ARRAY_BUFFER, _size, _data, GL_STATIC_DRAW);
	}
	buffer::~buffer() {
		glDeleteBuffers(1, &_ptr);
	}


	GLuint rawBufferView::ptr() const {
		return _buf->ptr();
	}
	uint rawBufferView::start() const {
		return _start;
	}
	uint rawBufferView::size() const {
		return _size;
	}
	uint rawBufferView::end() const {
		return _start + _size;
	}
	void rawBufferView::bufferSubData(uint localStart, uint dataSize, const void * _data) const {
		_buf->bufferSubData(start() + localStart, dataSize, _data);
	}
	void rawBufferView::readbackSubData(uint localStart, uint size, void * data) const {
		_buf->readbackSubData(_start + localStart, size, data);
	}
	rawBufferView::rawBufferView(buffer * buf, uint _start, uint _size) : _buf(buf), _start(_start), _size(_size) {
		if(end() > _buf->size()) throw std::runtime_error("rawBufferView range is larger than referenced buffer");
	}
	rawBufferView::rawBufferView(buffer * buf, uint _start, uint _size, const void * _data) : rawBufferView(buf, _start, _size) {
		bufferSubData(0, size(), _data);
	}
	rawBufferView::rawBufferView(buffer * buf) : rawBufferView(buf, 0, buf->size()) { }


	glslType typedBufferView::type() const {
		return _type;
	}
	GLenum typedBufferView::glType() const {
		return gpuGLType(_type);
	}
	unsigned int typedBufferView::typeComponents() const {
		return glslTypeComponents(_type);
	}
	uint typedBufferView::elementSize() const {
		return glslTypeSize(_type);
	}
	uint typedBufferView::count() const {
		return size() / elementSize();
	}
	typedBufferView::typedBufferView(buffer * buf, uint _start, uint _size, glslType _type)
		: _type(_type), rawBufferView(buf, _start, _size) { }
	typedBufferView::typedBufferView(buffer * buf, uint _start, uint _size, const void * _data, glslType _type)
		: _type(_type), rawBufferView(buf, _start, _size, _data) { }


	vertexView::vertexView(buffer * buf, uint _start, uint _size, glslType _type) : typedBufferView(buf, _start, _size, _type) { }
	vertexView::vertexView(buffer * buf, uint _start, uint _size, const void * _data, glslType _type) : typedBufferView(buf, _start, _size, _data, _type) { }


	indexView::indexView(buffer * buf, uint _start, uint _size, glslType _type) : typedBufferView(buf, _start, _size, _type) { }
	indexView::indexView(buffer * buf, uint _start, uint _size, const void * _data, glslType _type) : typedBufferView(buf, _start, _size, _data, _type) { }


	storageView::storageView(buffer * buf, uint _start, uint _size, glslType _type) : typedBufferView(buf, _start, _size, _type) { }
	storageView::storageView(buffer * buf, uint _start, uint _size, const void * _data, glslType _type) : typedBufferView(buf, _start, _size, _data, _type) { }
}