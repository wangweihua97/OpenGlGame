#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gAlbedoMap;
layout (location = 2) out vec3 gSpecularMap;
layout (location = 3) out vec3 gNormalMap;
layout (location = 4) out vec4 gHeightAoMetallicRoughnessMap;


in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in mat4 ModelMat;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;
uniform sampler2D aoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform float use_albedoMap;
uniform float use_specularMap;
uniform float use_normalMap;
uniform float use_heightMap;
uniform float use_aoMap;
uniform float use_metallicMap;
uniform float use_roughnessMap;

void main()
{	
    gPosition = WorldPos;
    gAlbedoMap = vec4(texture(albedoMap, TexCoords).rgb ,1.0f);
    vec3 normalM = texture(normalMap, TexCoords).rgb * 2.0 - 1.0;
    gNormalMap =  (1.0 - use_normalMap) *  Normal + use_normalMap *  mat3(transpose(inverse(ModelMat))) * normalM;
    gNormalMap = normalize(gNormalMap);
    //gNormalMap = Normal;
    gSpecularMap =  (1.0 - use_specularMap) *  vec3(0.8f,0.8f,0.8f) + use_specularMap * (texture(specularMap, TexCoords).rgb);  

    float height = (1.0 - use_heightMap) * 0.5f +  use_heightMap * texture(heightMap, TexCoords).r;
    float metallic  = (1.0 - use_metallicMap) * 1.0f +  use_metallicMap * texture(metallicMap, TexCoords).r;
    float roughness = (1.0 - use_roughnessMap) * 0.2f +  use_roughnessMap * texture(roughnessMap, TexCoords).r;
    float ao        = (1.0 - use_albedoMap) * 0.2f +  use_albedoMap * texture(aoMap, TexCoords).r;

    gHeightAoMetallicRoughnessMap = vec4(height ,ao ,metallic ,roughness);
}