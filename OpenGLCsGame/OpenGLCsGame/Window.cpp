#include "Window.h"
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
	while (!glfwWindowShouldClose(Window::window_ptr))
	{
		//for each frame 
		Input();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// all drawing goes here ..


		glfwSwapBuffers(window_ptr);
		glfwPollEvents();
	}
}