#version 450
layout(location = 0) in vec3 vert_position;

uniform mat4 projectionMat;
uniform mat4 viewMat;
uniform mat4 modelMat;
uniform mat4 modelViewProjectionMat;

void main() {
  gl_Position = modelViewProjectionMat * vec4(vert_position, 1.0);
}