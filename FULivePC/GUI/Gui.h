
#pragma once
#include <memory>
#include "gl3w.h"
#include "glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "glfw3native.h"

#include "imgui/imgui.h"
#include "imgui/imgui_tabs.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Texture.h"

#include <stdio.h>
#include <dinput.h>
#include <string>
#include "Nama.h"
#include "UIBridge.h"
//for virtual camera
#include "ipc/filtercommons.h"
#include "ipc/ipcbridge.h"
#include <uuids.h>

using namespace NamaExampleNameSpace;
class Gui
{
public:
	using UniquePtr = std::unique_ptr<Gui>;

	static UniquePtr create(uint32_t width, uint32_t height);

	~Gui(); 

	void render(Nama::UniquePtr& nama);
	
	void onWindowResize(uint32_t width, uint32_t height);

	static HWND hWindow;
protected:
private:
	Gui() = default;
	static GLFWwindow* window;	
	IpcBridge ipcBridge;
	std::tr1::shared_ptr<unsigned char> processedFrame;
	//cv::Mat processedFrame;
};