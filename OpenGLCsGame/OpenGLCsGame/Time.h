#ifndef Time_H
#define Time_H
#include "Window.h"

class Time
{
private:
	static float _deltaTime;
	static float _startTime;
	static float _curTime;
	friend void Window::SetDeltaTime(float time);
	friend void Window::SetCurTime(float time);
	friend void Window::SetStartTime(float time);
public:
	static float GetDeltaTime();
	static float GetCurTime();
	static float GetStartTimee();
};
#endif

