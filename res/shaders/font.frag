#version 430

in vec2 fTile;

out vec4 color;

uniform vec3 drawColor;

uniform sampler2D fontTex;

void main() {
	color = vec4(drawColor, 1);
	if(texture(fontTex, fTile).x < 0.5) {
		discard;
	}
}