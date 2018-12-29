#version 450
layout(location = 0) in vec3 vert_position;
layout(location = 2) in vec2 vert_uv;

uniform mat4 projectionMat;
uniform mat4 viewMat;
uniform mat4 modelMat;
uniform mat4 modelViewProjectionMat;

out vec2 inter_uv;

void main() {
  gl_Position = modelViewProjectionMat * vec4(vert_position, 1.0);
  inter_uv = vert_uv;
}