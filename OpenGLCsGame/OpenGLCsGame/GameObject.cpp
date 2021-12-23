#include "GameObject.h"

GameObject::GameObject(string name, Transform* parent)
{
    this->name = name;
    transform = new Transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f),parent);
    transform->SetGameObject(this);
}

void GameObject::Update()
{
    map<string, Component*>::iterator  iter;
    for (iter = Components.begin(); iter != Components.end(); iter++) {
        iter->second->Update();
    }

}

void GameObject::LateUpdate()
{
    map<string, Component*>::iterator  iter;
    for (iter = Components.begin(); iter != Components.end(); iter++) {
        iter->second->LateUpdate();
    }
}

void GameObject::Render()
{
    map<string, Component*>::iterator  iter;
    for (iter = Components.begin(); iter != Components.end(); iter++) {
        iter->second->Render();
    }
}

void GameObject::RenderShadow()
{
    map<string, Component*>::iterator  iter;
    for (iter = Components.begin(); iter != Components.end(); iter++) {
        iter->second->RenderShadow();
    }
}
