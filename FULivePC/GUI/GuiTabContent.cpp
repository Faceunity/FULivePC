#include "GuiTabContent.h"
#include "GuiTool.h"
#include "UIBridge.h"

using namespace gui_tool;

extern float scaleRatioW;
extern float scaleRatioH;
extern bool showUI;

namespace gui_tab_content
{
	void ShowTabBeautySkin(Nama * nama)
	{
		ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 0.f));
		if (UIBridge::mEnableSkinDect)
		{
			LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_skinbeauty_open.png", false)->getTextureID(), u8"精准美肤");
		}
		else
		{
			LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_skinbeauty_close.png", false)->getTextureID(), u8"精准美肤");
		}
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(225.f / 255.f, 228.f / 255.f, 238.f / 255.f, 1.f));
		if (LayoutSelectable(ImVec2(22, 11), ImVec2(118, 30), u8"开启", UIBridge::mEnableSkinDect == 1))
		{
			UIBridge::mEnableSkinDect = 1;
			nama->UpdateBeauty();
		}
		ImGui::SameLine();
		if (LayoutSelectable(ImVec2(22, 11), ImVec2(118, 30), u8"关闭", UIBridge::mEnableSkinDect == 0))
		{
			UIBridge::mEnableSkinDect = 0;
			nama->UpdateBeauty();
		}
		LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_BeautyMode_open.png", false)->getTextureID(), u8"美肤模式");
		ImGui::SameLine();
		if (LayoutSelectable(ImVec2(22, 11), ImVec2(78, 30), u8"精细磨皮##3", UIBridge::mEnableHeayBlur == 0))
		{
			UIBridge::mEnableHeayBlur = 0;
			nama->UpdateBeauty();
		}
		ImGui::SameLine();
		if (LayoutSelectable(ImVec2(5, 11), ImVec2(78, 30), u8"清晰磨皮##2", UIBridge::mEnableHeayBlur == 1))
		{
			UIBridge::mEnableHeayBlur = 1;
			nama->UpdateBeauty();
		}
		ImGui::SameLine();
		if (LayoutSelectable(ImVec2(5, 11), ImVec2(78, 30), u8"朦胧磨皮##2", UIBridge::mEnableHeayBlur == 2))
		{
			UIBridge::mEnableHeayBlur = 2;
			nama->UpdateBeauty();
		}
		ImGui::PopStyleColor();
		//ImGui::PopStyleColor();

		std::string sliderIconNameArr[MAX_BEAUTYFACEPARAMTER] = { "list_icon_Grindingskin_open","list_icon_Skinwhitening_open", "list_icon_Ruddy_open","list_iconsharpen_open",
			"list_icon_Brighteye_open","list_iconBeautifulteeth_open",
			"list_icon_dark_circles_open","list_icon_wrinkle_open" };

		std::string sliderNameArr[MAX_BEAUTYFACEPARAMTER] = { u8"   磨皮", u8"   美白",u8"   红润",u8"   锐化", u8"   亮眼", u8"   美牙" ,u8"去黑眼圈", u8"去法令纹" };

		for (int i = 0; i < MAX_BEAUTYFACEPARAMTER; i++)
		{
			if (UIBridge::mFaceBeautyLevel[i] == 0)
			{
				std::string closeIconFile = sliderIconNameArr[i].substr(0, sliderIconNameArr[i].find_last_of('_')) + "_close";
				LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile(closeIconFile + ".png", false)->getTextureID(), sliderNameArr[i].c_str());
			}
			else
			{
				LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile(sliderIconNameArr[i] + ".png", false)->getTextureID(), sliderNameArr[i].c_str());
			}
			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

			if (LayoutSlider(ImVec2(22, 22), ImVec2(252, 10), ("##slider221" + std::to_string(i)).c_str(), &UIBridge::mFaceBeautyLevel[i], 0, 100))
			{
				nama->UpdateBeauty();
			}
			ImGui::PopStyleVar();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
		if (LayoutButton(ImVec2(145, 38), ImVec2(126, 40), u8"恢复默认"))
		{
			resetBeautyParam();
			nama->UpdateBeauty();
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		ImGui::PopStyleColor();
	}

	void ShowTabFaceBeauty(Nama * nama)
	{
		ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));
		//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(225.f / 255.f, 228.f / 255.f, 238.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 0.f));
		LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_Facetype_open.png", false)->getTextureID(), u8"   脸型");
		ImGui::SameLine();
		std::string faceTypeNameArr[5] = { u8"自定义" ,u8"默认" ,u8"女神",u8"网红" ,u8"自然" };
		int shapeHandle[5] = { 4,3,0,1,2 };
		for (int i = 0; i < 5; i++)
		{
			if (LayoutSelectable(ImVec2(4, 11), ImVec2(44, 30), faceTypeNameArr[i].c_str(), UIBridge::faceType == i))
			{
				UIBridge::faceType = i;
				nama->SetCurrentShape(shapeHandle[i]);
			}
			ImGui::SameLine();
		}
		ImGui::Spacing();


		std::string faceShapeIconNameArr[MAX_FACESHAPEPARAMTER] = { "list_icon_Thinface_open", "list_icon_Bigeye_open",
			"list_icon_round_eye_open","list_icon_chin_open", "list_icon_forehead_open", "list_icon_Thinnose_open","list_icon_Mouthtype_open",
			"list_icon_v_open","list_icon_narrow_face_open","list_icon_little_face_open",
		"list_icon_cheekbones_open" ,"list_icon_lower_jaw_open" ,"list_icon_open_eyes_open" ,"list_icon_eye_distance_open" ,"list_icon_eye_angle_open" ,
		"list_icon_proboscis_open" ,"list_icon_shrinking_open" ,"list_icon_smile_mouth_open" };

		//眼角、眼距、眼睛角度、长鼻、缩人中、微笑嘴角

		std::string faceShapeNameArr[MAX_FACESHAPEPARAMTER] = { u8"   瘦脸" ,u8"   大眼" ,u8"   圆眼" ,u8"   下巴",u8"   额头" ,
			u8"   瘦鼻",u8"   嘴型",u8"   V脸",u8"   窄脸",u8"   小脸" ,
		 u8"  瘦颧骨", u8" 瘦下颌骨" ,u8"  开眼角", u8"   眼距", u8"眼睛角度", u8"   长鼻", u8"  缩人中", u8"微笑嘴角" };


		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 1.f));
		ImGui::BeginChild("faceShape##2223", ImVec2(400 * scaleRatioW, 550 * scaleRatioH));
		ImGui::PopStyleColor();
		for (int i = 0; i < MAX_FACESHAPEPARAMTER; i++)
		{
			if (UIBridge::faceType != 0 && i > 1)
			{
				break;
			}
			if (UIBridge::mFaceShapeLevel[i] == 0)
			{
				std::string closeIconFile = faceShapeIconNameArr[i].substr(0, faceShapeIconNameArr[i].find_last_of('_')) + "_close";
				LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile(closeIconFile + ".png", false)->getTextureID(), faceShapeNameArr[i].c_str());
			}
			else
			{
				LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile(faceShapeIconNameArr[i] + ".png", false)->getTextureID(), faceShapeNameArr[i].c_str());
			}
			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
			if (g_faceShapeParamShowFlag[i] == FACE_SHAPE_SHOW_FLAG_MIDDLE)
			{
				if (LayoutSlider(ImVec2(22, 22), ImVec2(252, 10), (std::string("##slider23") + std::to_string(i)).c_str(), &UIBridge::mFaceShapeLevel[i], -50, 50))
				{
					nama->UpdateBeauty();
				}
			}
			else if (g_faceShapeParamShowFlag[i] == FACE_SHAPE_SHOW_FLAG_NORMAL)
			{
				if (LayoutSlider(ImVec2(22, 22), ImVec2(252, 10), (std::string("##slider2") + std::to_string(i)).c_str(), &UIBridge::mFaceShapeLevel[i], 0, 100))
				{
					nama->UpdateBeauty();
				}
			}
			ImGui::PopStyleVar();
		}

		ImGui::EndChild();

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
		if (LayoutButton(ImVec2(145, 15), ImVec2(126, 40), u8"恢复默认"))
		{
			resetShapeParam();
			nama->UpdateBeauty();
			nama->SetCurrentShape(4);
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();


		ImGui::PopStyleColor();
	}

	void ShowTabFilter(Nama * nama)
	{
		ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));
		std::string filterNameArr[6] = { "list_image_origin", "list_image_bailiang1", "list_image_fennen1", "list_image_xiaoqingxin1", "list_image_lengsediao1", "list_image_nuansediao1" };
		for (int i = 0; i < 6; i++)
		{
			if (ImGui::ImageButton(Texture::createTextureFromFile(filterNameArr[i] + ".png", false)->getTextureID(), ImVec2(106 * scaleRatioW, 106 * scaleRatioH)))
			{
				nama->UpdateFilter(i);

				UIBridge::m_curFilterIdx = i;
				nama->UpdateBeauty();
				if (i != 0)
				{
					UIBridge::showFilterSlider = true;
				}
				else
				{
					UIBridge::showFilterSlider = false;
				}
			}
			if (i != 2 && i != 5)
			{
				ImGui::SameLine(0.f, 27.f);
			}
		}
		if (UIBridge::showFilterSlider) {
			if (LayoutSlider(ImVec2(54, 280), ImVec2(252, 10), "##slider36", &UIBridge::mFilterLevel[UIBridge::m_curFilterIdx], 0, 100))
			{
				nama->UpdateBeauty();
			}
		}
	}

	/*
	这个比较影响性能，参数均为0的时候会关闭
	*/
	void ShowTabBodyBeauty(Nama * nama)
	{
		std::string sliderIconNameArr[MAX_BODY_SHAPE_PARAM] = { "list_icon_slimming_open","list_icon_stovepipe_open", "list_icon_thin_waist_open",
			"list_icon_shoulder_open","list_icon_hip_open",
			"list_icon_little_head_open","list_icon_thin_leg_open" };

		std::string sliderNameArr[MAX_BODY_SHAPE_PARAM] = { u8"   瘦身", u8"   长腿",u8"   瘦腰", u8"   美肩", u8"   美臀" ,u8"    小头", u8"    瘦腿" };
		static bool _showItemSelectWindow;
		for (int i = 0; i < MAX_BODY_SHAPE_PARAM; i++)
		{
			if (UIBridge::mBodyShapeLevel[i] == 0)
			{
				std::string closeIconFile = sliderIconNameArr[i].substr(0, sliderIconNameArr[i].find_last_of('_')) + "_close";
				LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile(closeIconFile + ".png", false)->getTextureID(), sliderNameArr[i].c_str());
			}
			else
			{
				LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile(sliderIconNameArr[i] + ".png", false)->getTextureID(), sliderNameArr[i].c_str());
			}
			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

			if (g_bodyShapeParamShowFlag[i] == BODY_SHAPE_SHOW_FLAG_MIDDLE)
			{
				if (LayoutSlider(ImVec2(22, 22), ImVec2(252, 10), ("##slider221" + std::to_string(i)).c_str(), &UIBridge::mBodyShapeLevel[i], -50, 50))
				{
					
					// 添加2s时间，用于显示”AR功能跟美体模块无法共用“的提示
					if(!UIBridge::m_bShowingBodyBeauty){
						UIBridge::mLastTime = ImGui::GetTime() + 2.0;
						// 处理音乐滤镜,关闭音乐
						if (UIBridge::bundleCategory == BundleCategory::MusicFilter && UIBridge::mNeedPlayMP3) {
							nama->pauseCurrentMp3();
						}
					}
					UIBridge::mLastTime = ImGui::GetTime() + 2.0;
					UIBridge::m_bShowingBodyBeauty = true;
					_showItemSelectWindow = UIBridge::showItemSelectWindow;
					UIBridge::showItemSelectWindow = false;
					nama->UpdateBodyShape();
				}
			}
			else
			{
				if (LayoutSlider(ImVec2(22, 22), ImVec2(252, 10), ("##slider221" + std::to_string(i)).c_str(), &UIBridge::mBodyShapeLevel[i], 0, 100))
				{
					// 添加2s时间，用于显示”AR功能跟美体模块无法共用“的提示
					if(!UIBridge::m_bShowingBodyBeauty){
						UIBridge::mLastTime = ImGui::GetTime() + 2.0;
						// 处理音乐滤镜,关闭音乐
						if (UIBridge::bundleCategory == BundleCategory::MusicFilter && UIBridge::mNeedPlayMP3) {
							nama->pauseCurrentMp3();
						}
					}
					UIBridge::m_bShowingBodyBeauty = true;
					_showItemSelectWindow = UIBridge::showItemSelectWindow;
					UIBridge::showItemSelectWindow = false;
					nama->UpdateBodyShape();
				}
			}



			ImGui::PopStyleVar();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

		if (LayoutButton(ImVec2(145, 38), ImVec2(126, 40), u8"恢复默认"))
		{
			UIBridge::m_bShowingBodyBeauty = false;
			// 处理音乐滤镜，重新开启音乐
			if (UIBridge::bundleCategory == BundleCategory::MusicFilter && UIBridge::mNeedPlayMP3 && !UIBridge::showGreenScreen) {
				nama->resumeCurrentMp3();
			}
			UIBridge::showItemSelectWindow = _showItemSelectWindow;
			resetBodyShapeParam();
			nama->UpdateBodyShape();
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}
}
