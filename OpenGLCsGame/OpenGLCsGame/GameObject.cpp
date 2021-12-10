#include "GameObject.h"

GameObject::GameObject(string name, Transform* parent)
{
    this->name = name;
    transform = Transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f),parent);
}

void GameObject::Update()
{
    map<string, Component>::iterator  iter;
    for (iter = Components.begin(); iter != Components.end(); iter++) {
        iter->second.Update();
    }

}
