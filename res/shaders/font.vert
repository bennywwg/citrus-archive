#version 430

layout(location = 0) in ivec2 vert_position;
layout(location = 1) in vec2 vert_tile;

uniform ivec2 screen;

out vec2 gPos;
out vec2 gTile;

void main() {
	vec2 sspos = vec2(float(vert_position.x) / float(screen.x) * 2 - 1, float(vert_position.y) / float(screen.y) * -2 + 1);
	//sspos = vec2(0,0);
	gl_Position = vec4(sspos, 0, 1);
	gPos = sspos;
	gTile = vert_tile;
}