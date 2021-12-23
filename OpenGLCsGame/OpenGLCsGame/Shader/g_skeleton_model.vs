#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in vec4 Weights;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;
out mat4 ModelMat;

layout(std140 , binding = 0) uniform PV
{
	mat4 projection;
	mat4 view;
};
uniform mat4 model;

const int MAX_BONES = 100; // Max number of bones
uniform mat4 gBones[MAX_BONES]; // Bone transformations 
void main()
{
    mat4 BoneTransform = gBones[ BoneIDs[0] ] * Weights[0];
	BoneTransform += gBones[ BoneIDs[1] ] * Weights[1];
    BoneTransform += gBones[ BoneIDs[2] ] * Weights[2];
    BoneTransform += gBones[ BoneIDs[3] ] * Weights[3];

	// Transformed vertex position 
	vec4 tPos = BoneTransform * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
    WorldPos = vec3(model * tPos);
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    //Normal = mat3(model) * aNormal;   
    ModelMat = model;

    //gl_Position =  projection * view * vec4(WorldPos, 1.0);
    gl_Position =  projection * view * model * tPos;
}