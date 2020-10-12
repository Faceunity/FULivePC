
#pragma once
#include <memory>
#include "gl3w.h"
#include "glfw3.h"
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include "glfw3native.h"

#include "GuiTool.h"
#include "Texture.h"

#include <stdio.h>
#include <string>
#include "Nama.h"
#include "UIBridge.h"
#include "MouseControl.h"

using namespace std;
using namespace NamaExampleNameSpace;

class Gui
{
public:
    using UniquePtr = unique_ptr<Gui>;
	static UniquePtr create(uint32_t width, uint32_t height);

	~Gui(); 

	void render(Nama::UniquePtr& nama);

	void UpdateFrame(Nama * nama);

	void ShowMainWindow(Nama * nama);
	
	void onWindowResize(uint32_t width, uint32_t height);

	void ProcessGSSampleClick(Nama * nama);
	void ResetPreviewRect(Nama::UniquePtr& nama);
#ifdef _WIN32
	static HWND hWindow;
#else
    static void * hWindow;
#endif
    static GLFWwindow* window;

	static bool mIsOpenMiniWindow;

protected:
private:
	Gui() = default;
	MouseControl* mouseControl;
	cv::Mat  m_processedFrame;
	GLuint m_texIDNamaProcess = -1;
	GLuint m_texIDOrignal = -1;
};
