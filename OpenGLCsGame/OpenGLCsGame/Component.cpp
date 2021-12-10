#include "Component.h"

string Component::GetName()
{
	return _name;
}

Component::Component(GameObject* gameObject)
{
	this->gameObject = gameObject;
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

void Component::SetName(string name)
{
	_name = name;
}
