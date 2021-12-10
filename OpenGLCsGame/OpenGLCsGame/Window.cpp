#include "Window.h"
#include "Global.h"
#include "Time.h"
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
	window_ptr = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
	if (window_ptr == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window_ptr);
	glfwSetFramebufferSizeCallback(window_ptr, Resize);

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

void Window::Input()
{
	if (glfwGetKey(window_ptr, GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window_ptr, true);
	}
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

void Window::Mainloop()
{
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

		//for each frame 
		Input();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

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

void Window::InitGlobal()
{
	m_global = Global::Init();
}
