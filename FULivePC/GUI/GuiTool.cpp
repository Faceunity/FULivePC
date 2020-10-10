#include "GuiTool.h"
#include "UIBridge.h"
#if _WIN32
#include<windows.h>
#elif __APPLE__
#endif
extern float scaleRatioW;
extern float scaleRatioH;

namespace gui_tool
{

	void ShowHelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	bool ImageButtonWithLabel(const char* text)
	{
		ImGui::BeginGroup();
		ImGui::Text("%s", text);
		ImGui::EndGroup();
		return true;
	}

	bool LayoutButton(const ImVec2& pos, const ImVec2& size, const char* label)
	{
		ImGui::BeginGroup();
		ImGui::Dummy(ImVec2(1, pos.y * scaleRatioH));
		ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine();
		bool ret = ImGui::Button(label, ImVec2(size.x * scaleRatioW, size.y * scaleRatioH));
		ImGui::EndGroup();
		return ret;
	}

	bool LayoutSelectable(const ImVec2& pos, const ImVec2& size, const char* label, bool selected)
	{
		ImGui::BeginGroup();
		ImGui::Dummy(ImVec2(1, pos.y * scaleRatioH));
		ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine();
		bool ret = ImGui::SelectableBorder(label, selected, 0, ImVec2(size.x * scaleRatioW, size.y * scaleRatioH));
		ImGui::EndGroup();
		return ret;
	}

	bool LayoutSlider(const ImVec2& pos, const ImVec2& size, const char* label, float* v, float v_min, float v_max)
	{
		ImGui::BeginGroup();
		ImGui::Dummy(ImVec2(1, pos.y * scaleRatioH));
		ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine();
		ImGui::PushItemWidth(size.x * scaleRatioW);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
		bool ret = ImGui::SliderFloat(label, v, v_min, v_max, "%.f");
		ImGui::PopStyleVar();
		ImGui::PopItemWidth();
		ImGui::EndGroup();
		return ret;
	}

	void LayoutImageSameLine(const ImVec2& pos, const ImVec2& size, ImTextureID user_texture_id, const char* label)
	{
		ImGui::BeginGroup();
		ImGui::Dummy(ImVec2(1, pos.y * scaleRatioH));
		ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine();
		ImGui::Image(user_texture_id, ImVec2(size.x * scaleRatioW, size.y * scaleRatioW));
		ImGui::SameLine(); ImGui::Text(label);
		ImGui::EndGroup();
	}


	void LayoutImage(const ImVec2& pos, const ImVec2& size, ImTextureID user_texture_id, const char* label)
	{
		ImGui::BeginGroup();
		ImGui::Dummy(ImVec2(1, pos.y * scaleRatioH));
		ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine();
		ImGui::Image(user_texture_id, ImVec2(size.x * scaleRatioW, size.y * scaleRatioW));

		if (strlen(label) != 0)
		{
			ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine(); ImGui::Text(label);
		}
		
		ImGui::EndGroup();
	}

	void LayoutImage(RectLayoutDesc image, ImTextureID user_texture_id, ImVec2 textPos, const char* label)
	{
		ImGui::BeginGroup();
		ImGui::Dummy(ImVec2(1, image.pos.y * scaleRatioH));
		ImGui::Dummy(ImVec2(image.pos.x * scaleRatioW, 1)); ImGui::SameLine();
		ImGui::Image(user_texture_id, ImVec2(image.size.x * scaleRatioW, image.size.y * scaleRatioW));
		ImGui::Dummy(ImVec2(textPos.x * scaleRatioW, 1)); ImGui::SameLine(); ImGui::Text(label);
		ImGui::EndGroup();
	}

	bool LayoutImageButtonWithText(const ImVec2& pos, const ImVec2& size, ImTextureID user_texture_id, ImTextureID user_texture_id2, const char* label)
	{
		ImGui::BeginGroup();
		bool ret = ImGui::ImageRoundButton2(user_texture_id, user_texture_id2, ImVec2(size.x * scaleRatioW, size.y * scaleRatioH));/* ImGui::SameLine(0.f, 27.f);*/
		ImGui::Spacing();
		ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine(); ImGui::Text(label);
		ImGui::EndGroup();
		return ret;
	}


	void UpdateFrame2Tex(cv::Mat & frameData, GLuint texID)
	{
		glBindTexture(GL_TEXTURE_2D, texID);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, frameData.step / frameData.elemSize());
		glPixelStorei(GL_UNPACK_ALIGNMENT, (frameData.step & 3) ? 1 : 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frameData.cols, frameData.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, frameData.data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void funGenTex(GLuint & texId)
	{
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	};

	void funDestroyTex(GLuint texID)
	{
		glDeleteTextures(1, &texID);
	}

	void funShowImg(GLuint & texId, float rotio, ImVec2 frameSize, bool bNeedFlip)
	{
		ImVec2 frameUV_LB;
		ImVec2 frameUV_RT;

		if (bNeedFlip)
		{
			frameUV_LB = ImVec2(1, 0);
			frameUV_RT = ImVec2(0, 1);
			if (rotio < 1.7f)
			{
				float UVSub = (1.0f - 1.0f / 1.776f) / 4.0f;
				frameUV_LB = ImVec2(1.f, UVSub);
				frameUV_RT = ImVec2(0.f, 1.f - UVSub);
			}
		}
		else
		{
			frameUV_LB = ImVec2(0, 0);
			frameUV_RT = ImVec2(1, 1);
		}
		
		ImGui::Image((void *)(intptr_t)texId, frameSize, frameUV_LB, frameUV_RT, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
	};

	void generatePureColorMat(cv::Vec4b colorRGBA, cv::Mat & dataOut)
	{
		cv::Vec4b def = { 255,255,255,0 };
		dataOut.release();
		dataOut.create(DEF_COLOR_W, DEF_COLOR_H, CV_8UC4);
		dataOut.setTo(def);

		cv::Scalar color = cv::Scalar(colorRGBA[0], colorRGBA[1], colorRGBA[2], colorRGBA[3]);

		cv::circle(dataOut, { DEF_COLOR_W / 2,DEF_COLOR_W / 2 }, DEF_COLOR_W / 2, color, -1, cv::LINE_AA);
	}

	Texture::SharedPtr createColorTex(cv::Vec4b colorRGBA)
	{
		Texture::SharedPtr texRet = nullptr;

		cv::Mat colordata;
		generatePureColorMat(colorRGBA, colordata);

		if (colordata.data)
		{
			texRet = Texture::createTextureFromData(colordata.cols, colordata.rows, colordata.data);
		}

		return texRet;
	}

	typedef cv::Point3_<uchar> Pixel;

	void generateHSVMat(int iW, int iH, cv::Mat & outData)
	{
		outData.release();
		outData.create(iH, iW, CV_8UC3);
		outData.forEach<Pixel>([&](Pixel &pixel, const int* position) {
			int posY = iH - position[0];
			int posX = position[1];

			//printf("liufei:x,y(%d, %d)", posX, posY);

			pixel.x = (float)posX / iW * 180;  //H
			pixel.y = 255;  //S OK
			pixel.z = (float)posY / iH * 255;  //V
		});
	}

	/*
	
	*/
	Texture::SharedPtr createColorHSV(int iW, int iH, cv::Mat & outRGBMat)
	{
		Texture::SharedPtr texRet = nullptr;

		cv::Mat hsvMat;
		cv::Mat rgbaMat;
		generateHSVMat(iW, iH, hsvMat);

		outRGBMat.release();
		outRGBMat.create(iH, iW, CV_8UC3);
		rgbaMat.create(iH, iW, CV_8UC4);

		if (hsvMat.data)
		{
			cv::cvtColor(hsvMat, outRGBMat, cv::COLOR_HSV2RGB);

			cv::cvtColor(outRGBMat, rgbaMat, cv::COLOR_RGB2RGBA);

			texRet = Texture::createTextureFromData(rgbaMat.cols, rgbaMat.rows, rgbaMat.data);
		}

		return texRet;
	}


	void resetBeautyParam()
	{
		UIBridge::mEnableSkinDect = 1;
		UIBridge::mEnableHeayBlur = 0;
		UIBridge::mEnableExBlur = 0;
		UIBridge::mFaceBeautyLevel[0] = 70;
		UIBridge::mFaceBeautyLevel[1] = 30;
		UIBridge::mFaceBeautyLevel[2] = 30;
		UIBridge::mFaceBeautyLevel[3] = 70;
		UIBridge::mFaceBeautyLevel[4] = 0;
		UIBridge::mFaceBeautyLevel[5] = 0;
		UIBridge::mFaceBeautyLevel[6] = 0;
		UIBridge::mFaceBeautyLevel[7] = 0;
	}

	void resetShapeParam()
	{
		UIBridge::faceType = 0;
		UIBridge::mFaceShapeLevel[0] = 40;
		UIBridge::mFaceShapeLevel[1] = 40;
		UIBridge::mFaceShapeLevel[2] = -20;
		UIBridge::mFaceShapeLevel[3] = -20;
		UIBridge::mFaceShapeLevel[4] = 50;
		UIBridge::mFaceShapeLevel[5] = -10;

		UIBridge::mFaceShapeLevel[6] = 50;
		UIBridge::mFaceShapeLevel[7] = 0;
		UIBridge::mFaceShapeLevel[8] = 0;
		UIBridge::mFaceShapeLevel[9] = 0;
		UIBridge::mFaceShapeLevel[10] = 0;
		UIBridge::mFaceShapeLevel[11] = 0;
		UIBridge::mFaceShapeLevel[12] = 0;
		UIBridge::mFaceShapeLevel[13] = 0;
		UIBridge::mFaceShapeLevel[14] = 0;
	}

	void resetBodyShapeParam()
	{
		memset(UIBridge::mBodyShapeLevel, 0, sizeof(float) * MAX_BODY_SHAPE_PARAM);
	}
#if _WIN32
	int CalWstringWidth(const std::wstring & targetWstring,int fontH,int fontW, int fontWeight)
	{
		HDC hDC = ::GetDC(NULL);
		HFONT hFont = CreateFont(fontH, fontW, 0, 0, FW_DONTCARE, fontWeight, 0, 0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, NULL);
		SelectObject(hDC, hFont);
		LPCTSTR  string = targetWstring.c_str();
		SIZE size = { 0 };
		GetTextExtentPoint32(hDC, string, lstrlen(string), &size);
		RECT rect = { 0 };
		DeleteDC(hDC);
		return size.cx;
	}
#elif __APPLE__

#endif
}
