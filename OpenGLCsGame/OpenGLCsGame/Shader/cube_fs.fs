#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D albedoMap;
void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = vec4(texture(albedoMap, TexCoord).rgb ,1.0);
}