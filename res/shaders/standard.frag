#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 frag_norm;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in Lights {
	vec4 wp_tangentSpace;
	vec3 directions_tangentSpace[4];
} lights;

layout(location = 0) out vec4 color;
layout(location = 1) out uint index;

layout(set = 1, binding = 0) uniform sampler2D colorTex[11];
layout(set = 2, binding = 0) uniform samplerCube cubeMaps[1];

layout (set = 0, binding = 0) uniform UniformData {
	vec4 camPos;
	vec4 lightDirs[4];
	vec4 lightColors[4];
	uint lightCount;
} uniformData;

layout (push_constant) uniform FragPushConstants {
	layout(offset = 112) uint texIndex;
	layout(offset = 116) uint itemIndex;
	layout(offset = 120) uint normalIndex;
	layout(offset = 124) uint val3;
} frag_pc;

void main() {
	vec3 normal_tangentSpace = normalize(texture(colorTex[frag_pc.normalIndex], frag_uv).xyz * 2.0 - 1.0);
	
	//float brightness = dot(normal_tangentSpace, normalize(lights.directions_tangentSpace[0]));
	//
	//for(int i = 1; i < uniformData.lightCount; i++) {
	//	brightness += dot(normal_tangentSpace, normalize(lights.wp_tangentSpace.xyz - lights.directions_tangentSpace[i]));
	//}
	
	float brightness = 1.0;
	
	brightness = clamp(brightness, 0, 1);
	color = vec4(vec3(brightness), 1);
	//color = vec4(normal_tangentSpace, 1);
	//if(brightness < 0.5) brightness = 0.5; else brightness = 1;
	color = vec4(texture(colorTex[frag_pc.texIndex], frag_uv).xyz
	* brightness, 1);
	//color = vec4(frag_norm, 1);
	index = frag_pc.itemIndex;
}