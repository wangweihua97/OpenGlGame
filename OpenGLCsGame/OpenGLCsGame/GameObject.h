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
	Transform transform;
	map<string, Component> Components;
	virtual void Update();
	string name;

	template<typename T>
	T AddComponent()
	{
		T t(this);
		Component component = t;
		auto find = Components.find(component.GetName());
		if (find != Components.end())
		{
			find->second.SetNext(component);
		}
		else
		{
			Components[component.GetName()] = component;
		}
		return t;
	}
	
};
#endif

