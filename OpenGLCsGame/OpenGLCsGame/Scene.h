#ifndef Scene_H
#define Scene_H
#include "Window.h"
#include <glm/glm.hpp>
#include <vector>
using namespace std;
class Transform;
class GameObject;
class CameraComponent;
struct Light
{
	glm::vec3 lightDir;
	glm::vec3 color;
	glm::vec3 ambient;
	float gloss;
};
class Scene
{
private:
	 unsigned int _uboPV;
	 unsigned int _uboBaseView;
	 unsigned int _uboBaseLight;
	Scene();
	static Scene* Init();
	friend void Window::InitScene();
public:
	 GameObject* player;
	 GameObject* cube;
	 Light light;
	static Scene* Instace;
	void Update();
	void LateUpdate();
	void Render();
    GameObject* RootGameObject;
	Transform* RootTransform;
	CameraComponent* MainCamera;
	vector<GameObject*> Gos;
};
#endif