#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

uniform ivec2 screen;

uniform ivec2 position;
uniform ivec2 size;

void main() {
	vec2 ul = vec2(float(position.x) / float(screen.x) * 2 - 1, float(position.y) / float(screen.y) * -2 + 1);
	vec2 ur = vec2(ul.x + float(size.x) / float(screen.x) * 2, ul.y);
	vec2 bl = vec2(ul.x, ul.y - float(size.y) / float(screen.y) * 2);
	vec2 br = vec2(ur.x, bl.y);

	gl_Position = vec4(ul.x, ul.y, 0, 1);
	EmitVertex();
	gl_Position = vec4(bl.x, bl.y, 0, 1);
	EmitVertex();
	gl_Position = vec4(ur.x, ur.y, 0, 1);
	EmitVertex();
	EndPrimitive();
	
	gl_Position = vec4(bl.x, bl.y, 0, 1);
	EmitVertex();
	gl_Position = vec4(br.x, br.y, 0, 1);
	EmitVertex();
	gl_Position = vec4(ur.x, ur.y, 0, 1);
	EmitVertex();
	EndPrimitive();
}