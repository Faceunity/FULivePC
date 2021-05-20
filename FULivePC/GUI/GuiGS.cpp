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
bool UIBridge::m_bSamplingColor = false;
bool UIBridge::m_bShowColorChoice = false;
bool UIBridge::m_bShowingBodyBeauty = false;
bool UIBridge::m_bLoadWrongNamePNGFile = false;
int UIBridge::m_localVideoRotation = 0;
bool UIBridge::m_localVideoType = false;
float UIBridge::m_localVideoWidth = 0;
bool UIBridge::m_bResetPreviewRect = false;
float UIBridge::m_localVideoHeight = 0;
string UIBridge::m_openLocalFileTip = "";
string UIBridge::mCurRenderGSItemName = "";
int UIBridge::gsBundleCategory = BUNDLE_CATEGORY_NOMEAN;

CCameraDS * GUIGS::m_pCamBG = new CCameraDS();
bool GUIGS::m_bHasSetedBgForOnceChange = false;
vector<ColorBag> GUIGS::m_vecColorTex;
vector<ColorBag> GUIGS::m_vecColorRecentTex;
ColorBag GUIGS::m_curColor = ColorBag();
Texture::SharedPtr GUIGS::m_hsvTexture = nullptr;
cv::Mat GUIGS::m_hsvMat;
FURect GUIGS::previewRect = {DEF_FRAME_SHOW_POS_X+DEF_FRAME_SHOW_WIDTH/2.0,DEF_FRAME_SHOW_POS_Y + DEF_FRAME_SHOW_HEIGHT/2.0,DEF_FRAME_SHOW_WIDTH/2.0,DEF_FRAME_SHOW_HEIGHT/2.0};

/////////////////////////////////////////////////////////////////

GUIGS::GUIGS()
{
}

GUIGS::~GUIGS()
{
	m_pCamBG->closeCamera();
}

static bool openDeafultCamera(Nama * nama)
{
	int iLen = nama->CameraList().size();
	
	if (iLen == 0)
	{
		return false;
	}
	UIBridge::m_localVideoWidth = DEF_FRAME_SHOW_WIDTH/2.0;
	UIBridge::m_localVideoHeight = DEF_FRAME_SHOW_HEIGHT/2.0;
	UIBridge::m_bResetPreviewRect = true;
	int index = iLen > UIBridge::mSelectedCamera ? UIBridge::mSelectedCamera : 0;
	UIBridge::mSelectedCamera = index;
	nama->ReOpenCamera(index);
}

static bool IsCamDeviceOpen(Nama * nama)
{
	bool bCamDeviceOpen = nama->IsCameraInit() && (nama->GetCameraCaptureType() == GS_INPUT_TYPE_CAMERA);
	return bCamDeviceOpen;
}

static void LayoutShowCamChoice(Nama * nama)
{
	if (nama->CameraList().size())
	{
		if (!IsCamDeviceOpen(nama))
		{
			openDeafultCamera(nama);
		}

		ImGui::PushItemWidth(244 * scaleRatioW);
		static string item_current = nama->CameraList()[0];
		if (ImGui::BeginCombo("##slect camera2", item_current.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < nama->CameraList().size(); n++)
			{
				bool is_selected = (item_current == nama->CameraList()[n]);
				if (ImGui::Selectable(nama->CameraList()[n].c_str(), is_selected))
				{
					item_current = nama->CameraList()[n];
					bool bCamDeviceOpen = IsCamDeviceOpen(nama);

					if (UIBridge::mSelectedCamera != n || !bCamDeviceOpen)
					{
						UIBridge::mSelectedCamera = n;
						nama->ReOpenCamera(n);
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
		ImGui::PushItemWidth(244 * scaleRatioW);
		if (ImGui::BeginCombo("##slect camera4", u8"未检测到摄像头")) // The second parameter is the label previewed before opening the combo.				
			ImGui::EndCombo();
		ImGui::PopItemWidth();
	}
}

void GUIGS::ShowCameraChoiceForGS(Nama * nama)
{
	ImGui::SetNextWindowPos(ImVec2(200 * scaleRatioW, 240 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(510 * scaleRatioW, 390 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("WindowGSI_CHOICE_CAM##4", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	ImGui::Dummy(ImVec2(21 * scaleRatioW, 11 * scaleRatioH));
	ImGui::Text(u8"选择摄像头:");
	static bool setedChangeGSInputSrc = false;
	ImGui::SameLine();
	if (!IsCamDeviceOpen(nama)){
		if (UIBridge::m_bSetGSInputSrc && UIBridge::m_gsState.inputType != GS_INPUT_TYPE_CAMERA)
		{
			setedChangeGSInputSrc = true;
		}
	}
	LayoutShowCamChoice(nama);

	float frameWidth = (float)DEF_FRAME_SHOW_WIDTH / 1.8f;
	float frameHeight = (float)DEF_FRAME_SHOW_HEIGHT / 1.8f;
	bool bSuccessShow = false;

	if (nama->GetCameraCaptureType() == GS_INPUT_TYPE_CAMERA && nama->IsCameraInit())
	{
		cv::Mat frameMat = nama->GetOriginFrame();
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
		LayoutImage(ImVec2(0, 0), ImVec2(frameWidth, frameHeight), Texture::createTextureFromFile("bg_no_camera_detected.png", false)->getTextureID(), u8"");
	}

	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(120.f / 255.f, 136.f / 255.f, 234.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(150.f / 255.f, 157.f / 255.f, 181.f / 255.f, 1.f));

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

	ImGui::BeginGroup();

	ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));
	ImGui::Dummy(ImVec2(380 * scaleRatioW, 1));

	ImGui::SameLine();

	if (ImGui::Button(u8"确定"))
	{
		UIBridge::m_gsState.inputType = GS_INPUT_TYPE_CAMERA;
		UIBridge::m_gsState.iCmaeraID = nama->GetCameraID();
		
		
		
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

	ImGui::SameLine();

	if (ImGui::Button(u8"取消"))
	{
		UIBridge::m_bNeedShowGSCamChoice = false;

		setedChangeGSInputSrc = false;
	}
    
	ImGui::EndGroup();

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(4);

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
	ImGui::SetNextWindowPos(ImVec2(240 * scaleRatioW, 250 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(400 * scaleRatioW, 300 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("WindowGSI##3", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	
	RectLayoutDesc desc;
	desc.pos = ImVec2(160 , 0);
	desc.size = ImVec2(52 , 52 );
	
	ImVec2 textPos = ImVec2(60, 0);
	
	LayoutImage(desc, Texture::createTextureFromFile("icon_prompt.png", false)->getTextureID(), textPos,
				u8"请使用纯色背景，推荐绿色幕布效果效果最佳");
	
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(120.f / 255.f, 136.f / 255.f, 234.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(150.f / 255.f, 157.f / 255.f, 181.f / 255.f, 1.f));
	
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
	
	if (LayoutButton(ImVec2(120 , 60 ), ImVec2(120 , 40 ), u8"选择摄像头"))
	{
		UIBridge::m_bNeedShowGSCamChoice = true;
	}
	
	if (LayoutButton(ImVec2(120 , 0), ImVec2(120 , 40 ), u8"选择视频/图片"))
	{
		const int iCount = 6;
		const char* open_filename = nullptr;
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
					UIBridge::mLastTime = ImGui::GetTime() + 8.0;
					
				}
			}
		}
#elif __APPLE__
		vector<const char *> _filePaths = {};
		vector<const char*> types = { "mp4","mov","png","jpeg","jpg","gif"};
		FuToolMac::importFilesInObjectC("~/Desktop", types, &_filePaths,false);
		if (_filePaths.size() > 0) {
			open_filename = *_filePaths.begin();
		}
#endif
		if (open_filename){
			string videoPath = open_filename;
			if (videoPath.length() > 0)
			{
				if (regex_match(open_filename, regex("(.*)\\.(mp4|MP4|mov|MOV)"))) {
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
					
					
					if (SetCameraFileInput(nama, videoPath.c_str()))
					{
						
					}
				}else{
					if (SetCameraFileInput(nama, videoPath.c_str()))
					{
						UIBridge::m_localVideoRotation = 0;
						float imgWidth,imgHeight, dstW, dstH;
						cv::Size imgSize = nama->getCameraDstResolution();
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
	if(canCancel){
		if (LayoutButton(ImVec2(120, 0), ImVec2(120, 40), u8"取  消"))
		{
			UIBridge::m_bNeedReChooseInputSrc = false;
		}
	}
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(4);
	
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

bool GUIGS::SetCameraFileInput(NamaExampleNameSpace::Nama * nama, const char * path)
{
	bool bRet = false;
	string path_string{path};
	nama->OpenCamera(path_string);
	
	if (nama->GetCameraCaptureType() == GS_INPUT_TYPE_FILE && nama->IsCameraInit())
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
	string allCategory[] = { "list_icon_green_screen_cutout_nor",
		u8"  绿幕" };
	int amount = 1;
	
	categoryNameArr = allCategory;
	
	/* 为了居中 */
	int leftStart = (900 * scaleRatioW - amount * 52) / 2;
	ImGui::Dummy(ImVec2(leftStart, 1)); ImGui::SameLine();
	
	for (int i = 0; i < amount; i++)
	{
		if (UIBridge::gsBundleCategory == (i + GS_START))
		{
			// 正在取色时，禁止点击
			if (UIBridge::m_bSamplingColor){  
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.3f);
			}
			if (LayoutImageButtonWithText(ImVec2(0.f, 27.f), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_propmap_collapse.png", false)->getTextureID(),
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
			if (LayoutImageButtonWithText(ImVec2(0.f, 27.f), ImVec2(52, 52), Texture::createTextureFromFile(categoryNameArr[i] + ".png", false)->getTextureID(),
										  Texture::createTextureFromFile(iconFileName, false)->getTextureID(), categoryNameArr[amount + i].c_str()))
			{
				if (UIBridge::m_bSetGSInputSrc){  
					auto funSetSelectd = [&](int index) {
						UIBridge::gsBundleCategory = index + GS_START;
						UIBridge::showItemSelectWindow = true;
						Nama::mEnableAvatar = false;
					};
					
					funSetSelectd(i);
				}
			}
			if (!UIBridge::m_bSetGSInputSrc){  
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}
		ImGui::SameLine(0.f, 27.f * scaleRatioW);
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
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(82.f / 255.f, 88.f / 255.f, 112.f / 255.f, .78f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(82.f / 255.f, 88.f / 255.f, 112.f / 255.f, .0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(82.f / 255.f, 88.f / 255.f, 112.f / 255.f, 0.f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::SetNextWindowPos(ImVec2(760 * scaleRatioW, 172 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(120 * scaleRatioW, 40 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("ReChooseInput##23", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar /*| ImGuiWindowFlags_NoInputs*/);
	if (LayoutButton(ImVec2(0, 0), ImVec2(100, 25), u8"选择绿幕输入"))
	{
		UIBridge::m_bNeedReChooseInputSrc = true;
	}
	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

void GUIGS::CloseCamera(Nama * nama)
{
	if (nama)
	{
		nama->CloseCurCamera();
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
/// 输入绿幕背景视频
/// @param itemName 视频名称
void GUIGS::inputGreenScreenBg(string itemName){
	UIBridge::mCurRenderGSItemName = itemName;
	UIBridge::mLastTime = ImGui::GetTime() + 2.0;
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
	if (UIBridge::gsBundleCategory != BundleCategory::GreenScreen)
	{
		return;
	}

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .70f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .0f));
	
	ImGui::SetNextWindowPos(ImVec2(19 * scaleRatioW, 584 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(888 * scaleRatioW, 90 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("itemSelect##1564", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar);

	vector<string> vecFiles;
#if _WIN32	
	UIBridge::FindAllCommonPIC(gGSBgPic, vecFiles);
#elif __APPLE__
	UIBridge::FindAllCommonPICNameFromResourceBundle(gGSBgPic, vecFiles);
#endif
	{
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
			if (UIBridge::m_bSamplingColor){  
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.3f);
			}
			bool buttonClick;
			if (UIBridge::mCurRenderGSItemName == itemName)
			{
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH),ImVec2(0, 0),ImVec2(1, 1),-1,ImVec4(0, 0, 0, 0),ImVec4(1, 1, 1, 1),ImGui::ImGUI_Button_Operation_Type_Select);
			}else{
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH),ImVec2(0, 0),ImVec2(1, 1),-1,ImVec4(0, 0, 0, 0),ImVec4(1, 1, 1, 1),ImGui::ImGUI_Button_Operation_Type_Deselect);
			}
			if (buttonClick)

			{
				if (UIBridge::mCurRenderGSItemName != itemName)
				{
					UIBridge::mCurRenderGSItemName = itemName;
					UIBridge::mLastTime = ImGui::GetTime() + 2.0;
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
			if (UIBridge::m_bSamplingColor){ 
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
			}
		}
	}


	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
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
	UIBridge::mGSParam[0] = 45;
	UIBridge::mGSParam[1] = 30;
	UIBridge::mGSParam[2] = 20;
}

void GUIGS::InitColorVec()
{
	m_hsvTexture = createColorHSV(600, 200, m_hsvMat);


	m_curColor.setColor(g_color1);

	m_vecColorTex.emplace_back(g_color1);
	m_vecColorTex.emplace_back(g_color2);

}

#define COLOR_LINE_NUM (5)

void GUIGS::ShowColorChoiceWin(NamaExampleNameSpace::Nama * nama)
{
	bool bOpen = true;

	ImGui::SetNextWindowPos(ImVec2(1000 * scaleRatioW, 170 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(200 * scaleRatioW, 180 * scaleRatioH), ImGuiCond_Always);

	ImGui::BeginChild("ColorChoice",ImVec2(200 * scaleRatioW, 160 * scaleRatioH),true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

	ImGui::Text(u8"可选颜色:");

	auto funLoopColor = [&](vector<ColorBag> & vec, ImGuiID & guiID) {

		int iLoopIndex = 0;

		for (auto & colorBag : vec)
		{
			if (colorBag.pTex)
			{
				ImGui::PushID(&colorBag);

				if (ImGui::ImageRoundButton(guiID, colorBag.pTex->getTextureID(), ImVec2(20, 20)))
				{
					nama->SetGSKeyColor(colorBag.vecColorRGBA[0]);
					m_curColor.setColor(colorBag.vecColorRGBA[0]);
				}

				ImGui::PopID();

				iLoopIndex++;

				if (iLoopIndex < COLOR_LINE_NUM)
				{
					ImGui::SameLine();
				}
				else
				{
					iLoopIndex = 0;
				}
			}
		}
	};

	funLoopColor(m_vecColorTex, UIBridge::m_curRenderItemUIIDSec);
	
	ImGui::Dummy(ImVec2(2,2));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
	
	static const ImVec4 normalColor { 1.0f, 1.0f, 1.0f, 1.0f };
	static const ImVec4 hoverColor {152/255.0f, 165/255.0f, 245/255.0f, 1.0f};
	static const ImVec4 disableColor { 1.0f, 1.0f, 1.0f, 0.3f };
	static ImVec4 currentColor = normalColor;
	ImGui::PushStyleColor(ImGuiCol_Button, currentColor);
	float cancelBtnW = 80;
	float cancelBtnX = (200 * scaleRatioW - cancelBtnW) / 2.0 - 10;
	if (LayoutButton(ImVec2(cancelBtnX, 20), ImVec2(cancelBtnW, 25),u8"关    闭"))
	{
		UIBridge::m_bShowColorChoice = false;
	}
    ImGui::PopStyleColor(1);
	ImGui::PopStyleVar(2);

	ImGui::EndChild();
}


void GUIGS::DrawColorPanel(NamaExampleNameSpace::Nama * nama)
{
	ImGui::BeginGroup();
	ImGui::Dummy(ImVec2(1, 20 * scaleRatioH));
	ImGui::Dummy(ImVec2(23 * scaleRatioW, 1)); ImGui::SameLine();
	ImGui::Image(m_curColor.pTex->getTextureID(), ImVec2(50 * scaleRatioW, 50 * scaleRatioW));

	ImGui::SameLine();

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
	
	static const ImVec4 normalColor { 1.0f, 1.0f, 1.0f, 1.0f };
	static const ImVec4 hoverColor {152/255.0f, 165/255.0f, 245/255.0f, 1.0f};
	static const ImVec4 disableColor { 1.0f, 1.0f, 1.0f, 0.3f };
	static ImVec4 currentColor;
	
	if (UIBridge::m_bSamplingColor) {
		// 取色中，显示高亮
		currentColor = hoverColor;
	}else{
		// 取色完成，恢复默认
		currentColor = normalColor;
	}
	ImGui::PushStyleColor(ImGuiCol_Button, currentColor);
	// 在显示色卡时，使取色按钮禁止点击
	if (UIBridge::m_bShowColorChoice){
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.3f);
	}
	if (LayoutButton(ImVec2(20, 20), ImVec2(80, 25),u8"取    色"))
	{
		UIBridge::m_bSamplingColor = !UIBridge::m_bSamplingColor;
	}
	if (UIBridge::m_bShowColorChoice){
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
	ImGui::PopStyleColor(1);
    
	ImGui::SameLine();
	if (UIBridge::m_bShowColorChoice) {
		// 选择中，显示高亮
		currentColor = hoverColor;
	}else{
		// 选择完成，恢复默认
		currentColor = normalColor;
	}
	ImGui::PushStyleColor(ImGuiCol_Button, currentColor);
	// 在正在取色时，使’选择颜色‘按钮禁止点击
	if (UIBridge::m_bSamplingColor){
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.3f);
	}
	if (LayoutButton(ImVec2(20, 20), ImVec2(80, 25), u8"选择颜色"))
	{
		UIBridge::m_bShowColorChoice = !UIBridge::m_bShowColorChoice;
	}
	if (UIBridge::m_bSamplingColor){
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
    ImGui::PopStyleColor(1);
	ImGui::PopStyleVar(2);

	ImGui::EndGroup();


	if (UIBridge::m_bShowColorChoice)
	{
		ShowColorChoiceWin(nama);
	}
}


void GUIGS::SetCurColorCircle(cv::Vec4b color)
{
	m_curColor.setColor(color);
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

	// Tabs
	ImGui::BeginTabBar("##tabs1", ImGuiTabBarFlags_NoAnim | ImGuiTabBarFlags_SizingPolicyEqual);
	for (int doc_n = 0; doc_n < IM_ARRAYSIZE(GDocs); doc_n++)
	{
		MyDocument& doc = GDocs[doc_n];
		const bool selected = ImGui::TabItem(ImVec2(133 * scaleRatioW, 30 * scaleRatioH), doc.Name, &doc.Open, 0);

		if (!selected)
			continue;

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
}


void GUIGS::ShowGSParamRight(NamaExampleNameSpace::Nama * nama)
{
	ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));


	DrawColorPanel(nama);


	string sliderIconNameArr[MAX_GREEN_SCREEN_PARAM] = { "list_icon_tolerance_open", "list_icon_smooth_open",
		"list_icon_transparency_open" };

	string sliderNameArr[MAX_GREEN_SCREEN_PARAM] = { u8"  相似度",u8"   平滑" ,u8"  透明度" };

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

		if (LayoutSlider(ImVec2(22, 22), ImVec2(252, 10), ("##sliderGSP" + to_string(i)).c_str(), &UIBridge::mGSParam[i], 0, 100))
		{
			nama->UpdateGreenScreen();
		}
		ImGui::PopStyleVar();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

	if (LayoutButton(ImVec2(145, 38), ImVec2(126, 40), u8"恢复默认"))
	{
		UIBridge::m_bShowColorChoice = false;
		// 恢复为默认绿色
		auto & colorBag = m_vecColorTex[0];
		nama->SetGSKeyColor(colorBag.vecColorRGBA[0]);
		m_curColor.setColor(colorBag.vecColorRGBA[0]);
		// 取消选中
		UIBridge::m_curRenderItemUIIDSec = 0;
		ResetDefParam();
		nama->UpdateGreenScreen();
	}

	ImGui::PopStyleVar(2);
}

void GUIGS::ShowRightPanel(NamaExampleNameSpace::Nama * nama)
{
	bool bOpen = true;

	ImGui::Begin("GSParam", &bOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(120.f / 255.f, 136.f / 255.f, 234.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(150.f / 255.f, 157.f / 255.f, 181.f / 255.f, 1.f));

	if (UIBridge::showItemSelectWindow && UIBridge::gsBundleCategory == BundleCategory::GreenScreen)
	{
		ShowGSParamRight(nama);
	}
	else
	{
		ShowGSTab(nama);
	}
	

	ImGui::PopStyleColor(4);
	ImGui::End();
}
