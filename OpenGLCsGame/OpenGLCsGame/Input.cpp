#include "Input.h"
#include "EventCenter.h"
Input* Input::Instance = nullptr;
float Input::xoffset = 0.0f;
float Input::yoffset = 0.0f;
float Input::xpos = 0.0f;
float Input::ypos = 0.0f;
Input::Input()
{
	Input::Instance = this;
}

void Input::Update()
{
	if (glfwGetKey(Window::Instance->window_ptr, GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(Window::Instance->window_ptr, true);
	}
	Check(Key::A, glfwGetKey(Window::Instance->window_ptr, GLFW_KEY_A));
	Check(Key::W, glfwGetKey(Window::Instance->window_ptr, GLFW_KEY_W));
	Check(Key::S, glfwGetKey(Window::Instance->window_ptr, GLFW_KEY_S));
	Check(Key::D, glfwGetKey(Window::Instance->window_ptr, GLFW_KEY_D));
	Check(Key::Mouse1, glfwGetKey(Window::Instance->window_ptr, GLFW_MOUSE_BUTTON_LEFT));
	Check(Key::Mouse2, glfwGetKey(Window::Instance->window_ptr, GLFW_MOUSE_BUTTON_RIGHT));
	Check(Key::Space, glfwGetKey(Window::Instance->window_ptr, GLFW_KEY_SPACE));
	Check(Key::Shift, glfwGetKey(Window::Instance->window_ptr, GLFW_KEY_RIGHT_SHIFT));
}

bool Input::KeyIsDown(Key key)
{
	return _keys[(int)key];
}

void Input::Check(Key key, bool newValue)
{
	if (_keys[(int)key] != newValue)
	{
		if (newValue)
			EventCenter::Instance->Invoke<Key>("KeyDown", key);
		else
			EventCenter::Instance->Invoke<Key>("KeyUp", key);
		_keys[(int)key] = newValue;
	}}
