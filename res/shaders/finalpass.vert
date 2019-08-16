#version 450
#extension GL_ARB_separate_shader_objects : enable

vec2 positions[6] = vec2[](
    vec2(1.0, 1.0),
    vec2(-1.0, 1.0),
    vec2(-1.0, -1.0),
	
	vec2(1.0, 1.0),
	vec2(-1.0, -1.0),
	vec2(1.0, -1.0)
);

vec2 uvs[6] = vec2[](
    vec2(1.0, 0.0),
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
	
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0)
);

layout(location = 0) out vec2 fragUVCoord;
layout(location = 1) out vec3 fcpPos;

layout (set = 0, binding = 4) uniform UniformData {
	mat4 vp;
	vec4 cameraPos; //not sure of alignment, vec4 just in case
	float ncp;
	float fcp;
	uint widthPX;
	uint heightPX;
} uniformData;

void main() {
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
	fragUVCoord = uvs[gl_VertexIndex];
	vec4 inv = uniformData.vp * vec4(fragUVCoord * 2.0 - 1.0, 1.0, 1.0);
	fcpPos = inv.xyz / inv.w;
}
