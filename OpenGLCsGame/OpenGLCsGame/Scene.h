#ifndef Scene_H
#define Scene_H
#include "Window.h"
#include <glm/glm.hpp>
#include <vector>
using namespace std;
class Transform;
class GameObject;
class CameraComponent;
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
	 glm::mat4 light;
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