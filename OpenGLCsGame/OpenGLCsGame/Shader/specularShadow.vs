#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aNormal;
layout(location = 1) in vec2 aTexCoord;
layout(std140 , binding = 0) uniform PV
{
	mat4 projection;
	mat4 view;
};
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;  
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}