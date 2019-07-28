#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 frag_norm;
layout(location = 1) in vec2 frag_uv;

layout(location = 0) out vec4 color;
layout(location = 1) out uint index;

layout(set = 2, binding = 0) uniform sampler2D colorTex[4];

layout (push_constant) uniform FragPushConstants {
	layout(offset = 112) uint texIndex;
	layout(offset = 116) uint itemIndex;
	layout(offset = 120) uint val2;
	layout(offset = 124) uint val3;
} frag_pc;

void main() {
  color = vec4(texture(colorTex[frag_pc.texIndex], vec2(frag_uv.x, -frag_uv.y)).rgb, 1);
  index = frag_pc.itemIndex;
}