#version 450

out vec4 color;

uniform vec3 drawColor;
uniform float depth;

void main() {
  color = vec4(drawColor, 1);
  //gl_FragDepth = depth;
}