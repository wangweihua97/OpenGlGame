#include "CubeMeshComponent.h"
#include "ResourceManager.h"
#include "GameObject.h"

CubeMeshComponent::CubeMeshComponent(GameObject* gameObject) :Component(gameObject)
{
}

void CubeMeshComponent::SetShader(Shader& shader)
{
    _shader = &shader;
}

void CubeMeshComponent::SetBound(float length, float width, float height)
{
    ResourceManager::GetShader("CubeShader").Use();
    gameObject->transform->localScale = glm::vec3(length , width ,height);
    float v[]{
        // positions          // texture Coords
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,

        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,

        -1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

        -1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 1.0f,

        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f
    };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), &v, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUniform1i(glGetUniformLocation(ResourceManager::GetShader("CubeShader").ID, "albedoMap"), 0);
    // and finally bind the texture
    glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTexture("map").ID);
}

void CubeMeshComponent::Update()
{
}

void CubeMeshComponent::LateUpdate()
{
}

void CubeMeshComponent::Render()
{
    glm::mat4 a = gameObject->transform->worldTransformMat;
    ResourceManager::GetShader("CubeShader").SetMatrix4("model", a ,true);
   
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
