#version 450
out vec4 color;

uniform vec3 drawColor;

void main() {
  color = vec4(drawColor, 1);
}