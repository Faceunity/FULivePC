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

extern struct LightMakeupParam;
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
		bool RenderAvatarBear;
		tagNamaState() {

			EnableNama = false;
			EnableAvatar = false;
			EnableAvatarUI = false;
			RenderAvatar = false;
			RenderAvatarBear = false;
		}

	}NamaState;

	class Nama
	{		
	public:
		using UniquePtr = std::unique_ptr<Nama>;
		static void create(bool enable = true);
		static UniquePtr pNama;
		static Nama* get() {return pNama.get();}

		Nama();
		~Nama();
		
		bool CheckGLContext();
		bool Init();
        bool Release();
		bool InitController();
		bool IsInited() { return mHasSetup; }

		void LoadAvatarHandTrackBundle();
		void LoadAvatarBundles(const std::vector<std::string>& bundleNames);
		void UnLoadAvatar();
		void UnLoadMakeup();
		void ReloadItems();

		void SetCurDouble(const std::string& key, double v);
		void SetCMDoubles(const std::string& key, double* values, uint32_t count);
		void SetCMDouble(const std::string& key, double v);
		int SelectCustomMakeupBundle(std::string bundleName, std::string strType);
		void ClearAllCM();
		void UnbindCurFixedMakeup();
		void ChangeCleanFlag(bool bOpen);

		bool SelectBundle(std::string bundleName, int maxFace = 4);
		bool CheckModuleFaceInfo();
		bool CheckModuleBodyInfo();
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
		void UpdateGSSA(cv::Mat& dataRGBA);
		void NonuseGSSA();
		void UpdateGreenScreen();
		void SetGSKeyColor(cv::Vec4b data);
		void GetGSParamd(vector<int>& data);

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
		void setLightMakeupTex(string path, string value);
		void setLightMakeupParam(LightMakeupParam param);
		void setHumanSegScene(int type);
		void setMaxFaces(int face);
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
		std::queue<gui_tool::ColorBag> m_queueRencetColor;
		std::map<std::string, int> m_CMakeupTypeMap;
		std::unordered_map<std::string, int> m_CMakeupMap;

		std::vector<std::string> avatarBundles;
       
		static bool mHasSetup;
	public:	
		static NamaState mNamaAppState;
		static NamaState mNamaAppStateBackAR;
		static NamaState mNamaAppStateBackGS;

		uint32_t mFrameWidth, mFrameHeight;
		int mIsBeautyOn;
		int mIsDrawPoints;
		int mFrameID;		
		int mModuleCode, mModuleCode1;
		FURect gsPreviewRect;
	
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
