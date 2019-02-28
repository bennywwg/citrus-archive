#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 color;

layout(binding = 1) uniform sampler2D colorTex;

void main() {
	color = texture(colorTex, gl_FragCoord.xy);
}