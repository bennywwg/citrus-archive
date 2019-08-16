#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_uv;
layout(location = 3) in int vert_bone0;
layout(location = 4) in int vert_bone1;
layout(location = 5) in float vert_weight0;
layout(location = 6) in float vert_weight1;

layout (set = 0, binding = 0) uniform UniformData {
	vec4 lightDir;
} uniformData;

layout (set = 3, binding = 0) readonly buffer BoneData {
	mat4 transforms[];
} boneData;

layout (push_constant) uniform PushConstants {
	mat4	mvp;
	layout( row_major ) mat4x3	model;
} pushConstants;

layout(location = 0) out vec3 frag_norm;
layout(location = 1) out vec2 frag_uv;

void main() {
  vec3 transformed0 = vert_position * vert_weight0;
  vec3 transformed1 = vert_position * vert_weight1;
  vec3 normal0 = vert_normal * vert_weight0;
  vec3 normal1 = vert_normal * vert_weight1;
  if(vert_bone0 != -1) {
    transformed0 = vec3(boneData.transforms[vert_bone0] * vec4(vert_position, 1.0))	* vert_weight0;
	normal0 = vec3(boneData.transforms[vert_bone0] * vec4(vert_normal, 0.0))		* vert_weight0;
  }
  if(vert_bone1 != -1) {
    transformed1 = vec3(boneData.transforms[vert_bone1] * vec4(vert_position, 1.0))	* vert_weight1;
	normal1 = vec3(boneData.transforms[vert_bone1] * vec4(vert_normal, 0.0))		* vert_weight1;
  }
  gl_Position = pushConstants.mvp * vec4(transformed0 + transformed1, 1.0);
  frag_norm	= pushConstants.model * vec4(normal0 + normal1, 0.0);
  frag_uv = vert_uv;
}