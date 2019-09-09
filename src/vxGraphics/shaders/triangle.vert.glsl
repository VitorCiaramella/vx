#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(location = 0) out vec4 fragColor;

void main()
{
	gl_Position = vec4(position + vec3(0, -1, 0.5), 1.0);
	gl_Position.y = -gl_Position.y;

	fragColor = vec4(normal * 0.5 + vec3(0.5), 1.0);
}
