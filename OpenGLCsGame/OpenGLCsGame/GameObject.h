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
	string name;

	template<typename T>
	T AddComponent()
	{
		shared_ptr<T> t(new T(this));
		auto find = Components.find(t->GetName());
		if (find != Components.end())
		{
			find->second->SetNext(t);
		}
		else
		{
			Components[t->GetName()] = t;
		}
		return (T)(*t);
	}
};
#endif

