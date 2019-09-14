#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 color;

layout(set = 0, binding = 0) uniform sampler2D colorTex;
layout(set = 0, binding = 1) uniform usampler2D indexTex;
layout(set = 0, binding = 2) uniform sampler2D depthTex;

layout(set = 0, binding = 3) uniform samplerCube cubeMap;

layout(location = 0) in vec2 fragUVCoord;
layout(location = 1) in vec3 fcpPos;

/*
float linearize(float v) {
	float zNear = 0.1;
	float zFar = 50.0;
	return 1.0 / (zFar + zNear - (2.0 * v - 1.0) * (zFar - zNear));
}

float zdif(vec2 uv) {
	float miv = 1;
	float mav = 0;
	#define RS 2
	#define SF 0.0005
	for(int y = -RS; y <= RS; y++) {
		for(int x = -RS; x <= RS; x++) {
			float v = linearize(texture(depthTex, uv + vec2(x*SF, y*SF)).r);
			miv = min(miv, v);
			mav = max(mav, v);
		}
	}
	if(mav - miv > 0.01) return 0.5; else return 1;
}*/

layout (set = 0, binding = 4) uniform UniformData {
	mat4 vp;
	vec4 cameraPos; //not sure of alignment, vec4 just in case
	float ncp;
	float fcp;
	uint widthPX;
	uint heightPX;
	//uint specialIndex;
} uniformData;

float idif(vec2 uv) {
	#define RS 1
	float SX = 1.0 / float(uniformData.widthPX);
	float SY = 1.0 / float(uniformData.heightPX);
	uint mv = texture(indexTex, uv).r;
	float weight = 0;
	for(int y = -RS; y <= RS; y++) {
		for(int x = -RS; x <= RS; x++) {
			if(mv != texture(indexTex, uv + vec2(x*SX, y*SY)).r) weight += (abs(x) + abs(y));
		}
	}
	return 1 - weight / 12.0;
}

void main() {
	vec3 dir = fcpPos - uniformData.cameraPos.xyz;
	if(texture(indexTex, fragUVCoord).r == 0) {
		color = texture(cubeMap, dir);
	} else {
		color = texture(colorTex, fragUVCoord) * idif(fragUVCoord);
	}
}
