#version 430
in vec2 inter_uv;
out vec3 color;

uniform sampler2D tex;

void main() {
  color = texture(tex, vec2(inter_uv.x, -inter_uv.y)).rgb;
}