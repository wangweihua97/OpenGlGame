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
	ResourceManager::LoadShader("Shader/ambientGBuffer.vs", "Shader/ambientGBuffer.fs", nullptr, "AmbientGBuffer");
	ResourceManager::LoadShader("Shader/specularGBuffer.vs", "Shader/specularGBuffer.fs", nullptr, "SpecularGBuffer");
	ResourceManager::LoadShader("Shader/readSkyBox.vs", "Shader/readSkyBox.fs", nullptr, "ReadSkyBox");
	ResourceManager::LoadShader("Shader/skyBox.vs", "Shader/skyBox.fs", nullptr, "SkyBox");
	ResourceManager::LoadShader("Shader/animationVolume.vs", "Shader/volume.fs", "Shader/volume.gs", "Volume");
	_gBufferShader = ResourceManager::GetShaderP("AmbientGBuffer");
	_specularGBufferShader = ResourceManager::GetShaderP("SpecularGBuffer");
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
	InitGbuffer();
	InitResource();
	InitEvent();
	InitInput();
	InitSkyBox();
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
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		_input->Update();
		//for each frame 

		Scene::Instace->Update();
		Scene::Instace->LateUpdate();
		Scene::Instace->Render();

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//RenderShadowVolIntoStencil();
		//SpecularGBuffer();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
		glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
		// -----------------------------------------------------------------------------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT);
		glDepthMask(GL_FALSE);
		//glDrawBuffer(GL_BACK);
		AmbientGBuffer();
		glEnable(GL_STENCIL_TEST);
		//glEnable(GL_DEPTH_CLAMP);
		//glEnable(GL_DEPTH_TEST);
		//glDepthMask(GL_TRUE);
		RenderShadowVolIntoStencil();

		glDrawBuffer(GL_BACK);
		//glDisable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_CLAMP);
		// prevent update to the stencil buffer
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 0x0, 0xFF);
		glDepthMask(GL_FALSE);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		//glDrawBuffer(GL_BACK);
		SpecularGBuffer();
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_BLEND);


		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glDepthFunc(GL_LEQUAL);
		RenderSkyBox();
		glDepthFunc(GL_LESS);
		
		glClear(GL_DEPTH_BUFFER_BIT);

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

void Window::InitSkyBox()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	string skyboxPath = "Resources/hdr/parched_canal_4k.hdr";
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	// pbr: load the HDR environment map
	// ---------------------------------
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf(skyboxPath.c_str(), &width, &height, &nrComponents, 0);
	unsigned int hdrTexture;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}

	// pbr: setup cubemap to render to and attach to framebuffer
	// ---------------------------------------------------------
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	ResourceManager::GetShaderP("ReadSkyBox")->Use();
	ResourceManager::GetShaderP("ReadSkyBox")->SetInteger("equirectangularMap", 0);
	ResourceManager::GetShaderP("ReadSkyBox")->SetMatrix4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	//glUniform1i(glGetUniformLocation(ResourceManager::GetShaderP("ReadSkyBox")->ID, "equirectangularMap"), 0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		ResourceManager::GetShaderP("ReadSkyBox")->SetMatrix4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Width, Height);
	//glDeleteTextures(1, &hdrTexture);
}

void Window::RenderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Window::RenderSkyBox()
{
	ResourceManager::GetShaderP("SkyBox")->Use();
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(ResourceManager::GetShaderP("SkyBox")->ID, "environmentMap"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
	RenderCube();
}

void Window::AmbientGBuffer()
{
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
}

void Window::SpecularGBuffer()
{
	_specularGBufferShader->Use();
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(_specularGBufferShader->ID, "gPosition"), 0);
	// and finally bind the texture
	glBindTexture(GL_TEXTURE_2D, g_positionMap);
	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(_specularGBufferShader->ID, "gAlbedoMap"), 1);
	glBindTexture(GL_TEXTURE_2D, g_albedoMap);
	glActiveTexture(GL_TEXTURE2);
	glUniform1i(glGetUniformLocation(_specularGBufferShader->ID, "gSpecularMap"), 2);
	glBindTexture(GL_TEXTURE_2D, g_specularMap);
	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(_specularGBufferShader->ID, "gNormalMap"), 3);
	glBindTexture(GL_TEXTURE_2D, g_normalMap);
	glActiveTexture(GL_TEXTURE4);
	glUniform1i(glGetUniformLocation(_specularGBufferShader->ID, "gHeightAoMetallicRoughnessMap"), 4);
	glBindTexture(GL_TEXTURE_2D, g_height_ao_metallic_roughnessMap);
	renderQuad();
}

void Window::KeyDown(Key key)
{
	Log(to_string((int)key));
}

void Window::KeyUp(Key key)
{
	Log(to_string((int)key));
}

void Window::RenderShadowVolIntoStencil()
{
	glDrawBuffer(GL_NONE);
	glDepthMask(GL_FALSE);

	glDisable(GL_CULL_FACE);

	// We need the stencil test to be enabled but we want it
	// to succeed always. Only the depth test matters.
	glStencilFunc(GL_ALWAYS, 0, 0xff);

	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	Scene::Instace->RenderShadow();

	glEnable(GL_CULL_FACE);
}

void stbi_set_flip_vertically_on_load2(bool b)
{
	stbi_set_flip_vertically_on_load(b);
}
