#ifndef Input_H
#define Input_H
#include "Window.h"
enum class Key
{
	W = 0,
	A = 1,
	S = 2,
	D = 3,
	Mouse1 = 4,
	Mouse2 = 5,
	Space = 6,
	Shift = 7,
	R = 8,
};
class Input
{
private:
	bool _keys[9]{ false };
	void Check(Key key, bool newValue);
public:
	static Input* Instance;
	Input();
	void Update();
	bool KeyIsDown(Key key);
	static float xoffset, yoffset, xpos, ypos;

};
#endif

