#version 450
in vec2 inter_uv;
in float inter_weight;

out vec3 color;

uniform sampler2D tex;

void main() {
  color = texture(tex, vec2(inter_uv.x, -inter_uv.y)).rgb;
  color = color + vec3(inter_weight, -inter_weight, -inter_weight);
}