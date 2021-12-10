#ifndef Global_H
#define Global_H
#include "Window.h"
class Global
{
private:
	Global();
	static Global* Init();
	friend void Window::InitGlobal();
public:
	static Global* Instace;
};
#endif