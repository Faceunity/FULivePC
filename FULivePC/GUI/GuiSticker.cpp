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
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .70f));
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(150.f / 255.f, 157.f / 255.f, 181.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .60f));

	ImGui::SetNextWindowPos(ImVec2(19 * scaleRatioW, 544 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(876 * scaleRatioW, 130 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("itemSelectSticker##1563", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	
	// Tabs
	ImGui::BeginTabBar("##tabsSticker1", ImGuiTabBarFlags_NoAnim);

	for (int doc_n = 0; doc_n < mNetHolder->mTags.size(); doc_n++)
	{
		MyDocument& doc = GDocs[doc_n];

		ImGui::PushID(&doc);

		bool selected = false;
		if (mSelectSticker && mSelectTip == doc_n) {
			mSelectSticker = false;
			selected = ImGui::TabItem(ImVec2(100 * scaleRatioW, 26 * scaleRatioH), doc.Name, &doc.Open, 2);
		}else{
			selected = ImGui::TabItem(ImVec2(100 * scaleRatioW, 26 * scaleRatioH), doc.Name, &doc.Open, 0);
		}
		
		if (!selected)
		{
			ImGui::PopID();
			continue;
		}

		ShowStickerList(nama, doc_n);

		ImGui::PopID();
	}
	ImGui::EndTabBar();

	ImGui::End();
	ImGui::PopStyleColor(5);
}

void GUISticker::ShowStickerList(Nama * nama, int tagIndex)
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .70f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .0f));
	//ImGui::SetNextWindowPos(ImVec2(19 * scaleRatioW, 584 * scaleRatioH), ImGuiCond_Always);
	//ImGui::SetNextWindowSize(ImVec2(888 * scaleRatioW, 90 * scaleRatioH), ImGuiCond_Always);
	ImGui::BeginChild("##tabsSticker2", ImVec2(860 * scaleRatioW, 91 * scaleRatioH), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar);
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

		bundle->mHasNotBinded = true;

		for (auto& i : bundle->mBundleDirs)
			nama->SelectBundle(i, maxPeople);

	};

	if (tagIndex < mNetHolder->mTagBundleList.size())
	{
		for (int i = 0; i < mNetHolder->mTagBundleList[tagIndex].size(); i++)
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

			//ÕÒ²»µ½Í¼±ê
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
							for (auto& bundledir : mNetHolder->mTagBundleList[tagIndex][i]->mBundleDirs)
								mNetHolder->DownLoadFile(strUrl, bundledir);

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
			ImGui::SameLine(0.f, 24.f * scaleRatioW);

			ImGui::PopID();
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

}
