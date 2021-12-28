#ifndef ScheduleCommand_H
#define ScheduleCommand_H
#include <functional>
using namespace std;
class ScheduleCommand
{
public:
	float curTime;
	bool isDispose;
	bool isExecutedEnd;
	std::function<void(void)> action;
	std::function<void(void)> onComplete;
	ScheduleCommand(std::function<void(void)> fun)
	{
		isDispose = false;
		isExecutedEnd = false;
		curTime = 0;
		action = fun;
	}
	~ScheduleCommand()
	{

	}
	virtual void Execute(float deltaTime) {
		curTime += deltaTime;
	};
};

//每隔几秒，循环执行
class LoopInSecondScheduleCommand : public ScheduleCommand
{
public:
	float timePerLoop;
	float timeInLoop;
	LoopInSecondScheduleCommand(std::function<void(void)> fun,float time) :ScheduleCommand(fun)
	{
		timePerLoop = time;
		timeInLoop = 0;
	}
	void Execute(float deltaTime) override
	{
		__super::Execute(deltaTime);
		timeInLoop += deltaTime;
		if (timeInLoop > timePerLoop)
		{
			action();
			timeInLoop -= timePerLoop;
		}
	}
};

//每帧循环执行
class LoopPerFrameScheduleCommand : public ScheduleCommand
{
public:
	LoopPerFrameScheduleCommand(std::function<void(void)> fun) :ScheduleCommand(fun)
	{
	}
	void Execute(float deltaTime) override
	{
		__super::Execute(deltaTime);
		action();
	}
};

//执行固定次数的任务
class TimesScheduleCommand : public ScheduleCommand
{
public:
	unsigned int times;
	float timePerFrequency;
	float timeInFrequency;
	TimesScheduleCommand(std::function<void(void)> fun,int times , float timePerFrequency) :ScheduleCommand(fun) ,times(times) , timePerFrequency(timePerFrequency)
	{
		timeInFrequency = 0;
	}
	void Execute(float deltaTime) override
	{
		__super::Execute(deltaTime);
		timeInFrequency += deltaTime;
		if (timeInFrequency > timePerFrequency)
		{
			action();
			timeInFrequency -= timePerFrequency;
			times--;
			if (times <= 0)
				isExecutedEnd = true;
		}
	}
};

//每帧都执行，持续多少秒
class PerFrameScheduleCommand : public ScheduleCommand
{
public:
	float totalTime;
	PerFrameScheduleCommand(std::function<void(void)> fun, int totalTime) :ScheduleCommand(fun), totalTime(totalTime)
	{
	}
	void Execute(float deltaTime) override
	{
		__super::Execute(deltaTime);
		action();
		if(curTime > totalTime)
			isExecutedEnd = true;
	}
};

//延迟执行
class DelayScheduleCommand : public ScheduleCommand
{
public:
	float DelayTime;
	DelayScheduleCommand(std::function<void(void)> fun, int DelayTime) :ScheduleCommand(fun), DelayTime(DelayTime)
	{
	}
	void Execute(float deltaTime) override
	{
		__super::Execute(deltaTime);
		if (curTime > DelayTime)
		{
			action();
			isExecutedEnd = true;
		}
	}
};
#endif
