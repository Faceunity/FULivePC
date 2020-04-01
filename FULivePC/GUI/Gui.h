
#pragma once
#include <memory>
#include "gl3w.h"
#include "glfw3.h"
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include "glfw3native.h"

#include "imgui/imgui.h"
#include "imgui/imgui_tabs.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Texture.h"

#include <stdio.h>
#include <string>
#include "Nama.h"
#include "UIBridge.h"

//for virtual camera
#ifdef _WIN32
#include "ipc/filtercommons.h"
#include "ipc/ipcbridge.h"
#include <uuids.h>
#include <dinput.h>
#endif

using namespace NamaExampleNameSpace;

class Gui
{
public:
    using UniquePtr = std::unique_ptr<Gui>;
	static UniquePtr create(uint32_t width, uint32_t height);

	~Gui(); 

	void render(Nama::UniquePtr& nama);
	
	void onWindowResize(uint32_t width, uint32_t height);

#ifdef _WIN32
	static HWND hWindow;
#else
    static void * hWindow;
#endif
    static GLFWwindow* window;
protected:
private:
	Gui() = default;
	
#ifdef _WIN32
	IpcBridge ipcBridge;
#endif
	cv::Mat processedFrame;
};
