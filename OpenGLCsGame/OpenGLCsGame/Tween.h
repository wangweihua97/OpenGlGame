#ifndef Tween_H
#define Tween_H
#include <functional>
#include "ScheduleCommand.h"
#include "Time.h"
#include "Tweener.h"
#include <glm/glm.hpp>
using namespace std;
enum class Ease
{
	Linear,
	InSine,
	OutSine,
	InOutSine,
	InQuad,
	OutQuad,
	InOutQuad
};

class Tween
{
public:
	template<typename T>
	static Tweener<T>* DoTween(T start, T end, float duration, std::function<void(T)> fun, Ease ease = Ease::Linear)
	{
		std::function<T(T, T, float)> easeFun;
		switch (ease)
		{
		case Ease::Linear:
			easeFun = std::bind((T(*)(T ,T, float))Linear, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			break;
		case Ease::InSine:
			easeFun = std::bind((T(*)(T, T, float))InSine, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			break;
		case Ease::OutSine:
			easeFun = std::bind((T(*)(T, T, float))OutSine, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			break;
		case Ease::InOutSine:
			easeFun = std::bind((T(*)(T, T, float))InOutSine, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			break;
		case Ease::InQuad:
			easeFun = std::bind((T(*)(T, T, float))InQuad, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			break;
		case Ease::OutQuad:
			easeFun = std::bind((T(*)(T, T, float))OutQuad, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			break;
		case Ease::InOutQuad:
			easeFun = std::bind((T(*)(T, T, float))InOutQuad, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			break;
		default:
			break;
		}
		return new Tweener<T>(start, end, duration, fun, easeFun);
	}

private:

	static glm::vec2 Linear(glm::vec2 start, glm::vec2  end, float percent)
	{
		glm::vec2 a;
		a.x = Linear(start.x, end.x, percent);
		a.y = Linear(start.y, end.y, percent);
		return a;
		//return glm::vec2(Linear(start.x, end.x, percent), Linear(start.y, end.y, percent));
	}
	static glm::vec2 InSine(glm::vec2  start, glm::vec2  end, float percent)
	{
		glm::vec2 a;
		a.x = InSine(start.x, end.x, percent);
		a.y = InSine(start.y, end.y, percent);
		return a;
	}
	static glm::vec2  OutSine(glm::vec2  start, glm::vec2  end, float percent)
	{
		glm::vec2 a;
		a.x = OutSine(start.x, end.x, percent);
		a.y = OutSine(start.y, end.y, percent);
		return a;
	}
	static glm::vec2  InOutSine(glm::vec2  start, glm::vec2  end, float percent)
	{
		glm::vec2 a;
		a.x = InOutSine(start.x, end.x, percent);
		a.y = InOutSine(start.y, end.y, percent);
		return a;
	}
	static glm::vec2  InQuad(glm::vec2  start, glm::vec2  end, float percent)
	{
		glm::vec2 a;
		a.x = InQuad(start.x, end.x, percent);
		a.y = InQuad(start.y, end.y, percent);
		return a;
	}
	static glm::vec2  OutQuad(glm::vec2  start, glm::vec2  end, float percent)
	{
		glm::vec2 a;
		a.x = OutQuad(start.x, end.x, percent);
		a.y = OutQuad(start.y, end.y, percent);
		return a;
	}
	static glm::vec2  InOutQuad(glm::vec2  start, glm::vec2  end, float percent)
	{
		glm::vec2 a;
		a.x = InOutQuad(start.x, end.x, percent);
		a.y = InOutQuad(start.y, end.y, percent);
		return a;
	}

	static glm::vec3  Linear(glm::vec3 start, glm::vec3  end, float percent)
	{
		glm::vec3 a;
		a.x = Linear(start.x, end.x, percent);
		a.y = Linear(start.y, end.y, percent);
		a.z = Linear(start.z, end.z, percent);
		return a;
	}
	static glm::vec3  InSine(glm::vec3  start, glm::vec3  end, float percent)
	{
		glm::vec3 a;
		a.x = InSine(start.x, end.x, percent);
		a.y = InSine(start.y, end.y, percent);
		a.z = InSine(start.z, end.z, percent);
		return a;
	}
	static glm::vec3  OutSine(glm::vec3  start, glm::vec3  end, float percent)
	{
		glm::vec3 a;
		a.x = OutSine(start.x, end.x, percent);
		a.y = OutSine(start.y, end.y, percent);
		a.z = OutSine(start.z, end.z, percent);
		return a;
	}
	static glm::vec3  InOutSine(glm::vec3  start, glm::vec3  end, float percent)
	{
		glm::vec3 a;
		a.x = InOutSine(start.x, end.x, percent);
		a.y = InOutSine(start.y, end.y, percent);
		a.z = InOutSine(start.z, end.z, percent);
		return a;
	}
	static glm::vec3  InQuad(glm::vec3  start, glm::vec3  end, float percent)
	{
		glm::vec3 a;
		a.x = InQuad(start.x, end.x, percent);
		a.y = InQuad(start.y, end.y, percent);
		a.z = InQuad(start.z, end.z, percent);
		return a;
	}
	static glm::vec3  OutQuad(glm::vec3  start, glm::vec3  end, float percent)
	{
		glm::vec3 a;
		a.x = OutQuad(start.x, end.x, percent);
		a.y = OutQuad(start.y, end.y, percent);
		a.z = OutQuad(start.z, end.z, percent);
		return a;
	}
	static glm::vec3  InOutQuad(glm::vec3  start, glm::vec3  end, float percent)
	{
		glm::vec3 a;
		a.x = InOutQuad(start.x, end.x, percent);
		a.y = InOutQuad(start.y, end.y, percent);
		a.z = InOutQuad(start.z, end.z, percent);
		return a;
	}

	static float Linear(float start, float end, float percent);
	static float InSine(float start, float end, float percent);
	static float OutSine(float start, float end, float percent);
	static float InOutSine(float start, float end, float percent);
	static float InQuad(float start, float end, float percent);
	static float OutQuad(float start, float end, float percent);
	static float InOutQuad(float start, float end, float percent);
};
#endif

