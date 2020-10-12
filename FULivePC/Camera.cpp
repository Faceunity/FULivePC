#include "Camera.h"
#include "stdio.h"
#include "fu_tool.h"

#if _WIN32
#include "tinfiledlg/tinyfiledialogs.h"
#include<iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>
#include <locale>
#include <codecvt>
#elif __APPLE__
#include <unistd.h>
#include <pthread.h>
#include "fu_tool_mac.h"
#endif

extern int get_fps();
CCameraDS::CCameraDS()
{
	init();
}

CCameraDS::~CCameraDS()
{
	status = STATUS_NO_CAMERA;

	if (mCapture.isOpened())
	{
		mCapture.release();
	}
}

void CCameraDS::init() {
	m_isCameraInited = false;
	m_filepath = "";
	rs_width = 640;
	rs_height = 480;
	m_dstFrameSize.height = rs_height;
	m_dstFrameSize.width = rs_width;
	status = STATUS_UN_INIT;
	m_capture_camera_id = 0;
	frame_id = 0;
	frameCount = 0;
	getDeviceList();
}

#define  CAP_DELAY 10

static bool LoopVideo(CCameraDS* cc)
{
	if (cc->getCaptureType() == CAPTURE_FILE &&  cc->frame_id >= cc->frameCount && cc->IsLoop())
	{
		cc->mCapture.set(cv::CAP_PROP_POS_FRAMES, 0);
		cc->frame_id = 0;
		return true;
	}
	return false;
}

/// 以近似的方式，判断是否再次循环播放，
/// 去除  cc->frame_id >= cc->frameCount 判断，解决 部分视频无法循环播放的问题
/// @param cc 输入源
static bool LoopVideoApproximate(CCameraDS* cc)
{
	if (cc->getCaptureType() == CAPTURE_FILE && cc->IsLoop())
	{
		cc->mCapture.set(cv::CAP_PROP_POS_FRAMES, 0);
		cc->frame_id = 0;
		return true;
	}
	return false;
}
#if 0
void CCameraDS::savePngFilesToLocalDir(string dirPath,cv::Mat frame)
{
	std::stringstream pngPath;
	pngPath << dirPath << "localPNG_" << localPNGNum++ << ".png";
	std::cout << pngPath.str() << std::endl;
	cv::imwrite(pngPath.str(), frame);
}
#endif
static void ProcessFrame(CCameraDS* cc)
{
	while (cc->mCapture.grab() || 
	// 解决 部分视频文件无法重复循环的问题
	LoopVideoApproximate(cc))
	{
		if (cc->mCapture.isOpened())
		{
            cv::Mat orgMat;
		    cc->mCapture.retrieve(cc->frame);
			
			
			cc->frame_id++;
			//单帧图片啥的就不刷了
			if (cc->getCaptureType() == CAPTURE_FILE && cc->frameCount == 1)
			{
				break;
			}
			
			//开启循环的视频元素，要重新seek
			LoopVideo(cc);
			
			char key = (char)cv::waitKey(CAP_DELAY);
		}
		
		if (cc->getStatus() == STATUS_NO_CAMERA)
		{
			break;
		}
	}
	
}

#ifdef _WIN32

static DWORD WINAPI TL_FRAME(void * lpParamter)
{
	CCameraDS* cc = (CCameraDS*)lpParamter;
	ProcessFrame(cc);

	return 0;
}

#else

static void * TL_FRAME(void * lpParamter)
{
	CCameraDS* cc = (CCameraDS*)lpParamter;
	ProcessFrame(cc);

	pthread_exit(NULL);

	return NULL;
}
#endif



cv::Size CCameraDS::getCameraResolution()
{
	mCapture.set(cv::CAP_PROP_FRAME_WIDTH, 10000);
	mCapture.set(cv::CAP_PROP_FRAME_HEIGHT, 10000);
	int w = (int)mCapture.get(cv::CAP_PROP_FRAME_WIDTH);
	int h = (int)mCapture.get(cv::CAP_PROP_FRAME_HEIGHT);
	return cv::Size(w, h);
}


int CCameraDS::getCaptureType()
{
	return m_capture_type;
}

void CCameraDS::setCaptureCameraID(int _id)
{
	m_capture_camera_id = _id;
}

int CCameraDS::getCaptureCameraID()
{
	return m_capture_camera_id;
}

#define EPS (0.000001)

void CCameraDS::InitCameraFile(int width, int height, std::string path, bool bLoop)
{
	try {

		if (!mCapture.isOpened())
		{
			localPNGNum = 0;
			m_filepath = path;
			mCapture.open(path);

			if (!mCapture.isOpened())
				throw std::runtime_error("Unable to open video source");

			rs_width = width;
			rs_height = height;

			calculateRect();

			m_capture_type = CAPTURE_FILE;
			m_bloop = bLoop;
		}

		connectCamera();
	}
	catch (...) {
		loge("camera file init error\n");
	}
}

void CCameraDS::calculateRect()
{
	cv::Size maxRes = getCameraResolution();
	cv::Size neededRes = cv::Size(rs_width, rs_height);

	double aR = (double)maxRes.width / maxRes.height;
	double aN = (double)neededRes.width / neededRes.height;

	//保留Need的比例，实际像素改变
	if (fabs(aR - aN) < EPS)
	{
		m_dstFrameSize = neededRes;
	}
	else
	{
		m_dstFrameSize.width = MIN(MIN(maxRes.width, maxRes.height*aN), neededRes.width);  //取符合比例的值，同时小于needWidth
		m_dstFrameSize.height = m_dstFrameSize.width / aN;
	}
	mCapture.set(cv::CAP_PROP_FRAME_WIDTH, m_dstFrameSize.width);
	mCapture.set(cv::CAP_PROP_FRAME_HEIGHT, m_dstFrameSize.height);
}

void CCameraDS::initCamera(int width, int height, int camID) {
	logi("CCameraDS::initCamera\n");
	status = STATUS_NO_CAMERA;
	try {
		if (!mCapture.isOpened())
		{
#ifdef _WIN32
			bool bOK = mCapture.open(cv::CAP_DSHOW + camID);
#else
			bool bOK = mCapture.open(cv::CAP_AVFOUNDATION + camID);
#endif
			localPNGNum = 0;
			//mCapture.open(camID);
			rs_width = width;
			rs_height = height;

			if (!mCapture.isOpened())
				throw std::runtime_error("Unable to open video source");

			calculateRect();

			mCapture.set(cv::CAP_PROP_AUTOFOCUS, 0);

			m_capture_camera_id = camID;
			m_capture_type = CAPTURE_CAMERA;
		}

		connectCamera();
	}
	catch (...) {
		loge("camera init error\n");
	}
}
void CCameraDS::clearLastFrame(){
	cv::Mat blackMat(frame.rows, frame.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	frame = blackMat;
}
/// 设置默认的待机画面
void CCameraDS::setDefaultFrame(){
#if _WIN32
	string defaultPicPath = "../../res/frame.png";
#elif __APPLE__
	string defaultPicPath = FuToolMac::GetFileFullPathFromResPicBundle("frame.png");
#endif
	static cv::Mat defaulfFrame = cv::imread(defaultPicPath, cv::IMREAD_COLOR);
	if (!defaulfFrame.empty())
	{
		cv::resize(defaulfFrame, defaulfFrame, cv::Size(rs_width, rs_height));
		frame = defaulfFrame;
	}
}
void CCameraDS::closeCamera() {

	status = STATUS_NO_CAMERA;
	m_filepath = "";

#ifdef _WIN32
	Sleep(1000);
#else
	pthread_join(m_pid, NULL);
	sleep(1);
#endif

	if (mCapture.isOpened())
	{
		mCapture.release();
	}
}

void CCameraDS::connectCamera()
{
	if (!mCapture.isOpened()) {
		loge("can not open camera!!!\n");
		status = STATUS_NO_CAMERA;
	}
	if (mCapture.isOpened())
	{
		double fps = 60;   //读取视频的帧?
		if (m_capture_type == CAPTURE_FILE)
		{

			fps = mCapture.get(cv::CAP_PROP_FPS);   //读取视频的帧?
			frameCount = mCapture.get(cv::CAP_PROP_FRAME_COUNT);;
			int vfps = 1000 / fps;                                        //计算每帧播放的时间
		}
		

		{
#ifdef _WIN32
			hThread = CreateThread(NULL, 0, TL_FRAME, this, 0, NULL);
			CloseHandle(hThread);
#else
			pthread_create(&m_pid, NULL, TL_FRAME, (void *)this);
#endif
		}

		status = STATUS_INIT;
		frame_id = 0;
		m_isCameraInited = true;
		play();
	}
}

bool CCameraDS::isPlaying() {
	return status == STATUS_PLAYING;
}

void CCameraDS::restartCamera()
{
	closeCamera();
	initCamera(rs_width, rs_height, 0);
}

void CCameraDS::play() {
	if (!m_isCameraInited) {
		initCamera(rs_width, rs_height, 0);
	}

	if (status == STATUS_PLAYING)return;
	if (status == STATUS_STOP || status == STATUS_INIT) {

		status = STATUS_PLAYING;
		return;
	}
}

void CCameraDS::stop() {
	if (status == STATUS_PLAYING) {
		status = STATUS_STOP;

	}
}

cv::Mat CCameraDS::getFrame() {
	if (frame.rows == 0 && frame.cols == 0)
	{
#if _WIN32
		string defaultPicPath = "../../res/frame.png";
#elif __APPLE__
		string defaultPicPath = FuToolMac::GetFileFullPathFromResPicBundle("frame.png");
#endif
		static cv::Mat defaulfFrame = cv::imread(defaultPicPath, cv::IMREAD_COLOR);
		if (!defaulfFrame.empty())
		{
			cv::resize(defaulfFrame, defaulfFrame, cv::Size(rs_width, rs_height));
			frame = defaulfFrame;
		}
	}
	return frame;
}

#pragma region enum_camera


static void setcolor(unsigned int color)
{
#ifdef _WIN32
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon, color | FOREGROUND_INTENSITY);
#endif
}

#ifdef _WIN32

void CCameraDS::_FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		// pUnk should not be used.
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}


HRESULT CCameraDS::CamCaps(IBaseFilter *pBaseFilter)
{
	HRESULT hr = 0;
	std::vector<IPin*> pins;
	IEnumPins *EnumPins;
	pBaseFilter->EnumPins(&EnumPins);
	pins.clear();
	for (;;)
	{
		IPin *pin;
		hr = EnumPins->Next(1, &pin, NULL);
		if (hr != S_OK) { break; }
		pins.push_back(pin);
		pin->Release();
	}
	EnumPins->Release();

	//printf("Device pins number: %d\n", pins.size());

	PIN_INFO pInfo;
	for (int i = 0; i < pins.size(); i++)
	{
		pins[i]->QueryPinInfo(&pInfo);

		setcolor(RED);

		if (pInfo.dir == 0)
		{
		}

		if (pInfo.dir == 1)
		{
		}

		IEnumMediaTypes *emt = NULL;
		pins[i]->EnumMediaTypes(&emt);

		AM_MEDIA_TYPE *pmt;

		std::vector<SIZE> modes;
		setcolor(GRAY);
		for (;;)
		{
			hr = emt->Next(1, &pmt, NULL);
			if (hr != S_OK) { break; }

			if ((pmt->formattype == FORMAT_VideoInfo) &&
				//(pmt->subtype == MEDIASUBTYPE_RGB24) &&
				(pmt->cbFormat >= sizeof(VIDEOINFOHEADER)) &&
				(pmt->pbFormat != NULL))
			{
				VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)pmt->pbFormat;
				SIZE s;
				// Get frame size
				s.cy = pVIH->bmiHeader.biHeight;
				s.cx = pVIH->bmiHeader.biWidth;
				// Битрейт
				unsigned int bitrate = pVIH->dwBitRate;
				modes.push_back(s);
				// Bits per pixel
				unsigned int bitcount = pVIH->bmiHeader.biBitCount;
				REFERENCE_TIME t = pVIH->AvgTimePerFrame; // blocks (100ns) per frame
				int FPS = floor(10000000.0 / static_cast<double>(t));
				//printf("Size: x=%d\ty=%d\tFPS: %d\t bitrate: %ld\tbit/pixel:%ld\n", s.cx, s.cy, FPS, bitrate, bitcount);
			}
			_FreeMediaType(*pmt);
		}
		modes.clear();
		emt->Release();
	}

	pins.clear();

	return S_OK;
}

/*
* Do something with the filter. In this sample we just test the pan/tilt properties.
*/
void CCameraDS::process_filter(IBaseFilter *pBaseFilter)
{
	CamCaps(pBaseFilter);
}


/*
* Enumerate all video devices
*
* See also:
*
* Using the System Device Enumerator:
*     http://msdn2.microsoft.com/en-us/library/ms787871.aspx
*/
int CCameraDS::enum_devices()
{
	HRESULT hr;
	setcolor(GRAY);
	printf("Enumeraring videoinput devices ...\n");

	// Create the System Device Enumerator.
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr))
	{
		fprintf(stderr, "Error. Can't create enumerator.\n");
		return hr;
	}

	// Obtain a class enumerator for the video input device category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);

	if (hr == S_OK)
	{
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
				(void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				// To create an instance of the filter, do the following:
				IBaseFilter *pFilter;
				hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
					(void**)&pFilter);
				if (!pFilter)
				{
					printf("Warning! Create an instance of the filter failed! \n");
					setcolor(GRAY);
					pMoniker->Release();
					break;
				}
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{
					// Display the name in your UI somehow.
					setcolor(GREEN);
					std::string str;
					int len = WideCharToMultiByte(CP_ACP, 0, varName.bstrVal, wcslen(varName.bstrVal), NULL, 0, NULL, NULL);
					char    *m_char = new char[len + 1];
					WideCharToMultiByte(CP_ACP, 0, varName.bstrVal, wcslen(varName.bstrVal), m_char, len, NULL, NULL);
					m_char[len] = '\0';
					str = m_char;
					deviceList.push_back(str);
				}
				VariantClear(&varName);
				//Remember to release pFilter later.
				pPropBag->Release();
			}
			pMoniker->Release();
		}
		pEnumCat->Release();
	}
	setcolor(GRAY);
	pSysDevEnum->Release();

	return 0;
}

#endif

int CCameraDS::getDeviceList()
{

	int result;

#ifdef _WIN32

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	result = enum_devices();

	CoUninitialize();
#else

	//OSX
	deviceList = FuTool::getVideoDevices();

#endif



	return result;
}
std::vector<std::string> CCameraDS::getDeviceNameList()
{
	return deviceList;
}


#pragma endregion enum_camera
