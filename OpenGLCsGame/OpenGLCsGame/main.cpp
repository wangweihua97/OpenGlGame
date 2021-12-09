#include "main.h"
int main()
{
	Window* window = new Window(800, 600);
	window->Mainloop();
	delete window;
	return 0;
}
