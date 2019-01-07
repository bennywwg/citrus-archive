#pragma once

#ifndef TILEMAPFONT_H
#define TILEMAPFONT_H

#include <string>
#include <memory>

#include <graphics/texture/colorTexture.h>
#include <graphics/shader/shader.h>
#include <graphics/geometry/vertexarray.h>
#include <graphics/buffer/buffer.h>

namespace citrus {
	namespace engine {
		class tilemapFont {
			public:
			std::unique_ptr<graphics::texture3b> tex;
			std::unique_ptr<graphics::shader> sh;
			std::unique_ptr<graphics::buffer> buf;

			void streamText(std::string text, glm::ivec2 loc, glm::vec3 drawColor, float depth, glm::ivec2 screen);

			tilemapFont(std::string imagePath, int columns, int row);
		};
	}
}

#endif