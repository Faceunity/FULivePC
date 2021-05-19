#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <stack>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#ifdef _WIN32
#include <dshow.h>
#endif

#include <locale>
#include <vector>
#include "fu_lock_mutex.h"

#pragma comment(lib,"Strmiids.lib")

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

using namespace std;
enum CAPTURE_TYPE
{
	CAPTURE_NONE,
	CAPTURE_CAMERA,
	CAPTURE_FILE
};

typedef enum {
	STATUS_UN_INIT,
	STATUS_NAMA_UN_INIT,
	STATUS_INIT,
	STATUS_PLAYING,
	STATUS_STOP,
	STATUS_NO_CAMERA,
	STATUS_ERROR
}PlayStatus;

class CCameraDS
{

public:
	CCameraDS();
	~CCameraDS();

	void play();
	void stop();
#if 0
	void savePngFilesToLocalDir(string dirPath,cv::Mat frame);
#endif
	void InitCameraFile(int, int, std::string, bool bLoop = true);

	void InitCameraVideo(int, int, std::string, bool bLoop = true);

	void InitCameraSinglePic(int, int, std::string, bool bLoop = false);

	void initCamera(int,int,int);
	void closeCamera();
	void clearLastFrame();
	void setDefaultFrame();
	void connectCamera();
	bool isPlaying();
	void restartCamera();
	void LoadDefIFNone();
    void restartCameraWhenClosed();
	void calculateRect();
	bool checkFps();

	bool isInit() { return m_isCameraInited; }
	int getStatus() { return status; }
	bool IsLoop()
	{
		return m_bloop;
	}


	std::string getFilePath()
	{
		return m_filepath;
	}

	int getCaptureType();
	void setCaptureCameraID(int _id);
	int getCaptureCameraID();
	cv::Mat getFrame();
    
#ifdef _WIN32
	void _FreeMediaType(AM_MEDIA_TYPE & mt);
	HRESULT CamCaps(IBaseFilter * pBaseFilter);
	void process_filter(IBaseFilter * pBaseFilter);
#endif
	int enum_devices();
	int getDeviceList();
	std::vector<std::string> getDeviceNameList();
	void init();

	std::vector<std::string> deviceList;

	cv::Size getCameraDstResolution();

	bool IsVideoFile() const {
		return m_IsVideo;
	}

private:

	cv::Size getCameraResolution();

private:
	float w, h;
	
	std::string m_filepath;
	PlayStatus status;
	bool m_isCameraInited = false;
	int m_capture_type = CAPTURE_NONE;
	int m_capture_camera_id = -1;
	bool m_bloop = false;
	double m_FPS = 30.0;
	bool m_IsVideo = true;

    int64 m_iStartTick = 0;

public:
	int frameCount;
	int frame_id = 0;
	int rs_width;
	int rs_height;
	bool useDefaultFrame;
	cv::VideoCapture mCapture;
	cv::Mat tmpframe;
	cv::Mat frame;
	cv::Mat resize_frame;
	cv::Size m_dstFrameSize;

	//����Ӧ��Ū�¼���������ƽ̨�е㷳
	volatile bool m_bThreadEnd = false;
	L_Mutex m_mtxFrame;
    
#ifdef _WIN32
	HANDLE m_hThread = nullptr;
#else
    pthread_t m_pid = 0;
#endif
};

#endif

