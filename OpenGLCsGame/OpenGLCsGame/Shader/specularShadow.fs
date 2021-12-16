#version 420 core
out vec4 FragColor;

layout(std140 , binding = 1) uniform BaseLight
{
	vec3 lightDir;
	vec3 color;
	vec3 ambient;
    float gloss;
};
layout(std140 , binding = 2) uniform BaseView
{
	vec3 viewPos;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
uniform sampler2D albedoMap;
void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	//FragColor = vec4(texture(albedoMap, TexCoord).rgb ,1.0);

	// ambient
    vec3 ambient = BaseLight.ambient * texture(albedoMap, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, BaseLight.lightDir), 0.0);
    vec3 diffuse = BaseLight.color * diff * texture(albedoMap, TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(viewDir + BaseLight.lightDir);  
    float spec = pow(max(dot(norm, halfDir), 0.0), BaseLight.gloss);
    vec3 specular = BaseLight.color * spec * texture(albedoMap, TexCoords).rgb;  
        
    vec3 result = ambient + diffuse + specular;
    // HDR tonemapping
    result = result / (result + vec3(1.0));
    // gamma correct
    result = pow(result, vec3(1.0/2.2));
    FragColor = vec4(result, 1.0);
}