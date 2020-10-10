#ifndef _GUI_GS_H_
#define _GUI_GS_H_

#include "GuiTool.h"
#include "Nama.h"
#include "Texture.h"
#include "MouseControl.h"
using namespace std;
class CCameraDS;

class GUIGS
{
public:
	GUIGS();
	~GUIGS();

	static void ShowCameraChoiceForGS(NamaExampleNameSpace::Nama * nama);

	static void ShowGSInputChoice(NamaExampleNameSpace::Nama * nama,bool canCancel = false);

	static void ShowGreenScreenMenu(NamaExampleNameSpace::Nama * nama);

	static void CloseCamera(NamaExampleNameSpace::Nama * nama);


	static bool SetCameraFileInput(NamaExampleNameSpace::Nama * nama, const char * path);


	static void ShowReChooseInputSrc();
	static void inputGreenScreenBg(string itemName);
	static void ShowFloatMenuGS(NamaExampleNameSpace::Nama * nama);


	static void ShowRightPanel(NamaExampleNameSpace::Nama * nama);


	static void ShowGSTab(NamaExampleNameSpace::Nama * nama);

	static void ShowGSParamRight(NamaExampleNameSpace::Nama * nama);

	static void ResetDefParam();

	static void ChangeGreenScreenBg(std::string strFilePath);


	static void UpdateGreenScreenBg(NamaExampleNameSpace::Nama * nama);


	static void InitColorVec();

	static void DrawColorPanel(NamaExampleNameSpace::Nama * nama);


	static void SetCurColorCircle(cv::Vec4b color);

	static cv::Vec4b GetCurColorCircle();

	static void ShowColorChoiceWin(NamaExampleNameSpace::Nama * nama);

	static FURect previewRect;
	static void calculatePreViewForHalfScreen(float orgW,float orgH,float* dstW,float* dstH);
	
private:
	static CCameraDS * m_pCamBG;
	static bool m_bHasSetedBgForOnceChange;

	static std::vector<gui_tool::ColorBag> m_vecColorTex;
	static std::vector<gui_tool::ColorBag> m_vecColorRecentTex;
	static gui_tool::ColorBag m_curColor;
	static Texture::SharedPtr m_hsvTexture;
	static cv::Mat m_hsvMat;
};

#endif
