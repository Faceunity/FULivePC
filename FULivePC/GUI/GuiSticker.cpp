#include "GuiSticker.h"
#include "GuiTool.h"
#include "UIBridge.h"
#include "StickerHolder.h"
#include "GuiCustomMakeup.h"
#include "FuController.h"
#include <future>

using namespace NamaExampleNameSpace;
using namespace gui_tool;
using namespace std;

extern float scaleRatioW;
extern float scaleRatioH;

std::shared_ptr<StikcerHolder> GUISticker::mNetHolder = nullptr;
std::thread GDownloadBundleThread;
std::atomic_bool GIsDownloading;

int GUISticker::mSelectTip = 0;

bool GUISticker::mSelectSticker = false;

GUISticker::GUISticker()
{
	GIsDownloading = false;
}

GUISticker::~GUISticker()
{
}

void GUISticker::LoadResource()
{
	if (mNetHolder == nullptr)
	{
		mNetHolder = std::make_shared<StikcerHolder>();
	}
}

static std::vector<MyDocument> GDocs;
void GUISticker::ShowStickerPannel(Nama * nama)
{
	if (mNetHolder == nullptr)
	{
		mNetHolder = std::make_shared<StikcerHolder>();
	}
	if (GDocs.size() == 0)
	{
		for (int i = 0; i < mNetHolder->mTags.size(); i++)
		{
			GDocs.push_back(MyDocument());
			GDocs[i].Name = mNetHolder->mTags[i].c_str();
		}
	}
	if (UIBridge::m_curRenderItem == -1) {
		GUISticker::mSelectTip = 0;
	}
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 0.88f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(238.f / 255.f, 240.f / 255.f, 246.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 0.88f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 0.88f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(47.f / 255.f, 54.f / 255.f, 92.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(47.f / 255.f, 54.f / 255.f, 92.f / 255.f, 0.4f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::SetNextWindowPos(ImVec2(19 * scaleRatioW, 535 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(876 * scaleRatioW, 120 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("itemSelectSticker##1563", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	if (UIBridge::m_bShowingBodyBeauty) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	}
	// Tabs
	ImGui::BeginTabBar("##tabsSticker1", ImGuiTabBarFlags_NoAnim);

	for (int doc_n = 0; doc_n < mNetHolder->mTags.size(); doc_n++)
	{
		MyDocument& doc = GDocs[doc_n];

		ImGui::PushID(&doc);
		string str = doc.Name;
		str= str.substr(0, 6);

		bool selected = false;
		if (mSelectSticker && mSelectTip == doc_n) {
			mSelectSticker = false;
			selected = ImGui::TabItem(ImVec2(100 * scaleRatioW, 26 * scaleRatioH), str.data(), &doc.Open, 2, 5);
		}else{
			selected = ImGui::TabItem(ImVec2(100 * scaleRatioW, 26 * scaleRatioH), str.data(), &doc.Open, 0, 5);
		}
		
		if (!selected)
		{
			ImGui::PopID();
			continue;
		}

		ShowStickerList(nama, doc_n);

		ImGui::PopID();
	}
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	bool flag = false;
	ImGui::TabItem(ImVec2((875 - mNetHolder->mTags.size() * 100) * scaleRatioW, 26 * scaleRatioH), "", &flag, 0, 5);
	ImGui::PopItemFlag();
	ImGui::EndTabBar();
	if (UIBridge::m_bShowingBodyBeauty) {
		ImGui::PopItemFlag();
	}
	ImGui::End();
	ImGui::PopStyleVar(3);
	ImGui::PopStyleColor(6);
}

void GUISticker::ShowStickerList(Nama * nama, int tagIndex)
{
	static int lastIndex = 0;
	if (tagIndex != lastIndex) {
		lastIndex = tagIndex;
		UIBridge::bundleCategoryPage = 0;
	}
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .70f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.0f));

	ImGui::BeginChild("##tabsSticker2", ImVec2(860 * scaleRatioW, 91 * scaleRatioH), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::Dummy(ImVec2(0, 1));
	ImGui::Dummy(ImVec2(0, 1));
	ImGui::SameLine();
	auto funSelect = [&](std::weak_ptr<BundleRes> res) {

		if (res.expired())
		{
			return;
		}

		auto bundle = res.lock();

		if (UIBridge::showCustomMakeup)
		{
			GUICustomMakeup::Reset(nama);
			nama->DestroyAll();
		}

		UIBridge::showCustomMakeup = false;

		int maxPeople = 4;
		if (bundle->IsOnlyOnePeople())
		{
			maxPeople = 1;
		}
		UIBridge::lastMaxFace = maxPeople;
		nama->UpdateFilter(UIBridge::m_curFilterIdx);
		UIBridge::showLightMakeupTip = false;
		bundle->mHasNotBinded = true;
		//加载小熊
		if (bundle->mBundleDirs.size() > 1) {
				nama->UnLoadAvatar();
				nama->UnLoadMakeup();
				std::vector<std::string> mBundleDirs;
				for (auto& i : bundle->mBundleDirs) {
					mBundleDirs.emplace_back(i);
				}
				nama->LoadAvatarBundles(mBundleDirs);

				BodyTrackConfig c;
				c.DefaultType = BodyTrackType::FullBody;
				c.FullBody.EnableFaceProcessor = false;
				c.FullBody.UseRetargetRootScale = true;
				c.FullBody.UseRetargetRootScaleValue = 0;
				c.FullBody.AnimFilterParams_n_buffer_frames = 10;
				c.FullBody.AnimFilterParams_pos_w = 0;
				c.FullBody.AnimFilterParams_angle_w = 0;
				c.FullBody.GlobalOffset_x = 0;
				c.FullBody.GlobalOffset_y = 0;
				c.FullBody.GlobalOffset_z = 0;
				c.FullBody.Scale = 1.0f;
				c.FullBody.Pos_x = 70;
				c.FullBody.Pos_y = 50;
				c.FullBody.Pos_z = -1100;
				c.FullBody.TrackMoveRange_x = 0.9f;
				c.FullBody.TrackMoveRange_y = 0.9f;
				c.FullBody.TrackMoveRange_z = 0.1f;

				nama->SetBodyTrackType(BodyTrackType::FullBody);
				auto& params = c.FullBody;
				nama->m_Controller->EnableFaceProcessor(params.EnableFaceProcessor);
				nama->m_Controller->UseRetargetRootScale(params.UseRetargetRootScale, params.UseRetargetRootScaleValue);
				nama->m_Controller->SetAvatarAnimFilterParams(params.AnimFilterParams_n_buffer_frames, params.AnimFilterParams_pos_w, params.AnimFilterParams_angle_w);
				nama->m_Controller->SetAvatarGlobalOffset(params.GlobalOffset_x, params.GlobalOffset_y, params.GlobalOffset_z);
				nama->m_Controller->SetAvatarScale(params.Scale);
				nama->m_Controller->SetPos(params.Pos_x, params.Pos_y, params.Pos_z);
				//nama->m_Controller->EnableHumanFollowMode(params.EnableHumanFollowMode);
				nama->m_Controller->SetAvatarTranslationScale(params.TrackMoveRange_x, params.TrackMoveRange_y, params.TrackMoveRange_z);
				nama->m_Controller->EnableHumanFollowMode(false);
				Nama::mNamaAppState.RenderAvatarBear = true;
		}
		else {
			nama->UnLoadAvatar();
			for (auto& i : bundle->mBundleDirs)
				nama->SelectBundle(i, maxPeople);
		}

	};
	if (tagIndex < mNetHolder->mTagBundleList.size())
	{
		int bundlesSize = mNetHolder->mTagBundleList[tagIndex].size();
		int pageSize = 0;
		if (bundlesSize > 10) {
			if (bundlesSize % 9 == 0) {
				pageSize = bundlesSize / 9;
			}
			else {
				pageSize = bundlesSize / 9 + 1;
			}
		}
		ImGui::SameLine(0.f, 10.f * scaleRatioW);
		bool buttonClick;
		if (bundlesSize > 10) {
			if (UIBridge::bundleCategoryPage > 0) {
				ImGui::BeginGroup();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 18 * scaleRatioH);
				buttonClick = ImGui::ImageButton(Texture::createTextureFromFile("icon_lbutton.png", false)->getTextureID(), ImVec2(24 * scaleRatioW, 24 * scaleRatioH));
				if (buttonClick)
				{
					UIBridge::bundleCategoryPage--;
				}
				ImGui::EndGroup();
				ImGui::SameLine(0.f, 25.f * scaleRatioW);
			}
			else {
				ImGui::SameLine(0.f, 63.f * scaleRatioW);
			}
		}else {
			ImGui::SameLine(0, (10 - bundlesSize) * 38.5 * scaleRatioW + 22 * scaleRatioW);
		}
		int end = bundlesSize;
		if (bundlesSize > 10) {
			end = 9 * (UIBridge::bundleCategoryPage + 1);
			if (end > bundlesSize) {
				end = bundlesSize;
			}
		}
		for (int i = 9 * UIBridge::bundleCategoryPage; i < end; i++)
		{
			ImGui::PushID(i);

			std::string itemName = mNetHolder->mTagBundleList[tagIndex][i]->mItemName;
			std::string iconName = mNetHolder->mTagBundleList[tagIndex][i]->mIconDir;
			
			Texture::SharedPtr tex;
			if (mNetHolder->mTagBundleList[tagIndex][i]->mBundleIsDownload)
			{
				tex = Texture::createTextureFromFullPath(iconName, true);
			}
			else if (mNetHolder->mTagBundleList[tagIndex][i]->mIsDownloading)
			{
				tex = Texture::createLoadingTextureFromFullPath(iconName, true);
			}else
			{
				tex = Texture::createUnLoadTextureFromFullPath(iconName, true);
			}

			//找不到图标
			if (!tex)
			{
				tex = Texture::createTextureFromFile("icon_Movebutton_nor.png", false);
			}
			bool buttonClick;
			if (UIBridge::mCurRenderItemName == itemName)
			{
				if (!mNetHolder->mTagBundleList[tagIndex][i]->mHasNotBinded)
				{
					if (mNetHolder->mTagBundleList[tagIndex][i]->mBundleIsDownload)
					{
						funSelect(mNetHolder->mTagBundleList[tagIndex][i]);
					}
				}
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(),
					ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0),
					ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Select);
			}
			else
			{
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(),
					ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0),
					ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Deselect);
			}
			if (buttonClick && GIsDownloading == false)
			{
				if (UIBridge::mCurRenderItemName != itemName)
				{
					UIBridge::mCurRenderItemName = itemName;
					UIBridge::mStyleRecommendationIndex = 0;
					loadStyleParam();
					nama->UpdateBeauty();
					UIBridge::mLastTime = ImGui::GetTime() + 2.0;
					UIBridge::showItemTipsWindow = false;
				}
				else
				{
					UIBridge::mCurRenderItemName = "NONE";
					UIBridge::mLastTime = 0.0;
					UIBridge::showItemTipsWindow = false;
					UIBridge::mNeedStopMP3 = true;
				}

				if (mNetHolder->mTagBundleList[tagIndex][i]->mBundleIsDownload == false &&
					mNetHolder->mTagBundleList[tagIndex][i]->mIsDownloading == false && GIsDownloading == false)
				{
					std::packaged_task<void()> task([&, tagIndex, i, nama]() {
						mNetHolder->mTagBundleList[tagIndex][i]->mIsDownloading = true;

						auto strUrl = mNetHolder->RequestBundleUrl(mNetHolder->mTagBundleList[tagIndex][i]->mId);
						if (strUrl.length() > 0)
						{

							for (auto& bundledir : mNetHolder->mTagBundleList[tagIndex][i]->mBundleDirs) {
								bool bZip = false;
								string dirpath;
								if (bundledir.find(",") != string::npos) {
									dirpath = bundledir.substr(0, bundledir.rfind("/") + 1);
									bundledir = dirpath + "bundle.zip";
									bZip = true;
								}
								mNetHolder->DownLoadFile(strUrl, bundledir);
								if (bZip) {
									mNetHolder->UnzipFile(bundledir, dirpath, tagIndex, i);
									break;
								}
							}
							mNetHolder->mTagBundleList[tagIndex][i]->mBundleIsDownload = true;
						}
						
						GIsDownloading = false;
						GDownloadBundleThread.detach();
					});
					GIsDownloading = true;
					GDownloadBundleThread = std::thread(std::move(task));
				}

				if (mNetHolder->mTagBundleList[tagIndex][i]->mBundleIsDownload)
				{
					funSelect(mNetHolder->mTagBundleList[tagIndex][i]);
					mSelectTip = tagIndex;
				}
			}
			ImGui::SameLine(0.f, 21.f * scaleRatioW);

			ImGui::PopID();
		}
		if (bundlesSize > 10) {
			if (UIBridge::bundleCategoryPage != pageSize - 1 ) {
				ImGui::BeginGroup();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 18 * scaleRatioH);
				buttonClick = ImGui::ImageButton(Texture::createTextureFromFile(UIBridge::bundleCategoryPage < pageSize - 1 ?
					"icon_rbutton.png" : "icon_rbutton_nor.png", false)->getTextureID(), ImVec2(24 * scaleRatioW, 24 * scaleRatioH));
				if (buttonClick && UIBridge::bundleCategoryPage < pageSize - 1)
				{
					UIBridge::bundleCategoryPage++;
				}
				ImGui::EndGroup();
			}
		}
}

	ImGui::EndChild();
	ImGui::PopStyleColor(5);

}
