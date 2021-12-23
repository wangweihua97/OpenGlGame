#version 430
layout (location = 0) in vec3 aPos;

out vec3 VPosition;

layout(std140 , binding = 0) uniform PV
{
	mat4 projection;
	mat4 view;
};
uniform mat4 model;

void main()
{ 
	vec4 world = vec4(aPos, 1.0);
	VPosition = (view * model * world).xyz;
}