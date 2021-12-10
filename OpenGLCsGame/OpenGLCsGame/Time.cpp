#include "Time.h"

float Time::_deltaTime = 0.0f;
float Time::_curTime = 0.0f;
float Time::_startTime = 0.0f;
float Time::GetDeltaTime()
{
	return _deltaTime;
}

float Time::GetCurTime()
{
	return _curTime;
}

float Time::GetStartTimee()
{
	return _startTime;
}
