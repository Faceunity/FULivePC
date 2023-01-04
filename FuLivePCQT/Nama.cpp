#include <io.h>
#include "Nama.h"
#include <CNamaSDK.h>			//nama SDK
#include <authpack.h>			//nama SDK
#include <vector>
#include "Config.h"
#include "iostream"
#include <VirtualCamera.h>
#include <Windows.h>
#include <gl\GL.h>

PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),
    1u,
    PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW,
    PFD_TYPE_RGBA,
    32u,
    0u, 0u, 0u, 0u, 0u, 0u,
    8u,
    0u,
    0u,
    0u, 0u, 0u, 0u,
    24u,
    8u,
    0u,
    PFD_MAIN_PLANE,
    0u,
    0u, 0u };

void Nama::InitOpenGL()
{
    HWND hw = CreateWindowExA(
                0, "EDIT", "", ES_READONLY,
                0, 0, 1, 1,
                NULL, NULL,
                GetModuleHandleA(NULL), NULL);
    HDC hgldc = GetDC(hw);
    int spf = ChoosePixelFormat(hgldc, &pfd);
    int ret = SetPixelFormat(hgldc, spf, &pfd);
    HGLRC hglrc = wglCreateContext(hgldc);
    wglMakeCurrent(hgldc, hglrc);
    printf("hw=%08x hgldc=%08x spf=%d ret=%d hglrc=%08x\n",
           hw, hgldc, spf, ret, hglrc);
}

bool Nama::CheckGLContext()
{
    int add0, add1, add2, add3;
    add0 = (int)wglGetProcAddress("glGenFramebuffersARB");
    add1 = (int)wglGetProcAddress("glGenFramebuffersOES");
    add2 = (int)wglGetProcAddress("glGenFramebuffersEXT");
    add3 = (int)wglGetProcAddress("glGenFramebuffers");
    const GLubyte* OpenGLVersion = glGetString(GL_VERSION);
    printf("gl ver test (%s:%d):version:%s  %08x %08x %08x %08x\n", __FILE__, __LINE__, OpenGLVersion, add0, add1, add2, add3);
    return add0 | add1 | add2 | add3;
}

Nama::Nama()
{
}

Nama::~Nama()
{
    fuDestroyAllItems();
    fuOnDeviceLost();
    fuDestroyLibData();
    m_mp3->Cleanup();
}

bool Nama::LoadBundle(const string &filepath, vector<char> &data)
{
    ifstream fin(filepath, ios::binary);
    if (false == fin.good())
    {
        fin.close();
        return false;
    }
    streampos oldPos = fin.tellg();
    fin.seekg(0, ios::beg);
    streampos beg = fin.tellg();
    fin.seekg(0, ios::end);
    streampos end = fin.tellg();
    fin.seekg(oldPos, ios::beg);
    size_t size = static_cast<size_t>(end - beg);
    if (0 == size)
    {
        fin.close();
        return false;
    }
    data.resize(size);
    fin.read(reinterpret_cast<char*>(&data[0]), size);

    fin.close();
    return true;
}

int Nama::extractIntFromPath(string path)
{
    string filename = path;
    // Remove directory if present.
    // Do this before extension removal incase directory has a period character.
    const size_t last_slash_idx = filename.find_last_of("_");
    if (string::npos != last_slash_idx)
    {
        filename.erase(0, last_slash_idx + 1);
    }
    // Remove extension if present.
    const size_t period_idx = filename.rfind('.');
    if (string::npos != period_idx)
    {
        filename.erase(period_idx);
    }
    int index = atoi(filename.c_str());
    return index;
}

void Nama::InitNama()
{
    fuSetLogLevel(FU_LOG_LEVEL_DEBUG);
    //初始化系统环境，加载系统数据，并进行网络鉴权。必须在调用SDK其他接口前执行，否则会引发崩溃。
    //*authdata [in]*： 内存指针，指向鉴权数据的内容。如果是用包含 authpack.h 的方法在编译时提供鉴权数据，则这里可以写为 ```g_auth_package``` 。
    //*sz_authdata [in]*：鉴权数据的长度，以字节为单位。如果鉴权数据提供的是 authpack.h 中的 ```g_auth_package```，这里可写作 ```sizeof(g_auth_package)```
    //返回非0值代表成功，返回0代表失败。如初始化失败，可以通过 ```fuGetSystemError``` 获取错误代码。
    fuSetup(nullptr, 0, nullptr, g_auth_package, sizeof(g_auth_package));
    //setup without license, only render 1000 frames.
    //fuSetup(nullptr, 0, nullptr, nullptr, 0);

    printf("Nama version:%s \n", fuGetVersion());
    //舌头模型
    vector<char> tongue_model_data;

    if (false == LoadBundle(g_assetDir + g_tongue, tongue_model_data))
    {
        cout << "Error: fail load tongue model" << g_tongue << endl;
    }

    fuLoadTongueModel(reinterpret_cast<float*>(&tongue_model_data[0]), tongue_model_data.size());
    //加载AI能力
    vector<char> ai_model_data;
    if (false == LoadBundle(g_assetDir + g_ai_faceprocessor, ai_model_data))
    {
        cout << "Error:fail load faceprocessor model" << g_ai_faceprocessor << endl;
    }
    fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_model_data[0]), ai_model_data.size(), FUAITYPE::FUAITYPE_FACEPROCESSOR);

    vector<char> ai_beseg_green_model_data;
    if (false == LoadBundle(g_assetDir + g_ai_bgseg_green, ai_beseg_green_model_data))
    {
        cout << "Error: fail load bgseg green model" << g_ai_bgseg_green << endl;
    }
    fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_beseg_green_model_data[0]), ai_beseg_green_model_data.size(), FUAITYPE::FUAITYPE_BACKGROUNDSEGMENTATION_GREEN);

    vector<char> ai_hairseg_model_data;
    if (false == LoadBundle(g_assetDir + g_ai_hairseg, ai_hairseg_model_data))
    {
        cout << "Error: fail load hair seg model" << g_ai_hairseg << endl;
    }
    fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_hairseg_model_data[0]), ai_hairseg_model_data.size(), FUAITYPE::FUAITYPE_HAIRSEGMENTATION);


    vector<char> ai_human_model_data;
    if (false == LoadBundle(g_assetDir + g_ai_humanprocessor, ai_human_model_data))
    {
        cout << "Error: fail load humanprocessor model" << g_ai_humanprocessor << endl;
    }
    fuPreprocessAIModelFromPackage(reinterpret_cast<float*>(&ai_human_model_data[0]), ai_human_model_data.size(), FUAITYPE::FUAITYPE_HUMAN_PROCESSOR);
    fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_human_model_data[0]), ai_human_model_data.size(), FUAITYPE::FUAITYPE_HUMAN_PROCESSOR);

    vector<char> ai_gesture_model_data;
    if (false == LoadBundle(g_assetDir + g_ai_hand_processor, ai_gesture_model_data))
    {
        cout << "Error: fail load gesture model" << g_ai_hand_processor << endl;
    }
    fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_gesture_model_data[0]), ai_gesture_model_data.size(), FUAITYPE::FUAITYPE_HANDGESTURE);

    m_ModuleCode = fuGetModuleCode(0);
    m_ModuleCode1 = fuGetModuleCode(1);
    {
        vector<char> propData;
        if (false == LoadBundle(g_assetDir + g_faceBeautification, propData))
        {
            cout << "load face beautification data failed." << endl;
        }
        cout << "load face beautification data." << endl;
        //加载道具包，使其可以在主运行接口中被执行。一个道具包可能是一个功能模块或者多个功能模块的集合，加载道具包可以在流水线中激活对应的功能模块，在同一套SDK调用逻辑中实现即插即用。
        m_BeautyHandles = fuCreateItemFromPackage(&propData[0], propData.size());
    }

    {
        vector<char> propData;
        if (false == LoadBundle(g_assetDir + g_Makeup, propData))
        {
            cout << "load face makeup data failed." << endl;
        }
        cout << "load face makeup data." << endl;

        m_MakeUpHandle = fuCreateItemFromPackage(&propData[0], propData.size());
        //fuItemSetParamd(mMakeUpHandle, "is_clear_makeup", 1);
    }

    {
        vector<char> propData;
        if (false == LoadBundle(g_assetDir + g_bodySlim, propData))
        {
            cout << "load body slim data failed." << endl;
        }
        cout << "load body slim data." << endl;

        m_BodyShapeHandle = fuCreateItemFromPackage(&propData[0], propData.size());
    }

    {
        vector<char> propData;
        if (false == LoadBundle(g_assetDir + g_greenscreen, propData))
        {
            cout << "load gs data failed." << endl;
        }
        cout << "load gs data." << endl;

        m_GSHandle = fuCreateItemFromPackage(&propData[0], propData.size());
    }

    {
        vector<char> propData;
        if (false == LoadBundle(g_assetDir + g_fxaa, propData))
        {
            cout << "load fx data failed." << endl;
        }
        cout << "load fx data." << endl;

        m_FxaaHandles = fuCreateItemFromPackage(&propData[0], propData.size());
    }

    fuItemSetParamd(m_BodyShapeHandle,"Debug",0.0);

    fuItemSetParamd(m_MakeUpHandle, "machine_level", 1.0);
    float fValue = 0.5f;
    fuSetFaceTrackParam((void*)"mouth_expression_more_flexible", &fValue);
    fuSetMaxFaces(4);

    InitController();

    //设置输入相机矩阵
    fuSetInputCameraBufferMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
    fuSetInputCameraTextureMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);

    //设置输出矩阵
    //    fuSetOutputMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
    m_EnableNama = true;
    changeRenderList(RENDER_AR);
}

void Nama::changeRenderList(RenderType type,bool makeupFlag)
{
    m_renderType = type;
    m_renderList.clear();
    if(type == RENDER_PICKCOLOR){
        return;
    }
    if(type == RENDER_BODY){
        m_renderList.push_back(m_BodyShapeHandle);
        m_renderList.push_back(m_BeautyHandles);
    }else{
        m_renderList.push_back(m_BeautyHandles);
        //开启ar功能
        if(type == RENDER_AR){
            if (m_bundleCategory == BundleCategory::Makeup && makeupFlag)
            {
                fuSetMaxFaces(4);
                m_renderList.push_back(m_MakeUpHandle);
            }
            if(m_bundleCurrent != 0){
                m_renderList.push_back(m_bundleCurrent);
            }
        }
        //开启绿幕功能
        else{
            //不在取色状态
            if(type == RENDER_GREEN){
                fuSetMaxFaces(4);
                m_renderList.push_back(m_GSHandle);
            }
        }
    }
    m_renderList.push_back(m_FxaaHandles);
}

void Nama::RenderDefNama()
{
    if(m_bundleCurrent !=0 && b_start_mp3){
        fuItemSetParamd(m_bundleCurrent, "music_time", m_mp3->GetCurrentPosition() / 1e4);
        m_mp3->CirculationPlayCheck();
    }
#ifdef SynchronizingCamera
    if(m_getNewFrame){
#endif
        //当前fuRender图像长与前一个不相同时调用fuHumanProcessorReset重置人体算法模块,解决分辨率切换崩溃
        if(m_FrameWidth != m_frame.cols){
            m_FrameWidth = m_frame.cols;
            fuHumanProcessorReset();
            if(m_bIsCreateVirturalCamera){
                changeResolutionOfVirturalCamera(m_frame.cols, m_frame.rows);
            }
        }
        //打开虚拟摄像头
        if(m_bVirturalCamera){
            int height;
            if(m_frame.channels() == 4){
                height = m_frame.rows;
            }else if(m_frame.channels() == 1){
                height = m_frame.rows * 2 / 3;
            }
            cv::Mat result(height, m_frame.cols, CV_8UC4);
            if(m_frame.channels() == 4){
                m_texID = fuRender(FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(result.data), FU_FORMAT_BGRA_BUFFER, reinterpret_cast<int*>(m_frame.data),
                                   m_frame.cols, m_frame.rows, m_FrameID++, m_renderList.data(),
                                   m_renderList.size(), NAMA_RENDER_FEATURE_FULL, NULL);
            }else if(m_frame.channels() == 1){
                m_texID = fuRender(FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(result.data), FU_FORMAT_I420_BUFFER, reinterpret_cast<int*>(m_frame.data),
                                   m_frame.cols, m_frame.rows * 2 / 3, m_FrameID++, m_renderList.data(),
                                   m_renderList.size(), NAMA_RENDER_FEATURE_FULL, NULL);
            }
            if (!m_bIsCreateVirturalCamera)
            {
                createVirturalCamera(0);
                m_bIsCreateVirturalCamera = true;
            }
            pushDataToVirturalCamera(result.data, result.cols, result.rows);
        }else{
            //这里直接处理成纹理,opengl直接调用m_texID纹理id绘制,更快
            //FU_FORMAT_BGRA_BUFFER  FU_FORMAT_NV21_BUFFER FU_FORMAT_I420_BUFFER FU_FORMAT_RGBA_BUFFER
            if(m_frame.channels() == 4){
                fuRender(FU_FORMAT_RGBA_TEXTURE, &m_texID, FU_FORMAT_BGRA_BUFFER, reinterpret_cast<int*>(m_frame.data),
                         m_frame.cols, m_frame.rows, m_FrameID++, m_renderList.data(),
                         m_renderList.size(), NAMA_RENDER_FEATURE_FULL, NULL);
            }else if(m_frame.channels() == 1){
                fuRender(FU_FORMAT_RGBA_TEXTURE, &m_texID, FU_FORMAT_I420_BUFFER, reinterpret_cast<int*>(m_frame.data),
                         m_frame.cols, m_frame.rows * 2 / 3, m_FrameID++, m_renderList.data(),
                         m_renderList.size(), NAMA_RENDER_FEATURE_FULL, NULL);
            }
        }
#ifdef SynchronizingCamera
    }
    if(m_renderType != RENDER_GREEN){
        m_getNewFrame = false;
    }
#endif
    if (fuGetSystemError())
    {
        printf("%s \n", fuGetSystemErrorString(fuGetSystemError()));
    }
}

bool Nama::itemSetParamd(int handles, string name, double value)
{
    if(handles == 0) return false;
    int res = fuItemSetParamd(handles, name.data(), value);
    return (res != 0);
}

bool Nama::itemSetParams(int handles, string name, string value)
{
    if(handles == 0) return false;
    int res = fuItemSetParams(handles, name.data(), value.data());
    return (res != 0);
}

void Nama::ReloadItems()
{
    UnLoadAvatar();
    fuDestroyAllItems();
    fuOnDeviceLost();
    m_bundleCurrent = 0;
    m_curMakeupItem = 0;
    m_MakeUpHandle = 0;
    fuSetInputCameraBufferMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
    fuSetInputCameraTextureMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
    {
        vector<char> propData;
        if (false == LoadBundle(g_assetDir + g_faceBeautification, propData))
        {
            cout << "load face beautification data failed." << endl;
        }
        cout << "load face beautification data." << endl;
        //加载道具包，使其可以在主运行接口中被执行。一个道具包可能是一个功能模块或者多个功能模块的集合，加载道具包可以在流水线中激活对应的功能模块，在同一套SDK调用逻辑中实现即插即用。
        m_BeautyHandles = fuCreateItemFromPackage(&propData[0], propData.size());
    }

    {
        vector<char> propData;
        if (false == LoadBundle(g_assetDir + g_bodySlim, propData))
        {
            cout << "load body slim data failed." << endl;
        }
        cout << "load body slim data." << endl;

        m_BodyShapeHandle = fuCreateItemFromPackage(&propData[0], propData.size());
    }

    {
        vector<char> propData;
        if (false == LoadBundle(g_assetDir + g_greenscreen, propData))
        {
            cout << "load gs data failed." << endl;
        }
        cout << "load gs data." << endl;

        m_GSHandle = fuCreateItemFromPackage(&propData[0], propData.size());
    }

    {
        vector<char> propData;
        if (false == LoadBundle(g_assetDir + g_fxaa, propData))
        {
            cout << "load fx data failed." << endl;
        }
        cout << "load fx data." << endl;

        m_FxaaHandles = fuCreateItemFromPackage(&propData[0], propData.size());
    }

    fuItemSetParamd(m_BodyShapeHandle,"Debug",0.0);
    float fValue = 0.5f;
    fuSetFaceTrackParam((void*)"mouth_expression_more_flexible", &fValue);

    fuSetMaxFaces(4);

    m_Controller->InitController(g_assetDir + g_control,g_assetDir + g_control_cfg);
    m_Controller->InitFXAA(g_assetDir + g_fxaa);
}

void Nama::LoadMakeup()
{
    if(m_MakeUpHandle == 0){
        vector<char> propData;
        if (false == LoadBundle(g_assetDir + g_Makeup, propData))
        {
            cout << "load face makeup data failed." << endl;
        }
        cout << "load face makeup data." << endl;
        m_MakeUpHandle = fuCreateItemFromPackage(&propData[0], propData.size());
        fuItemSetParamd(m_MakeUpHandle, "machine_level", 1.0);
    }
}

void Nama::UnLoadMakeup()
{
    if(m_MakeUpHandle != 0){
        fuDestroyItem(m_MakeUpHandle);
        m_MakeUpHandle = 0;
    }
}

void Nama::getPresentFrame(const cv::Mat &frame)
{
    std::unique_lock<std::mutex> lock(m_frameMutex);
    m_frame = frame;
    m_getNewFrame = true;
}

void Nama::ClearAllCM()
{
    ChangeCleanFlag(true);
    //清楚咬唇设定
    fuItemSetParamd(m_MakeUpHandle,"is_two_color", 0);
    if (m_MakeUpHandle > 0)
    {
        for (auto it = m_mapMakeUpItem.begin(); it != m_mapMakeUpItem.end(); it++)
        {
            fuUnbindItems(m_MakeUpHandle, &it->second, 1);
            fuDestroyItem(it->second);
        }
    }
    m_mapMakeUpItem.clear();
}


void Nama::DestroyAll()
{
    ClearAllCM();
    if(m_curMakeupItem != 0){
        fuUnbindItems(m_MakeUpHandle, &m_curMakeupItem, 1);
        m_curMakeupItem = 0;
    }
    if(m_bundleCurrent != 0){
        fuDestroyItem(m_bundleCurrent);
        m_bundleCurrent = 0;
    }
}

void Nama::ChangeCleanFlag(bool bOpen)
{
    if (m_MakeUpHandle)
    {
        double v = bOpen ? 1.0 : 0.0;
        //在解绑妆容时是否清空除口红以外的妆容，0表示不清空，1表示清空，口红可由强度进行设置
        fuItemSetParamd(m_MakeUpHandle, "is_clear_makeup", v);
    }
}

bool Nama::SelectBundle(string bundleName, int person, bool bindFlag)
{
    if (false == m_EnableNama)
    {
        return false;
    }

    DestroyAll();

    if ((m_bundleCategory == BundleCategory::Makeup && !bindFlag) ||
            m_bundleCategory == BundleCategory::ItemJingpin){
        UnLoadMakeup();
    }
    //绑定一个临时道具
    vector<char> propData;
    if (false == LoadBundle(bundleName, propData))
    {
        cout << bundleName << "  load prop data failed." << endl;
        return false;
    }
    cout << "load prop data." << endl;
    m_bundleCurrent = fuCreateItemFromPackage(&propData[0], propData.size());

    //手势要设置角度
    if (m_bundleCategory == BundleCategory::GestureRecognition) {
        fuItemSetParamd(m_bundleCurrent, "rotMode", 0);
        if (bundleName.find("ctrl_flower") != std::string::npos) {
            fuItemSetParamd(m_bundleCurrent, "particleDirMode", 0);
        }
    }
    //美发要根据bundle结尾编号设置
    if (m_bundleCategory == BundleCategory::BeautyHair) {
        int index = extractIntFromPath(bundleName) - 1;
        fuItemSetParamd(m_bundleCurrent, "Index", index);
    }
    //自定义美妆要绑定道具
    if (m_bundleCategory == BundleCategory::Makeup)
    {
        if(bindFlag){
            LoadMakeup();
            m_curMakeupItem = m_bundleCurrent;
            fuBindItems(m_MakeUpHandle, &m_bundleCurrent, 1);
        }else{
            fuItemSetParamd(m_bundleCurrent, "machine_level", 1.0);
        }
    }
    //风格推荐和组合妆开启全脸分割
    if (m_bundleCategory == BundleCategory::StyleRecommendation)
    {
        fuItemSetParamd(m_bundleCurrent, "machine_level", 1.0);
    }
    //如果前面有打开过音乐滤镜,先关闭音乐
    b_start_mp3 = false;
    if(m_mp3 != nullptr){
        long long current = 0;
        m_mp3->SetPositions(&current, NULL, true);
        m_mp3->Stop();
        m_mp3 = nullptr;
    }
    //音乐滤镜要打开音乐
    if (m_bundleCategory == BundleCategory::MusicFilter)
    {
        string itemName = bundleName.substr(0, bundleName.find_last_of("."));
        m_mp3 = new Mp3;
        m_mp3->Load(itemName + ".mp3");
        m_mp3->Play();
        b_start_mp3 = true;
    }
    //旧版本统一设定参数,不设置道具会镜像
    fuItemSetParamd(m_bundleCurrent, "is3DFlipH", 1);
    fuItemSetParamd(m_bundleCurrent, "isFlipTrack", 1);
    fuItemSetParamd(m_bundleCurrent, "rotationMode", 2);

    //能接受最多人脸数改变 Animoji BackgroundSegmentation的前三个 BigHead 为1 其他为 4
    if (m_bundleCategory == Animoji)
    {
        m_maxFace = 1;
    }else if (m_bundleCategory == BackgroundSegmentation)
    {
        vector<string> humanSegmentations = {"boyfriend1","boyfriend2","boyfriend3"};
        bool isHumanSegmentation = false;
        std::vector<string>::iterator itr = humanSegmentations.begin();
        while (itr != humanSegmentations.end())
        {
            if (bundleName.find(*itr) != std::string::npos)
            {
                isHumanSegmentation = true;
                break;
            }
            itr++;
        }
        //如果是自定义背景分割
        if(bundleName.find("bg_segment") != std::string::npos){
            //bg_align_type决定如何对齐背景，1 拉伸平铺 2 居中非拉伸裁切显示
            fuItemSetParamd(m_bundleCurrent, "bg_align_type", 1);
        }
        if (isHumanSegmentation)
            m_maxFace = 1;
        else
            m_maxFace = 4;
    }else if (m_bundleCategory == BigHead)
    {
        m_maxFace = 1;
    }else if (m_bundleCategory == ItemJingpin)
    {
        m_maxFace = person;
    }else{
        m_maxFace = 4;
    }
    //Animoji需要跟踪头部,其他不需要
    if (m_bundleCategory == Animoji)
    {
        fuItemSetParamd(m_bundleCurrent, "{\"thing\":\"<global>\",\"param\":\"follow\"} ", 1);
    }else
    {
        fuItemSetParamd(m_bundleCurrent, "{\"thing\":\"<global>\",\"param\":\"follow\"} ", 0);
    }
    fuSetMaxFaces(m_maxFace);

    return true;
}

void Nama::RenderP2A()
{
    //avator人物晃动厉害,把传入帧跟摄像头输入帧率相同
    if(m_getNewFrame){
        if(m_bVirturalCamera){
            int height;
            if(m_frame.channels() == 4){
                height = m_frame.rows;
            }else if(m_frame.channels() == 1){
                height = m_frame.rows * 2 / 3;
            }
            cv::Mat result(height, m_frame.cols, CV_8UC4);
            if (!m_bIsCreateVirturalCamera)
            {
                createVirturalCamera(0);
                m_bIsCreateVirturalCamera = true;
            }
            m_Controller->RenderBundlesBuffer(result.data, m_frame, m_FrameID);
            pushDataToVirturalCamera(result.data, result.cols, result.rows);
        }else{
            m_texID = m_Controller->RenderBundlesTexture(m_frame, m_FrameID);
        }
        m_getNewFrame = false;
    }
}

void Nama::RenderBear()
{
    if(m_FrameWidth != m_frame.cols){
        m_FrameWidth = m_frame.cols;
        fuHumanProcessorReset();
    }
    if(m_getNewFrame){
        int height;
        if(m_frame.channels() == 4){
            height = m_frame.rows;
        }else if(m_frame.channels() == 1){
            height = m_frame.rows * 2 / 3;
        }
        cv::Mat result(height, m_frame.cols, CV_8UC4);
        fuSetInputCameraBufferMatrix(TRANSFORM_MATRIX::CCROT0);
        fuSetInputCameraTextureMatrix(TRANSFORM_MATRIX::CCROT0);
        vector<int> renderList;
        renderList.push_back(m_BeautyHandles);
        if(m_frame.channels() == 4){
            fuRender(FU_FORMAT_BGRA_BUFFER, reinterpret_cast<int*>(result.data), FU_FORMAT_BGRA_BUFFER, reinterpret_cast<int*>(m_frame.data),
                     m_frame.cols, m_frame.rows, m_FrameID, m_renderList.data(),
                     m_renderList.size(), NAMA_RENDER_FEATURE_FULL, NULL);
        }else if(m_frame.channels() == 1){
            fuRender(FU_FORMAT_BGRA_BUFFER, reinterpret_cast<int*>(result.data), FU_FORMAT_I420_BUFFER, reinterpret_cast<int*>(m_frame.data),
                     m_frame.cols, m_frame.rows * 2 / 3, m_FrameID, m_renderList.data(),
                     m_renderList.size(), NAMA_RENDER_FEATURE_FULL, NULL);
        }
        fuSetInputCameraBufferMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
        fuSetInputCameraTextureMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
        if(m_bVirturalCamera){
            if (!m_bIsCreateVirturalCamera)
            {
                createVirturalCamera(0);
                m_bIsCreateVirturalCamera = true;
            }
            m_Controller->RenderBundlesBuffer(result.data, m_frame, m_FrameID);
            pushDataToVirturalCamera(result.data, result.cols, result.rows);
        }else{
            m_texID = m_Controller->RenderBundlesTexture(result, m_FrameID);
        }
        m_getNewFrame = false;
    }
}

bool Nama::InitController()
{
    if (m_Controller != nullptr)
        return false;
    m_Controller = std::make_shared<FuController>();
    m_Controller->InitController(g_assetDir + g_control,g_assetDir + g_control_cfg);
    m_Controller->InitFXAA(g_assetDir + g_fxaa);
    return true;
}


bool Nama::LoadAvatarHandTrackBundle()
{
    vector<string> files;

    //Bind Animation For Hand
    intptr_t   hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    string p;
    string path = g_assetDir + g_pose_track_folder;
    p.assign(path).append("/*.").append("bundle");
    auto charLast = path.at(path.length() - 1);
    if (charLast != '/')
    {
        path += '/';
    }
    if ((hFile = _findfirst(p.c_str(), &fileinfo)) != -1)
    {
        do
        {
            //加入列表
            files.push_back(path+fileinfo.name);

        } while (_findnext(hFile, &fileinfo) == 0);

        _findclose(hFile);
    }

    for (auto & file : files)
    {
        m_Controller->BindBundle(file);
        avatarBundles.emplace_back(file);
    }
    return true;
}

void Nama::LoadAvatarBundles(const std::vector<std::string>& bundleNames)
{
    if (m_Controller == nullptr)
        return;
    for (auto& i : bundleNames) {
        m_Controller->BindBundle(i);
        avatarBundles.emplace_back(i);
    }
}

void Nama::UnLoadAvatar()
{
    if (m_Controller == nullptr)
        return;
    for (auto& i : avatarBundles) {
        m_Controller->UnBindBundle(i);
    }
    avatarBundles.clear();
}

void Nama::SetBodyTrackType(BodyTrackType type)
{
    switch (type)
    {
    case BodyTrackType::None:
        break;
    case BodyTrackType::HalfBody:
        m_Controller->SetAvatar3DScene(BodyTrackType::HalfBody);
        break;
    case BodyTrackType::FullBody:
        m_Controller->SetAvatar3DScene(BodyTrackType::FullBody);
        break;
    default:
        break;
    }
}

void Nama::SetGSKeyColor(cv::Vec4b data)
{
    if (m_GSHandle == -1)
    {
        return;
    }
    double key_color[] = { double(data[0]), double(data[1]), double(data[2]) };
    //printf("key color find:%d,%d,%d \r\n", (int)data[0], (int)data[1], (int)data[2]);
    fuItemSetParamdv(m_GSHandle, "key_color", key_color, 3);
}

void Nama::UpdateGreenScreenSegment(const cv::Mat & dataRGBA)
{
    if (m_GSHandle == -1)
    {
        return;
    }
    fuCreateTexForItem(m_GSHandle, "tex_bg", dataRGBA.data, dataRGBA.cols, dataRGBA.rows);

}

void Nama::UpdateBackgroundSegment(const cv::Mat &dataRGBA)
{
    fuCreateTexForItem(m_bundleCurrent, "tex_bg_seg", dataRGBA.data, dataRGBA.cols, dataRGBA.rows);
}


void Nama::UpdateGreenScreenSafeArea(const cv::Mat &dataRGBA)
{
    fuItemSetParamd(m_GSHandle, "is_use_template", 1);
    fuCreateTexForItem(m_GSHandle, "tex_template", dataRGBA.data, dataRGBA.cols, dataRGBA.rows);
}

void Nama::UpdateGreenScreenSafeArea(int texid)
{
    //使用绿幕安全区域
    fuItemSetParamd(m_GSHandle, "is_use_template", 1);
    fuItemSetParamd(m_GSHandle, "tex_template", texid);
}

void Nama::NonuseGreenScreenSafeArea()
{
    fuItemSetParamd(m_GSHandle, "is_use_template", 0);
}

void Nama::ApplyBodyTrackConfig(BodyTrackParam params)
{
    m_Controller->EnableFaceProcessor(params.EnableFaceProcessor);
    m_Controller->UseRetargetRootScale(params.UseRetargetRootScale, params.UseRetargetRootScaleValue);
    m_Controller->SetAvatarAnimFilterParams(params.AnimFilterParams_n_buffer_frames, params.AnimFilterParams_pos_w, params.AnimFilterParams_angle_w);
    m_Controller->SetAvatarGlobalOffset(params.GlobalOffset_x, params.GlobalOffset_y, params.GlobalOffset_z);
    m_Controller->SetAvatarScale(params.Scale);
    m_Controller->SetPos(params.Pos_x, params.Pos_y, params.Pos_z);
    m_Controller->SetAvatarTranslationScale(params.TrackMoveRange_x, params.TrackMoveRange_y, params.TrackMoveRange_z);
}

void Nama::changeGSPreviewRect(double startX, double startY, double endX, double endY)
{
    fuItemSetParamd(m_GSHandle, "start_x", startX);
    fuItemSetParamd(m_GSHandle, "start_y", startY);
    fuItemSetParamd(m_GSHandle, "end_x", endX);
    fuItemSetParamd(m_GSHandle, "end_y", endY);
}

void Nama::setMakeUpColor(string name, std::vector<double> color0, std::vector<double> color1, std::vector<double> color2)
{
    fuItemSetParamdv(m_MakeUpHandle, name.data(), color0.data(), color0.size());
    if(color1.size() > 0){
        fuItemSetParamdv(m_MakeUpHandle, (name + "2").data(), color1.data(), color1.size());
    }
    if(color2.size() > 0){
        fuItemSetParamdv(m_MakeUpHandle, (name + "3").data(), color2.data(), color2.size());
    }
}

void Nama::appendMakeUpItems(string name, string item, int type, int form)
{
    //如果前面有绑定,先解绑前一个
    if(m_mapMakeUpItem.find(name) != m_mapMakeUpItem.end()){
        int bundleID = m_mapMakeUpItem.find(name)->second;
        fuUnbindItems(m_MakeUpHandle, &bundleID, 1);
        fuDestroyItem(bundleID);
    }
    bindMakeUp(name, item);
    //口红相关
    if(form == 0){
        //咬唇特例
        if(type == 1){
            fuItemSetParamd(m_MakeUpHandle, "lip_type", 0);
            fuItemSetParamd(m_MakeUpHandle, "is_two_color", 1);
            double zeroArray[4] = { 0.0,0.0,0.0,0.0 };
            fuItemSetParamdv(m_MakeUpHandle, "makeup_lip_color2", zeroArray, 4);
        }else{
            fuItemSetParamd(m_MakeUpHandle, "lip_type", type);
            fuItemSetParamd(m_MakeUpHandle, "is_two_color", 0);
        }
    }
}

void Nama::bindMakeUp(string name, string item)
{
    ChangeCleanFlag(false);
    int bundleID = 0;
    vector<char> propData;
    if (false == LoadBundle(g_assetDir + item, propData))
    {
        cout << "load prop data failed." << endl;
        return;
    }
    bundleID = fuCreateItemFromPackage(&propData[0], propData.size());
    fuBindItems(m_MakeUpHandle, &bundleID, 1);
    //cout<<"bindMakeUp"<<name<<item;
    m_mapMakeUpItem[name] = bundleID;
}

void Nama::itemJingpinClick()
{
    if(m_bundleCurrent > 0){
        fuItemSetParamd(m_bundleCurrent, "is_click", 1.0);
        fuItemSetParamd(m_bundleCurrent, "mouse_down", 1.0);
    }
}

void Nama::CameraChange(){
    fuOnCameraChange();
}

