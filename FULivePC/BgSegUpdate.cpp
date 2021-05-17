#include "BgSegUpdate.h"
#include "Camera.h"
#include "GuiTool.h"

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
void BgSegUpdate::Update(NamaExampleNameSpace::Nama * nama)
{
	if (nama)
	{
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