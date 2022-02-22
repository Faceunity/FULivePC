#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

#include "Config.h"

class FuController
{
public:
    FuController();
    ~FuController();

    bool LoadBundle(const std::string &filepath, std::vector<char> &data);
    void InitController(std::string strControllerPath, std::string strConfigPath);
    void InitFXAA(std::string flipPath);

    int BindBundle(std::string strBundlePath);
    void UnBindBundle(std::string strBundlePath);

    void SetPos(float x, float y, float z);
    void SetScale(float s);

    int GetParam(std::string strKey);

    void SetParam(std::string strKey, double value);

    void SetParams(std::string strKey, double* pValue, int nCount);

    unsigned int RenderBundlesTexture(cv::Mat& bgraFrame, int & frameId);

    unsigned int RenderBundlesBuffer(uint8_t* out_buffer, cv::Mat& bgraFrame, int & frameId);

    //body track
    int GetHumanStatus();
    void EnableFaceProcessor(int enable);
    void EnableHumanFollowMode(int enable);
    void SetAvatar3DScene(BodyTrackType scene);
    void SetAvatarGlobalOffset(float offset_x, float offset_y, float offset_z);
    void SetAvatarScale(float scale);
    //true,1 | false = 完全跟踪人的位置， true,0 = 完全按照模型尺寸Retarget
    void UseRetargetRootScale(int enable, float scale);
    void SetAvatarAnimFilterParams(int n_buffer_frames, float pos_w, float angle_w);
    void SetAvatarTranslationScale(float scale_x, float scale_y, float scale_z);

    //face capture
    void OpenFaceCapture(bool bOpen);
    void OpenBlinkTrack(bool bOpen);

    void CreateTexForItemchar(const char* path);

    void setip_port(double *ip, int port);

    int PlayOnceClap();
    int PlayOnceCountDown();
    int PlayOnceHeart();
    int PlayOnceThumbs();
    int PlayOnceZhuanshen();
    int PlayOnceDance();
    bool IsPlayAnim();
    double GetAnimPregress(int nHandle);
private:
    int m_nCtrlHandle = -1;
    int m_nCtrlCfgHandle = -1;
    int m_fxaaBundle = -1;
    std::map<std::string, int> m_vecBundles;
    void* m_pMocapTrackModel = nullptr;

    int mSceneHandle = -1;
    int mInstanceHandle = -1;

    std::vector<int> m_vecRender;

    enum EnAnimIndex
    {
        EnClap,
        EnCountDown,
        EnHeart,
        EnThumbs
    };
    std::vector<int> m_vecAnimBundles;
    int m_nCurAnimHandle = -1;

    int m_nZhuanshenHandle = -1;
    int m_nDanceHandle = -1;

    double dTransitionProgress = 0;
};

