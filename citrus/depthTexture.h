#pragma once

#ifndef DEPTHTEXTURE_H
#define DEPTHTEXTURE_H

#include "graphics_h.h"

namespace citrus {
	namespace graphics {
		class depthTexture : public texture {
		protected:
			depthTexture(unsigned int width, unsigned int height, GLint internalFormat) : texture(internalFormat, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, width, height, nullptr) {}
		};

		template<GLint internalFormat>
		class depthTextureT : public depthTexture {
			static_assert(textureInternalFormatTraits<internalFormat>::isDepth,
				"internalFormat must be a valid OpenGL internal texture format for depth");

		public:
			depthTextureT(unsigned int width, unsigned int height) : depthTexture(width, height, internalFormat) {}
		};

		using depthTexture16 = depthTextureT<GL_DEPTH_COMPONENT16>;
		using depthTexture24 = depthTextureT<GL_DEPTH_COMPONENT24>;
		using depthTexture32 = depthTextureT<GL_DEPTH_COMPONENT32>;
		using depthTexture32f = depthTextureT<GL_DEPTH_COMPONENT32F>;
	}
}

#endif