#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 color;
layout(location = 1) out uint index;

layout(location = 0) in vec2 frag_uv;
layout(location = 1) in vec3 frag_bary;

layout(set = 1, binding = 0) uniform sampler2D fontTex;

layout (set = 0, binding = 0) uniform UniformBlock {
	mat4	mvp;
	vec4	color;
	uint	index;
} uboData;

float edgeFactor(){
    return min(min(frag_bary.x, frag_bary.y), frag_bary.z);
}

void main() {
	float fval = texture(fontTex, frag_uv).r;
	
	if(edgeFactor() > 0.02) discard;
	
	color = vec4(uboData.color.rgb * (1 - fval), uboData.color.a);
	
	index = uboData.index;
}