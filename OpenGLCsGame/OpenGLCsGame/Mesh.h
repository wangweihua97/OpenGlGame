#ifndef Mesh_H
#define Mesh_H
#include"Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <map>
#include "Texture.h"
#include "Shader.h"
#define MAX_BONE_INFLUENCE 4
using namespace std;
struct Vertex {
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    ////weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};
struct BoneInfo
{
    glm::mat4 FinalTransformation; // Final transformation to apply to vertices 
    glm::mat4 BoneOffset; // Initial offset from local to bone space. 

    BoneInfo()
    {
        BoneOffset = glm::mat4(1.0f);
        FinalTransformation = glm::mat4(1.0f);
    }

    void SetBoneOffset(glm::mat4 a)
    {
        if (BoneOffset == glm::mat4(1.0f))
            BoneOffset = a;
    }
};
struct Texture {
    Texture2D* texture2D;
    string type;
    string path;
    bool haveData;
};
struct VertexBoneData
{
    unsigned int IDs[4]; //!< An array of 4 bone Ids that influence a single vertex.
    float Weights[4]; //!< An array of the weight influence per bone. 

    VertexBoneData()
    {
        // 0's out the arrays. 
        Reset();
    }

    void Reset()
    {
        memset(IDs, 0, 4 * sizeof(IDs[0]));
        memset(Weights, 0, 4 * sizeof(Weights[0]));
    }

    void AddBoneData(unsigned int BoneID, float Weight)
    {
        for (unsigned int i = 0; i < 4; i++) {

            // Check to see if there are any empty weight values. 
            if (Weights[i] == 0.0) {
                // Add ID of bone. 
                IDs[i] = BoneID;

                // Set the vertex weight influence for this bone ID. 
                Weights[i] = Weight;
                return;
            }

        }
        // should never get here - more bones than we have space for
        assert(0);
    }
};
class Mesh
{
public:
    /*  网格数据  */
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;
    unsigned int BaseVertex;

    /*  函数  */
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, int offset, std::vector<VertexBoneData> bones , Shader* shader);
    void setupMesh(vector<VertexBoneData>& Bones);
    void Draw();
private:
    /*  渲染数据  */
    Shader* m_pShaderProg;
    unsigned int VBO, EBO, boneBo;
    //map<unsigned int, glm::mat4> m_boneOffsets;
    /*  函数  */

};
#endif

