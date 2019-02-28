#version 430

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

uniform float tileX;
uniform float tileY;
uniform float ssX;
uniform float ssY;

in vec2 gPos[];
in vec2 gTile[];

out vec2 fTile;

void main() {
	vec2 start = gPos[0];
	gl_Position = vec4(start + vec2(0, 0), 0.0, 1.0);
	fTile = gTile[0] + vec2(-tileX, tileY);
	EmitVertex();
	gl_Position = vec4(start + vec2(0, -ssY), 0.0, 1.0);
	fTile = gTile[0] + vec2(-tileX, -tileY);
	EmitVertex();
	gl_Position = vec4(start + vec2(ssX, 0), 0.0, 1.0);
	fTile = gTile[0] + vec2(tileX, tileY);
	EmitVertex();
	EndPrimitive();

	gl_Position = vec4(start + vec2(ssX, -ssY), 0.0, 1.0);
	fTile = gTile[0] + vec2(tileX, -tileY);
	EmitVertex();
	gl_Position = vec4(start + vec2(ssX, 0), 0.0, 1.0);
	fTile = gTile[0] + vec2(tileX, tileY);
	EmitVertex();
	gl_Position = vec4(start + vec2(0, -ssY), 0.0, 1.0);
	fTile = gTile[0] + vec2(-tileX, -tileY);
	EmitVertex();
	EndPrimitive();
}