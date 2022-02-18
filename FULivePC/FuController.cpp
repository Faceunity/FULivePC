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
	fuSetLogLevel(FULOGLEVEL::FU_LOG_LEVEL_INFO);
	std::vector<char> vecCtrlBundle, vecCfgBundle;
	bool flag = FuTool::LoadBundle(strControllerPath, vecCtrlBundle);
	bool flagCfg = FuTool::LoadBundle(strConfigPath, vecCfgBundle);
	if (flag && flagCfg)
	{
		m_nCtrlHandle = fuCreateItemFromPackage(&vecCtrlBundle[0], vecCtrlBundle.size());
		m_nCtrlCfgHandle = fuCreateItemFromPackage(&vecCfgBundle[0], vecCfgBundle.size());
		m_vecRender.clear();
		m_vecRender.push_back(m_nCtrlHandle);

		mSceneHandle = fuCreateScene();
		mInstanceHandle = fuCreateInstance(mSceneHandle);

		fuSetCurrentScene(mSceneHandle);
		fuSetInstanceHumanProcessorType(mInstanceHandle, 0);
		fuSetInstanceEnableHumanAnimDriver(mInstanceHandle, true);

		fuEnableHumanProcessor(mSceneHandle, 1.0);
		fuBindItemsToScene(mSceneHandle, &m_nCtrlCfgHandle, 1);
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
int FuController::BindBundle(std::string strBundlePath)
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
		fuBindItemsToInstance(mInstanceHandle, &nHandle, 1);
		m_vecBundles.emplace(strBundlePath, nHandle);
		return nHandle;
	}
	return -1;
}

void FuController::UnBindBundle(std::string strBundlePath)
{
	auto itr = m_vecBundles.find(strBundlePath);
	if (itr != m_vecBundles.cend()) {
		auto nHandle = itr->second;
		fuUnbindItemsFromInstance(mInstanceHandle, &nHandle, 1);
		fuDestroyItem(nHandle);
		m_vecBundles.erase(itr);
	}
}

void FuController::SetPos(float x, float y, float z)
{
	fuSetInstanceTargetPosition(mInstanceHandle, x, y, z);
}

void FuController::SetScale(float s)
{
	fuSetInstanceScaleDelta(mInstanceHandle, s);
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

int FuController::GetHumanStatus()
{
	return fuItemGetParamd(m_nCtrlHandle, "human_status");
}

void FuController::EnableFaceProcessor(int enable)
{
	fuEnableFaceProcessor(mSceneHandle, enable);
}

void FuController::EnableHumanFollowMode(int enable)
{
	fuSetInstanceRiggingRetargeterAvatarFollowMode(mSceneHandle, FUAIHUMAN_FOLLOW_MODE_ALIGN);
}

void FuController::SetAvatar3DScene(BodyTrackType scene)
{
	fuHumanProcessorSet3DScene(mSceneHandle, (int)scene);
}

void FuController::SetAvatarGlobalOffset(float offset_x, float offset_y, float offset_z)
{
	fuSetInstanceRiggingRetargeterAvatarFixModeTransScale(mSceneHandle, offset_x, offset_y, offset_z);
}

void FuController::SetAvatarScale(float scale)
{
	//fuHumanProcessorSetAvatarScale(scale);
}

void FuController::UseRetargetRootScale(int enable, float scale)
{
	//fuHumanProcessorSetAvatarUseRetargetRootScale(enable, scale);
}

void FuController::SetAvatarAnimFilterParams(int n_buffer_frames, float pos_w, float angle_w)
{
	fuHumanProcessorSetAvatarAnimFilterParams(n_buffer_frames, pos_w, angle_w);
}

void FuController::SetAvatarTranslationScale(float scale_x, float scale_y, float scale_z)
{
	fuSetInstanceRiggingRetargeterAvatarFixModeTransScale(mSceneHandle, scale_x, scale_y, scale_z);
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
