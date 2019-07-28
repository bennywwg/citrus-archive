#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vert_pos;
layout(location = 1) in vec3 vert_norm;
layout(location = 2) in vec2 vert_uv;

layout(location = 0) out vec3 frag_norm;
layout(location = 1) out vec2 frag_uv;

layout (push_constant) uniform PushConstants {
	mat4	mvp;
	layout( row_major ) mat4x3	model;
} pushConstants;

void main() {
    gl_Position	= pushConstants.mvp * vec4(vert_pos, 1.0);
    frag_norm	= pushConstants.model * vec4(vert_norm, 0.0);
	frag_uv		= vert_uv;
}
