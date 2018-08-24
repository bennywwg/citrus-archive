#version 330

uniform sampler2D tex;
in vec2 inter_uv;
out vec4 res;

void main() {
	res = texture(tex, inter_uv);
}