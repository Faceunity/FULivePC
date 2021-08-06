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
#include <functional>
#include <iostream>

#include "Config.h"

class CCameraDS;
class FuController;
typedef unsigned char uchar;
namespace NamaExampleNameSpace
{

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

	typedef struct tagNamaState
	{
		bool EnableNama;
		bool EnableAvatar;
		bool EnableAvatarUI;
		bool RenderAvatar;

		tagNamaState() {

			EnableNama = false;
			EnableAvatar = false;
			EnableAvatarUI = false;
			RenderAvatar = false;

		}

	}NamaState;

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
        bool restartCameraWhenClosed();
        // lambda 用于 oc 的 回调
        std::function<void()> lock_screen_not_macosx() {
            return [&]{ std::cout << "lock_screen_not_macosx" << std::endl;
                CloseCurCamera();
            };
        }
        std::function<void()> unlock_screen_not_macosx() {
            return [&]{ std::cout << "unlock_screen_not_macosx" << std::endl;
                restartCameraWhenClosed();
            };
        }
		void CloseCurCamera();
		void OpenCamera(int);
		void OpenCamera(std::string strVideoPath);
		cv::Size getCameraDstResolution();
		bool IsCameraPlaying();
		bool IsCameraInit();
		int GetCameraCaptureType();
		int GetCameraID();
		void setDefaultFrame();

		bool CheckGLContext();
		bool Init(uint32_t& width, uint32_t& height);
		bool InitController();
		bool IsInited() { return mHasSetup; }

		void LoadAvatarHandTrackBundle();
		void LoadAvatarBundles(const std::vector<std::string>& bundleNames);
		void UnLoadAvatar();

		void SetCurDouble(const std::string& key, double v);
		void SetCMDoubles(const std::string& key, double* values, uint32_t count);
		void SetCMDouble(const std::string& key, double v);
		int SelectCustomMakeupBundle(std::string bundleName, std::string strType);
		void ClearAllCM();
		void UnbindCurFixedMakeup();
		void ChangeCleanFlag(bool bOpen);

		bool SelectBundle(std::string bundleName, int maxFace = 4);
		bool CheckModuleCode(int category);
		bool CheckModuleCodeSide(int categorySide);
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


		void UpdateSegBg(cv::Mat & dataRGBA);

		bool IsBodyShapeOpen();
		void UpdateBodyShape();

		void RenderItems(cv::Mat & picMat);
		void DrawLandmarks(uchar*  frame);		
		void DrawPoint(uchar*  frame, int x, int y, unsigned char r = 255, unsigned char g = 240, unsigned char b = 33);

		BodyTrackType GetBodyTrackType();
		void SwitchBodyTrackType();
		void SetBodyTrackType(BodyTrackType type);

		void AddRecentColor(cv::Vec4b data);
		
		void pauseCurrentMp3();
		void resumeCurrentMp3();
        void changeGSPreviewRect(FURect rect);
		void setGSPreviewRect(FURect rect);
		FURect getGSPreviewRect();

		void DestroyAll();
		int GetLastNamaError();
	private:
		
		void RenderGS(cv::Mat & picInput, int rotType);
		void RenderDefNama(cv::Mat & picInput, int rotType);
		void RenderP2A(cv::Mat & picBg, int rotType);

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
		std::queue<gui_tool::ColorBag> m_queueRencetColor;
		std::map<std::string, int> m_CMakeupTypeMap;
		std::unordered_map<std::string, int> m_CMakeupMap;

		std::vector<std::string> avatarBundles;
       
		static bool mHasSetup;
	public:	
		static NamaState mNamaAppState;
		static NamaState mNamaAppStateBackAR;
		static NamaState mNamaAppStateBackGS;

		int mIsBeautyOn;
		int mIsDrawPoints;
		int mFrameID;		
		int mModuleCode, mModuleCode1;
		FURect gsPreviewRect;
		
#ifdef _WIN32
		std::tr1::shared_ptr<CCameraDS> mCapture;
#else
        std::shared_ptr<CCameraDS> mCapture;
#endif
		static std::string mFilters[6];
		std::map<std::string, int> mBundlesMap;
		std::unordered_map<std::string, std::vector<MakeupParam> > mMakeupsMap;
		std::shared_ptr<FuController> m_Controller;
		BodyTrackType m_bodyTrackType = BodyTrackType::FullBody;
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
