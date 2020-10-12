#ifndef __NAMA_H__
#define __NAMA_H__

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <queue>
#include "GuiTool.h"
#include "MouseControl.h"
class CCameraDS;
class FuController;
typedef unsigned char uchar;
namespace NamaExampleNameSpace
{
	enum AvatarType
	{
		AVATAR_TYPE_NONE,
		AVATAR_TYPE_FULLBODY,
		AVATAR_TYPE_HALF
	};

	enum MakeupParamType
	{
		MString,
		MArray,
		MInt,
		MFloat,
	};
	struct MakeupParam
	{
		int type;
		std::string name;
		float value;
		std::vector<float> colorArr;
		std::string tex;
		int lip_type;
		float brow_warp;
		int brow_warp_type;
	};
	class Nama
	{		
	public:
		using UniquePtr = std::unique_ptr<Nama>;
		static UniquePtr create(uint32_t width, uint32_t height,bool enable = true);

		Nama();
		~Nama();

		std::vector<std::string> CameraList();
		cv::Mat GetOriginFrame();
		cv::Mat GetFrame();
		bool ReOpenCamera(int);
		bool ReOpenCamera(std::string strVideoPath);
		void CloseCurCamera();
		void OpenCamera(int);
		void OpenCamera(std::string strVideoPath);
		cv::Size getCameraResolution();
		bool IsCameraPlaying();
		bool IsCameraInit();
		int GetCameraCaptureType();
		int GetCameraID();
		void setDefaultFrame();

		bool CheckGLContext();
		bool Init(uint32_t& width, uint32_t& height);
		bool InitAvatarController();
		bool IsInited() { return mHasSetup; }
		bool SelectBundle(std::string bundleName);
		bool CheckModuleCode(int category);
		int  IsTracking();
		void SetCurrentShape(int index);
		int CreateMakeupBundle(std::string bundleName);
		void SelectMakeupBundle(std::string bundleName);
		void UpdateFilter(int);		
		void UpdateBeauty();
		void SwitchBeauty(bool);

		void UpdateGSBg(cv::Mat & dataRGBA);
		void UpdateGreenScreen();
		void SetGSKeyColor(cv::Vec4b data);

		std::vector<gui_tool::ColorBag> GetRencentColor();

		bool IsBodyShapeOpen();
		void UpdateBodyShape();

		void RenderItems(cv::Mat & picMat);
		void DrawLandmarks(uchar*  frame);		
		void DrawPoint(uchar*  frame, int x, int y, unsigned char r = 255, unsigned char g = 240, unsigned char b = 33);

		AvatarType GetAvatarType();
		void SetAvatarType(AvatarType type);
		void SwitchAvatar();

		void AddRecentColor(cv::Vec4b data);
		
		void pauseCurrentMp3();
		void resumeCurrentMp3();
        void changeGSPreviewRect(FURect rect);
		void setGSPreviewRect(FURect rect);
		FURect getGSPreviewRect();
	private:
		
		void RenderGS(cv::Mat & picInput);
		void RenderDefNama(uchar * frame);
		void RenderP2A(cv::Mat & picBg);

		void LoadRencentColorFromFile();
		void SaveRencentColorToFile();


	private:			
		int mBodyShapeHandle = -1;
		int mBeautyHandles;
		int mMakeUpHandle;
		int mGestureHandles;
		int mFxaaHandles;
		int mGSHandle = -1;
		uint32_t mFrameWidth, mFrameHeight;
		static bool mHasSetup;
		std::queue<gui_tool::ColorBag> m_queueRencetColor;
		

       
	public:	
		static bool mEnableNama;
		static bool mEnableAvatar;
		int mIsBeautyOn;	
		int mIsDrawPoints;
		int mMaxFace;
		int mFrameID;		
		int mModuleCode, mModuleCode1;
		FURect gsPreviewRect;
		
#ifdef _WIN32
		std::tr1::shared_ptr<CCameraDS> mCapture;
#else
        std::shared_ptr<CCameraDS> mCapture;
#endif
		static std::string mFilters[6];
		std::unordered_map<std::string, int> mBundlesMap;
		std::unordered_map<std::string, std::vector<MakeupParam> > mMakeupsMap;
		FuController * m_pCtrl;
		AvatarType m_avaType = AVATAR_TYPE_FULLBODY;
	};
}

template < class T>
std::string ConvertToString(T value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

struct MakeupParam
{
	std::string typeName;
	std::string valueName;
	std::string textureName;
	int value;
};

#endif
