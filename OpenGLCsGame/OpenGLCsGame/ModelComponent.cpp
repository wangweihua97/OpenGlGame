#include "ModelComponent.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Time.h"

ModelComponent::ModelComponent(GameObject* gameObject) :Component(gameObject)
{
    _isPlay = false;
    _meshIndex = 0;
}

void ModelComponent::Update()
{
    if (_isPlay)
    {
        animationTime += Time::GetDeltaTime();
        vector<glm::mat4> Transforms;
        BoneTransform(animationTime, Transforms);
        for (unsigned int i = 0; i < Transforms.size(); i++) {
            SetBoneTransform(i, Transforms[i]);
        }
    }
	__super::Update();
}

void ModelComponent::LateUpdate()
{
	__super::LateUpdate();
}

void ModelComponent::Render()
{
    glm::mat4 model = gameObject->transform->worldTransformMat;
    m_pShaderProg->SetMatrix4("model", model, true);
    Draw();
	__super::Render();
}

void ModelComponent::InitAnimation()
{
    for (unsigned int k = 0; k < pScene->mNumAnimations; k++) {
        string name(pScene->mAnimations[k]->mName.data);
        animations[name] = k;
    }
    animationIndex = 0;
    animationTime = 0.0f;
    _isPlay = true;
}

void ModelComponent::PlayAnimation(string animaName)
{
    if (animations.find(animaName) != animations.end())
    {
        animationTime = 0.0f;
        animationIndex = animations[animaName];
    }
}

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
// constructor, expects a filepath to a 3D model.
void ModelComponent::LoadModel(Shader* shader, string const& path, bool gamma = false)
{
    pScene = NULL;
    this->gammaCorrection = gamma;
    this->texturePath = texturePath;
    m_pShaderProg = shader;
    m_NumBones = 0;
    totalVertices = 0;
    shader->Use();
    for (unsigned int i = 0; i < ui_BoneArraySize; i++) {

        char Name[128];
        memset(Name, 0, sizeof(Name));
        _snprintf_s(Name, sizeof(Name), "gBones[%d]", i);
        m_boneLocation[i] = glGetUniformLocation(shader->ID, Name);
    }
    loadModel(path);
    aiMatrix4x4 a = pScene->mRootNode->mTransformation;
    m_GlobalInverseTransform = glm::transpose(glm::make_mat4(&a.a1));
    m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
}
void ModelComponent::Draw()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(*m_pShaderProg);
}

void ModelComponent::loadModel(string path)
{
    // read file via ASSIMP
    //pScene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights);
    pScene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Quality);

    m_RootNode = pScene->mRootNode;
    // check for errors
    if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));
    boneOffsets = new map<unsigned int, glm::mat4>[pScene->mNumMeshes];

    // process ASSIMP's root node recursively
    for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
        baseVertex.push_back(totalVertices);
        totalVertices += pScene->mMeshes[i]->mNumVertices;
    }

    Bones.resize(totalVertices);
    processNode(pScene->mRootNode, glm::mat4(1.0f));
}

void ModelComponent::processNode(aiNode* node, glm::mat4 parentTransfrom)
{
    /*for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
    {
        aiMesh* mesh = pScene->mMeshes[i];
        Mesh m = processMesh(mesh);
        m.BaseVertex = BaseVertex;
        BaseVertex += pScene->mMeshes[i]->mNumVertices;
        meshes.push_back(m);
    }*/
    // 处理节点所有的网格（如果有的话）
    //for (unsigned int i = 0; i < node->mNumMeshes; i++)
    //{
    //    aiMesh* mesh = pScene->mMeshes[node->mMeshes[i]];
    //    Mesh m = processMesh(mesh);
    //    m.BaseVertex = BaseVertex;
    //    BaseVertex += m.vertices.size();
    //    meshes.push_back(m);
    //}
    //// 接下来对它的子节点重复这一过程
    //for (unsigned int i = 0; i < node->mNumChildren; i++)
    //{
    //    processNode(node->mChildren[i]);
    //}
    /*for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
        aiMesh* mesh = pScene->mMeshes[i];
        LoadBones(i, mesh, Bones, baseVertex);
        meshes.push_back(processMesh(i, mesh));
    }*/
    aiMatrix4x4 a = node->mTransformation;
    glm::mat4 nodeTransform = parentTransfrom * glm::transpose(glm::make_mat4(&a.a1));

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        cout << node->mName.data << endl;
        aiMesh* mesh = pScene->mMeshes[node->mMeshes[i]];
        LoadBones(_meshIndex, mesh, Bones, baseVertex, nodeTransform);
        meshes.push_back(processMesh(_meshIndex, mesh));
        _meshIndex++;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], nodeTransform);
    }
}

Mesh ModelComponent::processMesh(unsigned int meshId, aiMesh* mesh)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            if (mesh->mTangents != NULL)
            {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
            }

            if (mesh->mBitangents != NULL)
            {
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            // bitangent
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = pScene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, "albedoMap");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specularMap");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "normalMap");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType::aiTextureType_HEIGHT, "heightMap");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // 5. Ao maps
    std::vector<Texture> aoMaps = loadMaterialTextures(material, aiTextureType::aiTextureType_AMBIENT_OCCLUSION, "aoMap");
    textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());

    // 6. metallicMap
    std::vector<Texture> metallicMaps = loadMaterialTextures(material, aiTextureType::aiTextureType_METALNESS, "metallicMap");
    textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());

    // 7. roughnessMap
    std::vector<Texture> roughnessMaps = loadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS, "roughnessMap");
    textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
    int offset = baseVertex[meshId];
    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures, offset, Bones , m_pShaderProg);
}

vector<Texture> ModelComponent::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        string s = str.C_Str();
        int a = s.find_last_of('\\') + 1;
        if (a == 0)
            a = s.find_last_of('/') + 1;
        string b = s.substr(a, s.length() - a);
        string filename = directory + '/' + b;
        Texture texture;
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        if (ResourceManager::HaveTexture(filename))
        {
            texture.haveData = true;
            texture.texture2D = ResourceManager::GetTextureP(filename);
            texture.type = typeName;
            texture.path = filename;
        }
        else

        {
            if (ResourceManager::IsFileExistent(filename))
            {
                texture.haveData = true;
                texture.texture2D = TextureFromFile(filename);
                texture.type = typeName;
                texture.path = filename;
            }
            else
            {
                cout << filename << endl;
                texture.haveData = false;
                texture.type = typeName;
                texture.path = filename;
            }
        }
        if(texture.haveData)
            textures.push_back(texture);
    }
    return textures;
}

Texture2D* ModelComponent::TextureFromFile(const string& filename)
{
    ResourceManager::LoadTexture(filename.c_str(), filename);
    return ResourceManager::GetTextureP(filename);
}
void ModelComponent::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones, std::vector<unsigned int> baseVertex, glm::mat4 nodeTransform)
{
    // Loop through all bones in the Assimp mesh.
    for (unsigned int i = 0; i < pMesh->mNumBones; i++) {

        unsigned int BoneIndex = 0;

        // Obtain the bone name.
        std::string BoneName(pMesh->mBones[i]->mName.data);

        // If bone isn't already in the map. 
        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {

            // Set the bone ID to be the current total number of bones. 
            BoneIndex = m_NumBones;

            // Increment total bones. 
            m_NumBones++;

            // Push new bone info into bones vector. 
            BoneInfo bi;
            m_BoneInfo.push_back(bi);
        }
        else {
            // Bone ID is already in map. 
            BoneIndex = m_BoneMapping[BoneName];
        }

        m_BoneMapping[BoneName] = BoneIndex;
        auto Offset = pMesh->mBones[i]->mOffsetMatrix;
        // Obtains the offset matrix which transforms the bone from mesh space into bone space. 
        m_BoneInfo[BoneIndex].SetBoneOffset(glm::transpose(glm::make_mat4(&Offset.a1)));
        //boneOffsets[MeshIndex][BoneIndex] = glm::transpose(glm::make_mat4(&Offset.a1));

        // Iterate over all the affected vertices by this bone i.e weights. 
        for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
            // Obtain an index to the affected vertex within the array of vertices.
            unsigned int VertexID = baseVertex[MeshIndex] + pMesh->mBones[i]->mWeights[j].mVertexId;
            // The value of how much this bone influences the vertex. 
            float Weight = pMesh->mBones[i]->mWeights[j].mWeight;

            // Insert bone data for particular vertex ID. A maximum of 4 bones can influence the same vertex. 
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
        //for (unsigned int j = 0; j < pMesh->mNumVertices; j++) {

        //    // Obtain an index to the affected vertex within the array of vertices.
        //    unsigned int VertexID = baseVertex[MeshIndex] + j;
        //    // The value of how much this bone influences the vertex. 
        //    float Weight = pMesh->mBones[i]->mWeights[j].mWeight;

        //    // Insert bone data for particular vertex ID. A maximum of 4 bones can influence the same vertex. 
        //    Bones[VertexID].AddBoneData(BoneIndex, 1.0f);
        //}
    }
}

void ModelComponent::BoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms)
{
    glm::mat4 Identity;
    Identity = glm::mat4(1.0f);
    /*unsigned int numPosKeys = pScene->mAnimations[0]->mChannels[0]->mNumPositionKeys;
    double animDuration = pScene->mAnimations[0]->mChannels[0]->mPositionKeys[numPosKeys - 1].mTime;

    float ticksPerSecond = (float)(pScene->mAnimations[0]->mTicksPerSecond != 0 ? pScene->mAnimations[0]->mTicksPerSecond : 25.0f);

    float timeInTicks = TimeInSeconds * ticksPerSecond;
    float AnimationTime = std::fmod(timeInTicks, animDuration);*/
    float TicksPerSecond = pScene->mAnimations[animationIndex]->mTicksPerSecond;
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, pScene->mAnimations[animationIndex]->mDuration);

    ReadNodeHierarchy(AnimationTime, pScene->mRootNode, Identity);

    Transforms.resize(m_NumBones);
    // Populates transforms vector with new bone transformation matrices. 
    for (unsigned int i = 0; i < m_NumBones; i++) {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }

}

unsigned int ModelComponent::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // Check if there are rotation keyframes. 
    assert(pNodeAnim->mNumRotationKeys > 0);

    // Find the rotation key just before the current animation time and return the index. 
    for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;

        }
    }
    //assert(0);

    return pNodeAnim->mNumRotationKeys - 1;
}

unsigned int ModelComponent::FindScale(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    // Find the scaling key just before the current animation time and return the index. 
    for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }
    //assert(0);

    return pNodeAnim->mNumScalingKeys - 1;
}

unsigned int ModelComponent::FindTranslation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumPositionKeys > 0);

    // Find the translation key just before the current animation time and return the index. 
    for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }
    //assert(0);

    return pNodeAnim->mNumPositionKeys - 1;
}

void ModelComponent::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }
    unsigned int ScalingIndex = FindScale(AnimationTime, pNodeAnim);
    if (ScalingIndex == pNodeAnim->mNumScalingKeys - 1) {
        //Out = pNodeAnim->mRotationKeys[0].mValue;
        Out = pNodeAnim->mScalingKeys[pNodeAnim->mNumScalingKeys - 1].mValue;
        return;
    }
    unsigned int NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void ModelComponent::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
    // Obtain the current rotation keyframe. 
    unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    if (RotationIndex == pNodeAnim->mNumRotationKeys - 1) {
        //Out = pNodeAnim->mRotationKeys[0].mValue;
        Out = pNodeAnim->mRotationKeys[pNodeAnim->mNumRotationKeys - 1].mValue;
        return;
    }
    // Calculate the next rotation keyframe and check bounds. 
    unsigned int NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);

    // Calculate delta time, i.e time between the two keyframes.
    float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;

    // Calculate the elapsed time within the delta time.  
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);

    // Obtain the quaternions values for the current and next keyframe. 
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;

    // Interpolate between them using the Factor. 
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);

    // Normalise and set the reference. 
    Out = Out.Normalize();
}
void ModelComponent::CalcInterpolatedTranslation(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    unsigned int PositionIndex = FindTranslation(AnimationTime, pNodeAnim);
    if (PositionIndex == pNodeAnim->mNumPositionKeys - 1) {
        //Out = pNodeAnim->mPositionKeys[0].mValue;
        Out = pNodeAnim->mPositionKeys[pNodeAnim->mNumPositionKeys - 1].mValue;
        return;
    }
    unsigned int NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime;
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;

    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

void ModelComponent::SetBoneTransform(unsigned int Index, const glm::mat4& Transform)
{
    assert(Index < 100);
    /*glm::mat4 a = glm::transpose(Transform);*/
    glUniformMatrix4fv(m_boneLocation[Index], 1, GL_FALSE, &Transform[0][0]);
    //m_pShaderProg->setUniformIndex(Index, Transform);
}

void ModelComponent::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
{
    glm::mat4 IdentityTest;
    IdentityTest = glm::mat4(1.0f);

    // Obtain the name of the current node 
    std::string NodeName(pNode->mName.data);

    // Use the first animation 
    const aiAnimation* pAnimation = pScene->mAnimations[animationIndex];

    // Obtain transformation relative to node's parent. 
    auto a = pNode->mTransformation;
    glm::mat4 NodeTransformation = glm::transpose(glm::make_mat4(&a.a1));
    /*if (NodeName.find("$AssimpFbx$") != NodeName.npos)
        NodeTransformation = glm::mat4(1.0f);*/


    const aiNodeAnim* pNodeAnim = NULL;

    // Find the animation channel of the current node. 
    for (unsigned i = 0; i < pAnimation->mNumChannels; i++) {
        const aiNodeAnim* pNodeAnimIndex = pAnimation->mChannels[i];

        // If there is a match for a channel with the current node's name, then we've found the animation channel. 
        if (std::string(pNodeAnimIndex->mNodeName.data) == NodeName) {
            pNodeAnim = pNodeAnimIndex;
        }
    }

    if (pNodeAnim) {

        //// Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        glm::mat4 ScalingM;
        ScalingM = glm::scale(glm::mat4(1.0f), glm::vec3(Scaling.x, Scaling.y, Scaling.z));

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        glm::quat rotation = glm::quat(RotationQ.w, RotationQ.x, RotationQ.y, RotationQ.z);
        glm::mat4 RotationM = glm::mat4_cast(rotation);
        /*aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        auto c = RotationQ.GetMatrix();
        glm::mat4 RotationM = ai_to_glm2(c);*/

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedTranslation(Translation, AnimationTime, pNodeAnim);
        glm::mat4 TranslationM;
        TranslationM = glm::translate(IdentityTest, glm::vec3(Translation.x, Translation.y, Translation.z));
        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
        //NodeTransformation = glm::mat4(1.0f);
    }

    glm::mat4 GlobalTransformation2 = ParentTransform * NodeTransformation;
    // Apply the final transformation to the indexed bone in the array. 
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        unsigned int BoneIndex = m_BoneMapping[NodeName];
        //m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation2;
        m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation2 * m_BoneInfo[BoneIndex].BoneOffset;
    }

    // Do the same for all the node's children. 
    for (unsigned i = 0; i < pNode->mNumChildren; i++) {
        ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation2);
    }
}
