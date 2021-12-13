#include "Component.h"
#include "GameObject.h"

string Component::GetName()
{
	return _name;
}

Component::Component()
{
	_next = nullptr;
}

Component::Component(GameObject* gameObject)
{
	this->gameObject = gameObject;
	_next = nullptr;
}

void Component::SetNext(Component* next)
{
	if (_next == nullptr)
		_next = next;
	else
		_next->SetNext(next);
}

void Component::Update()
{
	if (_next != nullptr)
		_next->Update();
}

void Component::LateUpdate()
{
	if (_next != nullptr)
		_next->LateUpdate();
}

void Component::Render()
{
	if (_next != nullptr)
		_next->Render();
}

Component::~Component()
{
	;
}

void Component::SetName(string name)
{
	_name = name;
}
