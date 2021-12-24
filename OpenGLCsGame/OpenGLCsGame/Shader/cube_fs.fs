#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gAlbedoMap;
layout (location = 2) out vec3 gSpecularMap;
layout (location = 3) out vec3 gNormalMap;
layout (location = 4) out vec4 gHeightAoMetallicRoughnessMap;

in vec2 TexCoord;
in vec3 WorldPos;

uniform sampler2D albedoMap;
void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	gPosition = WorldPos;
	gAlbedoMap = vec4(texture(albedoMap, TexCoord).xyz,1.0);
	//gAlbedoMap = vec4(0.8);
	gNormalMap = vec3(0.0 ,1.0 ,0.0);
	gSpecularMap = vec3(0.3 ,0.3,0.3);
	gHeightAoMetallicRoughnessMap = vec4(0.5,1.0,1.0,1.0);
}