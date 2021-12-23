#include "Transform.h"
#include "GameObject.h"
#include <glm/gtc/quaternion.hpp>


Transform::Transform(glm::vec3 localPosition, glm::vec3 localXYZAngle, glm::vec3 localScale, Transform* parent) :
	localPosition(localPosition), localXYZAngle(localXYZAngle), localScale(localScale), parent(parent)
{
	m_forward = glm::vec3(0.0f, 0.0f, 1.0f);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_right = glm::vec3(1.0f, 0.0f, 0.0f);
	if (parent != nullptr)
	{
		parent->childs.push_back(this);
		worldTransformMat = parent->worldTransformMat;
	}
	else
	{
		worldTransformMat = glm::mat4(1.0f);
	}
}
Transform* Transform::Root = nullptr;

glm::mat4 Transform::GetLocalTransformMat()
{
	/*glm::mat4 model(1.0f);
	model = glm::translate(model, localPosition);
	model = localXYZAngle.z == 0.0f ? model : glm::rotate(model, localXYZAngle.z, m_forward);
	model = localXYZAngle.y == 0.0f ? model : glm::rotate(model, localXYZAngle.y, m_up);
	model = localXYZAngle.x == 0.0f ? model : glm::rotate(model, localXYZAngle.x, m_right);
	model = glm::scale(model, localScale);
	return model;*/
	return GetLocalScaleMat(GetLocalRotateMat(GetLocalPositionMat()));
}

glm::mat4 Transform::SetWorldTransformMat()
{
	glm::mat4 worldTransformMat = GetLocalTransformMat();
	if (this != Transform::Root && parent != Transform::Root)
		worldTransformMat = parent->worldTransformMat * worldTransformMat;
	this->worldTransformMat = worldTransformMat;
	return worldTransformMat;
}

glm::mat4 Transform::GetWorldToLocalMat()
{
	return glm::inverse(worldTransformMat);
}

glm::vec3 Transform::GetWordPosition()
{
	glm::vec3 worldP =  glm::vec3(parent->worldTransformMat * glm::vec4(localPosition ,1.0f));
	return worldP;
}

glm::mat4 Transform::GetLocalPositionMat()
{
	glm::mat4 model(1.0f);
	return glm::translate(model, localPosition);
}

glm::mat4 Transform::GetLocalRotateMat(glm::mat4 model)
{
	//先旋转y再x再z
	/*glm::quat Rotation = glm::quat(1.0f, glm::radians(localXYZAngle.x), glm::radians(localXYZAngle.y), glm::radians(localXYZAngle.z));
	glm::mat4 RotationM = glm::mat4_cast(Rotation);*/
	m_forward = glm::vec3(0.0f, 0.0f, 1.0f);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_right = glm::vec3(1.0f, 0.0f, 0.0f);
	auto a = glm::angleAxis(glm::radians(localXYZAngle.y),m_up);
	m_forward = a * m_forward;
	m_right = a * m_right;
	auto b = glm::angleAxis(glm::radians(localXYZAngle.x), m_right);
	m_up = b * m_up; 
	m_forward = b * m_forward;
	auto c = glm::angleAxis(glm::radians(localXYZAngle.z), m_forward);
	m_up = c * m_up;
	m_right = c * m_right;
	/*model = localXYZAngle.z == 0.0f ? model : glm::rotate(model, localXYZAngle.z ,m_forward);
	model = localXYZAngle.y == 0.0f ? model : glm::rotate(model, localXYZAngle.y, m_up);
	model = localXYZAngle.x == 0.0f ? model : glm::rotate(model, localXYZAngle.x, m_right);*/
	return glm::mat4_cast(c*b*a) * model;
}

glm::mat4 Transform::GetLocalScaleMat(glm::mat4 model)
{
	return glm::scale(model, localScale);
}

glm::vec3 Transform::GetWordForward()
{
	return LocalDirToWorld(glm::vec3(0.0f,0.0f,1.0f));
	//return LocalDirToWorld(GetLocalForward());
}

glm::vec3 Transform::GetWordRight()
{
	return LocalDirToWorld(glm::vec3(1.0f, 0.0f, 0.0f));
	//return LocalDirToWorld(GetLocalRight());
}

glm::vec3 Transform::GetWordUp()
{
	return LocalDirToWorld(glm::vec3(0.0f, 1.0f, 0.0f));
	//return LocalDirToWorld(GetLocalUp());
}

glm::vec3 Transform::GetLocalForward()
{
	/*glm::vec3 f(0.0f);
	f.y = sin(glm::radians(localXYZAngle.x));
	float k = sqrtf(1 - f.y * f.y);
	f.x = k * sin(glm::radians(localXYZAngle.y));
	f.z = k * cos(glm::radians(localXYZAngle.y));
	return glm::normalize(f);*/
	return m_forward;
}

glm::vec3 Transform::GetLocalRight()
{
	/*glm::vec3 f(0.0f);
	f.y = sin(-localXYZAngle.z);
	float k = sqrtf(1 - f.y * f.y);
	f.x = k * cos(localXYZAngle.y);
	f.z = k * sin(localXYZAngle.y);
	return glm::normalize(f);*/
	return m_right;
}

glm::vec3 Transform::GetLocalUp()
{
	/*glm::vec3 f(0.0f);
	f.z = sin(-localXYZAngle.x);
	float k = sqrtf(1 - f.z * f.z);
	f.x = k * sin(localXYZAngle.z);
	f.z = k * cos(localXYZAngle.z);
	return glm::normalize(f);*/
	return m_up;
}

void Transform::SetLocalForward(glm::vec3 forward)
{
	localXYZAngle.y = atan2f(-forward.x, forward.z) * (180.f / PI);
	localXYZAngle.x = atan2f(forward.y, forward.x * forward.x + forward.z * forward.z) * (180.f / PI);
	localXYZAngle.z = 0.0f;
}

void Transform::SetLocalRight(glm::vec3 right)
{
	localXYZAngle.y = atan2f(right.z, right.x) * (180.f / PI);
	localXYZAngle.z = atan2f(-right.y, right.x * right.x + right.z * right.z) * (180.f / PI);
	localXYZAngle.x = 0.0f;
}

void Transform::SetLocalUp(glm::vec3 up)
{
	localXYZAngle.z = atan2f(up.x, up.y) * (180.f / PI);
	localXYZAngle.x = atan2f(-up.z, up.x * up.x + up.y * up.y) * (180.f / PI);
	localXYZAngle.y = 0.0f;
}

glm::mat4 Transform::WorldMatToLocal(glm::mat4 worldMat)
{
	return GetWorldToLocalMat() * worldMat;
}

glm::vec3 Transform::WorldDirToLocal(glm::vec3 worldVec)
{
	glm::mat3 worldToLocalMat(glm::transpose(GetWorldToLocalMat()));
	glm::vec3 localVec3 = glm::inverse(worldToLocalMat) * worldVec;
	return glm::normalize(localVec3);
}

glm::vec3 Transform::LocalDirToWorld(glm::vec3 LocalVec)
{
	glm::mat3 worldToObject(GetWorldToLocalMat());
	glm::vec3 worldVec = glm::transpose(worldToObject) * LocalVec;
	return glm::normalize(worldVec);
}

void Transform::Updete()
{
	SetWorldTransformMat();
	gameObject->Update();
	for (vector<Transform*>::iterator iter = childs.begin(); iter != childs.end(); iter++)
	{
		(*iter)->Updete();
	}
}

void Transform::LateUpdate()
{
	gameObject->LateUpdate();
	for (vector<Transform*>::iterator iter = childs.begin(); iter != childs.end(); iter++)
	{
		(*iter)->LateUpdate();
	}
}

void Transform::Render()
{
	gameObject->Render();
	for (vector<Transform*>::iterator iter = childs.begin(); iter != childs.end(); iter++)
	{
		(*iter)->Render();
	}
}

void Transform::RenderShadow()
{
	gameObject->RenderShadow();
	for (vector<Transform*>::iterator iter = childs.begin(); iter != childs.end(); iter++)
	{
		(*iter)->RenderShadow();
	}
}

void Transform::SetGameObject(GameObject* go)
{
	gameObject = go;
}
