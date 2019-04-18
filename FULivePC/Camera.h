#pragma once

#include <stack>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include <dshow.h>
#include <locale>
#include <vector>

#pragma comment(lib,"Strmiids.lib")
#ifdef _DEBUG
#pragma comment(lib,"opencv_world400d.lib")
#else
#pragma comment(lib,"opencv_world400.lib")
#endif // DEBUG



#define BLUE    0x0001
#define GREEN   0x0002
#define RED     0x0004
#define GRAY    0x0007

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p) = nullptr; } }
#endif

#ifndef loge
#define loge    printf
#define logi    printf
#endif
enum CAPTURE_TYPE
{
	CAPTURE_CAMERA,
	CAPTURE_FILE
};

class CCameraDS
{

public:
	CCameraDS();
	~CCameraDS();

	void play();
	void stop();

	void initCamera(int,int,int);
	void closeCamera();
	void connectCamera();
	bool openFileDlg();
	bool isPlaying();
	void restartCamera();
	bool isInit() { return m_isCameraInited; }
	int getStatus() { return status; }
	void setFilePath(std::string path);
	void setCaptureType(int _type);
	void setCaptureCameraID(int _id);
	cv::Mat getFrame();
	void _FreeMediaType(AM_MEDIA_TYPE & mt);
	HRESULT CamCaps(IBaseFilter * pBaseFilter);
	void process_filter(IBaseFilter * pBaseFilter);
	int enum_devices();
	int getDeviceList();
	std::vector<std::string> getDeviceNameList();
	void init();

	std::vector<std::string> deviceList;
	cv::Size getCameraResolution();
	void QueryFrame();
private:
	float w, h;
	typedef enum {
		STATUS_UN_INIT,
		STATUS_NAMA_UN_INIT,
		STATUS_INIT,
		STATUS_PLAYING,
		STATUS_STOP,
		STATUS_NO_CAMERA,
		STATUS_ERROR
	}PlayStatus;
	
	std::string m_filepath;
	PlayStatus status;
	bool m_isCameraInited = false;
	int m_capture_type;
	int m_capture_camera_id;
	int frame_id;
	int frameCount;
public:
	int rs_width;
	int rs_height;
	bool useDefaultFrame;
	cv::VideoCapture mCapture;
	cv::Mat frame;
	cv::Mat resize_frame;
	HANDLE hThread;
};

