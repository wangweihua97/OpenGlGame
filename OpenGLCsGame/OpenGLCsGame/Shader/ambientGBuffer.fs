#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gAlbedoMap;
uniform sampler2D gSpecularMap;
uniform sampler2D gNormalMap;
uniform sampler2D gHeightAoMetallicRoughnessMap;

layout(std140 , binding = 0) uniform PV
{
	mat4 projection;
	mat4 view;
};

layout(std140 , binding = 1) uniform BaseLight
{
	vec4 lightDir;
	vec4 color;
	vec4 ambient;
    vec4 lightPos;
};
layout(std140 , binding = 2) uniform BaseView
{
	vec3 viewPos;
    //vec3 viewDir;
};
// lights


void main()
{	
    // obtain normal from normal map in range [0,1]
    //vec3 normal = normalize(texture(gNormalMap, TexCoords).rgb);
    // get diffuse color
    vec3 diffuseColor = texture(gAlbedoMap, TexCoords).rgb;
    //vec3 pos = texture(gPosition, TexCoords).rgb;
    // diffuse
    //vec3 lightDir2 = normalize(lightPos.xyz - pos);
    //lightDir2 = normalize(vec3(0.0,1.5,1.0));
    //float diff = max(dot(lightDir2, normal), 0.0);
    //vec3 diffuse = color.xyz * diff * diffuseColor;
    // specular
    //vec3 viewDir = normalize(viewPos - pos);
    //vec3 reflectDir = reflect(-lightDir, normal);
    //vec3 halfwayDir = normalize(lightDir + viewDir);  
    //vec3 viewDir = normalize(viewPos - pos);
    //vec3 a = normalize(lightDir2 + viewDir);
    //float spec = pow(max(dot(normal, a), 0.0),32);
    

    //vec3 specular = color.xyz * spec * texture(gSpecularMap, TexCoords).rgb;
    FragColor = vec4(ambient.xyz*diffuseColor, 1.0);

    //vec4 k = projection * view * vec4(pos ,1.0);
}