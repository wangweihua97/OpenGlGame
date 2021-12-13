#ifndef Window_H
#define Window_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;
class Scene;
class Time;
class Input;
enum class Key;
void Resize(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
class Window
{
public:
	static Window* Instance;
	Window(int width, int height);

	~Window();
	void InitEvent();
	void InitInput();
	void InitResource();
	void Mainloop();
	int Width, Height;
	void SetDeltaTime(float time);
	void SetStartTime(float time);
	void SetCurTime(float time);
	void InitScene();
	void Log(string str);

	GLFWwindow* window_ptr;
private:
	void KeyDown(Key key);
	void KeyUp(Key key);
	Scene* m_scene;
	Input* _input;
};
#endif

