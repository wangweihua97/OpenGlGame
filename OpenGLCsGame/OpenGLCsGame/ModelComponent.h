#ifndef ModelComponent_H
#define ModelComponent_H
#include"Component.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/anim.h>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
class ModelComponent : public Component
{
public:
    vector<Mesh>    meshes;
    string directory;
    std::vector<VertexBoneData> Bones;
    unsigned int totalVertices = 0;

    bool gammaCorrection;
    const aiScene* pScene; //!< The Assimp aiScene object. 
	ModelComponent(GameObject* gameObject);
	void Update()override;
	void LateUpdate()override;
	void Render()override;
	void InitAnimation();
	void PlayAnimation(string animaName);
	void LoadModel(Shader* shader, string const& path, bool gamma);
    void Draw();
    void BoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms); //!< Traverses the scene hierarchy and fetches the matrix transformation for each bone given the time. 
    void SetBoneTransform(unsigned int Index, const glm::mat4& Transform); //!< Inserts a bone transformation in the uniform array at the given index. 
    int animationIndex;
    float animationTime;
    map<string,int > animations;

private:
    bool _isPlay;
    static const unsigned int ui_BoneArraySize = 100;
    GLint m_boneLocation[ui_BoneArraySize];
    Assimp::Importer importer;
    string texturePath;
    unsigned int m_NumBones;
    Shader* m_pShaderProg;
    aiNode* m_RootNode;
    vector<unsigned int> baseVertex;
    map<unsigned int, glm::mat4>* boneOffsets;
    void loadModel(string path);
    void processNode(aiNode* node, glm::mat4 parentTransfrom);
    Mesh processMesh(unsigned int meshId, aiMesh* mesh);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        string typeName);
    Texture2D* TextureFromFile(const string& filename);

    std::map<std::string, unsigned int> m_BoneMapping; //!< Map of bone names to ids

    std::vector<BoneInfo> m_BoneInfo; //!< Array containing bone information such as offset matrix and final transformation. 

    glm::mat4 GlobalTransformation; //!< Root node transformation. 
    glm::mat4 m_GlobalInverseTransform;
    void LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones, std::vector<unsigned int> baseVertex, glm::mat4 nodeTransform); //!< Loads the bone data from a given mesh. 
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim); //!< Calculates the interpolated quaternion between two keyframes. 
    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim); //!< Calculates the interpolated scaling vector between two keyframes. 
    void CalcInterpolatedTranslation(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim); //!< Calculates the interpolated translation vector between two keyframes. 

    unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim); //!< Finds a rotation key given the current animation time. 
    unsigned int FindScale(float AnimationTime, const aiNodeAnim* pNodeAnim); // Finds a scaling key given the current animation time. 
    unsigned int FindTranslation(float AnimationTime, const aiNodeAnim* pNodeAnim); // Finds a translation key given the current animation time. 

    void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
};
#endif

