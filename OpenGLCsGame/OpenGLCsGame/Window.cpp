#include "Window.h"
#include "Scene.h"
#include "Time.h"
#include "Input.h"
#include "EventCenter.h"
#include "ResourceManager.h"
#include <boost/bind/bind.hpp>
#include "Shader.h"
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
	ResourceManager::LoadShader("Shader/g_skeleton_model.vs", "Shader/g_skeleton_model.fs", nullptr, "SkeletonModelShader");
	ResourceManager::LoadShader("Shader/gBuffer.vs", "Shader/gBuffer.fs", nullptr, "GBufferShader");
	_gBufferShader = ResourceManager::GetShaderP("GBufferShader");
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

	InitGbuffer();
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
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_input->Update();
		//for each frame 

		Scene::Instace->Update();
		Scene::Instace->LateUpdate();
		Scene::Instace->Render();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
		// -----------------------------------------------------------------------------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_gBufferShader->Use();
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(_gBufferShader->ID, "gPosition"), 0);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, g_positionMap);
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(_gBufferShader->ID, "gAlbedoMap"), 1);
		glBindTexture(GL_TEXTURE_2D, g_albedoMap);
		glActiveTexture(GL_TEXTURE2);
		glUniform1i(glGetUniformLocation(_gBufferShader->ID, "gSpecularMap"), 2);
		glBindTexture(GL_TEXTURE_2D, g_specularMap);
		glActiveTexture(GL_TEXTURE3);
		glUniform1i(glGetUniformLocation(_gBufferShader->ID, "gNormalMap"), 3);
		glBindTexture(GL_TEXTURE_2D, g_normalMap);
		glActiveTexture(GL_TEXTURE4);
		glUniform1i(glGetUniformLocation(_gBufferShader->ID, "gHeightAoMetallicRoughnessMap"), 4);
		glBindTexture(GL_TEXTURE_2D, g_height_ao_metallic_roughnessMap);
		renderQuad();
		// all drawing goes here ..
		glfwSwapBuffers(window_ptr);
		glfwPollEvents();
	}
}

void Window::InitGbuffer()
{
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//Î»ÖÃ»º³å
	glGenTextures(1, &g_positionMap);
	glBindTexture(GL_TEXTURE_2D, g_positionMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_positionMap, 0);

	//ÑÕÉ«»º³å
	glGenTextures(1, &g_albedoMap);
	glBindTexture(GL_TEXTURE_2D, g_albedoMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_albedoMap, 0);

	//¸ß¹âÑÕÉ«»º³å
	glGenTextures(1, &g_specularMap);
	glBindTexture(GL_TEXTURE_2D, g_specularMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_specularMap, 0);

	//·¨Ïß»º³å
	glGenTextures(1, &g_normalMap);
	glBindTexture(GL_TEXTURE_2D, g_normalMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, g_normalMap, 0);

	//¸ß¶È»º³å,AO»º³å ,¹âÔó¶È»º³å,´Ö²Ú¶È»º³å
	glGenTextures(1, &g_height_ao_metallic_roughnessMap);
	glBindTexture(GL_TEXTURE_2D, g_height_ao_metallic_roughnessMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, g_height_ao_metallic_roughnessMap, 0);


	unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4,};
	glDrawBuffers(5, attachments);

	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void Window::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
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
