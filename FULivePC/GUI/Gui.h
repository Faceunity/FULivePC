
#pragma once
#include <memory>
#include "gl3w.h"
#include "glfw3.h"
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include "glfw3native.h"

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

	void init(int width, int height,bool enableNama);
	void render(Nama* nama);

	void UpdateFrame(Nama * nama);
    
    void tipToGrantCameraAcess(Nama * nama);
	void ShowMainWindow(Nama * nama);
	void ShowTitle();	
	void onWindowResize(uint32_t width, uint32_t height);

	void ProcessGSSampleClick(Nama * nama);
	void ResetPreviewRect(Nama* nama);
#ifdef _WIN32
	static HWND hWindow;
	static HWND hOffscreenWindow;
#else
    static void * hWindow;
	static void* hOffscreenWindow;
#endif
    static GLFWwindow* window;
	static GLFWwindow* offscreen_window;

	static bool mIsOpenMiniWindow;
	static bool mIsEnableHumanFollowMode;

protected:
private:
	Gui() = default;
	cv::Mat mTransparentMat;
	MouseControl* m_mouseControl;
	MouseControl* m_mouseControlSec;
	MouseControl* m_mouseControlWindow;
	cv::Mat  m_processedFrame;
	GLuint m_texIDNamaProcess = -1;
	GLuint m_texIDOrignal = -1;
	int WindowPosX = 280;
    int WindowPosY = 105;
	int gsSelectColorWHTpye = 0;//0长宽各按比例,1图像长放大按宽比例算取色点，2按长比例算取色点
};
