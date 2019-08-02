#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vert_pos;

layout(set = 0, binding = 0) uniform UniformBlock {
	mat4	mvp;
	vec4	color;
} uboData;

void main() {
    gl_Position	= uboData.mvp * vec4(vert_pos, 1.0);
}
