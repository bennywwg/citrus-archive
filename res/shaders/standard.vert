#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vert_pos;
layout(location = 1) in vec3 vert_norm;
layout(location = 2) in vec2 vert_uv;
layout(location = 3) in vec3 vert_tangent;
layout(location = 4) in vec3 vert_bitangent;

layout(location = 0) out vec3 frag_norm;
layout(location = 1) out vec2 frag_uv;
layout(location = 2) out Lights {
	vec3 directions_tangentSpace[4];
	vec3 colors[4];
} lights;

layout (set = 0, binding = 0) uniform UniformData {
	vec4 camDir;
	vec4 lightDir;
} uniformData;

layout (push_constant) uniform PushConstants {
	mat4	mvp;
	layout( row_major ) mat4x3	model;
} pushConstants;

void main() {	
	mat3 toTangentSpace = transpose(mat3(
		pushConstants.model * vec4(vert_tangent, 0.0),
		pushConstants.model * vec4(vert_bitangent, 0.0),
		pushConstants.model * vec4(vert_norm, 0.0)
	));
	
	lights.directions_tangentSpace[0] = toTangentSpace * vec3(uniformData.lightDir);

    gl_Position	= pushConstants.mvp * vec4(vert_pos, 1.0);
    frag_norm	= pushConstants.model * vec4(vert_norm, 0.0);
	frag_uv		= vert_uv;
}
