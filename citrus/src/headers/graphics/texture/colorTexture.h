#pragma once

#ifndef COLORTEXTURE_H
#define COLORTEXTURE_H

#include "texture.h"

namespace citrus {
	namespace graphics {
		class colorTexture : public texture {
		protected:
			template<typename P>
			inline colorTexture(const imageT<P>& img, GLint internalFormat) : texture(internalFormat, img.glFormat(), img.glType(), img.width(), img.height(), img.data()) {}
		};

		template<GLint internalFormat>
		class colorTextureT : public colorTexture {
			static_assert(textureInternalFormatTraits<internalFormat>::isFormat && textureInternalFormatTraits<internalFormat>::isColor,
				"internalFormat must be a valid OpenGL internal texture format for color");

		public:
			template<typename P>
			inline colorTextureT(const imageT<P>& img) : colorTexture(img, internalFormat) {}
		};

		using texture1b = colorTextureT<GL_R8>;
		using texture2b = colorTextureT<GL_RG8>;
		using texture3b = colorTextureT<GL_RGB8>;
		using texture4b = colorTextureT<GL_RGBA8>;
						  
		using texture1f = colorTextureT<GL_R32F>;
		using texture2f = colorTextureT<GL_RG32F>;
		using texture3f = colorTextureT<GL_RGB32F>;
		using texture4f = colorTextureT<GL_RGBA32F>;
	}
}

#endif