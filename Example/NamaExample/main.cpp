#include "Nama.h"

#include <iostream>
#include <memory>

//opengl
#include <gl\glew.h>
#include <GLFW\glfw3.h>

#ifndef GL_LIB
#ifdef _DEBUG
#define GL_LIB(name) name "d.lib"
#else
#define GL_LIB(name) name ".lib"
#endif
#endif

#pragma comment(lib, GL_LIB("glfw3"))
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

namespace NE
{
	GLFWwindow* window = NULL;
	const int wndWidth = 1280;
	const int wndHeight = 720;
	GLuint texID=0;
	std::tr1::shared_ptr<Nama> nama;

	bool InitWindow();

	void SetOpenGLState();
	void GenTexture();

	void SetTextureData(std::tr1::shared_ptr<unsigned char> frame);
	void ShowFrame();

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
}



int main()
{
	using namespace NE;

	if (false == InitWindow())
	{
		return 1;
	}

	nama = std::tr1::shared_ptr<Nama>(new Nama);
	nama->Init(wndWidth, wndHeight);

	SetOpenGLState();
	GenTexture();

	while (false == glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		std::tr1::shared_ptr<unsigned char> frame = nama->NextFrame();

		SetTextureData(frame);

		ShowFrame();

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

bool NE::InitWindow()
{
	if (false == glfwInit())
	{
		std::cout << "ERROR: could not start GLFW3" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(wndWidth, wndHeight, "Nama Example", NULL, NULL);
	if (!window) {
		std::cout << "ERROR: could not open window with GLFW3" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glfwSetKeyCallback(window, key_callback);

	glfwSwapInterval(0);

	return true;
}

void NE::SetOpenGLState()
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, wndWidth, 0, wndHeight, 0, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void NE::GenTexture()
{
	glGenTextures(0, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void NE::SetTextureData(std::tr1::shared_ptr<unsigned char> frame)
{
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wndWidth, wndHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, frame.get());
}

void NE::ShowFrame()
{
	glBindTexture(GL_TEXTURE_2D, texID);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, wndHeight, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(wndWidth, wndHeight, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(wndWidth, 0.0f, 0.0f);
	glEnd();
}

void NE::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	// ÉèÖÃ°´¼ü
	if (GLFW_PRESS == action)
	{
		switch (key)
		{
		case GLFW_KEY_UP:
			nama->PreBundle();
			break;
		case GLFW_KEY_DOWN:
			nama->NextBundle();
			break;
		case GLFW_KEY_TAB:
			nama->SwitchRenderMode();
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;
		default:
			break;
		}
	}
}
