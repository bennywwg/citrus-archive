#version 450
#extension GL_ARB_separate_shader_objects : enable

vec2 positions[3] = vec2[](
    vec2(0.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, 1.0)
);

vec2 uvs[3] = vec2[](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0)
);

layout(binding = 0) uniform UBO {
	mat4 model;
} ubo;

layout(location = 0) out vec2 fragUVCoord;

void main() {
	gl_Position = ubo.model * vec4(positions[gl_VertexIndex], 0.0, 1.0);
	fragUVCoord = uvs[gl_VertexIndex];
}
