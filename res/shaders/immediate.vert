#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vert_pos;
layout(location = 1) in vec2 vert_uv;

layout(location = 0) out vec2 frag_uv;

layout(set = 0, binding = 0) uniform UniformBlock {
	mat4	mvp;
	vec4	color;
} uboData;

void main() {
    gl_Position	= uboData.mvp * vec4(vert_pos, 1.0);
	frag_uv = vert_uv;
}
