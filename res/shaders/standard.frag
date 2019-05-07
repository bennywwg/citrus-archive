#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 frag_norm;
layout(location = 1) in vec3 frag_tangent;
layout(location = 2) in vec2 frag_uv;

layout(location = 0) out vec4 color;

layout(set = 1, binding = 0) uniform sampler2D colorTex[4];

layout (push_constant) uniform FragPushConstants {
	layout(offset = 112) uint texIndex;
} frag_pc;

void main() {
	//color = vec4(1, 0, 0, 1);
	color = vec4(texture(colorTex[frag_pc.texIndex], frag_uv).xyz, 1);
}