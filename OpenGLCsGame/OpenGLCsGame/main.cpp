#include "main.h"
Test T()
{
	Test t = Test();
	return t;
}
Test* T2()
{
	Test* t = new Test(10);
	return t;
}
int main()
{
	Window* window = new Window(800, 600);
	window->Mainloop();
	delete window;
	return 0;
}
