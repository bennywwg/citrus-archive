#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 color;

layout(set = 0, binding = 0) uniform sampler2D colorTex;
layout(set = 0, binding = 1) uniform sampler2D depthTex;

layout(location = 0) in vec2 fragUVCoord;

void main() {
	color = texture(colorTex, fragUVCoord);
	//float val = texture(depthTex, fragUVCoord).x;
	//color = vec4(val, val, val, 1);
}
