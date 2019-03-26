#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 frag_norm;
layout(location = 1) in vec3 frag_tangent;
layout(location = 2) in vec2 frag_uv;

layout(location = 0) out vec4 color;

layout(binding = 1) uniform sampler2D colorTex;

void main() {
	color = vec4(frag_uv * frag_norm.y, 0, 1);
}