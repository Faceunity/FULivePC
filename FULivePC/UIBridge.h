//#pragma once
#ifndef UIBRIDGE_H
#define UIBRIDGE_H

#include <string>
#include <vector>
#include "imgui/imgui.h"

#define MAX_PATH_LENGTH 1024  
#define MAX_BEAUTYFACEPARAMTER 7
#define MAX_FACESHAPEPARAMTER 15

enum BundleCategory 
{
	Animoji,
	ItemSticker,
	ARMask,
	ChangeFace,
	ExpressionRecognition,
	MusicFilter,
	BackgroundSegmentation,
	GestureRecognition,
	MagicMirror,
	PortraitDrive,
	Makeup ,
	Hair,
	ChangeFaceEx,
	ExpressionGif,
	Facebeauty,
	LightMakeup,
	Facepup,

	Count
};

class UIBridge
{
public:

	static int bundleCategory;
	static int renderBundleCategory;
	static int faceType; 
	
	static bool showItemSelectWindow;
	static bool showItemTipsWindow;
	static bool showDegubInfoWindow;
	static bool showFilterSlider; 
	static int showMakeUpWindow;
	static bool mNeedIpcWrite; 
	static bool mNeedPlayMP3;
	static bool mNeedStopMP3;

	static uint32_t mFPS;
	static uint32_t mResolutionWidth;
	static uint32_t mResolutionHeight;
	static uint32_t mRenderTime;

	static int m_curFilterIdx;
	static int m_curRenderItem;	
	static int m_curBindedItem; ;
	static ImGuiID m_curRenderItemUIID;
	
	static int mEnableSkinDect;
	static int mEnableHeayBlur;
	static int mEnableExBlur;
	static float mFaceBeautyLevel[MAX_BEAUTYFACEPARAMTER];
	static float mFaceShapeLevel[MAX_FACESHAPEPARAMTER];
	static float mFilterLevel[10];
	static float mMakeupLevel[10];
		
	static int mSelectedCamera;
	static double mLastTime;
	static std::string mCurRenderItemName;
	static std::vector<std::string> categoryBundles[BundleCategory::Count];

    static void FindAllBundle(std::string folder,std::vector<std::string> &files);
    
protected:
private:
};

const std::string g_faceBeautyParamName[MAX_BEAUTYFACEPARAMTER] = { "blur_level","color_level", "red_level", "eye_bright", "tooth_whiten" ,"remove_pouch_strength", "remove_nasolabial_folds_strength" };

const std::string g_faceShapeParamName[MAX_FACESHAPEPARAMTER] = { "cheek_thinning","eye_enlarging", "intensity_chin", "intensity_forehead", "intensity_nose","intensity_mouth",
		"cheek_v","cheek_narrow","cheek_small", 
	"intensity_canthus", "intensity_eye_space", "intensity_eye_rotate", "intensity_long_nose",
	"intensity_philtrum", "intensity_smile" };

//区分是否从中间值开始突变的Flag 0:变化从0->1 1:0->0.5 0.5->1

#define FACE_SHAPE_SHOW_FLAG_NORMAL (0)
#define FACE_SHAPE_SHOW_FLAG_MIDDLE (1)
const int g_faceShapeParamShowFlag[MAX_FACESHAPEPARAMTER] = { 0,0,1,1,0,
1,0,0,0,0,
1,1,1,1,0 };

#ifdef __APPLE__
const std::string g_assetDir = "";
#else
const std::string g_assetDir = "assets/";
#endif
const std::string gBundlePath[] = {
	g_assetDir + "items/" + "Animoji",
	g_assetDir + "items/" + "ItemSticker",
	g_assetDir + "items/" + "ARMask",
	g_assetDir + "items/" + "ChangeFace",
	g_assetDir + "items/" + "ExpressionRecognition",
	g_assetDir + "items/" + "MusicFilter",
	g_assetDir + "items/" + "BackgroundSegmentation",
	g_assetDir + "items/" + "GestureRecognition",
	g_assetDir + "items/" + "MagicMirror",
	g_assetDir + "items/" + "PortraitDrive",
	g_assetDir + "items/" + "Makeup",
};
#endif
