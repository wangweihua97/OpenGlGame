#ifndef Window_H
#define Window_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;
class Scene;
class Time;
class Input;
class Shader;
enum class Key;
void Resize(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void stbi_set_flip_vertically_on_load2(bool b);
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
	void InitGbuffer();
	int Width, Height;
	void SetDeltaTime(float time);
	void SetStartTime(float time);
	void SetCurTime(float time);
	void InitScene();
	void renderQuad();
	void Log(string str);
	void InitSkyBox();
	void RenderCube();
	void RenderSkyBox();
	void AmbientGBuffer();
	void SpecularGBuffer();

	unsigned int envCubemap;
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;

	unsigned int captureFBO;
	unsigned int captureRBO;

	unsigned int gBuffer;
	unsigned int g_positionMap;
	unsigned int g_albedoMap;
	unsigned int g_specularMap;
	unsigned int g_normalMap;
	unsigned int g_height_ao_metallic_roughnessMap;
	unsigned int rboDepth;

	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	GLFWwindow* window_ptr;
private:
	void KeyDown(Key key);
	void KeyUp(Key key);
	Scene* m_scene;
	Input* _input;
	Shader* _gBufferShader;
	Shader* _specularGBufferShader;

	void RenderShadowVolIntoStencil();
};
#endif

