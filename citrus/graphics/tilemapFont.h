#pragma once

#ifndef TILEMAPFONT_H
#define TILEMAPFONT_H

#include <string>
#include <memory>
#include "citrus/util.h"

namespace citrus {
	namespace engine {
		class tilemapFont {
			public:

			void streamText(string text, ivec2 loc, vec3 drawColor, float depth, ivec2 screen);

			tilemapFont(string imagePath, int columns, int row);
		};
	}
}

#endif
