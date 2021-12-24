#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(std140 , binding = 0) uniform PV
{
	mat4 projection;
	mat4 view;
};
out vec2 TexCoord;
out vec3 WorldPos;

uniform mat4 model;

void main()
{
    vec4 world = model * vec4(aPos, 1.0);
	gl_Position = projection * view *  world;
	WorldPos = world.xyz;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}