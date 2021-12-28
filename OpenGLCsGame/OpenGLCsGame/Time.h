#ifndef Time_H
#define Time_H
#include "Window.h"
#include "ScheduleCommand.h"
class Time
{
private:
	static float _deltaTime;
	static float _startTime;
	static float _curTime;
	friend void Window::SetDeltaTime(float time);
	friend void Window::SetCurTime(float time);
	friend void Window::SetStartTime(float time);
	static void PullCommand(ScheduleCommand* schedules);
	static void MoveToBack(ScheduleCommand* schedules);
	static void Execute();
public:
	static const unsigned int MAX_COMMANDNUM = 1999;
	static ScheduleCommand* commands[];
	static int commandStartIndex;
	static int commandCount;
	static float GetDeltaTime();
	static float GetCurTime();
	static float GetStartTimee();
	static void Update();
	static void PushCommand(ScheduleCommand* schedules);
};
#endif

