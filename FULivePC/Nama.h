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
class CCameraDS;
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
	class Nama
	{		
	public:
		using UniquePtr = std::unique_ptr<Nama>;
		static UniquePtr create(uint32_t width, uint32_t height,bool enable = true);

		Nama();
		~Nama();
		std::vector<std::string> CameraList();
		cv::Mat GetFrame();
		bool ReOpenCamera(int);
		bool CheckGLContext();
		bool Init(uint32_t& width, uint32_t& height);
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
		void RenderItems(uchar* frame);		
		void DrawLandmarks(uchar*  frame);		
		uchar*  RenderEx(uchar*);
		void DrawPoint(uchar*  frame, int x, int y, unsigned char r = 255, unsigned char g = 240, unsigned char b = 33);
	private:				
		int mBeautyHandles;
		int mMakeUpHandle;
		int mGestureHandles;
		int mFxaaHandles;
		uint32_t mFrameWidth, mFrameHeight;
		static bool mHasSetup;
	public:	
		static bool mEnableNama;
		int mIsBeautyOn;	
		int mIsDrawPoints;
		int mMaxFace;
		int mFrameID;		
		int mModuleCode, mModuleCode1;
#ifdef _WIN32
		std::tr1::shared_ptr<CCameraDS> mCapture;
#else
        std::shared_ptr<CCameraDS> mCapture;
#endif
		static std::string mFilters[6];
		std::unordered_map<std::string, int> mBundlesMap;
		std::unordered_map<std::string, std::vector<MakeupParam>> mMakeupsMap;
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
