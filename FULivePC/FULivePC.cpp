// FULivePC.cpp : Defines the entry point for the console application.
//by liujia mybbs2200@163.com

#include "Nama.h"
#include "GUI/Gui.h"
#include <windows.h>
#define _CRT_SECURE_NO_WARNINGS
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main()
{	
	using namespace NamaExampleNameSpace;
	Gui::UniquePtr gui = Gui::create(1360, 830);

	Nama::UniquePtr nama = Nama::create(640, 480);

	gui->render(nama);

    return 0;
}

