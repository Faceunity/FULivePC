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
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(92.f / 255.f, 96.f / 255.f, 113.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
		ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));
		if (UIBridge::mFaceBeautyLevel[0])
		{
			LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_skinbeauty_open.png", false)->getTextureID(), u8"精准美肤");
		}
		else
		{
			LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_skinbeauty_close.png", false)->getTextureID(), u8"精准美肤");
		}

		ImGui::SameLine();

		if (LayoutSelectable(ImVec2(22, 11), ImVec2(80, 30), u8"开启", UIBridge::mFaceBeautyLevel[0] == 1))
		{
			UIBridge::mFaceBeautyLevel[0] = 1;
			nama->UpdateBeauty();
			saveStyleConfig();
		}
		ImGui::SameLine();
		if (LayoutSelectable(ImVec2(0, 11), ImVec2(80, 30), u8"关闭", UIBridge::mFaceBeautyLevel[0] == 0))
		{
			UIBridge::mFaceBeautyLevel[0] = 0;
			nama->UpdateBeauty();
			saveStyleConfig();
		}
		LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_BeautyMode_open.png", false)->getTextureID(), u8"美肤模式");
		ImGui::SameLine();
		if (LayoutSelectable(ImVec2(22, 11), ImVec2(57, 30), u8"均匀磨皮##3", UIBridge::mFaceBeautyLevel[1] == 3))
		{
			UIBridge::mFaceBeautyLevel[1] = 3;
			nama->UpdateBeauty();
			saveStyleConfig();
		}
		ImGui::SameLine();
		if (LayoutSelectable(ImVec2(0, 11), ImVec2(57, 30), u8"精细磨皮##2", UIBridge::mFaceBeautyLevel[1] == 0))
		{
			UIBridge::mFaceBeautyLevel[1] = 0;
			nama->UpdateBeauty();
			saveStyleConfig();
		}
		ImGui::SameLine();
		if (LayoutSelectable(ImVec2(0, 11), ImVec2(57, 30), u8"清晰磨皮##2", UIBridge::mFaceBeautyLevel[1] == 1))
		{
			UIBridge::mFaceBeautyLevel[1] = 1;
			nama->UpdateBeauty();
			saveStyleConfig();
		}
		ImGui::SameLine();
		if (LayoutSelectable(ImVec2(0, 11), ImVec2(57, 30), u8"朦胧磨皮##2", UIBridge::mFaceBeautyLevel[1] == 2))
		{
			UIBridge::mFaceBeautyLevel[1] = 2;
			nama->UpdateBeauty();
			saveStyleConfig();
		}
		ImGui::PopStyleVar();
		std::string sliderIconNameArr[MAX_BEAUTYFACEPARAMTER] = { "list_icon_Grindingskin_open","list_icon_acne_open","list_icon_Skinwhitening_open","list_icon_Skinwhitening_open", "list_icon_Ruddy_open","list_icon_clearness_open","list_iconsharpen_open",
			"list_icon_stereoscopic_open", "list_icon_Brighteye_open", "list_iconBeautifulteeth_open",
			"list_icon_dark_circles_open","list_icon_wrinkle_open" };

		std::string sliderNameArr[MAX_BEAUTYFACEPARAMTER] = { u8"   磨皮", u8"  祛斑痘", u8"   美白模式", u8"   美白",u8"   红润",u8"   清晰",u8"   锐化", u8"五官立体",u8"   亮眼", u8"   美牙" ,u8"去黑眼圈", u8"去法令纹" };

		for (int i = 0; i < MAX_BEAUTYFACEPARAMTER - 2; i++)
		{
			if (i == 2) {
				if (UIBridge::mFaceBeautyLevel[4])
				{
					LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_Skinwhitening_open.png", false)->getTextureID(), u8"美白模式");
				}
				else
				{
					LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_Skinwhitening_close.png", false)->getTextureID(), u8"美白模式");
				}

				ImGui::SameLine();

				if (LayoutSelectable(ImVec2(22, 11), ImVec2(80, 30), u8"全局美白", UIBridge::mFaceBeautyLevel[4] == 0))
				{
					UIBridge::mFaceBeautyLevel[4] = 0;
					nama->UpdateBeauty();
					saveStyleConfig();
				}
				ImGui::SameLine();
				if (LayoutSelectable(ImVec2(0, 11), ImVec2(80, 30), u8"皮肤美白", UIBridge::mFaceBeautyLevel[4] == 1))
				{
					UIBridge::mFaceBeautyLevel[4] = 1;
					nama->UpdateBeauty();
					saveStyleConfig();
				}
			}
			else {
				if (UIBridge::mFaceBeautyLevel[i + 2] == 0)
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

				if (LayoutSlider(ImVec2(22, 22), ImVec2(232, 10), ("##slider221" + std::to_string(i)).c_str(), ("##slidertext221" + std::to_string(i)).c_str(), &UIBridge::mFaceBeautyLevel[i + 2], 0, 100))
				{
					nama->UpdateBeauty();
					saveStyleConfig();
				}
				ImGui::PopStyleVar();
			}
		}
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(246.f / 255.f, 246.f / 255.f, 250.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 1.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
		if (LayoutButton(ImVec2(145, 38), ImVec2(126, 32), u8"恢复默认"))
		{
			resetBeautyParam();
			nama->UpdateBeauty();
			saveStyleConfig();
		}

		ImGui::NewLine();
		ImGui::PopStyleVar(2);

		ImGui::PopStyleColor(9);
	}

	void ShowTabFaceBeauty(Nama * nama)
	{
		ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(92.f / 255.f, 96.f / 255.f, 113.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.f));

		UIBridge::faceType = 0;
		nama->SetCurrentShape(4);

		std::string faceShapeIconNameArr[MAX_FACESHAPEPARAMTER] = { "list_icon_Thinface_open", "list_icon_Bigeye_open",
			"list_icon_round_eye_open","list_icon_chin_open", "list_icon_forehead_open", "list_icon_Thinnose_open","list_icon_Mouthtype_open","list_icon_mouth_thickness_open",
			"list_icon_v_open","list_icon_narrow_face_open","list_icon_short_face_open","list_icon_little_face_open",
		"list_icon_cheekbones_open" ,"list_icon_lower_jaw_open" ,"list_icon_eye_position_open", "list_icon_open_eyes_open" ,"list_icon_eyelid_down_open" ,"list_icon_eye_distance_open" ,"list_icon_eye_angle_open" ,
		"list_icon_proboscis_open" ,"list_icon_shrinking_open" ,"list_icon_smile_mouth_open" ,"list_icon_eyebrow_position_open" ,"list_icon_eyebrow_spacing_open" ,"list_icon_eyebrow_thickness_open" };

		//眼角、眼距、眼睛角度、长鼻、缩人中、微笑嘴角

		std::string faceShapeNameArr[MAX_FACESHAPEPARAMTER] = { u8"   瘦脸" ,u8"   大眼" ,u8"   圆眼" ,u8"   下巴",u8"   额头" ,
			u8"   瘦鼻",u8"   嘴型", u8"嘴唇厚度",u8"   V脸",u8"   窄脸",u8"   短脸" ,u8"   小脸" ,u8" 瘦颧骨", u8"瘦下颌骨" , u8"眼睛位置",
			u8" 开眼角", u8"眼睑下至", u8"   眼距", u8"眼睛角度", u8"   长鼻", u8" 缩人中", u8"微笑嘴角" , u8"眉毛上下" , u8" 眉间距" , u8"眉毛粗细" };


		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 1.f));
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
				if (LayoutSlider(ImVec2(22, 22), ImVec2(232, 10), (std::string("##slider23") + std::to_string(i)).c_str(), (std::string("##slidertext23") + std::to_string(i)).c_str(), &UIBridge::mFaceShapeLevel[i], -50, 50))
				{
					nama->UpdateBeauty();
					saveStyleConfig();
				}
			}
			else if (g_faceShapeParamShowFlag[i] == FACE_SHAPE_SHOW_FLAG_NORMAL)
			{
				if (LayoutSlider(ImVec2(22, 22), ImVec2(232, 10), (std::string("##slider2") + std::to_string(i)).c_str(), (std::string("##slidertext2") + std::to_string(i)).c_str(), &UIBridge::mFaceShapeLevel[i], 0, 100))
				{
					nama->UpdateBeauty();
					saveStyleConfig();
				}
			}
			ImGui::PopStyleVar();
		}

		//ImGui::EndChild();
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(246.f / 255.f, 246.f / 255.f, 250.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 1.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
		if (LayoutButton(ImVec2(145, 15), ImVec2(126, 32), u8"恢复默认"))
		{
			resetShapeParam();
			nama->UpdateBeauty();
			saveStyleConfig();
			nama->SetCurrentShape(4);
		}
		ImGui::NewLine();
		ImGui::PopStyleVar(2);

		ImGui::PopStyleColor(10);
	}

	void ShowTabFilter(Nama* nama)
	{
		if (UIBridge::showLightMakeupTip && !UIBridge::showGreenScreen) {
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.3f);
		}
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.f));
		ImGui::Dummy(ImVec2(1 * scaleRatioW, 10 * scaleRatioH));
		ImGui::Dummy(ImVec2(12 * scaleRatioW, 1));
		ImGui::SameLine();
		std::string filterNameArr[6] = { "list_image_origin", "list_image_bailiang1", "list_image_fennen1", "list_image_xiaoqingxin1", "list_image_lengsediao1", "list_image_nuansediao1" };
		std::string filterTextArr[6] = { u8"   原图",  u8"   白亮",  u8"   粉嫩",  u8"  小清新",  u8"  冷色调",  u8"  暖色调" };

		for (int i = 0; i < 6; i++)
		{
			if (LayoutImageButtonWithTextFilter(ImVec2(0.f, 0.f), ImVec2(106 * scaleRatioW, 106 * scaleRatioH), 
				Texture::createTextureFromFile(filterNameArr[i] + ".png", false)->getTextureID(), 
				Texture::createTextureFromFile("list_image_filter_hover.png", false)->getTextureID(),
				Texture::createTextureFromFile("list_image_filter_nor.png", false)->getTextureID(),
				Texture::createTextureFromFile("list_image_filter_selected.png", false)->getTextureID(),
				filterTextArr[i].data(), UIBridge::m_curFilterIdx == i))
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
			if (i == 2) {
				ImGui::Dummy(ImVec2(12 * scaleRatioW, 1));
				ImGui::SameLine();
			}
		}
		ImGui::PopStyleColor(2);
		if (UIBridge::showLightMakeupTip && !UIBridge::showGreenScreen) {
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
			LayoutImageSameLine(ImVec2(34, 86), ImVec2(20, 20), Texture::createTextureFromFile("icon_tips.png", false)->getTextureID(), u8"轻美妆与滤镜无法共用，如需编辑请先取消");
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.3f);
		}
		if (UIBridge::showFilterSlider) {
			ImGui::Dummy(ImVec2(1, 180 * scaleRatioH));
			if (LayoutSlider(ImVec2(20, 200), ImVec2(320, 10), "##slider36", "##slidertext36", &UIBridge::mFilterLevel[UIBridge::m_curFilterIdx], 0, 100))
			{
				nama->UpdateBeauty();
			}
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
			if (UIBridge::mFilterLevel[UIBridge::m_curFilterIdx] == 40.0f) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(246.f / 255.f, 246.f / 255.f, 250.f / 255.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(246.f / 255.f, 246.f / 255.f, 250.f / 255.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(127.f / 255.f, 134.f / 255.f, 152.f / 255.f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 0.3f));
				if (LayoutButton(ImVec2(135, 18), ImVec2(126, 32), u8"恢复默认"))
				{
					UIBridge::mFilterLevel[UIBridge::m_curFilterIdx] = 40.0f;
					nama->UpdateBeauty();
				}
				ImGui::PopStyleColor(5);
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(246.f / 255.f, 246.f / 255.f, 250.f / 255.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 1.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
				if (LayoutButton(ImVec2(135, 18), ImVec2(126, 32), u8"恢复默认"))
				{
					UIBridge::mFilterLevel[UIBridge::m_curFilterIdx] = 40.f;
					nama->UpdateBeauty();
				}
				ImGui::PopStyleColor(5);
			}
			ImGui::PopStyleVar(2);
		}

		if (UIBridge::showLightMakeupTip && !UIBridge::showGreenScreen) {
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
	}

	/*
	这个比较影响性能，参数均为0的时候会关闭
	*/
	void ShowTabBodyBeauty(Nama * nama)
	{
		ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(92.f / 255.f, 96.f / 255.f, 113.f / 255.f, 1.f));
		std::string sliderIconNameArr[MAX_BODY_SHAPE_PARAM] = { "list_icon_slimming_open","list_icon_stovepipe_open", "list_icon_thin_waist_open",
			"list_icon_shoulder_open","list_icon_hip_open",
			"list_icon_little_head_open","list_icon_thin_leg_open" };

		std::string sliderNameArr[MAX_BODY_SHAPE_PARAM] = { u8"   瘦身", u8"   长腿",u8"   瘦腰", u8"   美肩", u8"   美臀" ,u8"    小头", u8"    瘦腿" };
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
				if (LayoutSlider(ImVec2(22, 22), ImVec2(232, 10), ("##slider221" + std::to_string(i)).c_str(), ("##slidertext221" + std::to_string(i)).c_str(), &UIBridge::mBodyShapeLevel[i], -50, 50))
				{
					
					// 添加2s时间，用于显示”AR功能跟美体模块无法共用“的提示
					if(!UIBridge::m_bShowingBodyBeauty){
						UIBridge::mLastTime = ImGui::GetTime() + 2.0;
						// 处理音乐滤镜,关闭音乐
						if (UIBridge::bundleCategory == BundleCategory::MusicFilter && UIBridge::mNeedPlayMP3) {
							nama->pauseCurrentMp3();
						}
					}
					if (UIBridge::bundleCategory == BundleCategory::LightMakeup && UIBridge::showLightMakeupTip) {
						nama->UpdateFilter(UIBridge::m_curFilterIdx);
						UIBridge::showLightMakeupTip = false;
					}
					UIBridge::mLastTime = ImGui::GetTime() + 2.0;
					UIBridge::m_bShowingBodyBeauty = true;
					//UIBridge::showItemSelectWindow = false;
					nama->UpdateBodyShape();
				}
			}
			else
			{
				if (LayoutSlider(ImVec2(22, 22), ImVec2(232, 10), ("##slider221" + std::to_string(i)).c_str(), ("##slidertext221" + std::to_string(i)).c_str(), &UIBridge::mBodyShapeLevel[i], 0, 100))
				{
					// 添加2s时间，用于显示”AR功能跟美体模块无法共用“的提示
					if(!UIBridge::m_bShowingBodyBeauty){
						UIBridge::mLastTime = ImGui::GetTime() + 2.0;
						// 处理音乐滤镜,关闭音乐
						if (UIBridge::bundleCategory == BundleCategory::MusicFilter && UIBridge::mNeedPlayMP3) {
							nama->pauseCurrentMp3();
						}
					}
					if (UIBridge::bundleCategory == BundleCategory::LightMakeup && UIBridge::showLightMakeupTip) {
						nama->UpdateFilter(UIBridge::m_curFilterIdx);
						UIBridge::showLightMakeupTip = false;
					}
					UIBridge::m_bShowingBodyBeauty = true;
					//UIBridge::showItemSelectWindow = false;
					nama->UpdateBodyShape();
				}
			}



			ImGui::PopStyleVar();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

		auto funIsAllClosed = []() {
			
			bool bTestOK = true;

			for (int i = 0; i < MAX_BODY_SHAPE_PARAM; i++)
			{
				if (UIBridge::mBodyShapeLevel[i] != 0) {
					bTestOK = false;
					break;
				}
			}

			return bTestOK;

		};

		if (funIsAllClosed() && UIBridge::m_bShowingBodyBeauty)
		{
			UIBridge::m_bShowingBodyBeauty = false;
			// 处理音乐滤镜，重新开启音乐
			if (UIBridge::bundleCategory == BundleCategory::MusicFilter && UIBridge::mNeedPlayMP3 && !UIBridge::showGreenScreen) {
				nama->resumeCurrentMp3();
			}
			if (UIBridge::bundleCategory == BundleCategory::LightMakeup && UIBridge::m_curRenderItem != -1) {
				nama->setLightMakeupParam(UIBridge::m_lightMakeupParam);
				UIBridge::showLightMakeupTip = true;
			}
			UIBridge::showItemSelectWindow = (UIBridge::bundleCategory != BUNDLE_CATEGORY_NOMEAN);
		}
		ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(246.f / 255.f, 246.f / 255.f, 250.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 1.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		if (LayoutButton(ImVec2(145, 0), ImVec2(126, 32), u8"恢复默认"))
		{
			UIBridge::m_bShowingBodyBeauty = false;
			// 处理音乐滤镜，重新开启音乐
			if (UIBridge::bundleCategory == BundleCategory::MusicFilter && UIBridge::mNeedPlayMP3 && !UIBridge::showGreenScreen) {
				nama->resumeCurrentMp3();
			}
			if (UIBridge::bundleCategory == BundleCategory::LightMakeup && UIBridge::m_curRenderItem != -1) {
				nama->setLightMakeupParam(UIBridge::m_lightMakeupParam);
				UIBridge::showLightMakeupTip = true;
			}
			UIBridge::showItemSelectWindow = (UIBridge::bundleCategory != BUNDLE_CATEGORY_NOMEAN);
			resetBodyShapeParam();
			nama->UpdateBodyShape();
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(6);
	}
}
