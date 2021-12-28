#include "Time.h"
#include <boost/coroutine2/all.hpp>

float Time::_deltaTime = 0.0f;
float Time::_curTime = 0.0f;
float Time::_startTime = 0.0f;

int Time::commandStartIndex = 0;
int Time::commandCount = 0;
ScheduleCommand* Time::commands[MAX_COMMANDNUM];
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

void Time::Update()
{
	Execute();
}

void Time::PushCommand(ScheduleCommand* schedules)
{
	assert(commandCount < MAX_COMMANDNUM);
	int insertIndex = (commandStartIndex + commandCount) % MAX_COMMANDNUM;
	commands[insertIndex] = schedules;
	commandCount++;

}
void Time::PullCommand(ScheduleCommand* schedules)
{
	delete schedules;
	commandCount--;
}
void Time::MoveToBack(ScheduleCommand* schedules)
{
	int moveIndex = (commandStartIndex + commandCount) % MAX_COMMANDNUM;
	commands[moveIndex] = schedules;
}
void Time::Execute()
{
	unsigned int start = commandStartIndex;
	unsigned int end = start + commandCount;
	while (start < end)
	{
		int index = start % MAX_COMMANDNUM;
		start++;
		if (commands[index]->isDispose)
		{
			PullCommand(commands[index]);
			commandStartIndex++;
			continue;
		}
		commands[index]->Execute(_deltaTime);
		if (commands[index]->isExecutedEnd)
		{
			if (commands[index]->onComplete)
				commands[index]->onComplete();
			PullCommand(commands[index]);
		}
		else
		{
			MoveToBack(commands[index]);
		}
		commandStartIndex++;
	}
}
