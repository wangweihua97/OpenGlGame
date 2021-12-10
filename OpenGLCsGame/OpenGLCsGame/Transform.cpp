#include "Transform.h"


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
Transform* Transform::Root = new Transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), nullptr);

glm::mat4 Transform::GetLocalTransformMat()
{
	return GetLocalScaleMat(GetLocalRotateMat(GetLocalPositionMat()));
}

glm::mat4 Transform::SetWorldTransformMat()
{
	glm::mat4 worldTransformMat = GetLocalTransformMat();
	if (parent != Transform::Root)
		worldTransformMat = parent->worldTransformMat * worldTransformMat;
	return worldTransformMat;
}

glm::mat4 Transform::GetWorldToLocalMat()
{
	return glm::inverse(worldTransformMat);
}

glm::mat4 Transform::GetLocalPositionMat()
{
	glm::mat4 model(1.0f);
	return glm::translate(model, localPosition);
}

glm::mat4 Transform::GetLocalRotateMat(glm::mat4 model)
{
	model = localXYZAngle.z == 0.0f ? model : glm::rotate(model, localXYZAngle.z ,m_forward);
	model = localXYZAngle.y == 0.0f ? model : glm::rotate(model, localXYZAngle.y, m_up);
	model = localXYZAngle.x == 0.0f ? model : glm::rotate(model, localXYZAngle.x, m_right);
	return model;
}

glm::mat4 Transform::GetLocalScaleMat(glm::mat4 model)
{
	return glm::scale(model, localScale);
}

glm::vec3 Transform::GetWordForward()
{
	return LocalDirToWorld(glm::vec3(0.0f, 0.0f, 1.0f));
}

glm::vec3 Transform::GetWordRight()
{
	return LocalDirToWorld(glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 Transform::GetWordUp()
{
	return LocalDirToWorld(glm::vec3(0.0f, 1.0f, 0.0f));
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
