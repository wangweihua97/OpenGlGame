#ifndef CameraComponent_H
#define CameraComponent_H
#include"Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class CameraComponent : public Component
{
public:
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float ZOOM = 45.0f;
	CameraComponent(GameObject* gameObject);
	glm::mat4 GetPerspective();
	glm::mat4 GetViewMatrix();
	void Active();
	void Update()override;
	void LateUpdate()override;
	void Render()override;
};
#endif

