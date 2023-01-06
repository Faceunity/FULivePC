// FULivePC.cpp : Defines the entry point for the console application.
//by liujia mybbs2200@163.com

#include "Nama.h"
#include "GUI/Gui.h"
#include <string.h>


#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include "fu_tool_mac.h"
#endif

extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}


int main(int argc, char* argv[])
{	
	bool enableNama = true;
#ifdef _WIN32
	if (argc == 2 && _stricmp(argv[1], "-disableNama") == 0)
		enableNama = false;
#elif __APPLE__
    if (argc == 2 && strcasecmp(argv[1], "-disableNama") == 0)
        enableNama = false;

    // 这里检测macosx的相机权限有没有打开
    bool isgrant = FuToolMac::granteCameraAccess();
    if(!isgrant)
    {
      //  FuToolMac::tipToOpenPrivacyPanel();
   //     return 0;
    }
    printf("isgrant---::%d\n",isgrant);
#endif
	
	using namespace NamaExampleNameSpace;

	Gui::UniquePtr gui = Gui::create(1360, 870);

	if (nullptr == gui)
	{
		return -1;
	}

	gui->init(1280, 720, enableNama);

	gui->render(Nama::get());

    return 0;
}
