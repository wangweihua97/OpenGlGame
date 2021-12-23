#ifndef GameObject_H
#define GameObject_H
#include "Transform.h"
#include <map>
#include <string>
#include "Component.h"
using namespace std;
class GameObject
{
public:
	GameObject(string name, Transform* parent);
	Transform* transform;
	map<string, Component*> Components;
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render();
	virtual void RenderShadow();
	string name;

	template<typename T>
	T* AddComponent()
	{
		T* t = new T(this);
		Component* component = &(*t);
		string name = typeid(T).name();
		auto find = Components.find(name);
		if (find != Components.end())
		{
			find->second->SetNext(component);
		}
		else
		{
			Components[name] = component;
		}
		return t;
	}
	template<typename T>
	T* GetComponent()
	{
		string name = typeid(T).name();
		auto find = Components.find(name);
		if (find != Components.end())
		{
			return find->second;
		}
		return nullptr;
	}
};
#endif

