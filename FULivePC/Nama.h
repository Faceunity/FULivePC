#pragma once

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
		int  IsTracking();
		void SetCurrentShape(int index);
		void SetCurrentMakeup(int index);
		void UpdateFilter(int);
		void UpdateMakeupParams();
		void UpdateBeauty();
		void SwitchBeauty(bool);
		void RenderItems(uchar* frame);		
		void DrawLandmarks(uchar*  frame);		
		uchar*  RenderEx(uchar*);
		void DrawPoint(uchar*  frame, int x, int y, unsigned char r = 255, unsigned char g = 240, unsigned char b = 33);
	private:				
		int mBeautyHandles;
		int mLightMakeUpHandle;
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
		std::tr1::shared_ptr<CCameraDS> mCapture;
		static std::string mFilters[6];
		std::unordered_map<std::string, int> mBundlesMap;
	};

	size_t FileSize(std::ifstream& file);

	bool LoadBundle(const std::string& filepath, std::vector<char>& data);
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
