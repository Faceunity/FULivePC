// FULivePC.cpp : Defines the entry point for the console application.
//by liujia mybbs2200@163.com

#include "Nama.h"
#include "GUI/Gui.h"
#include <windows.h>

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
	if (argc == 2 && _stricmp(argv[1], "-disableNama") == 0)
		enableNama = false;
	
	using namespace NamaExampleNameSpace;
	Gui::UniquePtr gui = Gui::create(1360, 830);

	Nama::UniquePtr nama = Nama::create(640, 480, enableNama);

	gui->render(nama);

    return 0;
}

