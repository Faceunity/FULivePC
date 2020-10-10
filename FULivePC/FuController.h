#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

class FuController
{
public:
	FuController();
	~FuController();

	void InitController(std::string strControllerPath, std::string strConfigPath);
	void InitFXAA(std::string flipPath);

	int BindBundleToController(std::string strBundlePath);
	void UnBindBundle(int nHandle);

	int GetParam(std::string strKey);

	void SetParam(std::string strKey, double value);

	void SetParams(std::string strKey, double* pValue, int nCount);

	unsigned int RenderBundlesTexture(cv::Mat& bgraFrame, int & frameId);

	unsigned int RenderBundlesBuffer(uint8_t* out_buffer, cv::Mat& bgraFrame, int & frameId);

	//body track
	void CheckHalfBody(bool bHalfBody);
	void OpenBodyTracking(bool bOpen);
	int GetHumanStatus();

	//face capture
	void CreateFaceCapture(std::string strPath);
	void OpenFaceCapture(bool bOpen);
	void OpenBlinkTrack(bool bOpen);
	void RunFaceCapture(cv::Mat& bgraFrame);
	int GetFaceCaptureNum();
	
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
	std::vector<int> m_vecBundles;
	void* m_pMocapTrackModel = nullptr;
	void* m_pFaceCaptureModel = nullptr;

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

