#include "Window.h"
#include "Scene.h"
#include "Time.h"
#include "Input.h"
#include "EventCenter.h"
#include "ResourceManager.h"
#include <boost/bind/bind.hpp>
#include <stb_image.h>
Window* Window::Instance = NULL;
Window::Window(int width, int height)
{
	Width = width;
	Height = height;
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


#endif

	// glfw window creation
	// --------------------
	window_ptr = glfwCreateWindow(width, height, "CSGame", NULL, NULL);
	if (window_ptr == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window_ptr);
	glfwSetFramebufferSizeCallback(window_ptr, Resize);
	glfwSetCursorPosCallback(window_ptr, mouseCallback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
	Window::Instance = this;
}

Window::~Window()
{
	glfwTerminate();
}

void Window::InitEvent()
{
	EventCenter::Instance->AddEvent<string>("Log", boost::bind(&Window::Log, this, placeholders::_1));
	EventCenter::Instance->AddEvent<Key>("KeyDown", boost::bind(&Window::KeyDown, this, placeholders::_1));
	EventCenter::Instance->AddEvent<Key>("KeyUp", boost::bind(&Window::KeyUp, this, placeholders::_1));
}

void Window::InitInput()
{
	_input = new Input();
}

void Window::InitResource()
{
	
	ResourceManager::LoadShader("Shader/cube_vs.vs", "Shader/cube_fs.fs", nullptr, "CubeShader");
	ResourceManager::LoadShader("Shader/model.vs", "Shader/model.fs", nullptr, "ModelShader");
	ResourceManager::LoadShader("Shader/skeleton_model.vs", "Shader/skeleton_model.fs", nullptr, "SkeletonModelShader");
	ResourceManager::LoadTexture("Texture/map.jpeg", false, "map");
}

void Resize(GLFWwindow* window, int width, int height)
{
	glfwGetWindowSize(window, &width, &height);
	if (width != Window::Instance->Width || Window::Instance->Height)
	{
		Window::Instance->Width = width;
		Window::Instance->Height = height;
		glViewport(0, 0, width, height);
		printf("resized \n");
	}
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	Input::xoffset = xpos - Input::xpos;
	Input::yoffset = ypos - Input::ypos;

	Input::xpos = xpos;
	Input::ypos = ypos;
}

void Window::Mainloop()
{
	
	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load2(true);
	glEnable(GL_DEPTH_TEST);

	InitResource();
	InitEvent();
	InitInput();
	InitScene();
	float starTime = glfwGetTime();
	float currentFrame, lastFrame = starTime;
	SetStartTime(starTime);

	while (!glfwWindowShouldClose(Window::window_ptr))
	{
		currentFrame = glfwGetTime();
		SetCurTime(currentFrame);
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		SetDeltaTime(deltaTime);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		_input->Update();
		//for each frame 

		Scene::Instace->Update();
		Scene::Instace->LateUpdate();
		Scene::Instace->Render();
		// all drawing goes here ..
		glfwSwapBuffers(window_ptr);
		glfwPollEvents();
	}
}

void Window::SetDeltaTime(float time)
{
	Time::_deltaTime = time;
}

void Window::SetStartTime(float time)
{
	Time::_startTime = time;
}

void Window::SetCurTime(float time)
{
	Time::_curTime = time;
}

void Window::InitScene()
{
	m_scene = Scene::Init();
}

void Window::Log(string str)
{
	cout << str << endl;
}

void Window::KeyDown(Key key)
{
	Log(to_string((int)key));
}

void Window::KeyUp(Key key)
{
	Log(to_string((int)key));
}

void stbi_set_flip_vertically_on_load2(bool b)
{
	stbi_set_flip_vertically_on_load(b);
}
