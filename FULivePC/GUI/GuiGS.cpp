#if _WIN32
#include "Windows.h"
#elif __APPLE__
#include "fu_tool_mac.h"
#endif
#include "fu_tool.h"
#include "GuiGS.h"
#include "UIBridge.h"
#include "tinfiledlg/tinyfiledialogs.h"
#include "Camera.h"
#include "GuiTabContent.h"
#include "imgui/imgui_internal.h"
#include "Loader.h"
#include <iostream>
#include <string>
#include <regex>
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

using namespace gui_tool;
using namespace NamaExampleNameSpace;

//有空改下
extern float scaleRatioW;
extern float scaleRatioH;
extern bool showUI;

bool UIBridge::m_bSetGSInputSrc = false;
GreenScreenState UIBridge::m_gsState;
bool UIBridge::m_bNeedShowGSCamChoice = false;
uint32_t UIBridge::m_id4ChoiceCam = -1;
bool UIBridge::m_bNeedReChooseInputSrc = false;
float UIBridge::mGSParam[MAX_GREEN_SCREEN_PARAM] = { 0.0f };
vector<StyleRecommendationParam> UIBridge::mStyleParamList;
bool UIBridge::m_bSamplingColor = false;
bool UIBridge::m_bShowColorChoice = false;
bool UIBridge::m_bShowingBodyBeauty = false;
bool UIBridge::m_bShowingSafeAreaTip = false;
bool UIBridge::m_bLoadWrongNamePNGFile = false;
int UIBridge::m_localVideoRotation = 0;
bool UIBridge::m_localVideoType = false;
float UIBridge::m_localVideoWidth = 0;
bool UIBridge::m_bResetPreviewRect = false;
float UIBridge::m_localVideoHeight = 0;
string UIBridge::m_openLocalFileTip = "";
string UIBridge::mCurRenderGSItemName = "";
string UIBridge::mCurRenderGSSAItemName = "NONE";
int UIBridge::gsBundleCategory = BUNDLE_CATEGORY_NOMEAN;

int UIBridge::m_localBgSegVideoRotation = 0;
// 用于区别是图片类型还是视频类型
bool UIBridge::m_localBgSegVideoType = 0;
// 记录本地视频的分辨率
float UIBridge::m_localBgSegVideoWidth = 0;
float UIBridge::m_localBgSegVideoHeight = 0;

CCameraDS * GUIGS::m_pCamBG = new CCameraDS();
bool GUIGS::m_bHasSetedBgForOnceChange = false;
vector<ColorBag> GUIGS::m_vecColorTex;
vector<ColorBag> GUIGS::m_vecColorRecentTex;
vector<cv::Vec4b> m_vecColorRecent;
ColorBag GUIGS::m_curColor = ColorBag();
Texture::SharedPtr GUIGS::m_hsvTexture = nullptr;
cv::Mat GUIGS::m_hsvMat;
FURect GUIGS::previewRect = {DEF_FRAME_SHOW_POS_X+DEF_FRAME_SHOW_WIDTH/2.0,DEF_FRAME_SHOW_POS_Y + DEF_FRAME_SHOW_HEIGHT/2.0,DEF_FRAME_SHOW_WIDTH/2.0,DEF_FRAME_SHOW_HEIGHT/2.0};
FURect GUIGS::titleRect = {60 * scaleRatioW  ,0, 1300 * scaleRatioW ,60 * scaleRatioH };
bool GUIGS::mIsSelectGSSafeAreaFileOK = false;
bool GUIGS::mIsUserConfigOK = false;
GSSafeAreaUserConfig GUIGS::mConfig;
/////////////////////////////////////////////////////////////////

static string GetCongfigPath() {


	string ConfPath = "";

#ifdef __APPLE__

	ConfPath = FuToolMac::GetDocumentPath() + "/" + gUserConfig;


#else

	ConfPath = gUserConfig;

#endif

	return ConfPath;
}


GUIGS::GUIGS()
{
}

GUIGS::~GUIGS()
{
	m_pCamBG->closeCamera();
}

static bool openDeafultCamera(Nama * nama)
{
	int iLen = CCameraManage::getInstance()->CameraList().size();
	
	if (iLen == 0)
	{
		return false;
	}
	UIBridge::m_localVideoWidth = DEF_FRAME_SHOW_WIDTH/2.0;
	UIBridge::m_localVideoHeight = DEF_FRAME_SHOW_HEIGHT/2.0;
	UIBridge::m_bResetPreviewRect = true;
	int index = iLen > UIBridge::mSelectedCamera ? UIBridge::mSelectedCamera : 0;
	UIBridge::mSelectedCamera = index;
	CCameraManage::getInstance()->ReOpenCamera(index);
}

static bool IsCamDeviceOpen(Nama * nama)
{
	bool bCamDeviceOpen = CCameraManage::getInstance()->IsCameraInit() && (CCameraManage::getInstance()->GetCameraCaptureType() == GS_INPUT_TYPE_CAMERA);
	return bCamDeviceOpen;
}

static void LayoutShowCamChoice(Nama * nama)
{
	vector<string> CameraList = CCameraManage::getInstance()->CameraList();
	if (CameraList.size())
	{
		if (!IsCamDeviceOpen(nama))
		{
			openDeafultCamera(nama);
		}

		ImGui::PushItemWidth(380 * scaleRatioW);
		if (ImGui::BeginCombo("##slect camera2", UIBridge::mCurCameraName.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < CameraList.size(); n++)
			{
				bool is_selected = (UIBridge::mCurCameraName == CameraList[n]);
				if (ImGui::Selectable(CameraList[n].c_str(), is_selected))
				{
					UIBridge::mCurCameraName = CameraList[n];
					bool bCamDeviceOpen = IsCamDeviceOpen(nama);

					if (UIBridge::mSelectedCamera != n || !bCamDeviceOpen)
					{
						UIBridge::mSelectedCamera = n;
						CCameraManage::getInstance()->ReOpenCamera(n);
					}
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
	}
	else if (showUI)
	{
		ImGui::PushItemWidth(380 * scaleRatioW);
		if (ImGui::BeginCombo("##slect camera4", u8"未检测到摄像头")) // The second parameter is the label previewed before opening the combo.				
			ImGui::EndCombo();
		ImGui::PopItemWidth();
	}
}

void GUIGS::ShowCameraChoiceForGS(Nama * nama)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8);
	ImGui::SetNextWindowPos(ImVec2(230 * scaleRatioW, 200 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(476 * scaleRatioW, 400 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("WindowGSI_CHOICE_CAM##4", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(247.f / 255.f, 248.f / 255.f, 250.f / 255.f, 1.f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 8));
	ImGui::BeginTabBar("##tabcm1", ImGuiTabBarFlags_NoAnim | ImGuiTabBarFlags_SizingPolicyEqual | ImGuiTabBarFlags_NoSelectionOnAppearing);
	ImGui::TabItem(ImVec2(476 * scaleRatioW, 36 * scaleRatioH), u8"摄像头设置");
	ImGui::EndTabBar();
	ImGui::PopStyleVar();
	static bool setedChangeGSInputSrc = false;
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 30 * scaleRatioH);
	ImGui::Dummy(ImVec2(440 * scaleRatioW, 1));
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(247.f / 255.f, 248.f / 255.f, 250.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(247.f / 255.f, 248.f / 255.f, 250.f / 255.f, 1.f));
	if (ImGui::Button(u8"×"))
	{
		UIBridge::m_bNeedShowGSCamChoice = false;
		UIBridge::m_gsState.inputType = GS_INPUT_TYPE_NONE;
		setedChangeGSInputSrc = false;
	}
	ImGui::PopStyleColor(2);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(47.f / 255.f, 54.f / 255.f, 92.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(240.f / 255.f, 244.f / 255.f, 255.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(240.f / 255.f, 244.f / 255.f, 255.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(240.f / 255.f, 244.f / 255.f, 255.f / 255.f, 1.f));

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);

	float frameWidth = (float)DEF_FRAME_SHOW_WIDTH / 2.0f;
	float frameHeight = (float)DEF_FRAME_SHOW_HEIGHT / 2.0f;
	bool bSuccessShow = false;
	ImGui::NewLine();

	if (CCameraManage::getInstance()->GetCameraCaptureType() == GS_INPUT_TYPE_CAMERA && CCameraManage::getInstance()->IsCameraInit())
	{
		ImGui::Dummy(ImVec2(7 * scaleRatioW, 1));
		ImGui::SameLine();

		//ImGui::SameLine(0, 7 * scaleRatioW);
		cv::Mat frameMat = CCameraManage::getInstance()->GetOriginFrame();
		if (frameMat.data)
		{
			cv::Mat processedFrame = frameMat.clone();
			cv::cvtColor(frameMat, processedFrame, cv::COLOR_BGR2RGBA);
			UpdateFrame2Tex(processedFrame, UIBridge::m_id4ChoiceCam);

			funShowImg(UIBridge::m_id4ChoiceCam, ((float)UIBridge::mResolutionWidth) / UIBridge::mResolutionHeight, ImVec2(frameWidth, frameHeight), true);

			bSuccessShow = true;
		}
	}
	if (!bSuccessShow)
	{
		ImGui::SameLine(0 , 7 * scaleRatioW);
		LayoutImage(ImVec2(0, 0), ImVec2(frameWidth, frameHeight), Texture::createTextureFromFile("bg_no_camera_detected.png", false)->getTextureID(), u8"");
	}

	ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));
	ImGui::Dummy(ImVec2(6 * scaleRatioW, 1));
	ImGui::SameLine();

	ImGui::Text(u8"选择设备:");
	ImGui::SameLine();
	if (!IsCamDeviceOpen(nama)) {
		if (UIBridge::m_bSetGSInputSrc && UIBridge::m_gsState.inputType != GS_INPUT_TYPE_CAMERA)
		{
			setedChangeGSInputSrc = true;
		}
	}

	LayoutShowCamChoice(nama);

	ImGui::PopStyleColor(12);

	ImGui::BeginGroup();

	ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));
	ImGui::Dummy(ImVec2(325 * scaleRatioW, 1));

	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
	if (ImGui::Button(u8"   取消   "))
	{
		UIBridge::m_bNeedShowGSCamChoice = false;
		UIBridge::m_gsState.inputType = GS_INPUT_TYPE_NONE;
		setedChangeGSInputSrc = false;
	}
	ImGui::PopStyleColor(6);

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(1.0f, 1.0f, 1.0f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.f));

	ImGui::SameLine();
	if (ImGui::Button(u8"   确定   "))
	{
		UIBridge::m_gsState.inputType = GS_INPUT_TYPE_CAMERA;
		UIBridge::m_gsState.iCmaeraID = CCameraManage::getInstance()->GetCameraID();
		
		
		
		UIBridge::m_bNeedShowGSCamChoice = false;
		UIBridge::m_bNeedReChooseInputSrc = false;
		openDeafultCamera(nama);
		if (UIBridge::m_bSetGSInputSrc == false){
			// 选择默认绿幕背景视频，默认为数组第一个
			vector<string> vecFiles;
#if _WIN32	
			UIBridge::FindAllCommonPIC(gGSBgPic, vecFiles);
#elif __APPLE__
			UIBridge::FindAllCommonPICNameFromResourceBundle(gGSBgPic, vecFiles);
#endif
			if (vecFiles.size() > 0) {
				string firstStr = *(vecFiles.begin());
				inputGreenScreenBg(firstStr);
			}
		}
		
		UIBridge::m_bSetGSInputSrc = true;
		setedChangeGSInputSrc = false;
	}
	ImGui::PopStyleVar(3);
	ImGui::PopStyleColor(5);

	ImGui::EndGroup();

	ImGui::PopStyleVar(3);


	ImGui::End();
}



/// 显示选择输入源的弹窗，如果整个app生命周期第一次弹出，
/// 是没有取消按钮的，必须选择一个输入源，弹窗才能消失，
/// 此时canCancel传false，在已经存在输入源时，
/// 弹窗是可以取消的，这时传true,将会显示"取消重选"按钮
/// @param nama 图形渲染对象
/// @param canCancel 是否可以"取消重选"弹窗，
///true将会显示取消按钮，false不显示"取消重选"按钮
void GUIGS::ShowGSInputChoice(Nama * nama,bool canCancel)
{
	ImGui::SetNextWindowPos(ImVec2(270 * scaleRatioW, 250 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(356 * scaleRatioW, 315 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("WindowGSI##3", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	if (canCancel) {
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.f, 1.f, 1.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 1.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 1.f, 1.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(1.f, 1.f, 1.f, 1.f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
		if (LayoutButton(ImVec2(270, 0), ImVec2(40, 40), u8"取  消"))
		{
			UIBridge::m_bNeedReChooseInputSrc = false;
		}
		ImGui::PopStyleVar(1);
		ImGui::PopStyleColor(5);
	}
	RectLayoutDesc desc;
	desc.pos = ImVec2(122 , 20);
	desc.size = ImVec2(78 , 78 );
	if (canCancel) {
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 40);
	}
	ImVec2 textPos = ImVec2(34, 0);
	
	LayoutImage(desc, Texture::createTextureFromFile("icon_prompt.png", false)->getTextureID(), textPos,
				u8"请使用纯色背景，推荐绿色幕布效果效果最佳");
	
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(120.f / 255.f, 136.f / 255.f, 234.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.0f);
	
	if (LayoutButton(ImVec2(60 , 20 ), ImVec2(200 , 40 ), u8"选择摄像头"))
	{
		UIBridge::m_bNeedShowGSCamChoice = true;
		if (CCameraManage::getInstance()->CameraList().size() && UIBridge::m_gsState.inputType != GS_INPUT_TYPE_CAMERA) {
			openDeafultCamera(nama);
			UIBridge::m_gsState.inputType = GS_INPUT_TYPE_CAMERA;
		}
	}
	
	if (LayoutButton(ImVec2(60 , 5), ImVec2(200 , 40 ), u8"选择视频/图片"))
	{
		const int iCount = 6;
		const char* open_filename = nullptr;
        string videoPath;
		char const* filter_patterns[iCount] = { "*.mp4","*.mov","*.png","*.jpeg","*.jpg","*.gif" };  // "*.png",
#if _WIN32
		open_filename = tinyfd_openFileDialog(
											  "Load Video Or Pic",
											  "",
											  iCount,
											  filter_patterns,
											  nullptr,
											  0);
		if (open_filename)
		{
			std::set<char> delims{ '\\' };
			std::vector<std::string> paths = FuTool::splitpath(open_filename, delims);
			cout << paths.back() << endl;
			string open_filename_lastPath = paths.back(); //哈哈.png
			if (regex_match(open_filename_lastPath, regex("(.*)\.(png|PNG|gif|GIF|jpeg|JPEG)"))) {
				if (regex_match(open_filename_lastPath, regex("(.*[^0-9]+.*)\.(png|PNG|gif|GIF|jpeg|JPEG)")))
				{
					UIBridge::m_openLocalFileTip = u8"当前类型文件名称只能包含数字字符，请修改！";
					open_filename = nullptr;
					UIBridge::m_bLoadWrongNamePNGFile = true;
					UIBridge::mLastTimeExtra = ImGui::GetTime() + 8.0;
					
				}
			}
		}
        if (open_filename) videoPath = open_filename;
#elif __APPLE__
		vector<std::string> _filePaths = {};
		vector<const char*> types = { "mp4","mov","png","jpeg","jpg","gif"};
		FuToolMac::importFilesInObjectC("~/Desktop", types, &_filePaths,false);
		if (_filePaths.size() > 0) {
            videoPath = *_filePaths.begin();
		}
#endif
		{
			if (videoPath.length() > 0)
			{
				if (regex_match(videoPath.data(), regex("(.*)\\.(mp4|MP4|mov|MOV)"))) {
					UIBridge::m_localVideoType = true;
				}else{
					UIBridge::m_localVideoType = false;
				}
				if (UIBridge::m_localVideoType){
					int rotation = FuTool::getLocalVideoRotation(videoPath);
					UIBridge::m_localVideoRotation = rotation;
					
					
					float videoWidth,videoHeight, dstW, dstH;
					bool isSuccess = FuTool::getLocalVideoResolution(videoPath, &videoWidth, &videoHeight);
					
					switch (UIBridge::m_localVideoRotation) {
						case 0:
							break;
						case 90:
						{
							// 交换宽高
							float tmp = videoWidth;
							videoWidth = videoHeight;
							videoHeight = tmp;
						}
							break;
						case 180:
							break;
						case 270:
							break;
							
						default:
							break;
					}
					calculatePreViewForHalfScreen(videoWidth, videoHeight, &dstW, &dstH);
					if (isSuccess) {
						UIBridge::m_localVideoWidth = dstW;
						UIBridge::m_localVideoHeight = dstH;
					}
					else {
						UIBridge::m_localVideoWidth = DEF_FRAME_SHOW_WIDTH / 2.0;
						UIBridge::m_localVideoHeight = DEF_FRAME_SHOW_HEIGHT / 2.0;
					}
					UIBridge::m_bResetPreviewRect = true;
					
					
					if (SetCameraFileInput(nama, videoPath.c_str(), false))
					{
						//if (videoWidth == 0) {
							float imgWidth, imgHeight, dstW, dstH;
							cv::Size imgSize = CCameraManage::getInstance()->getCameraDstResolution();
							imgWidth = imgSize.width;
							imgHeight = imgSize.height;
							calculatePreViewForHalfScreen(imgWidth, imgHeight, &dstW, &dstH);
							UIBridge::m_localVideoWidth = dstW;
							UIBridge::m_localVideoHeight = dstH;
						//}
					}
				}else{
					if (SetCameraFileInput(nama, videoPath.c_str()))
					{
						UIBridge::m_localVideoRotation = 0;
						float imgWidth,imgHeight, dstW, dstH;
						cv::Size imgSize = CCameraManage::getInstance()->getCameraDstResolution();
						imgWidth = imgSize.width;
						imgHeight = imgSize.height;
						calculatePreViewForHalfScreen(imgWidth, imgHeight, &dstW, &dstH);
						UIBridge::m_localVideoWidth = dstW;
						UIBridge::m_localVideoHeight = dstH;
						UIBridge::m_bResetPreviewRect = true;
					}
				}
				UIBridge::m_bNeedReChooseInputSrc = false;
				
			}
		}
	}
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(8);
	
	ImGui::End();
	
	if (UIBridge::m_bNeedShowGSCamChoice)
	{
		ShowCameraChoiceForGS(nama);
	}
}
/// 适配至 1/2 屏幕
/// @param orgW 原始宽
/// @param orgH 原始高
/// @param dstW 返回的新宽
/// @param dstH 返回的新高
void GUIGS::calculatePreViewForHalfScreen(float orgW,float orgH,float* dstW,float* dstH){
	float ratioW = orgW / (DEF_FRAME_SHOW_WIDTH / 2.0);
	float ratioH = orgH / (DEF_FRAME_SHOW_HEIGHT / 2.0);
	if (ratioW > ratioH) {
		*dstW = (DEF_FRAME_SHOW_WIDTH / 2.0);
		*dstH = orgH / ratioW;
	}else{
		*dstH = (DEF_FRAME_SHOW_HEIGHT / 2.0);
		*dstW = orgW / ratioH;
	}
}

bool GUIGS::SetCameraFileInput(NamaExampleNameSpace::Nama * nama, const char * path, bool loop)
{
	bool bRet = false;
	string path_string{path};
	CCameraManage::getInstance()->OpenCamera(path_string, loop);
	
	if (CCameraManage::getInstance()->GetCameraCaptureType() == GS_INPUT_TYPE_FILE && CCameraManage::getInstance()->IsCameraInit())
	{
		UIBridge::m_gsState.inputType = GS_INPUT_TYPE_FILE;
		UIBridge::m_gsState.strFilePath = path_string;
		if (UIBridge::m_bSetGSInputSrc == false){
			// 选择默认绿幕背景视频，默认为数组第一个
			vector<string> vecFiles;
#if _WIN32	
			UIBridge::FindAllCommonPIC(gGSBgPic, vecFiles);
#elif __APPLE__
			UIBridge::FindAllCommonPICNameFromResourceBundle(gGSBgPic, vecFiles);
#endif
			inputGreenScreenBg(*vecFiles.begin());
		}
		UIBridge::m_bSetGSInputSrc = true;
		bRet = true;
	}
	
	return bRet;
}

void GUIGS::ShowGreenScreenMenu(Nama * nama)
{

	
	string *categoryNameArr = nullptr;
	/* 目前绿幕就一个 */
	string allCategory[] = { "list_icon_green_screen_cutout_nor", "list_icon_safe_area_nor",
		u8" 绿幕", u8"安全区域" };
	int amount = 2;
	
	categoryNameArr = allCategory;
	
	/* 为了居中 */
	int leftStart = (900 * scaleRatioW - amount * 62) / 2;
	ImGui::Dummy(ImVec2(leftStart, 1)); ImGui::SameLine();
	
	if (UIBridge::m_bNeedReChooseInputSrc) {
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.30f);
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	}
	for (int i = 0; i < amount; i++)
	{
		if (!nama->CheckModuleCode(i + GS_START)) {
			//显示灰色不可用
			string iconFileName = categoryNameArr[i].substr(0, categoryNameArr[i].find_last_of("_")) + "_dis.png";
			LayoutImageButtonWithText(ImVec2(0.f, 24.f), ImVec2(62, 62), Texture::createTextureFromFile(iconFileName, false)->getTextureID(),
				Texture::createTextureFromFile(iconFileName, false)->getTextureID(), categoryNameArr[amount + i].c_str());
		}else if (UIBridge::gsBundleCategory == (i + GS_START))
		{
			// 正在取色时，禁止点击
			if (UIBridge::m_bSamplingColor){  
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.3f);
			}
			if (LayoutImageButtonWithText(ImVec2(0.f, 27.f), ImVec2(62, 62), Texture::createTextureFromFile("list_icon_propmap_collapse.png", false)->getTextureID(),
										  Texture::createTextureFromFile("list_icon_propmap_collapse.png", false)->getTextureID(), categoryNameArr[amount + i].c_str()))
			{
				auto funSetUnSelectd = [&](int index) {
					UIBridge::gsBundleCategory = BUNDLE_CATEGORY_NOMEAN;
					UIBridge::showItemSelectWindow = false;
				};
				
				funSetUnSelectd(i);
			}
			if (UIBridge::m_bSamplingColor){ 
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}
		else
		{
			if (!UIBridge::m_bSetGSInputSrc){  
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.3f);
			}
			string iconFileName = categoryNameArr[i].substr(0, categoryNameArr[i].find_last_of("_")) + (1 ? "_hover.png" : "_nor.png");
			if (LayoutImageButtonWithText(ImVec2(0.f, 27.f), ImVec2(62, 62), Texture::createTextureFromFile(categoryNameArr[i] + ".png", false)->getTextureID(),
										  Texture::createTextureFromFile(iconFileName, false)->getTextureID(), categoryNameArr[amount + i].c_str()))
			{
				if (UIBridge::m_bSetGSInputSrc){  
					auto funSetSelectd = [&](int index) {
						UIBridge::gsBundleCategory = index + GS_START;
						UIBridge::showItemSelectWindow = true;
						Nama::mNamaAppState.EnableAvatar = false;
					};
					
					funSetSelectd(i);
					if (UIBridge::gsBundleCategory == BundleCategory::SafeArea) {
						UIBridge::mLastTime = ImGui::GetTime() + 2.0;
						UIBridge::m_bShowingSafeAreaTip = true;
						UIBridge::showItemTipsWindow = true;
					}
				}
			}
			if (!UIBridge::m_bSetGSInputSrc){  
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}
		ImGui::SameLine(0.f, 27.f * scaleRatioW);
	}
	
	if (UIBridge::m_bNeedReChooseInputSrc) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
#ifndef _WIN32

	if (UIBridge::m_bNeedReChooseInputSrc && FuToolMac::granteCameraAccess())
	{
		ShowGSInputChoice(nama, UIBridge::m_bSetGSInputSrc);
	}

#else

	if (UIBridge::m_bNeedReChooseInputSrc)
	{
		ShowGSInputChoice(nama, UIBridge::m_bSetGSInputSrc);
	}

#endif

	
	
	UpdateGreenScreenBg(nama);
	
}

void GUIGS::ShowReChooseInputSrc()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(235.f / 255.f, 235.f / 255.f, 235.f / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(235.f / 255.f, 235.f / 255.f, 235.f / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(235.f / 255.f, 235.f / 255.f, 235.f / 255.f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::SetNextWindowPos(ImVec2(760 * scaleRatioW, 172 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(120 * scaleRatioW, 40 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("ReChooseInput##23", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar /*| ImGuiWindowFlags_NoInputs*/);
	LayoutImage(ImVec2(12, 6), ImVec2(14, 14), Texture::createTextureFromFile("icon_reselect.png", false)->getTextureID(), u8"");
	ImGui::SameLine();
	if (LayoutButton(ImVec2(0, 0), ImVec2(80, 25), u8"重新选择"))
	{
		UIBridge::m_bNeedReChooseInputSrc = true;
	}
	ImGui::End();
	ImGui::PopStyleColor(5);
	ImGui::PopStyleVar(2);
}

void GUIGS::CloseCamera(Nama * nama)
{
	if (nama)
	{
		CCameraManage::getInstance()->CloseCurCamera();
	}
}

void GUIGS::CloseGreenScreenBg()
{
	m_pCamBG->closeCamera();
}

void GUIGS::ChangeGreenScreenBg(string strFilePath)
{
	
	if (strFilePath == "NONE") {
		m_pCamBG->closeCamera();
		m_pCamBG->clearLastFrame();
		m_bHasSetedBgForOnceChange = false;
		
	}else{
		auto strPath = m_pCamBG->getFilePath();
		
		if ((strPath == strFilePath && !m_pCamBG->isInit()) || strFilePath != strPath)
		{
			m_pCamBG->closeCamera();
			m_pCamBG->InitCameraFile(DEF_FRAME_SHOW_WIDTH, DEF_FRAME_SHOW_HEIGHT, strFilePath);
			m_bHasSetedBgForOnceChange = false;
		}
	}
}

void GUIGS::ChangeGreenScreenSA(string strFilePath, NamaExampleNameSpace::Nama* nama)
{
#if _WIN32
	cv::Mat mat = cv::imread(strFilePath, cv::IMREAD_REDUCED_COLOR_4);
#elif __APPLE__
	cv::Mat mat = cv::imread(strFilePath);
#endif
	cv::cvtColor(mat, mat, cv::COLOR_BGR2RGBA);
	nama->UpdateGSSA(mat);
}

static string GetUserIconPath() {

	string IconPath = "";

#ifdef __APPLE__

	IconPath = FuToolMac::GetDocumentPath() + "/" + GSSAFEAREA_USERFILE_PIC;


#else

	IconPath = GSSAFEAREA_USERFILE_PIC;

#endif

	return IconPath;

}

/// 输入绿幕背景视频
/// @param itemName 视频名称
void GUIGS::inputGreenScreenBg(string itemName){
	UIBridge::mCurRenderGSItemName = itemName;
	UIBridge::mLastTime = 0;
	UIBridge::showItemTipsWindow = false;
	string videoPath;
#if _WIN32
	videoPath = gGSBgPic + "/" + itemName;
#elif __APPLE__
	videoPath = UIBridge::GetFileFullPathFromResourceBundle((gGSBgPic + "/" + itemName + ".mp4").c_str());
#endif 
	ChangeGreenScreenBg(videoPath);
}

void GUIGS::ShowFloatMenuGS(NamaExampleNameSpace::Nama * nama)
{
	if (UIBridge::gsBundleCategory < BundleCategory::GreenScreen)
	{
		return;
	}

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 0.88f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.0f));
	ImGui::SetNextWindowPos(ImVec2(19 * scaleRatioW, 570 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(876 * scaleRatioW, 85 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("itemSelect##1564", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar);

	//如果是安全区域，增加添加和自定义安全区域图片
	if (UIBridge::gsBundleCategory == BundleCategory::SafeArea) {
		vector<string> vecFiles;
#if _WIN32	
		UIBridge::FindAllCommonPIC(gGSSAPic, vecFiles);
#elif __APPLE__
		UIBridge::FindAllCommonPICNameFromResourceBundle(gGSSAPic, vecFiles);
#endif
		int fileSize = vecFiles.size() + 1;
		if (mIsUserConfigOK){
			fileSize++;
		}
		ImGui::SameLine(0, (10 - fileSize) * 39 * scaleRatioW + 31 * scaleRatioW);
		// 正在取色时，禁止点击
		if (UIBridge::m_bSamplingColor) {
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.3f);
		}
		Texture::SharedPtr tex = Texture::createTextureFromFile(BGSEG_ADD_ICON, false);
		if (ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(),
			ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1,
			ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Reset))
		{
			SelectGSSafeAreaFile();
		}

		ImGui::SameLine(0.f, 22.f * scaleRatioW);
		if (mIsUserConfigOK) {

			string iconName = GetUserIconPath();
			tex = Texture::createTextureFromFullPath(iconName, true);
            string itemName = GSSAFEAREA_USERFILE_PIC;
			if (mIsSelectGSSafeAreaFileOK) {
				UIBridge::mCurRenderGSSAItemName = GSSAFEAREA_USERFILE_PIC;
				ChangeGreenScreenSA(iconName, nama);
				mIsSelectGSSafeAreaFileOK = false;
			}
			bool buttonClick;
			if (UIBridge::mCurRenderGSSAItemName == itemName)
			{
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Select);
			}
			else {
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Deselect);
			}
			if (buttonClick)
			{
				if (UIBridge::mCurRenderGSSAItemName != itemName)
				{
					UIBridge::mCurRenderGSSAItemName = itemName;
					UIBridge::showItemTipsWindow = false;
					ChangeGreenScreenSA(iconName, nama);
				}
				else {
					UIBridge::mCurRenderGSSAItemName = "NONE";
					UIBridge::showItemTipsWindow = false;
					nama->NonuseGSSA();
				}
			}
			ImGui::SameLine(0.f, 22.f * scaleRatioW);
		}
		{
			for (int i = 0; i < vecFiles.size(); i++)
			{
				string itemName = vecFiles[i];
				string iconName = itemName.substr(0, itemName.find_last_of('.'));
				Texture::SharedPtr tex = Texture::createTextureFromFile(iconName + ".png", false);
				bool buttonClick;
				if (UIBridge::mCurRenderGSSAItemName == itemName)
				{
					buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Select);
				}
				else {
					buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Deselect);
				}
				if (buttonClick)
				{
					if (UIBridge::mCurRenderGSSAItemName != itemName)
					{
						UIBridge::mCurRenderGSSAItemName = itemName;
						string imagePath;
#if _WIN32
						imagePath = gGSSAPic + "/" + itemName;
#elif __APPLE__
						imagePath = UIBridge::GetFileFullPathFromResourceBundle((gGSSAPic + "/" + itemName + ".jpg").c_str());
#endif 
						ChangeGreenScreenSA(imagePath, nama);
					}
					else {
						UIBridge::mCurRenderGSSAItemName = "NONE";
						UIBridge::showItemTipsWindow = false;
						nama->NonuseGSSA();
					}
				}
				ImGui::SameLine(0.f, 22.f * scaleRatioW);
				if (UIBridge::m_bSamplingColor) {
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
			}
		}
	}
	else {

		vector<string> vecFiles;
#if _WIN32	
		UIBridge::FindAllCommonPIC(gGSBgPic, vecFiles);
#elif __APPLE__
		UIBridge::FindAllCommonPICNameFromResourceBundle(gGSBgPic, vecFiles);
#endif
		{
			ImGui::SameLine(0, (10 - vecFiles.size()) * 39 * scaleRatioW + 31 * scaleRatioW);
			for (int i = 0; i < vecFiles.size(); i++)
			{
				string itemName = vecFiles[i];

				string iconName = itemName.substr(0, itemName.find_last_of('.'));
				Texture::SharedPtr tex = Texture::createTextureFromFile(iconName + ".png", false);
				//找不到图标
				if (!tex)
				{
					tex = Texture::createTextureFromFile("icon_Movebutton_nor.png", false);
				}
				// 正在取色时，禁止点击
				if (UIBridge::m_bSamplingColor) {
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.3f);
				}
				bool buttonClick;
				if (UIBridge::mCurRenderGSItemName == itemName)
				{
					buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Select);
				}
				else {
					buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Deselect);
				}

				if (buttonClick)

				{
					if (UIBridge::mCurRenderGSItemName != itemName)
					{
						UIBridge::mCurRenderGSItemName = itemName;
						UIBridge::mLastTime = 0;
						UIBridge::showItemTipsWindow = false;
						string videoPath;
#if _WIN32
						videoPath = gGSBgPic + "/" + itemName;
#elif __APPLE__
						videoPath = UIBridge::GetFileFullPathFromResourceBundle((gGSBgPic + "/" + itemName + ".mp4").c_str());
#endif 
						ChangeGreenScreenBg(videoPath);
					}
					else
					{
						UIBridge::mCurRenderGSItemName = "NONE";
						UIBridge::mLastTime = 0.0;
						UIBridge::showItemTipsWindow = false;
						ChangeGreenScreenBg(UIBridge::mCurRenderGSItemName);
					}

				}
				ImGui::SameLine(0.f, 22.f * scaleRatioW);
				if (UIBridge::m_bSamplingColor) {
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
			}
		}
	}


	ImGui::End();
	ImGui::PopStyleColor(4);
}


void GUIGS::UpdateGreenScreenBg(NamaExampleNameSpace::Nama * nama)
{
	if (nama)
	{
		/*单帧图片就别刷了，减少帧率损耗*/
		if (m_pCamBG->frameCount == 1 && m_bHasSetedBgForOnceChange)
		{
			return;
		}

		/* 背景视频解码后面换N卡硬解码，否则CPU占用过高 */
		auto frameMat = m_pCamBG->getFrame();
		if (frameMat.data)
		{
			cv::Mat dataBG = frameMat.clone();

			cv::cvtColor(frameMat, dataBG, cv::COLOR_BGR2RGBA);

			nama->UpdateGSBg(dataBG);

			m_bHasSetedBgForOnceChange = true;
		}
	}
}


void GUIGS::ResetDefParam()
{
	UIBridge::mGSParam[0] = 50;
	UIBridge::mGSParam[1] = 50;
	UIBridge::mGSParam[2] = 100;
}

void GUIGS::InitColorVec()
{
	m_hsvTexture = createColorHSV(600, 200, m_hsvMat);


	m_curColor.setColor(g_color1);

	m_vecColorTex.emplace_back(g_color1);
	m_vecColorTex.emplace_back(g_color2);
	m_vecColorTex.emplace_back(g_color3);
	for (int i = 0; i < m_vecColorRecent.size(); i++) {
		m_vecColorRecentTex.emplace_back(m_vecColorRecent[i]);
	}
}

#define COLOR_LINE_NUM (5)

void GUIGS::ShowColorChoiceWin(NamaExampleNameSpace::Nama * nama)
{
	bool bOpen = true;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(233.f / 255.f, 234.f / 255.f, 242.f / 255.f, 1.f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
	ImGui::SetNextWindowPos(ImVec2(960 * scaleRatioW, 200 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300 * scaleRatioW, 200 * scaleRatioH), ImGuiCond_Always);

    ImGui::Begin("ColorChoice", &bOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	ImGui::Dummy(ImVec2(1, 20* scaleRatioH));
	ImGui::Dummy(ImVec2(20 * scaleRatioW, 1)); ImGui::SameLine();
	ImGui::Text(u8"推荐颜色");
	ImGui::Dummy(ImVec2(1, 5 * scaleRatioH));
	ImGui::Dummy(ImVec2(20 * scaleRatioW, 1)); ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(233.f / 255.f, 234.f / 255.f, 242.f / 255.f, 1.f));
	auto funLoopColor = [&](vector<ColorBag> & vec, ImGuiID & guiID) {

		int iLoopIndex = 0;

		for (auto & colorBag : vec)
		{
			if (colorBag.pTex)
			{
				ImGui::PushID(&colorBag);
				bool buttonClick;
				if (GetCurColorCircle() == colorBag.vecColorRGBA[0])
				{
					buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIIDSec, colorBag.pTex->getTextureID(), ImVec2(20 * scaleRatioW, 20 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Select);
				}
				else {
					buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIIDSec, colorBag.pTex->getTextureID(), ImVec2(20 * scaleRatioW, 20 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Reset);
				}
				if (buttonClick)
				{
					nama->SetGSKeyColor(colorBag.vecColorRGBA[0]);
					m_curColor.setColor(colorBag.vecColorRGBA[0]);
					//绿幕设置颜色后设置绿幕参数
					vector<int> colorV;
					nama->GetGSParamd(colorV);
					UIBridge::mGSParam[0] = colorV[0];
					UIBridge::mGSParam[1] = colorV[1];
					UIBridge::mGSParam[2] = colorV[2];
				}

				ImGui::PopID();

				iLoopIndex++;

				if (iLoopIndex < COLOR_LINE_NUM)
				{
					ImGui::SameLine(0,10 * scaleRatioW);
				}
				else
				{
					iLoopIndex = 0;
				}
			}
		}
	};

	funLoopColor(m_vecColorTex, UIBridge::m_curRenderItemUIIDSec);
	ImGui::Dummy(ImVec2(1, 5 * scaleRatioH));
	ImGui::Dummy(ImVec2(20 * scaleRatioW, 1)); ImGui::SameLine();
	ImGui::Text(u8"最近使用");
	ImGui::Dummy(ImVec2(1, 5 * scaleRatioH));
	ImGui::Dummy(ImVec2(20 * scaleRatioW, 1)); ImGui::SameLine();
	funLoopColor(m_vecColorRecentTex, UIBridge::m_curRenderItemUIIDSec);

	ImGui::PopStyleColor(2);
	ImGui::End();
	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar(3);
}


void GUIGS::DrawColorPanel(NamaExampleNameSpace::Nama * nama)
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::BeginGroup();
	ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));
	ImGui::Dummy(ImVec2(23 * scaleRatioW, 1)); ImGui::SameLine();
	if (UIBridge::m_bSamplingColor) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	}
	if (ImGui::ImageButton(m_curColor.pTex->getTextureID(), ImVec2(50 * scaleRatioW, 50 * scaleRatioW))) {
		UIBridge::m_bShowColorChoice = !UIBridge::m_bShowColorChoice;
	}
	if (UIBridge::m_bSamplingColor) {
		ImGui::PopItemFlag();
	}
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 70 * scaleRatioW);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 20 * scaleRatioH);
	ImGui::Image(Texture::createTextureFromFile("iconn_colour_picker.png", false)->getTextureID(), ImVec2(14 * scaleRatioW, 14 * scaleRatioW));
	ImGui::Dummy(ImVec2(23 * scaleRatioW, 1));
	ImGui::SameLine();
	ImGui::Text(u8"  关键色");
	ImGui::SameLine();
	ImGui::EndGroup();
	
	ImGui::SameLine();
	ImGui::BeginGroup();
	ImGui::Dummy(ImVec2(1, 20 * scaleRatioH));
	ImGui::Dummy(ImVec2(20 * scaleRatioW, 1)); ImGui::SameLine();
	Texture::SharedPtr tex;
	if (UIBridge::m_bSamplingColor) {
		tex = Texture::createTextureFromFile("take_color_sel.png", false);
	}
	else {
		tex = Texture::createTextureFromFile("take_color_nor.png", false);
	}
	bool buttonClick = ImGui::ImageButton(tex->getTextureID(), ImVec2(82 * scaleRatioW, 32 * scaleRatioH));
	if(buttonClick)
	{
		UIBridge::m_bSamplingColor = !UIBridge::m_bSamplingColor;
		UIBridge::m_bShowColorChoice = false;
	}
	ImGui::EndGroup();
	ImGui::NewLine();
	ImGui::PopStyleColor(4);

}


void GUIGS::SetCurColorCircle(cv::Vec4b color)
{
	m_curColor.setColor(color);
	//存文件
	string pathSaveArea, pathBgSeg;
	ifstream in(GetCongfigPath().c_str());
	if (!in.is_open()) {
		fprintf(stderr, "fail to read json file: %s\n", gUserConfig.data());
	}

	string json_content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	in.close();
	rapidjson::Document dom;
	rapidjson::Document::AllocatorType& allocator = dom.GetAllocator();
	if (!dom.Parse(json_content.c_str()).HasParseError())
	{
		if (dom.HasMember("BgSegFilePath") && dom["BgSegFilePath"].IsString())
		{
			pathBgSeg = dom["BgSegFilePath"].GetString();
		}
	}
	//遍历判断颜色是否重复
	bool addFlag = true;
	for (int i = 0; i < m_vecColorRecent.size(); i++) {
		if (m_vecColorRecentTex[i].vecColorRGBA[0] == color) {
			addFlag = false;
			break;
		};
	}
	if (addFlag) {
		//添加颜色到列表
		if (m_vecColorRecent.size() == 4) {
			for (int i = 0; i < 3; i++) {
				m_vecColorRecentTex[i + 1].setColor(m_vecColorRecent[i]);
			}
			for (int i = 3; i > 0; i--) {
				m_vecColorRecent[i] = m_vecColorRecent[i - 1];
			}
			m_vecColorRecent[0] = color;
			m_vecColorRecentTex[0].setColor(color);
		}
		else {
			m_vecColorRecent.push_back(color);
			m_vecColorRecentTex.push_back(color);
		}
	}
	rapidjson::StringBuffer buf;
	//rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf); // it can word wrap
	writer.StartObject();
	writer.Key("GSSaveAreaFilePath");
	writer.String(mConfig.strFilePath.data());
	writer.Key("BgSegFilePath");
	writer.String(pathBgSeg.data());
	writer.Key("GSKeyColors");
	writer.StartArray();
	for (int i = 0; i < m_vecColorRecent.size(); i++) {
		writer.StartArray();
		for (int j = 0; j < 4; j++) {
			writer.Double(m_vecColorRecent[i][j]);
		}
		writer.EndArray();
	}	
	writer.EndArray();
	writer.EndObject();

	ofstream outfile;
	outfile.open(GetCongfigPath().data());
	if (!outfile.is_open()) {
		fprintf(stderr, "fail to open file to write: %s\n", gUserConfig.data());
	}

	outfile << buf.GetString() << endl;
	outfile.close();

}


cv::Vec4b GUIGS::GetCurColorCircle()
{
	return m_curColor.vecColorRGBA[0];
}

static MyDocument GDocs[3];

void GUIGS::ShowGSTab(NamaExampleNameSpace::Nama * nama)
{
	if (GDocs[0].Name == NULL)
	{
		GDocs[0].Name = u8"美肤";
		GDocs[1].Name = u8"美型";
		GDocs[2].Name = u8"滤镜";
		//GDocs[3].Name = u8"美体";
	}
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(45.f / 255.f, 41.f / 255.f, 86.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(45.f / 255.f, 41.f / 255.f, 86.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(150.f / 255.f, 157.f / 255.f, 181.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(47.f / 255.f, 54.f / 255.f, 92.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 8));
	// Tabs
	ImGui::BeginTabBar("##tabs1", ImGuiTabBarFlags_NoAnim | ImGuiTabBarFlags_SizingPolicyEqual);
	for (int doc_n = 0; doc_n < IM_ARRAYSIZE(GDocs); doc_n++)
	{
		MyDocument& doc = GDocs[doc_n];
		//const bool selected = ImGui::TabItem(ImVec2(133 * scaleRatioW, 30 * scaleRatioH), doc.Name, &doc.Open, 0);

		bool selected = false;
		if ((UIBridge::rightTabIndex - 1 == doc_n && doc_n == 0) ||
			(IM_ARRAYSIZE(GDocs) - 1 == doc_n && UIBridge::rightTabIndex + 1 == doc_n)) {
			selected = ImGui::TabItem(ImVec2(138 * scaleRatioW, 36 * scaleRatioH), doc.Name, &doc.Open, 0, 2);
		}
		else if (IM_ARRAYSIZE(GDocs) - 1 == doc_n) {
			selected = ImGui::TabItem(ImVec2(138 * scaleRatioW, 36 * scaleRatioH), doc.Name, &doc.Open, 0, 4);
		}
		else if (UIBridge::rightTabIndex + 1 == doc_n || doc_n == 0) {
			selected = ImGui::TabItem(ImVec2(138 * scaleRatioW, 36 * scaleRatioH), doc.Name, &doc.Open, 0, 1);
		}
		else if (UIBridge::rightTabIndex - 1 == doc_n) {
			selected = ImGui::TabItem(ImVec2(138 * scaleRatioW, 36 * scaleRatioH), doc.Name, &doc.Open, 0, 3);
		}
		else {
			selected = ImGui::TabItem(ImVec2(138 * scaleRatioW, 36 * scaleRatioH), doc.Name, &doc.Open, 0, 5);
		}

		if (!selected)
			continue;

		UIBridge::rightTabIndex = doc_n;
		switch (doc_n)
		{
		case 0:
		{
			gui_tab_content::ShowTabBeautySkin(nama);
		}
		break;
		case 1:
		{
			gui_tab_content::ShowTabFaceBeauty(nama);
		}
		break;
		case 2:
		{
			gui_tab_content::ShowTabFilter(nama);
		}
		break;
		case 3:
		{
			gui_tab_content::ShowTabBodyBeauty(nama);
		}
		break;
		}
	}
	ImGui::EndTabBar();
	ImGui::PopStyleColor(8);
	ImGui::PopStyleVar(3);
}


void GUIGS::ShowGSParamRight(NamaExampleNameSpace::Nama * nama)
{
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(45.f / 255.f, 41.f / 255.f, 86.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(45.f / 255.f, 41.f / 255.f, 86.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(45.f / 255.f, 41.f / 255.f, 86.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 8));
	ImGui::BeginTabBar("##tabcm1", ImGuiTabBarFlags_NoAnim | ImGuiTabBarFlags_SizingPolicyEqual | ImGuiTabBarFlags_NoSelectionOnAppearing);
	ImGui::TabItem(ImVec2(416 * scaleRatioW, 36 * scaleRatioH), u8"抠像参数");
	ImGui::EndTabBar();
	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar();

	ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));


	DrawColorPanel(nama);


	string sliderIconNameArr[MAX_GREEN_SCREEN_PARAM] = { "list_icon_tolerance_open", "list_icon_smooth_open",
		"list_icon_transparency_open" };

	string sliderNameArr[MAX_GREEN_SCREEN_PARAM] = { u8"  相似度",u8"   平滑" ,u8"  祛色度" };

	for (int i = 0; i < MAX_GREEN_SCREEN_PARAM; i++)
	{
		if (UIBridge::mGSParam[i] == 0)
		{
			string closeIconFile = sliderIconNameArr[i].substr(0, sliderIconNameArr[i].find_last_of('_')) + "_close";
			LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile(closeIconFile + ".png", false)->getTextureID(), sliderNameArr[i].c_str());
		}
		else
		{
			LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile(sliderIconNameArr[i] + ".png", false)->getTextureID(), sliderNameArr[i].c_str());
		}
		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

		if (LayoutSlider(ImVec2(22, 22), ImVec2(232, 10), ("##sliderGSP" + to_string(i)).c_str(), ("##slidertextGSP" + std::to_string(i)).c_str(), &UIBridge::mGSParam[i], 0, 100))
		{
			nama->UpdateGreenScreen();
		}
		ImGui::PopStyleVar();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(246.f / 255.f, 246.f / 255.f, 250.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));

	if (LayoutButton(ImVec2(145, 258), ImVec2(126, 32), u8"恢复默认"))
	{
		UIBridge::m_bShowColorChoice = false;
		// 恢复为默认绿色
		auto & colorBag = m_vecColorTex[0];
		nama->SetGSKeyColor(colorBag.vecColorRGBA[0]);
		m_curColor.setColor(colorBag.vecColorRGBA[0]);
		// 取消选中
		UIBridge::m_curRenderItemUIIDSec = 0;
		//取消绿幕安全区域选中
		//UIBridge::mCurRenderGSSAItemName = "NONE";
		//nama->NonuseGSSA();
		ResetDefParam();
		nama->UpdateGreenScreen();
	}

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(5);
}

void GUIGS::ShowRightPanel(NamaExampleNameSpace::Nama * nama)
{
	bool bOpen = true;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::Begin("GSParam", &bOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);

	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(120.f / 255.f, 136.f / 255.f, 234.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(150.f / 255.f, 157.f / 255.f, 181.f / 255.f, 1.f));

	if (UIBridge::showItemSelectWindow && UIBridge::gsBundleCategory >= BundleCategory::GreenScreen)
	{
		ShowGSParamRight(nama);
	}
	else
	{
		UIBridge::m_bShowColorChoice = false;
		ShowGSTab(nama);
	}

	if (UIBridge::m_bShowColorChoice)
	{
		ShowColorChoiceWin(nama);
	}
	ImGui::PopStyleColor(4);
	ImGui::End();
	ImGui::PopStyleVar(2);

}


void GUIGS::LoadResource()
{
	ifstream in(GetCongfigPath().c_str());
	if (!in.is_open()) {
		fprintf(stderr, "fail to read json file: %s\n", gUserConfig.data());
		return;
	}

	string json_content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	in.close();

	rapidjson::Document dom;

	if (!dom.Parse(json_content.c_str()).HasParseError())
	{
		if (dom.HasMember("GSKeyColors") && dom["GSKeyColors"].IsArray())
		{
			const auto& array = dom["GSKeyColors"];
			int size = array.Size();
			for (int j = 0; j < array.Size(); j++)
			{
				if (array[j].IsArray() && array[j].Size() >= 4)
				{
				    cv::Vec4b colorItem;
					for (int k = 0; k < array[j].Size(); k++)
					{
						colorItem[k] = array[j][k].GetFloat();
					}
					m_vecColorRecent.push_back(colorItem);
				}
			}
		}
		if (dom.HasMember("GSSaveAreaFilePath") && dom["GSSaveAreaFilePath"].IsString())
		{
			string path = dom["GSSaveAreaFilePath"].GetString();
			ifstream ifs(path.c_str());
			if (ifs.good()) {
				mConfig.strFilePath = dom["GSSaveAreaFilePath"].GetString();
			}
		}
	}

	//Check
	if (mConfig.strFilePath.length() > 0)
	{
		mIsUserConfigOK = FuTool::IsFileExit(mConfig.strFilePath.data());
	}
}



void GUIGS::SelectGSSafeAreaFile() {

	const int iCount = 3;
	const char* open_filename = nullptr;
    std::string filePath = "";
	char const* filter_patterns[iCount] = { "*.png","*.jpeg","*.jpg" };  // "*.png",
#if _WIN32
    open_filename = tinyfd_openFileDialog(
		"Load Pic",
		"",
		iCount,
		filter_patterns,
		nullptr,
		0);
    
    if(open_filename) filePath = open_filename;
#elif __APPLE__
	vector<std::string> _filePaths = {};
	vector<const char*> types = { "png","jpeg","jpg" };
	FuToolMac::importFilesInObjectC("~/Desktop", types, &_filePaths, false);
	if (_filePaths.size() > 0) {
        filePath = *_filePaths.begin();
	}
#endif
	if (filePath.length() > 0) {
		mConfig.strFilePath = filePath;
#if _WIN32
		cv::Mat mat = cv::imread(filePath, cv::IMREAD_REDUCED_COLOR_4);
#elif __APPLE__
		cv::Mat mat = cv::imread(filePath);
#endif
		if (!mat.empty()) {
			cv::imwrite(GetUserIconPath(), mat);
			SaveUserConfig();
			Texture::createTextureFromFullPath(GetUserIconPath(), true, true);
			mIsUserConfigOK = true;
			mIsSelectGSSafeAreaFileOK = true;
		}
		else {
			UIBridge::m_openLocalFileTip = u8"当前类型文件不支持";
			UIBridge::m_bLoadWrongNamePNGFile = true;
			UIBridge::mLastTimeExtra = ImGui::GetTime() + 8.0;
		}
	}

}

void GUIGS::SaveUserConfig() {

	string pathSaveArea, pathBgSeg;
	ifstream in(GetCongfigPath().c_str());
	if (!in.is_open()) {
		fprintf(stderr, "fail to read json file: %s\n", gUserConfig.data());
		return;
	}

	string json_content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	in.close();
	rapidjson::Document dom;
	rapidjson::Document::AllocatorType& allocator = dom.GetAllocator();
	vector<cv::Vec4b> m_vecColorRecent;
	if (!dom.Parse(json_content.c_str()).HasParseError())
	{
		if (dom.HasMember("GSSaveAreaFilePath") && dom["GSSaveAreaFilePath"].IsString())
		{
			pathSaveArea = dom["GSSaveAreaFilePath"].GetString();
		}
		if (dom.HasMember("BgSegFilePath") && dom["BgSegFilePath"].IsString())
		{
			pathBgSeg = dom["BgSegFilePath"].GetString();
		}
		if (dom.HasMember("GSKeyColors") && dom["GSKeyColors"].IsArray())
		{
			const auto& array = dom["GSKeyColors"];
			int size = array.Size();
			for (int j = 0; j < array.Size(); j++)
			{
				if (array[j].IsArray() && array[j].Size() >= 4)
				{
					cv::Vec4b colorItem;
					for (int k = 0; k < array[j].Size(); k++)
					{
						colorItem[k] = array[j][k].GetFloat();
					}
					m_vecColorRecent.push_back(colorItem);
				}
			}
		}
	}

	rapidjson::StringBuffer buf;
	//rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf); // it can word wrap
	writer.StartObject();
	if (pathSaveArea != "") {
		writer.Key("GSSaveAreaFilePath");
		writer.String(pathSaveArea.data());
	}
	if (pathBgSeg != "") {
		writer.Key("BgSegFilePath");
		writer.String(pathBgSeg.data());
	}
	writer.Key("GSKeyColors");
	writer.StartArray();
	for (int i = 0; i < m_vecColorRecent.size(); i++) {
		writer.StartArray();
		for (int j = 0; j < 4; j++) {
			writer.Double(m_vecColorRecent[i][j]);
		}
		writer.EndArray();
	}
	writer.EndArray();
	writer.EndObject();

	ofstream outfile;
	outfile.open(GetCongfigPath().data());
	if (!outfile.is_open()) {
		fprintf(stderr, "fail to open file to write: %s\n", gUserConfig.data());
	}

	outfile << buf.GetString() << endl;
	outfile.close();

}
