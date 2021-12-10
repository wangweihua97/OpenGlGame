#ifndef Window_H
#define Window_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;
class Global;
class Time;
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
	void SetDeltaTime(float time);
	void SetStartTime(float time);
	void SetCurTime(float time);
	void InitGlobal();
private:
	GLFWwindow* window_ptr;
	Global* m_global;
};
#endif

