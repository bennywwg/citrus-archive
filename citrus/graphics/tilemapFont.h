#pragma once

#ifndef TILEMAPFONT_H
#define TILEMAPFONT_H

#include <string>
#include <memory>

namespace citrus {
	namespace engine {
		class tilemapFont {
			public:

			void streamText(std::string text, glm::ivec2 loc, glm::vec3 drawColor, float depth, glm::ivec2 screen);

			tilemapFont(std::string imagePath, int columns, int row);
		};
	}
}

#endif
