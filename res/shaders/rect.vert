#version 430
layout(location = 0) in vec2 vert_position;

void main() {
  gl_Position = vec4(vert_position.x, vert_position.y, 0, 1);
}