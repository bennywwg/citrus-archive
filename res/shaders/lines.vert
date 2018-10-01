#version 460

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_color;

out vec3 inter_color;

uniform mat4 modelViewProjectionMat;

void main() {
	gl_Position = modelViewProjectionMat * vec4(vert_position, 1.0);
	inter_color = vert_color;
}