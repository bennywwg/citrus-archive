#pragma once

#include <exception>

#include <graphics/image/image.h>
#include <util/typeEnums.h>

#include <util/graphics_h.h>

namespace citrus::graphics {
	class texture {
	protected:
		GLuint _ptr;

		GLint _internalFormat;
		size_t _width, _height;

	public:
		GLuint ptr() const {
			return _ptr;
		}

		void bind(unsigned int textureUnit) const {
			glActiveTexture(textureUnit);
			glBindTexture(GL_TEXTURE_2D, _ptr);
		}

		size_t getWidth() const {
			return _width;
		}
		size_t getHeight() const {
			return _height;
		}

		GLint getFormat() const {
			return _internalFormat;
		}

		texture(GLint internalFormat, GLint inputFormat, GLint inputType, unsigned int width, unsigned int height, const void* data = nullptr)
		: _width(width), _height(height), _internalFormat(internalFormat) {
			if (!isInternalFormat(internalFormat)) throw std::runtime_error("Not an internal format"); 
			if (!isInputFormat(inputFormat)) throw std::runtime_error("Not an input format");
			if (!isInputType(inputType)) throw std::runtime_error("Not an input type");
			glGenTextures(1, &_ptr);
			glBindTexture(GL_TEXTURE_2D, _ptr);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, inputFormat, inputType, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
		~texture() {
			glDeleteTextures(1, &_ptr);
		}
	private:
		texture(const texture& other) = delete;
		texture& operator=(const texture& other) = delete;
	};
}