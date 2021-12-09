#ifndef Window_H
#define WindowH
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;
void Resize(GLFWwindow* window, int width, int height);
class Window
{
public:
	static Window* Instance;
	Window(int width, int height);

	~Window();

	void Input();
	void Mainloop();

	int Width, Height;
private:
	GLFWwindow* window_ptr;
	
};
#endif

