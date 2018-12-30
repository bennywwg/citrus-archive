#version 450
out vec3 color;

uniform vec3 drawColor;

void main() {
  color = drawColor;
}