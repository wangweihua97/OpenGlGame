#include "Mesh.h"
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, int offset, std::vector<VertexBoneData> bones ,Shader *shader)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    BaseVertex = offset;
    m_pShaderProg = shader;
    setupMesh(bones);
}

void Mesh::setupMesh(vector<VertexBoneData>& Bones)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &boneBo);
    glBindBuffer(GL_ARRAY_BUFFER, boneBo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * vertices.size(), &Bones[BaseVertex], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindVertexArray(VAO);
    // load data into vertex buffers
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    /*glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);*/

    // set the vertex attribute pointers
    // vertex Positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, boneBo);
    /*glBindBuffer(GL_ARRAY_BUFFER, boneBo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBoneData) * Bones.size(), &Bones[0], GL_STATIC_DRAW);*/

    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexBoneData), (const void*)0);

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const void*)16);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // ids
    //glEnableVertexAttribArray(5);
    //glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    //// weights
    //glEnableVertexAttribArray(6);
    //glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    
}
void Mesh::Draw(Shader& shader)
{
    /*auto iter = m_boneOffsets.begin();
    while (iter != m_boneOffsets.end()) {
        glm::mat4 boneOffset = Transforms[iter->first] * iter->second;
        shader.setUniformIndex(iter->first, boneOffset);
        iter++;
    }*/
    // bind appropriate textures
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    unsigned int normalNr = 0;
    unsigned int heightNr = 0;
    unsigned int aoNr = 0;
    unsigned int metallicNr = 0;
    unsigned int roughnessNr = 0;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if (name == "albedoMap")
        {
            number = std::to_string(diffuseNr++);
            //cout << "+++++++++++" << textures[i].path << endl;
        }
        else if (name == "specularMap")
            number = std::to_string(specularNr++); // transfer unsigned int to stream
        else if (name == "normalMap")
            number = std::to_string(normalNr++); // transfer unsigned int to stream
        else if (name == "heightMap")
            number = std::to_string(heightNr++); // transfer unsigned int to stream
        else if (name == "aoMap")
            number = std::to_string(aoNr++); // transfer unsigned int to stream
        else if (name == "metallicMap")
            number = std::to_string(metallicNr++); // transfer unsigned int to stream
        else if (name == "roughnessMap")
            number = std::to_string(roughnessNr++);

        string a = number.compare("0") == 0 ? name : name + number;
        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(m_pShaderProg->ID, a.c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].texture2D->ID);
    }
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}
