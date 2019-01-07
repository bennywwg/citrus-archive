#pragma once

#ifndef STANDALONEFRAMEBUFFER_H
#define STANDALONEFRAMEBUFFER_H

#include "framebuffer.h"
#include <memory>

namespace citrus {
	namespace graphics {
		//same as frameBuffer but it creates and manages the textures objects
		class simpleFrameBuffer : public frameBuffer {

			std::unique_ptr<colorTexture> _color;
			std::unique_ptr<depthTexture> _depth;

			public:
			inline simpleFrameBuffer(unsigned int width, unsigned int height, bool hasAlpha = false) :
				_color(getColors()[0].tex),
				_depth(getDepth()),
				frameBuffer( { colorAttachment(hasAlpha ? (colorTexture*) new texture4b(image4b(width, height)) : (colorTexture*) new texture3b(image3b(width, height)), 0) }, new depthTexture24(width, height), width, height) { }
		};
	}
}

#endif