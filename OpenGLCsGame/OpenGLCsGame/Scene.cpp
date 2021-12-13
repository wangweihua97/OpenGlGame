#include "Scene.h"
#include "Transform.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "EventCenter.h"
#include <glm/gtc/type_ptr.hpp>
#include "CubeMeshComponent.h"
#include "ResourceManager.h"
Scene* Scene::Instace = nullptr;
GameObject* Scene::player = nullptr;
GameObject* Scene::cube = nullptr;
GameObject* Scene::RootGameObject = nullptr;
Transform* Scene::RootTransform = nullptr;
CameraComponent* Scene::MainCamera = nullptr;
unsigned int Scene::_uboPV = 0;
Scene::Scene()
{
	Scene::Instace = this;
}

Scene* Scene::Init()
{
	glGenBuffers(1, &_uboPV);

	glBindBuffer(GL_UNIFORM_BUFFER, _uboPV);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, _uboPV);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4(1.0f)));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::mat4(1.0f)));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, _uboPV, 0, 2 * sizeof(glm::mat4));
	RootGameObject = new GameObject("Root", nullptr);
	Transform::Root = new Transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f),nullptr);
	Transform::Root->gameObject = RootGameObject;

	player = new GameObject("mySelf", Transform::Root);
	CameraComponent camera = player->AddComponent<CameraComponent>();
	camera.Active();

	cube = new GameObject("Cube", Transform::Root);
	cube->transform->localPosition = glm::vec3(0.0f, 0.0f, 10.0f);
	CubeMeshComponent cubeMeshComponent = cube->AddComponent<CubeMeshComponent>();
	Shader s = ResourceManager::GetShader("CubeShader");
	cubeMeshComponent.SetShader(s);
	cubeMeshComponent.SetBound(1.0f, 1.0f, 1.0f);
	return new Scene();
}

void Scene::Update()
{
	Transform::Root->Updete();
}

void Scene::LateUpdate()
{
	Transform::Root->LateUpdate();
	if (Scene::MainCamera != nullptr)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, _uboPV);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(Scene::MainCamera->GetPerspective()));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(Scene::MainCamera->GetViewMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}

void Scene::Render()
{
	Transform::Root->Render();
}
