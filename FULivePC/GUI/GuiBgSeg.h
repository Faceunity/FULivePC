#ifndef _GUI_BGSEG_H_
#define _GUI_BGSEG_H_

#include <vector>
#include <string>
#include "Nama.h"

using namespace std;

typedef struct tagBgSegUserConfig
{
	string strFilePath;

	tagBgSegUserConfig() {
		strFilePath = "";
	}

}BgSegUserConfig;

class GUIBgSeg
{
public:
	GUIBgSeg();
	~GUIBgSeg();

	static bool LoadUserConfig();

	static void SaveUserConfig();

	static void LoadResource();

	static void ShowBgSegPannel(NamaExampleNameSpace::Nama * nama);

	static void SelectBgSegFile();

	static void ConfigVideoInfo(string videoPath);

private:
	static vector<string> mVecBundleName;
	static BgSegUserConfig mConfig;
	static bool mIsUserConfigOK;
	static bool mIsSelectBgSegFileOK;
};

#endif