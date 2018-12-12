
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
	//using UniquePtr = std::unique_ptr<Gui>;

	typedef std::unique_ptr<Gui> UniquePtr;

	static UniquePtr create(uint32_t width, uint32_t height);

	~Gui(); 

	void render(Nama::UniquePtr& nama);
	
	void onWindowResize(uint32_t width, uint32_t height);

	static HWND hWindow;
protected:
private:
	Gui(){};
	static GLFWwindow* window;	
	IpcBridge ipcBridge;
	cv::Mat processedFrame;
};

#ifdef WIN32
 
inline std::wstring AnsiToUnicode(const char* buf)
{
    int len = ::MultiByteToWideChar(CP_ACP, 0, buf, -1, NULL, 0);
    if (len == 0) return L"";
 
    std::wstring unicode;
    unicode.resize(len);
    ::MultiByteToWideChar(CP_ACP, 0, buf, -1, &unicode[0], len);
 
    return unicode;
}
 
inline std::string UnicodeToUtf8(const wchar_t* buf)
{
    int len = ::WideCharToMultiByte(CP_UTF8, 0, buf, -1, NULL, 0, NULL, NULL);
    if (len == 0) return "";
 
    std::string utf8;
    utf8.resize(len);
    ::WideCharToMultiByte(CP_UTF8, 0, buf, -1, &utf8[0], len, NULL, NULL);
 
    return utf8;
}
 
inline
std::string AnsioUTF8(const char *strChar)
{
    return UnicodeToUtf8(AnsiToUnicode(strChar).c_str());
}
//Windows中做编码转换,
#define UTEXT(str) AnsioUTF8(str)
#else
//Android/iOS中不处理.
#define UTEXT(str) str
#endif