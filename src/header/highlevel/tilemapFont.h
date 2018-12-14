#pragma once

#ifndef TILEMAPFONT_H
#define TILEMAPFONT_H

#include <string>
#include <memory>

#include <graphics/texture/colorTexture.h>
#include <graphics/shader/shader.h>
#include <graphics/geometry/vertexarray.h>
#include <graphics/buffer/buffer.h>
#include <graphics/buffer/vertexView.h>

namespace citrus {
	namespace engine {
		class tilemapFont {
			public:
			std::unique_ptr<graphics::texture3b> tex;
			std::unique_ptr<graphics::shader> sh;
			std::unique_ptr<graphics::buffer> buf;

			void streamText(std::string text, glm::mat4 mvp) {
				std::vector<glm::vec2> positions, tiles;
				positions.reserve(text.size());
				tiles.reserve(text.size());
				int column = 0;
				int row = 0;
				for(uint32_t i = 0; i < text.size(); i++) {
					if(text[i] == '\n') {
						row++;
						column = 0;
						continue;
					}
					positions.push_back(glm::vec2(column * 0.5f, -row));

					tiles.push_back(glm::vec2((((unsigned char)text[i]) % 16 + 0.5f) / 16.0f, - (((unsigned char)text[i]) / 16 + 0.5f) / 16.0f));
					column++;
				}


				sh->use();
				sh->setUniform("tileX", 1.0f / 32.0f);
				sh->setUniform("tileY", 1.0f / 32.0f);
				sh->setUniform("modelViewProjectionMat", mvp);
				sh->setSampler("fontTex", *tex.get());

				graphics::vertexView2f pos(buf.get(), 0, positions), tile(buf.get(), positions.size() * sizeof(glm::vec2), tiles);

				graphics::vertexArray vao({graphics::vertexAttribute(0, false, pos), graphics::vertexAttribute(1, false, tile)});

				vao.drawAll(GL_POINTS);

				sh->unuse();
			}

			tilemapFont(std::string imagePath, int columns, int row) {
				graphics::image3b texImage(imagePath);

				buf = std::make_unique<graphics::buffer>(1024 * 1024);

				tex = std::make_unique<graphics::texture3b>(texImage);

				sh = std::make_unique<graphics::shader>(
					"#version 450\n"
					""
					"layout(location = 0) in vec2 vert_position;\n"
					"layout(location = 1) in vec2 vert_tile;\n"
					""
					"out vec2 gPos;\n"
					"out vec2 gTile;\n"
					""
					"void main() {\n"
					"  gl_Position = vec4(vert_position, 0.0, 1.0);\n"
					"  gPos = vert_position;\n"
					"  gTile = vert_tile;\n"
					"}\n",

					"#version 450\n"
					""
					"layout(points) in;\n"
					"layout(triangle_strip, max_vertices = 6) out;\n"
					""
					"uniform float tileX;\n"
					"uniform float tileY;\n"
					"uniform mat4 modelViewProjectionMat;\n"
					""
					"in vec2 gPos[];\n"
					"in vec2 gTile[];\n"
					""
					"out vec2 fTile;\n"
					""
					"void main() {\n"
					"  gl_Position = modelViewProjectionMat * vec4(gPos[0] + vec2(0.5, -0.5), 0.0, 1.0);\n"
					"  fTile = gTile[0] + vec2(tileX, -tileY);\n"
					"  EmitVertex();\n"
					"  gl_Position = modelViewProjectionMat * vec4(gPos[0] + vec2(-0.5, -0.5), 0.0, 1.0);\n"
					"  fTile = gTile[0] + vec2(-tileX, -tileY);\n"
					"  EmitVertex();\n"
					"  gl_Position = modelViewProjectionMat * vec4(gPos[0] + vec2(0.5, 0.5), 0.0, 1.0);\n"
					"  fTile = gTile[0] + vec2(tileX, tileY);\n"
					"  EmitVertex();\n"
					"  EndPrimitive();\n"
					""
					"  gl_Position = modelViewProjectionMat * vec4(gPos[0] + vec2(-0.5, -0.5), 0.0, 1.0);\n"
					"  fTile = gTile[0] + vec2(-tileX, -tileY);\n"
					"  EmitVertex();\n"
					"  gl_Position = modelViewProjectionMat * vec4(gPos[0] + vec2(-0.5, 0.5), 0.0, 1.0);\n"
					"  fTile = gTile[0] + vec2(-tileX, tileY);\n"
					"  EmitVertex();\n"
					"  gl_Position = modelViewProjectionMat * vec4(gPos[0] + vec2(0.5, 0.5), 0.0, 1.0);\n"
					"  fTile = gTile[0] + vec2(tileX, tileY);\n"
					"  EmitVertex();\n"
					"  EndPrimitive();\n"
					""
					"}\n",

					"#version 450\n"
					"in vec2 fTile;\n"
					"out vec4 color;\n"
					"uniform sampler2D fontTex;\n"
					"void main() {\n"
					"  color = texture(fontTex, fTile);\n"
					"  if(color.x < 0.25) {\n"
					"    discard;\n"
					"  }\n"
					"  color = vec4(1.0, 1.0, 1.0, 1.0);\n"
					"}\n"
				);
			}
		};
	}
}

#endif