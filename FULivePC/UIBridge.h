//#pragma once
#ifndef UIBRIDGE_H
#define UIBRIDGE_H

#include <string>
#include <vector>
#include "imgui/imgui.h"

#define MAX_PATH_LENGTH 1024  
#define MAX_BEAUTYFACEPARAMTER 8
#define MAX_FACESHAPEPARAMTER 18
#define MAX_BODY_SHAPE_PARAM  7
#define MAX_GREEN_SCREEN_PARAM 3

using namespace std;
enum BundleCategory 
{
	Animoji,
	ItemSticker,
	ARMask,
	ExpressionRecognition,
	MusicFilter,
	BackgroundSegmentation,
	GestureRecognition,
	MagicMirror,
	Makeup ,
	BeautyHair,
	BigHead,
	GreenScreen,

	Count
};

enum eGS_INPUT_TYPE
{
	GS_INPUT_TYPE_NONE,
	GS_INPUT_TYPE_CAMERA,
	GS_INPUT_TYPE_FILE
};

typedef struct tagGreenScreenState
{
	eGS_INPUT_TYPE inputType;
	string strFilePath;
	int iCmaeraID;

	tagGreenScreenState()
	{
		inputType = GS_INPUT_TYPE_NONE;
		strFilePath = "";
		iCmaeraID = -1;
	}

}GreenScreenState;

class UIBridge
{
public:

	static int bundleCategory;
	// 用于绿幕
	static int gsBundleCategory;
	static int renderBundleCategory;
	static int faceType; 
	
	static bool showItemSelectWindow;
	static bool showItemTipsWindow;
	static bool showDegubInfoWindow;
	static bool showFilterSlider; 
	static int showGreenScreen;
	static bool showCustomMakeup;

	static bool mNeedIpcWrite; 
	static bool mNeedPlayMP3;
	static bool mNeedStopMP3;

	static uint32_t mFPS;
	static uint32_t mResolutionWidth;
	static uint32_t mResolutionHeight;
	static uint32_t mRenderTime;

	static int m_curFilterIdx;
	static int m_curRenderItem;	
	static int m_curBindedItem;
	static int m_curPlayingMusicItem;
	static ImGuiID m_curRenderItemUIID;
	static ImGuiID m_curRenderItemUIIDSec;

	static GreenScreenState m_gsState;

	static bool m_bSetGSInputSrc;

	static bool m_bNeedShowGSCamChoice;
	static uint32_t m_id4ChoiceCam;
	static bool m_bNeedReChooseInputSrc;
	static bool m_bSamplingColor;
	static bool m_bShowColorChoice;
	// 是否正在显示美体功能
	static bool m_bShowingBodyBeauty;
	// 加載含有中文字符的png 文件，需要給用戶提示
	static bool m_bLoadWrongNamePNGFile;
	// 获取本地视频的角度
	static int m_localVideoRotation;
	// 用于区别是图片类型还是视频类型
	static bool m_localVideoType;
	// 记录本地视频的分辨率
	static float m_localVideoWidth;
	static float m_localVideoHeight;
	static bool m_bResetPreviewRect;
	static string m_openLocalFileTip;
	
	static int mEnableSkinDect;
	static int mEnableHeayBlur;
	static int mEnableExBlur;
	static float mFaceBeautyLevel[MAX_BEAUTYFACEPARAMTER];
	static float mFaceShapeLevel[MAX_FACESHAPEPARAMTER];
	static float mFilterLevel[10];
	static float mMakeupLevel[10];
	static float mBodyShapeLevel[MAX_BODY_SHAPE_PARAM];
	static float mGSParam[MAX_GREEN_SCREEN_PARAM];
		
	static int mSelectedCamera;
	static double mLastTime;
	static string mCurRenderItemName;
	// 专用于绿幕
	static string mCurRenderGSItemName;
	static vector<string> categoryBundles[BundleCategory::Count];

    static void FindAllBundle(string folder,vector<string> &files);

	static void FindAllCommonPIC(string folder, vector<string> &files);
	
	static void FindAllCommonPICFromResourceBundle(string folder, vector<string> &files);
    
	static void FindAllCommonPICNameFromResourceBundle(string folder, vector<string> &names);
 
	static string GetFileFullPathFromResourceBundle(const char * path);
protected:
private:
};

const string g_faceBeautyParamName[MAX_BEAUTYFACEPARAMTER] = { "blur_level","color_level", "red_level","sharpen","eye_bright", "tooth_whiten" ,"remove_pouch_strength", "remove_nasolabial_folds_strength" };

const string g_faceShapeParamName[MAX_FACESHAPEPARAMTER] = { "cheek_thinning","eye_enlarging","intensity_eye_circle", "intensity_chin", "intensity_forehead", "intensity_nose","intensity_mouth",
		"cheek_v","cheek_narrow","cheek_small","intensity_cheekbones","intensity_lower_jaw",
	"intensity_canthus", "intensity_eye_space", "intensity_eye_rotate", "intensity_long_nose",
	"intensity_philtrum", "intensity_smile" };

//区分是否从中间值开始突变的Flag 0:变化从0->1 1:0->0.5 0.5->1
#define FACE_SHAPE_SHOW_FLAG_NORMAL (0)
#define FACE_SHAPE_SHOW_FLAG_MIDDLE (1)

const int g_faceShapeParamShowFlag[MAX_FACESHAPEPARAMTER] = { 0,0,0,1,1,0,
1,0,0,0,0,0,0,
1,1,1,1,0 };

/////////////////////////////////////////////////////////////

#define BODY_SHAPE_SHOW_FLAG_NORMAL (0)
#define BODY_SHAPE_SHOW_FLAG_MIDDLE (1)

const string g_bodyShapeParamName[MAX_BODY_SHAPE_PARAM] = {"BodySlimStrength","LegSlimStrength" ,"WaistSlimStrength" , "ShoulderSlimStrength" ,
"HipSlimStrength" , "HeadSlim", "LegSlim"};

const int g_bodyShapeParamShowFlag[MAX_BODY_SHAPE_PARAM] = { 0,0,0,1,0,0,0 };

/////////////////////////////////////////////////////////////


const string g_GSParamName[MAX_GREEN_SCREEN_PARAM] = {"chroma_thres","chroma_thres_T",
"alpha_L"};


/////////////////////////////////////////////////////////////


extern const string g_assetDir;
const string gBundlePath[] = {
	g_assetDir + "items/" + "Animoji",
	g_assetDir + "items/" + "ItemSticker",
	g_assetDir + "items/" + "ARMask",
	g_assetDir + "items/" + "ExpressionRecognition",
	g_assetDir + "items/" + "MusicFilter",
	g_assetDir + "items/" + "BackgroundSegmentation",
	g_assetDir + "items/" + "GestureRecognition",
	g_assetDir + "items/" + "MagicMirror",
	g_assetDir + "items/" + "Makeup",
	g_assetDir + "items/" + "BeautyHair",
	g_assetDir + "items/" + "BigHead",
	g_assetDir + "items/" + "GreenScreen"
};

const string gGSBgPic = g_assetDir + "items/" + "GreenScreenBg";


const string gGSColorConfig = g_assetDir + "colorConfig.json";

const string gCustomCMConfig = g_assetDir + "items/Makeup/subs_setup.json";


#endif
