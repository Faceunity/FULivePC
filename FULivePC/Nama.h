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
		static UniquePtr create(int width, int height);

		Nama();
		~Nama();
		std::vector<std::string> CameraList();
		//cv::Mat GetFrame();
		std::tr1::shared_ptr<unsigned char> QueryFrame();
		//bool ReOpenCamera(int);
		void ReOpenCamera();
		bool CheckGLContext();
		bool Init(int& width, int& height);
		bool IsInited() { return mHasSetup; }
		bool SelectBundle(std::string bundleName);
		int  IsTracking();
		void SetCurrentShape(int index);
		void UpdateFilter(int);
		void UpdateBeauty();
		void SwitchBeauty(bool);
		void RenderItems(uchar* frame);		
		void DrawLandmarks(uchar*  frame);		
		uchar*  RenderEx(uchar*);
		void DrawPoint(uchar*  frame, int x, int y, unsigned char r = 255, unsigned char g = 240, unsigned char b = 33);
		unsigned char* ConvertBetweenBGRAandRGBA(unsigned char* frame);
	public:
		int mBeautyHandles;
		int mGestureHandles;
		int mFxaaHandles;
		int mFrameWidth, mFrameHeight;
		static bool mHasSetup;
	public:	
		int mIsBeautyOn;	
		int mIsDrawPoints;
		int mMaxFace;
		int mFrameID;		
		//std::tr1::shared_ptr<CCameraDS> mCapture;
		std::tr1::shared_ptr<CCameraDS> m_cap;
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