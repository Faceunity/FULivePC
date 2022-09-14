//#pragma once
#ifndef UIBRIDGE_H
#define UIBRIDGE_H

#include <string>
#include <vector>
#include "imgui/imgui.h"

#define MAX_PATH_LENGTH 1024  
#define MAX_BEAUTYFACEPARAMTER 8
#define MAX_FACESHAPEPARAMTER 21
#define MAX_BODY_SHAPE_PARAM  7
#define MAX_GREEN_SCREEN_PARAM 3


#define MAKEUP_CUSTOM_NAME ("demo_icon_customize.bundle")
#define LIGHT_MAKEUP_NAME ("light_makeup.bundle")
#define BGSEG_CUSTOM_NAME ("others/bg_segment.bundle")
#define BGSEG_USERFILE_PIC ("bg_seg_shot.png")
#define GSSAFEAREA_USERFILE_PIC ("gs_safearea_shot.png")
#define BGSEG_ADD_ICON ("portrait_segmentation_icon_add.png")

using namespace std;

enum CERTIFACITE_TYPE
{
	DEFINE_Face_Beautify,
	DEFINE_2D_Sticker,
	DEFINE_3D_Sticker,
	DEFINE_Skeletal_animation,
	DEFINE_Avatar,
	DEFINE_AR_Mask,
	DEFINE_AR_Mask_HD,
	DEFINE_Background_Segmentation,
	DEFINE_Gesture_Recognition,
	DEFINE_Video_Filter,
	DEFINE_Expression_Recognition,
	DEFINE_P2A_Prime,
	DEFINE_P2A_Creative,
	DEFINE_Portrait_Relighting,
	DEFINE_Facewarp,
	DEFINE_Music_Filter,
	DEFINE_Emotion_Recognition,
	DEFINE_Make_Up,
	DEFINE_Hair_Color,
	DEFINE_Cartoon_Filter,
	DEFINE_Face_Fusion,
	DEFINE_Voice_Lite,
	DEFINE_Voice,
	DEFINE_P2A_Gen_Head,
	DEFINE_P2A_Gen_Hair,
	DEFINE_P2A_Knead_Face,
	DEFINE_P2A_Get_Faceinfo,
	DEFINE_SIZE
};
static const int define_arr[DEFINE_SIZE] = { 0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x100,0x200,
0x400,0x800,0x1000,0x2000,0x4000,0x10000,0x20000,0x40000,0x80000,
0x100000,0x200000,0x800000,0x2000000,0x4000000,0x8000000,0x10000000,0x20000000,0x40000000 };

enum CERTIFACITE_TYPE_EXT
{
	DEFINE_P2A_IMAGE,
	DEFINE_P2A_VIDEO,
	DEFINE_P2A_GIF,
	DEFINE_LIGHT_MAKEUP,
	DEFINE_NAMA_KNEAD_FACE,
	DEFINE_BODY_SHAPE,
	DEFINE_RENDER,
	DEFINE_BODY_TRACK,
	DEFINE_BODY_TRACK_GESTURE,
	DEFINE_GREEN_SCREEN_EDIT,
	DEFINE_PUSH_STREAM,
	DEFINE_GET_FACEINFO,
	DEFINE_FACE_LANDMARK,
	DEFINE_EXPRESSION,
	DEFINE_2D_BODY_LANDMARKS,
	DEFINE_HEAD_SEGMENTATION,
	DEFINE_MOTION_RECOGNITION,
	DEFINE_BOUTIQUE_STICKER,
	DEFINE_ITEM_OFFLINE_SIGN,
	DEFINE_PK_GAME,
	DEFINE_SIZE_EXT
};
static const int define_arr_ext[DEFINE_SIZE_EXT] = { 0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80,0x100,0x200,0x400,0x800,0x1000,0x2000,0x4000,0x8000,
0x10000,0x20000,0x40000,0x80000};


enum BundleCategory 
{
	Avatar,
	Animoji,
	ItemSticker,
	ItemJingpin,
	ARMask,
	ExpressionRecognition,
	MusicFilter,
	BackgroundSegmentation,
	GestureRecognition,
	MagicMirror,
	Makeup,
	LightMakeup,
	BeautyHair,
	BigHead,
	GreenScreen,
	SafeArea,
	Count
};

static const int g_checkIndex[Count] = {
	1,0,0,1,0,0,0,0,0,0,0,1,0,1,1
};

static const int g_checkID[Count] = 
{ 
	define_arr_ext[DEFINE_BODY_TRACK], define_arr[DEFINE_Avatar],define_arr[DEFINE_2D_Sticker],define_arr_ext[DEFINE_BOUTIQUE_STICKER],
	define_arr[DEFINE_AR_Mask],  define_arr[DEFINE_Expression_Recognition], define_arr[DEFINE_Music_Filter],
	define_arr[DEFINE_Background_Segmentation], define_arr[DEFINE_Gesture_Recognition], define_arr[DEFINE_Facewarp],
	define_arr[DEFINE_Make_Up], define_arr_ext[DEFINE_LIGHT_MAKEUP], define_arr[DEFINE_Hair_Color], define_arr_ext[DEFINE_HEAD_SEGMENTATION],
	define_arr_ext[DEFINE_GREEN_SCREEN_EDIT]
};

enum SideCategory {

	BeautifyFaceSkin,
	BeautifyFaceShape,
	BeautifyFilter,
	BeautifyBody,

	SideCount
};

static const int g_checkSideIndex[SideCount] = {
	0,0,0,1
};

static const int g_checkSideID[Count] =
{
	define_arr[DEFINE_Face_Beautify], define_arr[DEFINE_Face_Beautify],define_arr[DEFINE_Face_Beautify],
	define_arr_ext[DEFINE_BODY_SHAPE]
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

struct LightMakeupParam
{
	std::string blusherPath;
	float blusher;
	std::string eyeshadowPath;
	float eyeshadow;
	std::string eyebrowPath;
	float eyebrow;
	std::string lipColorPath;
	float intensity;
	std::string filterName;
	float filterLevel;
};

class UIBridge
{
public:

	static int bundleCategory;
	static int bundleCategoryLast;
	// 用于绿幕
	static int gsBundleCategory;
	static int renderBundleCategory;
	static int faceType; 
	
	static bool showItemSelectWindow;
	static bool showItemTipsWindow;
	static bool showDegubInfoWindow;
	static bool showFilterSlider; 
	static int  showGreenScreen;
	static bool showCustomMakeup;
	static bool newMakeupType;
	static bool showLightMakeupTip;
	static LightMakeupParam m_lightMakeupParam;
	static bool mNeedIpcWrite; 
	static bool mNeedPlayMP3;
	static bool mNeedStopMP3;

	static volatile uint32_t mFPS;
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
	static bool m_bShowingSafeAreaTip;
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

	//背景分割用
	// 获取本地视频的角度
	static int m_localBgSegVideoRotation;
	// 用于区别是图片类型还是视频类型
	static bool m_localBgSegVideoType;
	// 记录本地视频的分辨率
	static float m_localBgSegVideoWidth;
	static float m_localBgSegVideoHeight;
	
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
	static string mCurRenderGSSAItemName;
	static vector<string> categoryBundles[BundleCategory::Count];

    static void FindAllBundle(string folder,vector<string> &files);

	static void FindAllCommonPIC(string folder, vector<string> &files);
	
	static void FindAllCommonPICFromResourceBundle(string folder, vector<string> &files);
    
	static void FindAllCommonPICNameFromResourceBundle(string folder, vector<string> &names);
 
	static string GetFileFullPathFromResourceBundle(const char * path);
protected:
private:
};

const string g_faceBeautyParamName[MAX_BEAUTYFACEPARAMTER] = { "blur_level","color_level_mode2", "red_level","sharpen","eye_bright", "tooth_whiten" ,"remove_pouch_strength_mode2", "remove_nasolabial_folds_strength_mode2" };

const string g_faceShapeParamName[MAX_FACESHAPEPARAMTER] = { "cheek_thinning_mode2","eye_enlarging_mode3","intensity_eye_circle", "intensity_chin", "intensity_forehead_mode2", "intensity_nose_mode2","intensity_mouth_mode3",
		"cheek_v","cheek_narrow_mode2","cheek_short","cheek_small_mode2","intensity_cheekbones","intensity_lower_jaw",
	"intensity_canthus", "intensity_eye_space", "intensity_eye_rotate", "intensity_long_nose",
	"intensity_philtrum", "intensity_smile" ,"intensity_brow_height" ,"intensity_brow_space" };

//区分是否从中间值开始突变的Flag 0:变化从0->1 1:0->0.5 0.5->1
#define FACE_SHAPE_SHOW_FLAG_NORMAL (0)
#define FACE_SHAPE_SHOW_FLAG_MIDDLE (1)

const int g_faceShapeParamShowFlag[MAX_FACESHAPEPARAMTER] = { 0,0,0,1,1,0,
1,0,0,0,0,0,0,0,
1,1,1,1,0,1,1 };

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


#ifdef __APPLE__
const string g_assetDir = "";
#else
const string g_assetDir = "assets/";
#endif

const string gBundlePath[] = {
	g_assetDir + "Avatars/",
	g_assetDir + "items/" + "Animoji",
	g_assetDir + "items/" + "ItemSticker",
	g_assetDir + "items/" + "JINPINHOLDER",
	g_assetDir + "items/" + "ARMask",
	g_assetDir + "items/" + "ExpressionRecognition",
	g_assetDir + "items/" + "MusicFilter",
	g_assetDir + "items/" + "BackgroundSegmentation",
	g_assetDir + "items/" + "GestureRecognition",
	g_assetDir + "items/" + "MagicMirror",
	g_assetDir + "items/" + "Makeup",
	g_assetDir + "items/" + "LightMakeup",
	g_assetDir + "items/" + "BeautyHair",
	g_assetDir + "items/" + "BigHead",
	g_assetDir + "items/" + "GreenScreen"
};

const string g_pose_track_folder = g_assetDir + "PoseTrack/";

const string gGSBgPic = g_assetDir + "items/" + "GreenScreenBg";

const string gGSSAPic = g_assetDir + "items/" + "SafeArea";

const string gGSColorConfig = g_assetDir + "colorConfig.json";

const string gCustomCMConfig = g_assetDir + "items/Makeup/subs_setup.json";

const string gUserConfig = "UserConfig.json";


#endif
