#include "citrus/graphics/tilemapFont.h"
#include "citrus/util.h"

namespace citrus::engine {
	void tilemapFont::streamText(std::string text, glm::ivec2 loc, glm::vec3 drawColor, float depth, glm::ivec2 screen) {
		/*std:vector<glm::ivec2> positions;
		std::vector<glm::vec2> tiles;
		positions.reserve(text.size());
		tiles.reserve(text.size());
		int column = 0;
		int row = 0;
		for(uint32_t i = 0; i < text.size(); i++) {
			if(text[i] == '\n') {
				row++;
				column = 0;	
			} else {
				positions.push_back(loc + glm::ivec2(column * 8, row * 16));
				tiles.push_back(glm::vec2((((unsigned char)text[i]) % 16 + 0.5f) / 16.0f, -(((unsigned char)text[i]) / 16 + 0.5f) / 16.0f));
				column++;
			}
		}

		sh->use();
		sh->setUniform("tileX", 1.0f / 32.0f);
		sh->setUniform("tileY", 1.0f / 32.0f);
		sh->setUniform("depth", depth);
		sh->setUniform("drawColor", drawColor);
		sh->setUniform("ssX", 2.0f * 16.0f / (float)screen.x);
		sh->setUniform("ssY", 2.0f * 16.0f / (float)screen.y);
		sh->setUniform("screen", screen);
		sh->setSampler("fontTex", *tex.get());

		graphics::vertexView2i pos(buf.get(), 0, positions);
		graphics::vertexView2f tile(buf.get(), positions.size() * sizeof(glm::ivec2), tiles);

		graphics::vertexArray vao({graphics::vertexAttribute(0, false, pos), graphics::vertexAttribute(1, false, tile)});

		vao.drawAll(GL_POINTS);

		sh->unuse();*/
	}
	tilemapFont::tilemapFont(std::string imagePath, int columns, int row) {
		/*graphics::image3b texImage(imagePath);

		buf = std::make_unique<graphics::buffer>(1024 * 1024);

		tex = std::make_unique<graphics::texture3b>(texImage);

		std::string resDir = "/home/benny/Desktop/folder/citrus/res/";
		sh = std::make_unique<graphics::shader>(
			util::loadEntireFile(resDir + "shaders/font.vert"),
			util::loadEntireFile(resDir + "shaders/font.geom"),
			util::loadEntireFile(resDir + "shaders/font.frag")
		);*/

		/*sh = std::make_unique<graphics::shader>(
			"#version 450\n"
			""
			"layout(location = 0) in ivec2 vert_position;\n"
			"layout(location = 1) in vec2 vert_tile;\n"
			""
			"uniform ivec2 screen;\n"
			""
			"out vec2 gPos;\n"
			"out vec2 gTile;\n"
			""
			"void main() {\n"
			"  vec2 sspos = vec2(float(vert_position.x) / float(screen.x) * 2 - 1, float(vert_position.y) / float(screen.y) * -2 + 1);\n"
			"  sspos = vec2(0, 0);\n"
			"  gl_Position = vec4(sspos, 0.0, 1.0);\n"
			"  gPos = sspos;\n"
			"  gTile = vert_tile;\n"
			"}\n",

			"#version 450\n"
			""
			"layout(points) in;\n"
			"layout(triangle_strip, max_vertices = 6) out;\n"
			""
			"uniform float tileX;\n"
			"uniform float tileY;\n"
			"uniform float ssX;\n"
			"uniform float ssY;\n"
			""
			"in vec2 gPos[];\n"
			"in vec2 gTile[];\n"
			""
			"out vec2 fTile;\n"
			""
			"void main() {\n"
			"  vec2 start = vec2(0, 0);\n"
			"  gl_Position = vec4(start + vec2(0, 0), 0.0, 1.0);\n"
			"  fTile = gTile[0] + vec2(tileX, -tileY);\n"
			"  EmitVertex();\n"
			"  gl_Position = vec4(start + vec2(0, -ssY), 0.0, 1.0);\n"
			"  fTile = gTile[0] + vec2(-tileX, -tileY);\n"
			"  EmitVertex();\n"
			"  gl_Position = vec4(start + vec2(ssX, 0), 0.0, 1.0);\n"
			"  fTile = gTile[0] + vec2(tileX, tileY);\n"
			"  EmitVertex();\n"
			"  EndPrimitive();\n"
			""
			"  gl_Position = vec4(start + vec2(ssX, -ssY), 0.0, 1.0);\n"
			"  fTile = gTile[0] + vec2(-tileX, -tileY);\n"
			"  EmitVertex();\n"
			"  gl_Position = vec4(start + vec2(ssX, 0), 0.0, 1.0);\n"
			"  fTile = gTile[0] + vec2(-tileX, tileY);\n"
			"  EmitVertex();\n"
			"  gl_Position = vec4(start + vec2(0, -ssY), 0.0, 1.0);\n"
			"  fTile = gTile[0] + vec2(tileX, tileY);\n"
			"  EmitVertex();\n"
			"  EndPrimitive();\n"
			""
			"}\n",

			"#version 450\n"
			"in vec2 fTile;\n"
			"out vec3 color;\n"
			"uniform sampler2D fontTex;\n"
			""
			"uniform vec3 drawColor;\n"
			"uniform float depth;\n"
			""
			"void main() {\n"
			"  color = drawColor;\n"
			"  return;\n"
			"  if(texture(fontTex, fTile).x < 0.5) {\n"
			"    discard;\n"
			"  }\n"
			"  gl_FragDepth = depth;\n"
			"  color = drawColor;\n"
			"}\n"
		);*/
	}
}