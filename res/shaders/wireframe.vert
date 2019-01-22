#version 430

layout(location = 0) in vec3 vert_position;

uniform mat4 modelViewProjectionMat;

void main() {
  gl_Position = modelViewProjectionMat * vec4(vert_position, 1.0);
}

