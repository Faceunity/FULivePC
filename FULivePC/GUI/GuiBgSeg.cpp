#if _WIN32
#include "Windows.h"
#elif __APPLE__
#include "fu_tool_mac.h"
#endif
#include "fu_tool.h"
#include "UIBridge.h"
#include "tinfiledlg/tinyfiledialogs.h"
#include <iostream>
#include <string>
#include <regex>

#include "GuiBgSeg.h"
#include "UIBridge.h"
#include "GuiCustomMakeup.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "BgSegUpdate.h"


extern float scaleRatioW;
extern float scaleRatioH;

vector<string> GUIBgSeg::mVecBundleName;
BgSegUserConfig GUIBgSeg::mConfig;
bool GUIBgSeg::mIsUserConfigOK = false;
bool GUIBgSeg::mIsSelectBgSegFileOK = false;

GUIBgSeg::GUIBgSeg() {

}

GUIBgSeg::~GUIBgSeg() {

}


static string GetCongfigPath() {


	string ConfPath = "";

#ifdef __APPLE__

	ConfPath = FuToolMac::GetDocumentPath() + "/" + gUserConfig;


#else

	ConfPath = gUserConfig;

#endif

	return ConfPath;
}

bool GUIBgSeg::LoadUserConfig() {

	ifstream in(GetCongfigPath().c_str());
	if (!in.is_open()) {
		fprintf(stderr, "fail to read json file: %s\n", gUserConfig.data());
		return false;
	}

	string json_content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	in.close();

	rapidjson::Document dom;
	if (!dom.Parse(json_content.c_str()).HasParseError())
	{
		if (dom.HasMember("BgSegFilePath") && dom["BgSegFilePath"].IsString())
		{
			string path = dom["BgSegFilePath"].GetString(); 
			ifstream ifs(path.c_str());
			if (ifs.good()) {
				mConfig.strFilePath = path;
			}
		}
	}

	//Check
	if (mConfig.strFilePath.length() > 0)
	{
		ConfigVideoInfo(mConfig.strFilePath);
		return FuTool::IsFileExit(mConfig.strFilePath.data());
	}

}

void GUIBgSeg::SaveUserConfig() {

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
	writer.Key("GSSaveAreaFilePath");
	writer.String(pathSaveArea.data());
	writer.Key("BgSegFilePath");
	writer.String(mConfig.strFilePath.data());
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
		fprintf(stderr, "fail to open file to write: %s\n", GetCongfigPath().data());
	}

	outfile << buf.GetString() << endl;
	outfile.close();

}


void GUIBgSeg::LoadResource()
{
	UIBridge::FindAllBundle(gBundlePath[BundleCategory::BackgroundSegmentation], mVecBundleName);
	mIsUserConfigOK = LoadUserConfig();
}

static string GetUserIconPath() {

	string IconPath = "";

#ifdef __APPLE__

	IconPath = FuToolMac::GetDocumentPath() + "/" + BGSEG_USERFILE_PIC;


#else

	IconPath = BGSEG_USERFILE_PIC;

#endif

	return IconPath;

}

void GUIBgSeg::ShowBgSegOption(NamaExampleNameSpace::Nama* nama)
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 0.88f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(250.f / 255.f, 250.f / 255.f, 250.f / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(120.f / 255.f, 136.f / 255.f, 230.f / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(250.f / 255.f, 250.f / 255.f, 250.f / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(92.f / 255.f, 96.f / 255.f, 113.f / 255.f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 20.0f);
	ImGui::SetNextWindowPos(ImVec2(19 * scaleRatioW, 570 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(876 * scaleRatioW, 85 * scaleRatioH), ImGuiCond_Always);

	ImGui::Begin("itemSelect##1563", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	if (UIBridge::m_bShowingBodyBeauty) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	}
	if (LayoutButton2(ImVec2(244, 10), ImVec2(176, 40), u8"通用分割版"))
	{
		UIBridge::mSelectedBsType = true;
		nama->setHumanSegScene(1);
	}
	ImGui::SameLine(0.f, 48.f * scaleRatioW);
	if (LayoutButton2(ImVec2(0, 10), ImVec2(176, 40), u8"视频会议版"))
	{
		UIBridge::mSelectedBsType = true;
		nama->setHumanSegScene(2);
	}
	if (UIBridge::m_bShowingBodyBeauty) {
		ImGui::PopItemFlag();
	}
	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor(5);
}

void GUIBgSeg::ShowBgSegPannel(NamaExampleNameSpace::Nama* nama) {
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 0.88f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.0f));
	ImGui::SetNextWindowPos(ImVec2(19 * scaleRatioW, 570 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(876 * scaleRatioW, 85 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("itemSelect##1563", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar);
	if (UIBridge::m_bShowingBodyBeauty) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	}
	int loopCnt = 0;

	if (mIsUserConfigOK)
	{
		loopCnt = mVecBundleName.size() + 2;
	}
	else
	{
		loopCnt = mVecBundleName.size() + 1;
	}


	for (int i = 0; i < loopCnt; i++)
	{
		ImGui::PushID(i);

		string itemName = "";
		Texture::SharedPtr tex = nullptr;
		string itemPath = "";

		/* 加号 */
		if (i == 0)
		{
			tex = Texture::createTextureFromFile(BGSEG_ADD_ICON, false);
			if (ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(),
				ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1,
				ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Reset))
			{
				SelectBgSegFile();
			}

			ImGui::PopID();

			ImGui::SameLine(0.f, 22.f * scaleRatioW);

			continue;
		}
		else if (i == 1 && mIsUserConfigOK) /* 用户素材截图 */
		{
			itemName = BGSEG_CUSTOM_NAME;
			itemPath = g_assetDir + BGSEG_CUSTOM_NAME;

			string iconName = GetUserIconPath();
			tex = Texture::createTextureFromFullPath(iconName, true);
		}
		else
		{
			int index = mIsUserConfigOK ? i - 2 : i - 1;
			itemName = mVecBundleName[index];
			itemPath = gBundlePath[UIBridge::bundleCategory] + "/" + itemName;

			string iconName = itemName.substr(0, itemName.find_last_of('.'));
			tex = Texture::createTextureFromFile(iconName + ".png", false);
		}

		//找不到图标
		if (!tex)
		{
			tex = Texture::createTextureFromFile("icon_Movebutton_nor.png", false);
		}

		bool buttonClick;

		if (mIsSelectBgSegFileOK) {
			mIsSelectBgSegFileOK = false;
			UIBridge::m_curRenderItem = -1;
			UIBridge::mCurRenderItemName = "NONE";
			UIBridge::mStyleRecommendationIndex = 0;
			loadStyleParam();
			nama->UpdateBeauty();
			buttonClick = true;
		}
		else {
			if (UIBridge::mCurRenderItemName == itemName)
			{
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(),
					ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1),
					-1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Select);
			}
			else {
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(),
					ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1),
					-1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Deselect);
			}
		}

		if (buttonClick)
		{
			if (UIBridge::showLightMakeupTip) {
				UIBridge::showLightMakeupTip = false;
				nama->UpdateFilter(UIBridge::m_curFilterIdx);
			}
			if (UIBridge::showCustomMakeup)
			{
				GUICustomMakeup::Reset(nama);
				nama->DestroyAll();
				UIBridge::showCustomMakeup = false;
			}
			if (UIBridge::mCurRenderItemName != itemName)
			{
				UIBridge::mCurRenderItemName = itemName;
				UIBridge::mStyleRecommendationIndex = 0;
				loadStyleParam();
				nama->UpdateBeauty();
				if (!UIBridge::m_bLoadWrongNamePNGFile) {
					UIBridge::mLastTimeExtra = ImGui::GetTime() + 2.0;
					UIBridge::showItemTipsWindowExtra = false;
				}
				nama->SelectBundle(itemPath);
			}
			else
			{
				UIBridge::mCurRenderItemName = "NONE";
				UIBridge::mLastTime = 0.0;
				UIBridge::showItemTipsWindow = false;
				UIBridge::mNeedStopMP3 = true;
			}
		}
		ImGui::SameLine(0.f, 22.f * scaleRatioW);

		ImGui::PopID();
	}
	if (UIBridge::m_bShowingBodyBeauty) {
		ImGui::PopItemFlag();
	}
	ImGui::End();
	ImGui::PopStyleColor(4);

}

static void calculatePreViewForScreen(float orgW, float orgH, float* dstW, float* dstH) {
	float ratioW = orgW / DEF_FRAME_SHOW_WIDTH;
	float ratioH = orgH / DEF_FRAME_SHOW_HEIGHT;
	if (ratioW > ratioH) {
		*dstW = DEF_FRAME_SHOW_WIDTH;
		*dstH = orgH / ratioW;
	}
	else {
		*dstH = DEF_FRAME_SHOW_HEIGHT;
		*dstW = orgW / ratioH;
	}
}

void GUIBgSeg::ConfigVideoInfo(string videoPath) {

	if (videoPath.length() > 0)
	{
		if (regex_match(videoPath, regex("(.*)\\.(mp4|MP4|mov|MOV)"))) {
			UIBridge::m_localBgSegVideoType = true;
		}
		else {
			UIBridge::m_localBgSegVideoType = false;
		}
		if (UIBridge::m_localBgSegVideoType) {
			int rotation = FuTool::getLocalVideoRotation(videoPath);
			UIBridge::m_localBgSegVideoRotation = rotation;

			float videoWidth, videoHeight, dstW, dstH;
			bool isSuccess = FuTool::getLocalVideoResolution(videoPath, &videoWidth, &videoHeight);
			switch (UIBridge::m_localBgSegVideoRotation) {
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
			calculatePreViewForScreen(videoWidth, videoHeight, &dstW, &dstH);
			if (isSuccess) {
				UIBridge::m_localBgSegVideoWidth = dstW;
				UIBridge::m_localBgSegVideoHeight = dstH;
			}
			else {
				UIBridge::m_localBgSegVideoWidth = DEF_FRAME_SHOW_WIDTH;
				UIBridge::m_localBgSegVideoHeight = DEF_FRAME_SHOW_HEIGHT;
			}

			BgSegUpdate::instance().SetSource(videoPath);
		}
		else {

			BgSegUpdate::instance().SetSource(videoPath);
			UIBridge::m_localBgSegVideoRotation = 0;
			float imgWidth, imgHeight, dstW, dstH;
			cv::Size imgSize = BgSegUpdate::instance().GetDstResolution();
			imgWidth = imgSize.width;
			imgHeight = imgSize.height;
			calculatePreViewForScreen(imgWidth, imgHeight, &dstW, &dstH);
			UIBridge::m_localBgSegVideoWidth = dstW;
			UIBridge::m_localBgSegVideoHeight = dstH;

		}

		mIsUserConfigOK = true;
	}

}

void GUIBgSeg::SelectBgSegFile() {

	const int iCount = 6;
	const char* open_filename = nullptr;
    std::string selectedPath = "";
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
        if (open_filename) selectedPath = open_filename;
	}
#elif __APPLE__
	vector<std::string> _filePaths = {};
	vector<const char*> types = { "mp4","mov","png","jpeg","jpg","gif" };
	FuToolMac::importFilesInObjectC("~/Desktop", types, &_filePaths, false);
	if (_filePaths.size() > 0) {
        selectedPath = *_filePaths.begin();
	}
#endif
	if (selectedPath.length() > 0) {
		mConfig.strFilePath = selectedPath;
		SaveUserConfig();
		ConfigVideoInfo(selectedPath);
		BgSegUpdate::instance().SaveCurFrame2File(GetUserIconPath());
		Texture::createTextureFromFullPath(GetUserIconPath(), true, true);
		mIsSelectBgSegFileOK = true;
	}

}
