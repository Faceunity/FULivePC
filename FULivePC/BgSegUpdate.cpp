#include "BgSegUpdate.h"
#include "Camera.h"
#include "GuiTool.h"
#include "UIBridge.h"

using namespace gui_tool;

extern float scaleRatioW;
extern float scaleRatioH;

BgSegUpdate::BgSegUpdate()
{
}

BgSegUpdate::~BgSegUpdate()
{
	if (m_pFrameInput)
	{
		m_pFrameInput->closeCamera();
		delete m_pFrameInput;
	}
}

void BgSegUpdate::SetSource(std::string strURL)
{
	if (nullptr == m_pFrameInput)
	{
		m_pFrameInput = new CCameraDS();
	}

	if (strURL == "NONE") {

		m_pFrameInput->closeCamera();
		m_pFrameInput->clearLastFrame();
		m_bHasSetedBgForOnceChange = false;

	}
	else {
		auto strPath = m_pFrameInput->getFilePath();

		if ((strPath == strURL && !m_pFrameInput->isInit()) || strURL != strPath)
		{
			m_pFrameInput->closeCamera();
			m_pFrameInput->InitCameraFile(DEF_FRAME_SHOW_WIDTH, DEF_FRAME_SHOW_HEIGHT, strURL);
			m_bHasSetedBgForOnceChange = false;
		}
	}
}

bool BgSegUpdate::SaveCurFrame2File(std::string strPath)
{
	bool bRet = false;
	if (m_pFrameInput && m_pFrameInput->isInit())
	{
		m_pFrameInput->play();

#ifdef _WIN32
		Sleep(100);
#else
        sleep(1);
#endif

		auto frameMat = m_pFrameInput->getFrame();
		if (frameMat.data)
		{
			bRet = cv::imwrite(strPath, frameMat);
		}

		m_pFrameInput->pause();
	}

	return bRet;
}

void BgSegUpdate::Update(NamaExampleNameSpace::Nama * nama)
{
	auto FunCheckIsNeedPlaying = []() {

		return UIBridge::mCurRenderItemName == BGSEG_CUSTOM_NAME;

	};

	if (nama && m_pFrameInput)
	{
		if (FunCheckIsNeedPlaying())
		{
			m_pFrameInput->play();
		}
		else
		{
			m_pFrameInput->pause();
			return;
		}
		///*单帧图片就别刷了，减少帧率损耗*/
		//if (m_pFrameInput->frameCount == 1 && m_bHasSetedBgForOnceChange)
		//{
		//	return;
		//}

		/* 背景视频解码后面换N卡硬解码，否则CPU占用过高 */
		auto frameMat = m_pFrameInput->getFrame();
		if (frameMat.data)
		{
			cv::Mat dataBG = frameMat.clone();

			cv::cvtColor(frameMat, dataBG, cv::COLOR_BGR2RGBA);

			nama->UpdateSegBg(dataBG);

			m_bHasSetedBgForOnceChange = true;
		}
	}
}

cv::Size BgSegUpdate::GetDstResolution()
{
	cv::Size ret;
	if (m_pFrameInput)
	{
		ret = m_pFrameInput->getCameraDstResolution();
	}

	return ret;
}
