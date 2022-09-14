#include "Camera.h"
#include "stdio.h"
#include "fu_tool.h"
#include "UIBridge.h"


#if _WIN32
//#define OpencvDShow
#include "tinfiledlg/tinyfiledialogs.h"
#include <iostream>
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

#include <regex>
#include <CNamaSDK.h>

#ifndef _WIN32
#define OpencvDShow
extern uint64_t GetTickCount();
#endif

#ifndef OpencvDShow
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include <libavutil/imgutils.h>
}
AVCodecContext* pCodecCtx;
AVFormatContext* pFormatCtx;
AVPacket* pPacket;
AVFrame* pFrame, * pFrameRGB;
struct SwsContext* img_convert_ctx;
uint8_t* rgbBuffer;
//流ID
int streamIndex = -1;
string GbkToUtf8(const char* src_str)
{
    int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    string strTemp = str;
    if (wstr) delete[] wstr;
    if (str) delete[] str;
    return strTemp;
}

string Utf8ToGbk(const char* src_str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    string strTemp(szGBK);
    if (wszGBK) delete[] wszGBK;
    if (szGBK) delete[] szGBK;
    return strTemp;
}
#endif


//#define PERFORMANCE_OPEN
extern int get_fps(bool);
CCameraDS::CCameraDS()
{
    init();
}

CCameraDS::~CCameraDS()
{
    status = STATUS_NO_CAMERA;
    closeCamera();
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
    if (cc->getCaptureType() == CAPTURE_FILE && cc->frame_id >= cc->frameCount && cc->IsLoop())
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
void CCameraDS::savePngFilesToLocalDir(string dirPath, cv::Mat frame)
{
    std::stringstream pngPath;
    pngPath << dirPath << "localPNG_" << localPNGNum++ << ".png";
    std::cout << pngPath.str() << std::endl;
    cv::imwrite(pngPath.str(), frame);
}
#endif

#include <future> 

#ifndef OpencvDShow
static void ProcessFrameFFMPEG(CCameraDS* cc)
{
    while (!cc->m_bThreadEnd)
    {
        if (av_read_frame(pFormatCtx, pPacket) >= 0) {
            if (pPacket->stream_index == streamIndex) {
                int ret;
                ret = avcodec_send_packet(pCodecCtx, pPacket);
                if (ret < 0)
                {
                    cout << "Decode Error." << endl;
                    continue;
                }
                av_packet_unref(pPacket);
                while(avcodec_receive_frame(pCodecCtx, pFrame) == 0)
                {
                    // AVFrame 转RGB32数据
                    sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                    cv::Mat mat = cv::Mat(pCodecCtx->height, pCodecCtx->width, CV_8UC4, (uchar*)rgbBuffer);
                    Locker locker(&cc->m_mtxFrame);
                    if (!mat.empty()) {
                        cc->frame = mat.clone();
                        CCameraManage::getInstance()->SetNewFrame(true);
                    }
                }
            }
        }
    }
    av_packet_free(&pPacket);
    // 释放内存
    sws_freeContext(img_convert_ctx);
    img_convert_ctx = nullptr;
    av_free(rgbBuffer);
    av_free(pFrame);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}

static DWORD WINAPI TL_FRAME_FFMPEG(void* lpParamter)
{
    CCameraDS* cc = (CCameraDS*)lpParamter;
    ProcessFrameFFMPEG(cc);

    return 0;
}
#endif
static void ProcessFrame(CCameraDS* cc)
{
    while (!cc->m_bThreadEnd)
    {
        if (!cc->checkFps() || !cc->isPlaying())
        {
            continue;
        }

        if (!cc->mCapture.grab() && !LoopVideoApproximate(cc))
        {
            break;
        }

        if (cc->mCapture.isOpened())
        {
            cc->mCapture.retrieve(cc->tmpframe);
            {
#ifndef PERFORMANCE_OPEN
                Locker locker(&cc->m_mtxFrame);
                cc->frame = cc->tmpframe.clone();
                CCameraManage::getInstance()->SetNewFrame(true);
#else
                cc->frame = cc->tmpframe;
#endif		
            }

            bool justBreak = (cc->getCaptureType() == CAPTURE_FILE && cc->frameCount == 1) ||
                (cc->getCaptureType() == CAPTURE_FILE && !cc->IsVideoFile());

            if (justBreak)
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

    if (cc->mCapture.isOpened())
    {
        cc->mCapture.release();
    }
}

#ifdef _WIN32

static DWORD WINAPI TL_FRAME(void* lpParamter)
{
    CCameraDS* cc = (CCameraDS*)lpParamter;
    ProcessFrame(cc);

    return 0;
}

#else

static void* TL_FRAME(void* lpParamter)
{
    CCameraDS* cc = (CCameraDS*)lpParamter;
    ProcessFrame(cc);

    pthread_exit(NULL);

    return NULL;
}
#endif

cv::Size CCameraDS::getCameraDstResolution()
{
    return cv::Size(m_dstFrameSize.width, m_dstFrameSize.height);
}

//Tip 数据帧读完之后，mCapture就会被关闭，这边就会读出0数据
cv::Size CCameraDS::getCameraResolution()
{
    mCapture.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    mCapture.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
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
    if (regex_match(path, regex("(.*)\\.(mp4|MP4|mov|MOV)"))) {
        m_IsVideo = true;
        InitCameraVideo(width, height, path, bLoop);
    }
    else {
        m_IsVideo = false;
        InitCameraSinglePic(width, height, path, bLoop);
    }
}

void CCameraDS::InitCameraVideo(int width, int height, std::string path, bool bLoop)
{
    try {

        if (!mCapture.isOpened())
        {
            m_filepath = path;
            mCapture.release();
            mCapture.open(path);

            if (!mCapture.isOpened())
                throw std::runtime_error("Unable to open video source");

            rs_width = width;
            rs_height = height;

            m_capture_type = CAPTURE_FILE;

            calculateRect();

            m_bloop = bLoop;
        }

        connectCamera();
    }
    catch (...) {
        loge("camera file init error\n");
    }
}

void CCameraDS::InitCameraSinglePic(int width, int height, std::string path, bool bLoop)
{
    try {

        closeCamera();
        frame = cv::imread(path.c_str(), cv::IMREAD_UNCHANGED);

        if (frame.data)
        {
            //最高720的宽，防止图片过大造成卡顿
            rs_height = MIN(720, frame.rows);
            rs_width = (float)frame.cols / frame.rows * rs_height;
            cv::resize(frame, frame, cv::Size(rs_width, rs_height));
            m_filepath = path;

            rs_width = frame.cols;
            rs_height = frame.rows;
            m_dstFrameSize.height = rs_height;
            m_dstFrameSize.width = rs_width;

        }
        else
        {
            m_filepath = "";
            rs_width = 0;
            rs_height = 0;
            m_dstFrameSize.height = 0;
            m_dstFrameSize.width = 0;
        }

        status = STATUS_INIT;
        m_capture_type = CAPTURE_FILE;
        m_isCameraInited = true;
    }
    catch (...) {
        loge("camera file init error\n");
    }
}

bool CCameraDS::checkFps()
{
    if (getCaptureType() != CAPTURE_FILE)
    {
        return true;
    }

    if (frame_id == 0)
    {
        m_iStartTick = GetTickCount();
        //printf("loop once \r\n");
    }

    int64 curTick = GetTickCount();
    float tickDelta = curTick - m_iStartTick;
    float curFrameTime = frame_id * (1000.0 / m_FPS);

    bool bNeedAdd2GetOriFps = (tickDelta > curFrameTime) || (frame_id == 0);
    float curRealFps = tickDelta > 0 ? frame_id * 1000.0f / tickDelta : 0;
    bool bQuicker = curRealFps > UIBridge::mFPS;

    if (bNeedAdd2GetOriFps)
    {
        frame_id++;
        return true;
    }
    else
    {
        return false;
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
        m_dstFrameSize.width = MIN(MIN(maxRes.width, maxRes.height * aN), neededRes.width);  //取符合比例的值，同时小于needWidth
        m_dstFrameSize.height = m_dstFrameSize.width / aN;
    }
    //mCapture.set(cv::CAP_PROP_FPS, 30.0);
    //mCapture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
    mCapture.set(cv::CAP_PROP_FRAME_WIDTH, m_dstFrameSize.width);
    mCapture.set(cv::CAP_PROP_FRAME_HEIGHT, m_dstFrameSize.height);
    mCapture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

    //不一定能设置进去，所以还得读一遍
    m_dstFrameSize.width = (int)mCapture.get(cv::CAP_PROP_FRAME_WIDTH);
    m_dstFrameSize.height = (int)mCapture.get(cv::CAP_PROP_FRAME_HEIGHT);
    cout << "++++++++++" << m_dstFrameSize.width << "+++++" << m_dstFrameSize.height << "+++++" << mCapture.get(cv::CAP_PROP_FOURCC) << endl;
}

void CCameraDS::initCamera(int width, int height, int camID) {
    logi("CCameraDS::initCamera\n");
    status = STATUS_NO_CAMERA;
    try {
#ifdef OpencvDShow
        if (!mCapture.isOpened())
        {
#ifdef _WIN32
            bool bOK = mCapture.open(cv::CAP_DSHOW + camID);
#else
            bool bOK = mCapture.open(cv::CAP_AVFOUNDATION + camID);
#endif
            rs_width = width;
            rs_height = height;

            if (!mCapture.isOpened())
                throw std::runtime_error("Unable to open video source");

            mCapture.set(cv::CAP_PROP_AUTOFOCUS, 0);

            m_capture_camera_id = camID;
            m_capture_type = CAPTURE_CAMERA;

            calculateRect();
        }
        connectCamera();
#endif
#ifndef OpencvDShow
        const AVCodec* pCodec;

        avformat_network_init();
        avdevice_register_all();
 

        //参数设置 设置的参数要看摄像头是否支持 如果不支持设置后是无法打开的 没有特别需求默认打开即可
        //比如切换分辨率的时候 就需要设置对应的framerate 否则打开错误，可以使用amcap 工具打开摄像头查看不同分辨率对应的framerate
        //在比如专业的摄像头支持的分辨率比较高800万 这个时候要设置摄像头的format格式为mjpeg 会比较流畅
        AVDictionary* pOptions = NULL;
        //设置编码格式
        av_dict_set(&pOptions, "input_format", "mjpeg", 0);
        //设置分辨率
        av_dict_set(&pOptions, "video_size", "1280x720", 0);
        //设置帧率
        av_dict_set(&pOptions, "framerate", "30", 0);
        std::string str = "video=" + deviceList.at(camID);
        // 打开摄像头
        pFormatCtx = avformat_alloc_context();
        //AVInputFormat *ifmt = av_find_input_format("vfwcap"); // vfwcap方式打开
        //if(avformat_open_input(&pFormatCtx, 0, ifmt, &pOptions))
        const AVInputFormat* ifmt = av_find_input_format("dshow"); // dshow方式打开
        if (avformat_open_input(&pFormatCtx, str.data(), ifmt, &pOptions) != 0)
        {
            cout << "Couldn't open input stream." << endl;
            return;
        }
        if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
        {
            cout << "Couldn't find stream information." << endl;
            return;
        }
        // 查找视频流id
        for (int i = 0; i < pFormatCtx->nb_streams; i++)
        {
            if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                streamIndex = i;
                break;
            }
        }
        if (streamIndex == -1)
        {
            cout << "Couldn't find a video stream." << endl;
            return;
        }
        // 获取对应视频流编码格式
        pCodec = avcodec_find_decoder(pFormatCtx->streams[streamIndex]->codecpar->codec_id);//通过解码器编号来遍历codec_list[]数组,来找到AVCodec
        pCodecCtx = avcodec_alloc_context3(pCodec);
        avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[streamIndex]->codecpar); //初始化AVCodecContext

        if (pCodec == NULL)
        {
            cout << "Codec not found." << endl;
            return;
        }
        if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
        {
            cout << "Could not open codec." << endl;
            return;
        }
        m_capture_camera_id = camID;
        m_capture_type = CAPTURE_CAMERA;

        pFrame = av_frame_alloc();
        pFrameRGB = av_frame_alloc();

        // 创建图片缓存
        int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, 1);
        rgbBuffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
        av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, rgbBuffer, AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, 1);

        // 创建包缓存
        pPacket = (AVPacket*)av_malloc(sizeof(AVPacket));

        // 创建图片转换格式结构体RGB32
        img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
        m_bThreadEnd = false;
        status = STATUS_INIT;
        frame_id = 0;
        m_isCameraInited = true;
        m_hThread = CreateThread(NULL, 0, TL_FRAME_FFMPEG, this, 0, NULL);
#endif
    }
    catch (...) {
        loge("camera init error\n");
    }
}
void CCameraDS::clearLastFrame() {

#ifndef PERFORMANCE_OPEN
    Locker locker(&m_mtxFrame);
#endif

    cv::Mat blackMat(frame.rows, frame.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    frame = blackMat;

}
/// 设置默认的待机画面
void CCameraDS::setDefaultFrame() {

    LoadDefIFNone();

}
void CCameraDS::closeCamera() {

    status = STATUS_NO_CAMERA;
    m_filepath = "";

    m_bThreadEnd = true;

#ifdef _WIN32
    if (m_hThread)
    {
        WaitForSingleObject(m_hThread, INFINITE);
        CloseHandle(m_hThread);
        m_hThread = nullptr;
    }


#else

    if (m_pid)
    {
        pthread_join(m_pid, NULL);
        m_pid = 0;
    }

#endif

}

void CCameraDS::connectCamera()
{
    if (!mCapture.isOpened()) {
        loge("can not open camera!!!\n");
        status = STATUS_NO_CAMERA;
    }
    if (mCapture.isOpened())
    {
        m_FPS = 60;
        if (m_capture_type == CAPTURE_FILE)
        {

            m_FPS = mCapture.get(cv::CAP_PROP_FPS);
            frameCount = mCapture.get(cv::CAP_PROP_FRAME_COUNT);;
        }

        m_bThreadEnd = false;

        {
#ifdef _WIN32
            m_hThread = CreateThread(NULL, 0, TL_FRAME, this, 0, NULL);
#else
            pthread_create(&m_pid, NULL, TL_FRAME, (void*)this);
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
void CCameraDS::restartCameraWhenClosed()
{
    if (!mCapture.isOpened()) {
        closeCamera();
        initCamera(rs_width, rs_height, 0);
    }
}

void CCameraDS::play() {
    if (!m_isCameraInited) {
        initCamera(rs_width, rs_height, 0);
    }

    if (status == STATUS_PLAYING)return;
    if (status == STATUS_STOP || status == STATUS_INIT || status == STATUS_PAUSE) {

        status = STATUS_PLAYING;
        return;
    }
}

void CCameraDS::stop() {
    if (status == STATUS_PLAYING) {
        status = STATUS_STOP;

    }
}

void CCameraDS::pause() {
    if (status == STATUS_PLAYING) {
        status = STATUS_PAUSE;
    }
}

void CCameraDS::LoadDefIFNone()
{
    //性能测试，不加锁切换视频容易崩
#ifndef PERFORMANCE_OPEN
    Locker locker(&m_mtxFrame);
#endif

    if (frame.rows == 0 && frame.cols == 0)
    {
#if _WIN32
        string defaultPicPath = "../../res/bg_no_camera_detected.png";
#elif __APPLE__
        string defaultPicPath = FuToolMac::GetFileFullPathFromResPicBundle("bg_no_camera_detected.png");
#endif
        static cv::Mat defaulfFrame = cv::imread(defaultPicPath, cv::IMREAD_COLOR);
        if (!defaulfFrame.empty())
        {
            cv::resize(defaulfFrame, defaulfFrame, cv::Size(rs_width, rs_height));
            frame = defaulfFrame;
        }
    }
}

cv::Mat CCameraDS::getFrame() {

    LoadDefIFNone();

    //性能测试，不加锁切换视频容易崩
#ifdef PERFORMANCE_OPEN

    cv::Mat frameCopy = frame;

#else
    //cv::Mat分深浅拷贝，clone是深，直接赋值是浅，这里主要多线程没办法
    Locker locker(&m_mtxFrame);
    cv::Mat frameCopy = frame.clone();
#endif
    return frameCopy;
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


HRESULT CCameraDS::CamCaps(IBaseFilter* pBaseFilter)
{
    HRESULT hr = 0;
    std::vector<IPin*> pins;
    IEnumPins* EnumPins;
    pBaseFilter->EnumPins(&EnumPins);
    pins.clear();
    for (;;)
    {
        IPin* pin;
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

        IEnumMediaTypes* emt = NULL;
        pins[i]->EnumMediaTypes(&emt);

        AM_MEDIA_TYPE* pmt;

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
                VIDEOINFOHEADER* pVIH = (VIDEOINFOHEADER*)pmt->pbFormat;
                SIZE s;
                // Get frame size
                s.cy = pVIH->bmiHeader.biHeight;
                s.cx = pVIH->bmiHeader.biWidth;

                unsigned int bitrate = pVIH->dwBitRate;
                modes.push_back(s);
                // Bits per pixel
                unsigned int bitcount = pVIH->bmiHeader.biBitCount;
                REFERENCE_TIME t = pVIH->AvgTimePerFrame; // blocks (100ns) per frame
                int FPS = floor(10000000.0 / static_cast<double>(t));
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
void CCameraDS::process_filter(IBaseFilter* pBaseFilter)
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
    //printf("Enumeraring videoinput devices ...\n");

    // Create the System Device Enumerator.
    ICreateDevEnum* pSysDevEnum = NULL;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
        IID_ICreateDevEnum, (void**)&pSysDevEnum);
    if (FAILED(hr))
    {
        fprintf(stderr, "Error. Can't create enumerator.\n");
        return hr;
    }

    // Obtain a class enumerator for the video input device category.
    IEnumMoniker* pEnumCat = NULL;
    hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);

    if (hr == S_OK)
    {
        // Enumerate the monikers.
        IMoniker* pMoniker = NULL;
        ULONG cFetched;
        while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
        {
            IPropertyBag* pPropBag;
            hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
                (void**)&pPropBag);
            if (SUCCEEDED(hr))
            {
                // To create an instance of the filter, do the following:
                IBaseFilter* pFilter;
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
                    char* m_char = new char[len + 1];
                    WideCharToMultiByte(CP_ACP, 0, varName.bstrVal, wcslen(varName.bstrVal), m_char, len, NULL, NULL);
                    m_char[len] = '\0';
#ifndef OpencvDShow
                    str = GbkToUtf8(m_char);
#else
                    str = m_char;
#endif
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


#define DEF_CAMERA_WIDTH (1280)
#define DEF_CAMERA_HEIGHT (720)


CCameraManage::CCameraManage()
{
    if (mCapture == nullptr) {
#ifdef _WIN32
        mCapture = tr1::shared_ptr<CCameraDS>(new CCameraDS);
#else
        mCapture = shared_ptr<CCameraDS>(new CCameraDS);
#endif
    }
}

CCameraManage::~CCameraManage()
{
}

CCameraManage* CCameraManage::mInstance = nullptr;

CCameraManage* CCameraManage::getInstance()
{
    if (mInstance == nullptr)
    {
        mInstance = new CCameraManage();
    }
    return mInstance;
}

vector<string> CCameraManage::CameraList()
{
    return mCapture->getDeviceNameList();
}
cv::Mat CCameraManage::GetOriginFrame()
{
    return mCapture->getFrame();
}

cv::Mat CCameraManage::GetFrame()
{
    cv::Mat frameMat = mCapture->getFrame();
    cv::Mat tmpMat;
    if (!UIBridge::showGreenScreen) {
        tmpMat = frameMat;
    }
    else {
        auto srcType = mCapture->getCaptureType();
        bool bNeedFlip = srcType != GS_INPUT_TYPE_FILE;

        if (bNeedFlip) {
            cv::flip(frameMat, tmpMat, 1);
        }
        else {   // 本地视频文件
            switch (UIBridge::m_localVideoRotation) {
            case 0:
                tmpMat = frameMat;
                break;
            case 90:
                cv::rotate(frameMat, tmpMat, cv::ROTATE_90_CLOCKWISE);
                break;
            case 180:
                cv::rotate(frameMat, tmpMat, cv::ROTATE_180);
                break;
            case 270:
                cv::rotate(frameMat, tmpMat, cv::ROTATE_90_COUNTERCLOCKWISE);
                break;

            default:
                tmpMat = frameMat;
                break;
            }
        }
    }

    return tmpMat;
}


bool CCameraManage::ReOpenCamera(int camID)
{
    if (mCapture->isInit())
    {
        mCapture->closeCamera();
        mCapture->initCamera(DEF_CAMERA_WIDTH, DEF_CAMERA_HEIGHT, camID);
        mFrameWidth = mCapture->m_dstFrameSize.width;
        mFrameHeight = mCapture->m_dstFrameSize.height;
        fuOnCameraChange();
    }
    return true;
}
bool CCameraManage::restartCameraWhenClosed()
{
    mCapture->restartCameraWhenClosed();
    return true;
}

bool CCameraManage::ReOpenCamera(string strVideoPath)
{
    if (mCapture->isInit())
    {
        mCapture->closeCamera();
        mCapture->InitCameraFile(mCapture->rs_width, mCapture->rs_height, strVideoPath);
        mFrameWidth = mCapture->m_dstFrameSize.width;
        mFrameHeight = mCapture->m_dstFrameSize.height;
        fuOnCameraChange();
    }
    return true;
}

void CCameraManage::CloseCurCamera()
{
    if (mCapture->isInit())
    {
        mCapture->closeCamera();
    }
}

void CCameraManage::OpenCamera(int iCamID)
{
    int CamID = mCapture->getCaptureCameraID();
    auto curCaptureType = mCapture->getCaptureType();

    if ((CamID == iCamID && !mCapture->isInit()) || CamID != iCamID || curCaptureType != CAPTURE_CAMERA)
    {
        mCapture->closeCamera();
        mCapture->initCamera(DEF_CAMERA_WIDTH, DEF_CAMERA_HEIGHT, iCamID);
        mFrameWidth = mCapture->m_dstFrameSize.width;
        mFrameHeight = mCapture->m_dstFrameSize.height;
        fuOnCameraChange();
    }
}

void CCameraManage::OpenCamera(string strVideoPath)
{
    auto strPath = mCapture->getFilePath();
    auto curCaptureType = mCapture->getCaptureType();

    if ((strPath == strVideoPath && !mCapture->isInit()) || strVideoPath != strPath || curCaptureType != CAPTURE_FILE)
    {
        mCapture->closeCamera();
        mCapture->InitCameraFile(DEF_CAMERA_WIDTH, DEF_CAMERA_HEIGHT, strVideoPath);
        mFrameWidth = mCapture->m_dstFrameSize.width;
        mFrameHeight = mCapture->m_dstFrameSize.height;
        fuOnCameraChange();
    }
}

bool CCameraManage::IsCameraPlaying()
{
    return mCapture->isPlaying();
}

bool CCameraManage::IsCameraInit()
{
    return mCapture->isInit();
}

int CCameraManage::GetCameraCaptureType()
{
    return mCapture->getCaptureType();
}
cv::Size CCameraManage::getCameraDstResolution()
{
    return mCapture->getCameraDstResolution();
}
int CCameraManage::GetCameraID()
{
    return mCapture->getCaptureCameraID();
}

/// 手动设置最后一帧为默认画面
void CCameraManage::setDefaultFrame()
{
    mCapture->setDefaultFrame();
}