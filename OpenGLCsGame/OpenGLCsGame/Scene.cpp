#include "Scene.h"
#include "Transform.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "EventCenter.h"
#include <glm/gtc/type_ptr.hpp>
#include "CubeMeshComponent.h"
#include "ResourceManager.h"
#include "MoveComponent.h"
#include "ModelComponent.h"
Scene* Scene::Instace = nullptr;

Scene::Scene()
{
	Scene::Instace = this;
	light.lightDir = glm::normalize(glm::vec3(0.0f, -1.0f, 1.5f));
	light.color = glm::vec3(1.0f);
	light.ambient = glm::vec3(0.0f,0.0f,0.0f);
	light.gloss = 5.0f;

	glGenBuffers(1, &_uboPV);
	glBindBuffer(GL_UNIFORM_BUFFER, _uboPV);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, _uboPV, 0, 2 * sizeof(glm::mat4));


	glGenBuffers(1, &_uboBaseLight);
	glBindBuffer(GL_UNIFORM_BUFFER, _uboBaseLight);
	glBufferData(GL_UNIFORM_BUFFER,sizeof(Light), &light, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, _uboBaseLight, 0, sizeof(Light));

	glGenBuffers(1, &_uboBaseView);
	glBindBuffer(GL_UNIFORM_BUFFER, _uboBaseView);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 2, _uboBaseView, 0, sizeof(glm::vec3));
	
	/*glBindBuffer(GL_UNIFORM_BUFFER, _uboPV);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4(1.0f)));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::mat4(1.0f)));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);*/
	RootGameObject = new GameObject("Root", nullptr);
	Transform::Root = new Transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), nullptr);
	Transform::Root->gameObject = RootGameObject;

	player = new GameObject("mySelf", Transform::Root);
	player->transform->localPosition = glm::vec3(0.0f, 0.5f, 0.0f);
	CameraComponent* camera = player->AddComponent<CameraComponent>();
	MoveComponent* move = player->AddComponent<MoveComponent>();
	camera->Active();
	cube = new GameObject("Cube", Transform::Root);
	cube->transform->localPosition = glm::vec3(0.0f, 0.0f, 10.0f);
	CubeMeshComponent* cubeMeshComponent = cube->AddComponent<CubeMeshComponent>();
	cubeMeshComponent->SetShader(ResourceManager::GetShaderP("CubeShader"));
	cubeMeshComponent->SetBound(100.0f, 0.2f, 100.0f);

	GameObject* c = new GameObject("swat", Transform::Root);
	c->transform->localPosition = glm::vec3(0.0f, 0.5f, 0.0f);
	c->transform->localScale = glm::vec3(0.1f, 0.1f, 0.1f);
	ModelComponent* model = c->AddComponent<ModelComponent>();
	model->LoadModel(ResourceManager::GetShaderP("ModelShader") ,"Model/swat/Swat.fbx" ,false);
	Gos.push_back(c);
}

Scene* Scene::Init()
{
	return new Scene();
}

void Scene::Update()
{
	Transform::Root->Updete();
}

void Scene::LateUpdate()
{
	if (Scene::MainCamera != nullptr)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, _uboPV);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(Scene::MainCamera->GetPerspective()));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(Scene::MainCamera->GetViewMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBuffer(GL_UNIFORM_BUFFER, _uboBaseView);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), glm::value_ptr(Scene::MainCamera->gameObject->transform->GetWordPosition()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	Transform::Root->LateUpdate();
}

void Scene::Render()
{
	Transform::Root->Render();
}
