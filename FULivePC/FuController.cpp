#include "FuController.h"
#include "CNamaSDK.h"
#include "CNamaSDKScene.h"
#include "opencv2/core/core_c.h"
#include "fu_tool.h"




FuController::FuController()
{
	
}

FuController::~FuController()
{

}

void FuController::InitController(std::string strControllerPath, std::string strConfigPath)
{

	std::vector<char> vecCtrlBundle, vecCfgBundle;
	bool flag = FuTool::LoadBundle(strControllerPath, vecCtrlBundle);
	bool flagCfg = FuTool::LoadBundle(strConfigPath, vecCfgBundle);
	if (flag && flagCfg)
	{
		m_nCtrlHandle = fuCreateItemFromPackage(&vecCtrlBundle[0], vecCtrlBundle.size());
		m_nCtrlCfgHandle = fuCreateItemFromPackage(&vecCfgBundle[0], vecCfgBundle.size());
		//fuBindItems(m_nCtrlHandle, &m_nCtrlCfgHandle, 1);
		m_vecRender.push_back(m_nCtrlHandle);

		mScreenHandle = fuCreateScene();
		mInstanceHandle = fuCreateInstance(mScreenHandle);

		fuEnableHumanProcessor(mScreenHandle, 1.0);

		fuBindItemsToInstance(mInstanceHandle, &m_nCtrlCfgHandle, 1);
		fuSetInstanceTargetPosition(mInstanceHandle, 0.0, 50.0, -900);

		//fuItemSetParamd(m_nCtrlHandle, "reset_all", 1.0);
	}
}

void FuController::InitFXAA(std::string flipPath)
{
    std::vector<char> fxaabundle;
    bool flagFlip = FuTool::LoadBundle(flipPath, fxaabundle);
	if (flagFlip)
	{
		m_fxaaBundle = fuCreateItemFromPackage(&fxaabundle[0], fxaabundle.size());
		m_vecRender.push_back(m_fxaaBundle);
	}
    
}

/// 向controller.bundle 上绑定道具
/// @param strBundlePath 相对路径
int FuController::BindBundleToController(std::string strBundlePath)
{
	if (m_nCtrlHandle < 0)
	{
		return -1;
	}
	std::vector<char> vecBundle;
	bool flag = FuTool::LoadBundle(strBundlePath, vecBundle);
	if (flag)
	{
		int nHandle = fuCreateItemFromPackage(&vecBundle[0], vecBundle.size());

		//fuBindItems(m_nCtrlHandle, &nHandle, 1);
		fuBindItemsToInstance(mInstanceHandle, &nHandle, 1);
		m_vecBundles.emplace_back(nHandle);

		return nHandle;
	}
	return -1;
}

void FuController::UnBindBundle(int nHandle)
{
	//fuUnbindItems(m_nCtrlHandle, &nHandle, 1);
	fuUnbindItemsFromInstance(mInstanceHandle, &nHandle, 1);
}

void FuController::SetPos(float x, float y, float z)
{
	fuSetInstanceTargetPosition(mInstanceHandle, x, y, z);
}

int FuController::GetParam(std::string strKey)
{
	return fuItemGetParamd(m_nCtrlHandle, strKey.c_str());
}

void FuController::SetParam(std::string strKey, double value)
{
	fuItemSetParamd(m_nCtrlHandle, const_cast<char*>(strKey.c_str()), value);
}

void FuController::SetParams(std::string strKey, double* pValue, int nCount)
{
	fuItemSetParamdv(m_nCtrlHandle, const_cast<char*>(strKey.c_str()), pValue, nCount);
}

unsigned int FuController::RenderBundlesTexture(cv::Mat& bgraFrame, int & frameId)
{
	unsigned int glNamaRetTexture;
	fuRenderBundles(FU_FORMAT_RGBA_TEXTURE, (void*)(&glNamaRetTexture), FU_FORMAT_RGBA_BUFFER, (void*)(bgraFrame.data),
		bgraFrame.cols, bgraFrame.rows, frameId++, m_vecRender.data(), m_vecRender.size());
	return glNamaRetTexture;
}

unsigned int FuController::RenderBundlesBuffer(uint8_t* out_buffer, cv::Mat& bgraFrame, int & frameId)
{
	return fuRenderBundles(FU_FORMAT_RGBA_BUFFER, (void*)(out_buffer), FU_FORMAT_RGBA_BUFFER, (void*)(bgraFrame.data),
		bgraFrame.cols, bgraFrame.rows, frameId++, m_vecRender.data(), m_vecRender.size());
}


void FuController::OpenBodyTracking(bool bOpen)
{
	fuEnableHumanFollowMode(mScreenHandle, bOpen ? 1:0);
}

void FuController::CheckHalfBody(bool bHalfBody)
{
	fuHumanProcessorSet3DScene(mScreenHandle, bHalfBody ? 0:1);
}

int FuController::GetHumanStatus()
{
	return fuItemGetParamd(m_nCtrlHandle, "human_status");
}

void FuController::OpenFaceCapture(bool bOpen)
{
	fuItemSetParamd(m_nCtrlHandle, "close_face_capture", bOpen ? 0.0 : 1.0);
}

void FuController::OpenBlinkTrack(bool bOpen)
{
	fuItemSetParamd(m_nCtrlHandle, "enable_expression_blend", bOpen ? 1.0 : 0.0);
	double blend_expression[57] = { 1 };
	if (bOpen)  
	{
		blend_expression[0] = 0;
		blend_expression[1] = 0;
	}
	fuItemSetParamdv(1, "blend_expression", blend_expression, 57);
}


void FuController::CreateTexForItemchar(const char* path)
{
	cv::Mat pic = cv::imread(path, -1);
	int n = pic.type();
	cv::cvtColor(pic, pic, cv::COLOR_BGRA2RGBA);
	fuCreateTexForItem(m_nCtrlHandle, "picture_in_frame", pic.data,
		pic.cols, pic.rows);
}

void FuController::setip_port(double *ip, int port)
{
	fuItemSetParamdv(m_nCtrlHandle, "set_ip", ip, 4);
	SetParam("set_port", port);
	SetParam("set_human_processor_type", 2.0);
}

int FuController::PlayOnceClap()
{
	//SetParam("enable_collide_ik", 0);
	SetParam("play_animation_once", m_vecAnimBundles[EnClap]);
	return m_nCurAnimHandle = m_vecAnimBundles[EnClap];
}

int FuController::PlayOnceCountDown()
{
	//SetParam("enable_collide_ik", 0);
	SetParam("play_animation_once", m_vecAnimBundles[EnCountDown]);
	return m_nCurAnimHandle = m_vecAnimBundles[EnCountDown];
}

int FuController::PlayOnceHeart()
{
	//SetParam("enable_collide_ik", 0);
	SetParam("play_animation_once", m_vecAnimBundles[EnHeart]);
	return m_nCurAnimHandle = m_vecAnimBundles[EnHeart];
}

int FuController::PlayOnceThumbs()
{
	//SetParam("enable_collide_ik", 0);
	SetParam("play_animation_once", m_vecAnimBundles[EnThumbs]);
	return m_nCurAnimHandle = m_vecAnimBundles[EnThumbs];
}

int FuController::PlayOnceZhuanshen()
{
	SetParam("stop_animation", 1);
	SetParam("play_animation_once", m_nZhuanshenHandle);
	return m_nCurAnimHandle = m_nZhuanshenHandle;
}

int FuController::PlayOnceDance()
{
	SetParam("play_animation_once", m_nDanceHandle);
	return m_nCurAnimHandle = m_nDanceHandle;
}

bool FuController::IsPlayAnim()
{
	double progress = GetAnimPregress(m_nCurAnimHandle);
	return progress >= 1.0 ? false : true;
}

double FuController::GetAnimPregress(int nHandle)
{
	char arrKey[124] = { 0 };
	sprintf(arrKey, "{\"name\":\"get_animation_transition_progress\", \"anim_id\":%d}", nHandle);
	dTransitionProgress = fuItemGetParamd(m_nCtrlHandle, arrKey);
	//std::cout << "Play anim transition progress: " << dTransitionProgress << std::endl;
	if (dTransitionProgress >= 0.99 || dTransitionProgress < 0.0)
	{
		char arrKey[124] = { 0 };
		sprintf(arrKey, "{\"name\":\"get_animation_progress\", \"anim_id\":%d}", nHandle);
		double progress = fuItemGetParamd(m_nCtrlHandle, arrKey);
		return progress;
	}

	return 0.0;
}
