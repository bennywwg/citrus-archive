#version 430

in vec3 inter_color;

layout(location = 0) out vec3 color;

void main() {
	color = inter_color;
}