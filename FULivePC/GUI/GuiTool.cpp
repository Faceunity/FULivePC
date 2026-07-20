#if _WIN32
#include "Windows.h"
#elif __APPLE__
#include "fu_tool_mac.h"
#endif
#include "GuiTool.h"
#include "UIBridge.h"
#include <fu_tool.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include <iostream>
#include <fstream>
#include <map>
extern float scaleRatioW;
extern float scaleRatioH;

using namespace cv;

namespace gui_tool
{
	static bool parseStyleParamObject(const rapidjson::Value& obj, StyleRecommendationParam& tempSRP)
	{
		if (!obj.IsObject()) {
			return false;
		}
		if (obj.HasMember("BeautyLevelDefault"))
		{
			const auto& arrayValue = obj["BeautyLevelDefault"];
			for (rapidjson::SizeType i = 0; i < arrayValue.Size() && i < MAX_BEAUTYFACEPARAMTER; i++)
			{
				tempSRP.mFaceBeautyLevelDefault[i] = arrayValue[i].GetFloat();
			}
		}
		if (obj.HasMember("ShapeLevelDefault"))
		{
			const auto& arrayValue = obj["ShapeLevelDefault"];
			for (rapidjson::SizeType i = 0; i < arrayValue.Size() && i < MAX_FACESHAPEPARAMTER; i++)
			{
				tempSRP.mFaceShapeLevelDefault[i] = arrayValue[i].GetFloat();
			}
		}
		if (obj.HasMember("BeautyLevel"))
		{
			const auto& arrayValue = obj["BeautyLevel"];
			for (rapidjson::SizeType i = 0; i < arrayValue.Size() && i < MAX_BEAUTYFACEPARAMTER; i++)
			{
				tempSRP.mFaceBeautyLevel[i] = arrayValue[i].GetFloat();
			}
		}
		if (obj.HasMember("ShapeLevel"))
		{
			const auto& arrayValue = obj["ShapeLevel"];
			for (rapidjson::SizeType i = 0; i < arrayValue.Size() && i < MAX_FACESHAPEPARAMTER; i++)
			{
				tempSRP.mFaceShapeLevel[i] = arrayValue[i].GetFloat();
			}
		}
		if (obj.HasMember("FilterLevel"))
		{
			tempSRP.mFilterLevel = obj["FilterLevel"].GetInt();
		}
		if (obj.HasMember("BeautyFilterIdx"))
		{
			tempSRP.mBeautyFilterIdx = obj["BeautyFilterIdx"].GetInt();
		}
		if (obj.HasMember("BeautyFilterLevel"))
		{
			tempSRP.mBeautyFilterLevel = obj["BeautyFilterLevel"].GetInt();
		}
		if (obj.HasMember("MakeUpIntensity"))
		{
			tempSRP.mMakeUpIntensity = obj["MakeUpIntensity"].GetInt();
		}
		if (obj.HasMember("BodyBlurLevelDefault"))
		{
			tempSRP.mBodyBlurLevelDefault = obj["BodyBlurLevelDefault"].GetFloat();
		}
		if (obj.HasMember("BodyBlurLevel"))
		{
			tempSRP.mBodyBlurLevel = obj["BodyBlurLevel"].GetFloat();
		}
		if (obj.HasMember("FacialPlumpLevelDefault"))
		{
			tempSRP.mFacialPlumpLevelDefault = obj["FacialPlumpLevelDefault"].GetFloat();
		}
		if (obj.HasMember("FacialPlumpLevel"))
		{
			tempSRP.mFacialPlumpLevel = obj["FacialPlumpLevel"].GetFloat();
		}
		if (obj.HasMember("EyePupilLevelDefault"))
		{
			tempSRP.mEyePupilLevelDefault = obj["EyePupilLevelDefault"].GetFloat();
		}
		if (obj.HasMember("EyePupilLevel"))
		{
			tempSRP.mEyePupilLevel = obj["EyePupilLevel"].GetFloat();
		}
		return true;
	}

	static bool parseStyleConfigContent(const string& json_content, vector<StyleRecommendationParam>& outList)
	{
		rapidjson::Document dom;
		if (dom.Parse(json_content.c_str()).HasParseError() || !dom.IsObject()) {
			return false;
		}
		outList.clear();
		for (rapidjson::Value::ConstMemberIterator itr = dom.MemberBegin(); itr != dom.MemberEnd(); itr++)
		{
			if (!itr->name.IsString()) {
				continue;
			}
			StyleRecommendationParam tempSRP;
			tempSRP.styleName = itr->name.GetString();
			const auto& jValue = itr->value;
			if (jValue.IsArray() && jValue.Size() > 0)
			{
				parseStyleParamObject(jValue[0], tempSRP);
			}
			outList.push_back(tempSRP);
		}
		return !outList.empty();
	}

	static bool readStyleConfigFile(const string& path, vector<StyleRecommendationParam>& outList)
	{
		ifstream in(path.c_str());
		if (!in.is_open()) {
			return false;
		}
		string json_content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
		in.close();
		return parseStyleConfigContent(json_content, outList);
	}

	// 仅按 mStyleParamList 写文件，不把当前 UI 滑杆同步进列表（合并写回时使用）
	static void writeStyleParamListToFile(const string& confPath)
	{
		if (UIBridge::mStyleParamList.empty() || confPath.empty()) {
			return;
		}
		rapidjson::StringBuffer buf;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
		writer.StartObject();
		for (size_t i = 0; i < UIBridge::mStyleParamList.size(); i++) {
			StyleRecommendationParam srp = UIBridge::mStyleParamList.at(i);
			writer.Key(srp.styleName.c_str());
			writer.StartArray();
			writer.StartObject();
			writer.Key("BeautyLevelDefault");
			writer.StartArray();
			for (int j = 0; j < MAX_BEAUTYFACEPARAMTER; j++) {
				writer.Double(srp.mFaceBeautyLevelDefault[j]);
			}
			writer.EndArray();
			writer.Key("ShapeLevelDefault");
			writer.StartArray();
			for (int j = 0; j < MAX_FACESHAPEPARAMTER; j++) {
				writer.Double(srp.mFaceShapeLevelDefault[j]);
			}
			writer.EndArray();
			writer.Key("BeautyLevel");
			writer.StartArray();
			for (int j = 0; j < MAX_BEAUTYFACEPARAMTER; j++) {
				writer.Double(srp.mFaceBeautyLevel[j]);
			}
			writer.EndArray();
			writer.Key("ShapeLevel");
			writer.StartArray();
			for (int j = 0; j < MAX_FACESHAPEPARAMTER; j++) {
				writer.Double(srp.mFaceShapeLevel[j]);
			}
			writer.EndArray();
			writer.Key("FilterLevel");
			writer.Int(srp.mFilterLevel);
			if (srp.mBeautyFilterIdx >= 0) {
				writer.Key("BeautyFilterIdx");
				writer.Int(srp.mBeautyFilterIdx);
				writer.Key("BeautyFilterLevel");
				writer.Int(srp.mBeautyFilterLevel);
			}
			writer.Key("MakeUpIntensity");
			writer.Int(srp.mMakeUpIntensity);
			writer.Key("BodyBlurLevelDefault");
			writer.Double(srp.mBodyBlurLevelDefault);
			writer.Key("BodyBlurLevel");
			writer.Double(srp.mBodyBlurLevel);
			writer.Key("FacialPlumpLevelDefault");
			writer.Double(srp.mFacialPlumpLevelDefault);
			writer.Key("FacialPlumpLevel");
			writer.Double(srp.mFacialPlumpLevel);
			writer.Key("EyePupilLevelDefault");
			writer.Double(srp.mEyePupilLevelDefault);
			writer.Key("EyePupilLevel");
			writer.Double(srp.mEyePupilLevel);
			writer.EndObject();
			writer.EndArray();
		}
		writer.EndObject();
		ofstream outfile(confPath.c_str());
		if (!outfile.is_open()) {
			fprintf(stderr, "fail to open file to write: %s\n", confPath.c_str());
			return;
		}
		outfile << buf.GetString() << endl;
		outfile.close();
	}

	void ShowHelpMarker(const char* desc)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 * scaleRatioH);
		static bool hover = false;
		if (hover) {
			ImGui::Image(Texture::createTextureFromFile("icon_notes_hover.png", false)->getTextureID(), ImVec2(18 * scaleRatioW, 18 * scaleRatioW));
		}
		else {
			ImGui::Image(Texture::createTextureFromFile("icon_notes_nor.png", false)->getTextureID(), ImVec2(18 * scaleRatioW, 18 * scaleRatioW));
		}
		if (ImGui::IsItemHovered())
		{
			hover = true;
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		else {
			hover = false;
		}
	}

	bool ImageButtonWithLabel(const char* text)
	{
		ImGui::BeginGroup();
		ImGui::Text("%s", text);
		ImGui::EndGroup();
		return true;
	}

	bool LayoutButton(const ImVec2& pos, const ImVec2& size, const char* label, int flag)
	{
		ImGui::BeginGroup();
		ImGui::Dummy(ImVec2(1, pos.y * scaleRatioH));
		ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine();
		bool ret = ImGui::Button(label, ImVec2(size.x * scaleRatioW, size.y * scaleRatioH), flag);
		ImGui::EndGroup();
		return ret;
	}

	bool LayoutButton2(const ImVec2& pos, const ImVec2& size, const char* label)
	{
		ImGui::BeginGroup();
		ImGui::Dummy(ImVec2(1, pos.y * scaleRatioH));
		ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine();
		bool ret = ImGui::Button2(label, ImVec2(size.x * scaleRatioW, size.y * scaleRatioH));
		ImGui::EndGroup();
		return ret;
	}

	bool LayoutSelectable(const ImVec2& pos, const ImVec2& size, const char* label, bool selected)
	{
		ImGui::BeginGroup();
		ImGui::Dummy(ImVec2(1, pos.y * scaleRatioH));
		ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
		bool ret = ImGui::SelectableBorder(label, selected, 0, ImVec2(size.x * scaleRatioW, size.y * scaleRatioH));
		ImGui::PopStyleColor();
		ImGui::EndGroup();
		return ret;
	}

	bool LayoutSlider(const ImVec2& pos, const ImVec2& size, const char* label, const char* label2, float* v, float v_min, float v_max)
	{
		ImGui::BeginGroup();
		ImGui::Dummy(ImVec2(1, pos.y * scaleRatioH));
		ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1));
		ImGui::SameLine();
		ImGui::PushItemWidth(size.x * scaleRatioW);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(119.f / 255.f, 135.f / 255.f, 233.f / 255.f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238 / 255.f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
		bool ret1 = ImGui::SliderFloat(label, v, v_min, v_max, "%.f");
		ImGui::PopStyleColor(6);
		ImGui::PopStyleVar(2);
		ImGui::PopItemWidth();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f));
		ImGui::SameLine(0, 16 * scaleRatioW);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 10);
		ImGui::PushItemWidth(32 * scaleRatioW);
		bool ret2 = ImGui::DragFloat(label2, v, 1, v_min, v_max, "%.f");
		if (*v > v_max) {
			*v = v_max;
		}
		else if (*v < v_min) {
			*v = v_min;
		}
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndGroup();
		return ret1 || ret2;
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
			ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1));
			ImGui::SameLine();
			ImGui::Text(label);
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
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.0f));
		bool ret = ImGui::ImageRoundButton2(user_texture_id, user_texture_id2, ImVec2(size.x * scaleRatioW, size.y * scaleRatioH));/* ImGui::SameLine(0.f, 27.f);*/
		ImGui::Spacing();
		ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine(); ImGui::Text(label);
		ImGui::PopStyleColor();
		ImGui::EndGroup();
		return ret;
	}

	bool LayoutImageButtonWithTextFilter(const ImVec2& pos, const ImVec2& size, ImTextureID user_texture_id, ImTextureID user_texture_id2, ImTextureID user_texture_id3, ImTextureID user_texture_id4, const char* label, bool select)
	{
		ImGui::BeginGroup();
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
		bool ret = ImGui::ImageButton(user_texture_id, ImVec2(size.x, size.y));/* ImGui::SameLine(0.f, 27.f);*/

		//ImGui::SetCursorPosX(ImGui::GetCursorPosX());
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - size.y - 8);
		if (select) {
			ImGui::Image(user_texture_id4, ImVec2(size.x, size.y));
		}
		else {
			if (ImGui::IsItemHovered())
			{
				ImGui::Image(user_texture_id2, ImVec2(size.x, size.y));
			}
			else {
				ImGui::Image(user_texture_id3, ImVec2(size.x, size.y));
			}
		}
		if (select) {
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 29 * scaleRatioW);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 63 * scaleRatioH);
		}
		else {
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 29 * scaleRatioW);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 27 * scaleRatioH);
		}
		ImGui::Text(label);
		ImGui::PopStyleColor(2);
		ImGui::EndGroup();
		return ret;
	}

	bool LayoutRectImageButtonWithText(const ImVec2& pos, const ImVec2& size, ImTextureID user_texture_id, const char* label)
	{
		ImGui::BeginGroup();
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
		bool ret = ImGui::ImageButton(user_texture_id, ImVec2(size.x * scaleRatioW, size.y * scaleRatioH));/* ImGui::SameLine(0.f, 27.f);*/
		ImGui::Spacing();
		int length = 5 - strlen(label) / 3;
		ImGui::Dummy(ImVec2(pos.x * scaleRatioW * length, 1)); ImGui::SameLine(); ImGui::Text(label);
		ImGui::PopStyleColor(3);
		ImGui::EndGroup();
		return ret;
	}


	void UpdateFrame2Tex(cv::Mat& frameData, GLuint texID)
	{
		glBindTexture(GL_TEXTURE_2D, texID);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, frameData.step / frameData.elemSize());
		glPixelStorei(GL_UNPACK_ALIGNMENT, (frameData.step & 3) ? 1 : 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frameData.cols, frameData.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, frameData.data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void funGenTex(GLuint& texId)
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

	void funShowImg(GLuint& texId, float rotio, ImVec2 frameSize, bool bNeedFlip)
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

		ImGui::Image((void*)(intptr_t)texId, frameSize, frameUV_LB, frameUV_RT, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
	};

	void generatePureColorMat(cv::Vec4b colorRGBA, cv::Mat& dataOut)
	{
		cv::Vec4b def = { 255,255,255,0 };
		dataOut.release();
		dataOut.create(DEF_COLOR_W, DEF_COLOR_H, CV_8UC4);
		dataOut.setTo(def);

		cv::Scalar color = cv::Scalar(colorRGBA[0], colorRGBA[1], colorRGBA[2], colorRGBA[3]);

		cv::circle(dataOut, { DEF_COLOR_W / 2,DEF_COLOR_W / 2 }, DEF_COLOR_W / 2, color, -1, cv::LINE_AA);
	}

	void CutCircleInMiddle(cv::Mat& dataIn, cv::Mat& dataOut) {

		Mat& srcImg = dataIn;
		Mat maskImg;

		maskImg = Mat(srcImg.rows, srcImg.cols, CV_8UC1, Scalar(0));

		dataOut = srcImg.clone();
		maskImg = Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC1);

		int X = srcImg.cols / 2;
		int Y = srcImg.rows / 2;
		cv::Point pt_o = cv::Point(X, Y);
		int r = MIN(srcImg.rows, srcImg.cols) / 2;

		cv::circle(maskImg, pt_o, r, Scalar(255), -1);

		//带阿尔法通道的原始图
		if (srcImg.channels() == 4)
		{
			int height = srcImg.rows;
			int width = srcImg.cols;
			int sum = 0;
			for (int row = 0; row < height; row++) {
				for (int col = 0; col < width; col++) {
					int srcAlpha = srcImg.at<cv::Vec4b>(row, col)[3];
					int maskAlpha = maskImg.at<cv::uint8_t>(row, col);
					if (maskAlpha == 255)
					{
						//圆圈区域赋值成阿尔法原始值
						maskImg.at<cv::uint8_t>(row, col) = srcAlpha;
					}
				}
			}
		}


		//imshow("testMask", maskImg);

		cv::Mat bgra[4];
		cv::split(dataOut, bgra);
		bgra[3] = maskImg;
		cv::merge(bgra, 4, dataOut);

		//imshow("testOut", dataOut);
	}

	void generateTwoColorMat(cv::Vec4b colorRGBA0, cv::Vec4b colorRGBA1, cv::Mat& dataOut)
	{
		cv::Mat img(cv::Size(DEF_COLOR_W * 2, DEF_COLOR_H * 2), CV_8UC3);

		rectangle(img, cv::Rect(0, 0, DEF_COLOR_W * 2, DEF_COLOR_H), colorRGBA0, -1);
		rectangle(img, cv::Rect(0, DEF_COLOR_H, DEF_COLOR_W * 2, DEF_COLOR_H), colorRGBA1, -1);

		//imshow("img1", img);

		CutCircleInMiddle(img, dataOut);
	}

	void generateThreeColorMat(cv::Vec4b colorRGBA0, cv::Vec4b colorRGBA1, cv::Vec4b colorRGBA2, cv::Mat& dataOut)
	{

		cv::Mat img;
		img.create(DEF_COLOR_W * 3, DEF_COLOR_H * 3, CV_8UC4);

		rectangle(img, cv::Rect(0, 0, DEF_COLOR_W * 3, DEF_COLOR_H), colorRGBA0, -1);
		rectangle(img, cv::Rect(0, DEF_COLOR_H, DEF_COLOR_W * 3, DEF_COLOR_H), colorRGBA1, -1);
		rectangle(img, cv::Rect(0, DEF_COLOR_H * 2, DEF_COLOR_W * 3, DEF_COLOR_H), colorRGBA2, -1);

		CutCircleInMiddle(img, dataOut);

	}

	Texture::SharedPtr createColorTex(std::vector<cv::Vec4b> vecColorRGBA)
	{
		Texture::SharedPtr texRet = nullptr;

		cv::Mat colordata;
		int nCnt = vecColorRGBA.size();
		if (nCnt == 1)
		{
			generatePureColorMat(vecColorRGBA[0], colordata);
		}
		else if (nCnt == 2)
		{
			generateTwoColorMat(vecColorRGBA[0], vecColorRGBA[1], colordata);
		}
		else if (nCnt == 3)
		{
			generateThreeColorMat(vecColorRGBA[0], vecColorRGBA[1], vecColorRGBA[2], colordata);
		}


		if (colordata.data)
		{
			texRet = Texture::createTextureFromData(colordata.cols, colordata.rows, colordata.data);
		}

		return texRet;
	}

	typedef cv::Point3_<uchar> Pixel;

	void generateHSVMat(int iW, int iH, cv::Mat& outData)
	{
		outData.release();
		outData.create(iH, iW, CV_8UC3);
		outData.forEach<Pixel>([&](Pixel& pixel, const int* position) {
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
	Texture::SharedPtr createColorHSV(int iW, int iH, cv::Mat& outRGBMat)
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
	void readStyleConfig() {
		UIBridge::mStyleParamList.clear();

		// 包内配置作为基准（含新增风格、且保持与 bundle 序号对齐的顺序）
		string bundledPath = FuTool::GetFileFullPathFromeSearchPath(gCustomStyleConfig.c_str());
		vector<StyleRecommendationParam> bundledList;
		if (!readStyleConfigFile(bundledPath, bundledList)) {
			fprintf(stderr, "fail to read bundled style json: %s\n", gCustomStyleConfig.data());
		}

		string userPath = "";
#ifdef __APPLE__
		userPath = FuToolMac::GetDocumentPath() + "/style_setup.json";
#else
		// Windows 直接读写 assets 同路径；无独立 Documents 副本时不做合并
		userPath = bundledPath;
#endif

		vector<StyleRecommendationParam> userList;
		bool hasUserFile = false;
#ifdef __APPLE__
		hasUserFile = readStyleConfigFile(userPath, userList);
#else
		// 非 Mac：仅用包内（或 search path）结果
		hasUserFile = false;
#endif

		if (bundledList.empty() && hasUserFile) {
			// 包内失败时退化为用户文件
			UIBridge::mStyleParamList = userList;
			return;
		}
		if (bundledList.empty()) {
			return;
		}

		bool needMergeWriteBack = false;
		if (hasUserFile) {
			map<string, StyleRecommendationParam> userMap;
			for (size_t i = 0; i < userList.size(); i++) {
				userMap[userList[i].styleName] = userList[i];
			}
			UIBridge::mStyleParamList.clear();
			UIBridge::mStyleParamList.reserve(bundledList.size());
			for (size_t i = 0; i < bundledList.size(); i++) {
				auto it = userMap.find(bundledList[i].styleName);
				if (it != userMap.end()) {
					// 保留用户已调过的参数
					UIBridge::mStyleParamList.push_back(it->second);
				} else {
					// 旧配置缺失的新风格，用包内默认补齐
					UIBridge::mStyleParamList.push_back(bundledList[i]);
					needMergeWriteBack = true;
				}
			}
			if (userList.size() < bundledList.size()) {
				needMergeWriteBack = true;
			}
			if (needMergeWriteBack) {
				writeStyleParamListToFile(userPath);
			}
		} else {
			UIBridge::mStyleParamList = bundledList;
#ifdef __APPLE__
			// Documents 尚无配置时写入一份，后续用户调节可保存在此
			writeStyleParamListToFile(userPath);
#endif
		}
	}

	void saveStyleConfig() {
		if (UIBridge::mStyleParamList.size() == 0) {
			return;
		}
		StyleRecommendationParam tempSRP = UIBridge::mStyleParamList.at(UIBridge::mStyleRecommendationIndex);
		for (int i = 0; i < MAX_BEAUTYFACEPARAMTER; i++) {
			tempSRP.mFaceBeautyLevel[i] = UIBridge::mFaceBeautyLevel[i];
		}
		for (int i = 0; i < MAX_FACESHAPEPARAMTER; i++) {
			tempSRP.mFaceShapeLevel[i] = UIBridge::mFaceShapeLevel[i];
		}
		tempSRP.mBodyBlurLevel = UIBridge::mBodyBlurLevel;
		tempSRP.mFacialPlumpLevel = UIBridge::mFacialPlumpLevel;
		tempSRP.mEyePupilLevel = UIBridge::mEyePupilLevel;
		UIBridge::mStyleParamList.at(UIBridge::mStyleRecommendationIndex) = tempSRP;

		rapidjson::StringBuffer buf;
		//rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf); // it can word wrap
		writer.StartObject();
		for (int i = 0; i < UIBridge::mStyleParamList.size(); i++) {
			StyleRecommendationParam srp = UIBridge::mStyleParamList.at(i);
			writer.Key(srp.styleName.c_str());
			writer.StartArray();
			writer.StartObject();
			writer.Key("BeautyLevelDefault");
			writer.StartArray();
			for (int j = 0; j < MAX_BEAUTYFACEPARAMTER; j++) {
				writer.Double(srp.mFaceBeautyLevelDefault[j]);
			}
			writer.EndArray();
			writer.Key("ShapeLevelDefault");
			writer.StartArray();
			for (int j = 0; j < MAX_FACESHAPEPARAMTER; j++) {
				writer.Double(srp.mFaceShapeLevelDefault[j]);
			}
			writer.EndArray();
			writer.Key("BeautyLevel");
			writer.StartArray();
			for (int j = 0; j < MAX_BEAUTYFACEPARAMTER; j++) {
				writer.Double(srp.mFaceBeautyLevel[j]);
			}
			writer.EndArray();
			writer.Key("ShapeLevel");
			writer.StartArray();
			for (int j = 0; j < MAX_FACESHAPEPARAMTER; j++) {
				writer.Double(srp.mFaceShapeLevel[j]);
			}
			writer.EndArray();
			writer.Key("FilterLevel");
			writer.Int(srp.mFilterLevel);
			if (srp.mBeautyFilterIdx >= 0) {
				writer.Key("BeautyFilterIdx");
				writer.Int(srp.mBeautyFilterIdx);
				writer.Key("BeautyFilterLevel");
				writer.Int(srp.mBeautyFilterLevel);
			}
			writer.Key("MakeUpIntensity");
			writer.Int(srp.mMakeUpIntensity);
			writer.Key("BodyBlurLevelDefault");
			writer.Double(srp.mBodyBlurLevelDefault);
			writer.Key("BodyBlurLevel");
			writer.Double(srp.mBodyBlurLevel);
			writer.Key("FacialPlumpLevelDefault");
			writer.Double(srp.mFacialPlumpLevelDefault);
			writer.Key("FacialPlumpLevel");
			writer.Double(srp.mFacialPlumpLevel);
			writer.Key("EyePupilLevelDefault");
			writer.Double(srp.mEyePupilLevelDefault);
			writer.Key("EyePupilLevel");
			writer.Double(srp.mEyePupilLevel);
			writer.EndObject();
			writer.EndArray();
		}
		writer.EndObject();
		string ConfPath = "";
#ifdef __APPLE__
		ConfPath = FuToolMac::GetDocumentPath() + "/style_setup.json";
#else
		ConfPath = gCustomStyleConfig;
#endif 
		ofstream outfile;
		outfile.open(ConfPath.c_str());
		if (!outfile.is_open()) {
			fprintf(stderr, "fail to open file to write: %s\n", gCustomStyleConfig.data());
		}

		outfile << buf.GetString() << endl;
		outfile.close();
	}

	void loadStyleParam()
	{
		StyleRecommendationParam tempSRP = UIBridge::mStyleParamList.at(UIBridge::mStyleRecommendationIndex);
		for (int i = 0; i < MAX_BEAUTYFACEPARAMTER; i++) {
			UIBridge::mFaceBeautyLevel[i] = tempSRP.mFaceBeautyLevel[i];
		}
		for (int i = 0; i < MAX_FACESHAPEPARAMTER; i++) {
			UIBridge::mFaceShapeLevel[i] = tempSRP.mFaceShapeLevel[i];
		}
		UIBridge::mBodyBlurLevel = tempSRP.mBodyBlurLevel;
		UIBridge::mFacialPlumpLevel = tempSRP.mFacialPlumpLevel;
		UIBridge::mEyePupilLevel = tempSRP.mEyePupilLevel;
		if (tempSRP.mBeautyFilterIdx >= 0) {
			UIBridge::m_curFilterIdx = tempSRP.mBeautyFilterIdx;
			UIBridge::mFilterLevel[tempSRP.mBeautyFilterIdx] = float(tempSRP.mBeautyFilterLevel);
			UIBridge::showFilterSlider = tempSRP.mBeautyFilterIdx != 0;
		} else {
			UIBridge::m_curFilterIdx = 0;
			UIBridge::mFilterLevel[0] = 0.f;
			UIBridge::showFilterSlider = false;
		}
	}

	void resetBeautyParam()
	{
		StyleRecommendationParam tempSRP = UIBridge::mStyleParamList.at(UIBridge::mStyleRecommendationIndex);
		for (int i = 0; i < MAX_BEAUTYFACEPARAMTER; i++) {
			tempSRP.mFaceBeautyLevel[i] = tempSRP.mFaceBeautyLevelDefault[i];
			UIBridge::mFaceBeautyLevel[i] = tempSRP.mFaceBeautyLevelDefault[i];
		}
		tempSRP.mBodyBlurLevel = tempSRP.mBodyBlurLevelDefault;
		UIBridge::mBodyBlurLevel = tempSRP.mBodyBlurLevelDefault;
		tempSRP.mFacialPlumpLevel = tempSRP.mFacialPlumpLevelDefault;
		UIBridge::mFacialPlumpLevel = tempSRP.mFacialPlumpLevelDefault;
		tempSRP.mEyePupilLevel = tempSRP.mEyePupilLevelDefault;
		UIBridge::mEyePupilLevel = tempSRP.mEyePupilLevelDefault;
		UIBridge::mStyleParamList.at(UIBridge::mStyleRecommendationIndex) = tempSRP;
	}

	void resetShapeParam()
	{
		UIBridge::faceType = 0;
		StyleRecommendationParam tempSRP = UIBridge::mStyleParamList.at(UIBridge::mStyleRecommendationIndex);
		for (int i = 0; i < MAX_FACESHAPEPARAMTER; i++) {
			tempSRP.mFaceShapeLevel[i] = tempSRP.mFaceShapeLevelDefault[i];
			UIBridge::mFaceShapeLevel[i] = tempSRP.mFaceShapeLevelDefault[i];
		}
		tempSRP.mEyePupilLevel = tempSRP.mEyePupilLevelDefault;
		UIBridge::mEyePupilLevel = tempSRP.mEyePupilLevelDefault;
		UIBridge::mStyleParamList.at(UIBridge::mStyleRecommendationIndex) = tempSRP;
	}

	void resetBodyShapeParam()
	{
		memset(UIBridge::mBodyShapeLevel, 0, sizeof(float) * MAX_BODY_SHAPE_PARAM);
		UIBridge::mBreastStrengthLevel = 0.f;
	}
#if _WIN32
	int CalWstringWidth(const std::wstring& targetWstring, int fontH, int fontW, int fontWeight)
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
