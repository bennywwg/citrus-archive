#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 color;
layout(location = 1) out uint index;

layout (set = 0, binding = 0) uniform UniformBlock {
	mat4	mvp;
	vec4	color;
	uint	index;
} uboData;

void main() {
	color = uboData.color;
	index = uboData.index;
}