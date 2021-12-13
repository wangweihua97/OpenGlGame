#ifndef Scene_H
#define Scene_H
#include "Window.h"
using namespace std;
class Transform;
class GameObject;
class CameraComponent;
class Scene
{
private:
	static unsigned int _uboPV;
	Scene();
	static Scene* Init();
	friend void Window::InitScene();
public:
	static GameObject* player;
	static GameObject* cube;
	static Scene* Instace;
	void Update();
	void LateUpdate();
	void Render();
	static GameObject* RootGameObject;
	static Transform* RootTransform;
	static CameraComponent* MainCamera;
};
#endif