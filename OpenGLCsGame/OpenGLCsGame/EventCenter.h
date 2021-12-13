#ifndef EventCenter_H
#define EventCenter_H
#include <string>
#include <vector>
#include <Map>
#include <boost/function.hpp>
using namespace std;

class MultipleBase
{

};
class MultipleAciton : public MultipleBase
{
private:
    map<int ,boost::function<void()>> _map;
    unsigned int _id;
public:
    MultipleAciton()
    {
        _id = 0;
    }
    int AddAction(boost::function<void()> ation)
    {
        _map[++_id] = ation;
        return _id;
    }

    void RemoveAction(int id)
    {
        _map.erase(id);
    }
    void Invoke()
    {
        for (map<int, boost::function<void()>>::iterator itera = _map.begin(); itera != _map.end(); itera++)
        {
            itera->second();
            
        }
    }
};
template <typename T>
class MultipleAcitonOneParame : public MultipleBase
{
private:
    map<int, boost::function<void(T)>> _map;
    unsigned int _id;
public:
    MultipleAcitonOneParame()
    {
        _id = 0;
    }
    int AddAction(boost::function<void(T)> ation)
    {
        _map[++_id] = ation;
        return _id;
    }

    void RemoveAction(int id)
    {
        _map.erase(id);
    }
    void Invoke(T value)
    {
        for (auto iter = _map.begin(); iter != _map.end(); iter++)
        {
            iter->second(value);

        }
    }
};

class EventCenter
{
public:
    static EventCenter* Instance;
    int AddEvent(string eventName ,boost::function<void()> ation)
    {
        if (_map.find(eventName) == _map.end())
        {
            _map[eventName] = new MultipleAciton();
        }
        return reinterpret_cast<MultipleAciton*>(_map[eventName])->AddAction(ation);
    }
    template <typename T>
    int AddEvent(string eventName ,boost::function<void(T)> ation)
    {
        if (_map.find(eventName) == _map.end())
        {
            _map[eventName] = new MultipleAcitonOneParame<T>();
        }
        return reinterpret_cast<MultipleAcitonOneParame<T>*>(_map[eventName])->AddAction(ation);
    }
    void Remove(string eventName, int id)
    {
        if (_map.find(eventName) != _map.end())
        {
            reinterpret_cast<MultipleAciton*>(_map[eventName])->RemoveAction(id);
        }
    }
    template <typename T>
    void Remove(string eventName, int id)
    {
        if (_map.find(eventName) != _map.end())
        {
            reinterpret_cast<MultipleAcitonOneParame*>(_map[eventName])->RemoveAction(id);
        }
        
    }
    void Invoke(string eventName)
    {
        if (_map.find(eventName) != _map.end())
        {
            reinterpret_cast<MultipleAciton*>(_map[eventName])->Invoke();
        }
    }
    template <typename T>
    void Invoke(string eventName ,T value)
    {
        if (_map.find(eventName) != _map.end())
        {
            reinterpret_cast<MultipleAcitonOneParame<T>*>(_map[eventName])->Invoke(value);
        }
    }

  
private:
    EventCenter();
    map<string, MultipleBase*> _map;
};
#endif

