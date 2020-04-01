// FULivePC.cpp : Defines the entry point for the console application.
//by liujia mybbs2200@163.com

#include "Nama.h"
#include "GUI/Gui.h"
#include <string.h>

#ifdef _WIN32
#include <windows.h>
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
#else
    if (argc == 2 && strcasecmp(argv[1], "-disableNama") == 0)
        enableNama = false;
#endif
	
	using namespace NamaExampleNameSpace;
	Gui::UniquePtr gui = Gui::create(1360, 830);

	Nama::UniquePtr nama = Nama::create(1280, 720, enableNama);

	gui->render(nama);

    return 0;
}

