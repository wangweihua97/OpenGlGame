#ifndef CubeMeshComponent_H
#define CubeMeshComponent_H
#include"Component.h"
#include"Shader.h"
class CubeMeshComponent :public Component
{
public:
	float length, width, height;
	unsigned int cubeVAO;
	unsigned int cubeVBO;
	float* cubeVertices;
	CubeMeshComponent(GameObject* gameObject);
	void SetShader(Shader* shader);
	void SetBound(float length, float width, float height);
	void Update()override;
	void LateUpdate()override;
	void Render()override;
private:
	Shader* _shader;
};
#endif

