#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 color;

layout(set = 0, binding = 0) uniform sampler2D colorTex;
layout(set = 0, binding = 1) uniform usampler2D indexTex;
layout(set = 0, binding = 2) uniform sampler2D depthTex;

layout(location = 0) in vec2 fragUVCoord;

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

float idif(vec2 uv) {
	#define RS 1
	#define SF 0.001
	uint mv = texture(indexTex, uv).r;
	for(int y = -RS; y <= RS; y++) {
		for(int x = -RS; x <= RS; x++) {
			if(mv != texture(indexTex, uv + vec2(x*SF, y*SF)).r) return 0.5;
		}
	}
	return 1;
}

void main() {
	color = texture(colorTex, fragUVCoord) * idif(fragUVCoord);
}
