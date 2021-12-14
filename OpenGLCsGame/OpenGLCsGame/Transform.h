#ifndef Transform_H
#define Transform_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
using namespace std;
const float PI = 3.1415926536f;
class GameObject;
class Transform
{
public:
	static Transform* Root;
	GameObject* gameObject;
	Transform* parent;
	vector<Transform*> childs;

	glm::vec3 localPosition;
	glm::vec3 localXYZAngle;
	glm::vec3 localScale;

	glm::mat4 worldTransformMat;

	

	Transform(glm::vec3 localPosition = glm::vec3(0.0f), glm::vec3 localXYZAngle = glm::vec3(0.0f), glm::vec3 localScale = glm::vec3(1.0f), Transform* parent = Transform::Root);
	glm::mat4 GetLocalTransformMat();
	glm::mat4 SetWorldTransformMat();
	glm::mat4 GetWorldToLocalMat();
	glm::vec3 GetWordPosition();


	glm::mat4 GetLocalPositionMat();
	glm::mat4 GetLocalRotateMat(glm::mat4 model);
	glm::mat4 GetLocalScaleMat(glm::mat4 model);

	glm::vec3 GetWordForward();
	glm::vec3 GetWordRight();
	glm::vec3 GetWordUp();
	glm::vec3 GetLocalForward();
	glm::vec3 GetLocalRight();
	glm::vec3 GetLocalUp();

	void SetLocalForward(glm::vec3 forward);
	void SetLocalRight(glm::vec3 right);
	void SetLocalUp(glm::vec3 up);

	/*void SetForwardUp(glm::vec3 forward , glm::vec3 up);
	void SetRightUP(glm::vec3 right, glm::vec3 up);
	void SetForwardRight(glm::vec3 forward , glm::vec3 right);*/

	glm::mat4 WorldMatToLocal(glm::mat4 worldMat);
	glm::vec3 WorldDirToLocal(glm::vec3 worldVec);
	glm::vec3 LocalDirToWorld(glm::vec3 LocalVec);
	void Updete();
	void LateUpdate();
	void Render();
	void SetGameObject(GameObject* go);


private:
	glm::vec3 m_forward; //Z
	glm::vec3 m_right; //X
	glm::vec3 m_up; //Y
};
#endif

