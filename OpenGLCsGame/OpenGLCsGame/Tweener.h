#ifndef Tweener_H
#define Tweener_H
#include <functional>
#include "ScheduleCommand.h"
#include "Time.h"
template <typename T>
class Tweener
{
private:
	PerFrameScheduleCommand* _command;
	std::function<void(void)> _complete;
	std::function<void(T)> _fun;
	std::function<T(T, T, float)> _easeFun;
	T _start;
	T _end;
	float _time;
	float _duration;
public:
	bool isRunning;
	Tweener(T start, T end, float duration, std::function<void(T)> fun, std::function<T(T, T, float)> ease)
	{
		_start = start;
		_end = end;
		_fun = fun;
		_time = 0;
		_duration = duration;
		_easeFun = ease;
		isRunning = true;
		auto perFrameScheduleCommand = [this]()
		{
			_time += Time::GetDeltaTime();
			T value = _easeFun(_start, _end, _time / _duration);
			_fun(value);
		};
		auto complete = [this]()
		{
			if (_complete)
				_complete();
			isRunning = false;
		};
		_command = new PerFrameScheduleCommand(perFrameScheduleCommand, duration);
		_command->onComplete = complete;
		Time::PushCommand(_command);
	}
	void OnComplete(std::function<void(void)> complete)
	{
		_complete = complete;
	}

	void DoKill(bool finish)
	{
		if (!isRunning)
			return;
		if (!finish)
		{
			_command->isDispose = true;
			delete this;
			return;
		}
		_fun(_end);
		if (_command->onComplete)
			_command->onComplete();
		_command->isDispose = true;
		delete this;
	}
};
#endif

