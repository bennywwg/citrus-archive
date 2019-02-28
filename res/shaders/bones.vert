#version 430
layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_uv;
layout(location = 3) in int vert_bone0;
layout(location = 4) in int vert_bone1;
layout(location = 5) in float vert_weight0;
layout(location = 6) in float vert_weight1;

uniform mat4 projectionMat;
uniform mat4 viewMat;
uniform mat4 modelMat;
uniform mat4 modelViewProjectionMat;
uniform mat4 boneData[64];
uniform uint viewBone;

out vec2 inter_uv;
out float inter_weight;

void main() {
  vec3 transformed0 = vert_position * vert_weight0;
  vec3 transformed1 = vert_position * vert_weight1;
  if(vert_bone0 != -1) {
    transformed0 = vec3(boneData[vert_bone0] * vec4(vert_position, 1.0)) * vert_weight0;
  }
  if(vert_bone1 != -1) {
    transformed1 = vec3(boneData[vert_bone1] * vec4(vert_position, 1.0)) * vert_weight1;
  }
  gl_Position = modelViewProjectionMat * vec4(transformed0 + transformed1, 1.0);
  //gl_Position = modelViewProjectionMat * vec4(vert_position, 1.0);
  inter_uv = vert_uv;
  if(vert_bone0 == viewBone) {
    inter_weight = vert_weight0;
  } else {
    inter_weight = 0.0;
  }
}

