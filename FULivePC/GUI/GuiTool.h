#ifndef _GUI_TOOL_
#define _GUI_TOOL_

#include "gl3w.h"
#include "glfw3.h"
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include "glfw3native.h"


#include "imgui/imgui.h"
#include "imgui/imgui_tabs.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "Texture.h"

#define DEF_FRAME_SHOW_POS_X (10 * scaleRatioW)
#define DEF_FRAME_SHOW_POS_Y (156 * scaleRatioH)
#define DEF_FRAME_SHOW_WIDTH (888 * scaleRatioW)
#define DEF_FRAME_SHOW_HEIGHT (500 * scaleRatioH)

#define BUNDLE_CATEGORY_NOMEAN (-999)

#define GS_START (BundleCategory::GreenScreen)

#define DEF_COLOR_W (150)
#define DEF_COLOR_H (150)

const cv::Vec4b g_color1 = { 0, 255, 0, 255 };
const cv::Vec4b g_color2 = { 0, 0, 255, 255 };
const cv::Vec4b g_color3 = { 40, 0, 55,   255 };
const cv::Vec4b g_color4 = { 0,  80,  95, 255 };

namespace gui_tool
{
	typedef struct tagRectLayoutDesc
	{
		ImVec2 pos;
		ImVec2 size;
	}RectLayoutDesc;


	void ShowHelpMarker(const char* desc);

	bool ImageButtonWithLabel(const char* text);

	bool LayoutButton(const ImVec2& pos, const ImVec2& size, const char* label);

	bool LayoutSelectable(const ImVec2& pos, const ImVec2& size, const char* label, bool selected);

	bool LayoutSlider(const ImVec2& pos, const ImVec2& size, const char* label, float* v, float v_min, float v_max);

	void LayoutImageSameLine(const ImVec2& pos, const ImVec2& size, ImTextureID user_texture_id, const char* label);

	void LayoutImage(const ImVec2& pos, const ImVec2& size, ImTextureID user_texture_id, const char* label);

	void LayoutImage(RectLayoutDesc image, ImTextureID user_texture_id, ImVec2 textPos, const char* label);

	bool LayoutImageButtonWithText(const ImVec2& pos, const ImVec2& size, ImTextureID user_texture_id, ImTextureID user_texture_id2, const char* label);

	bool LayoutRectImageButtonWithText(const ImVec2& pos, const ImVec2& size, ImTextureID user_texture_id, const char* label);

	void UpdateFrame2Tex(cv::Mat & frameData, GLuint texID);

	void funGenTex(GLuint & texId);

	void funDestroyTex(GLuint texID);

	void funShowImg(GLuint & texId, float rotio, ImVec2 frameSize, bool bNeedFlip);

	void generatePureColorMat(cv::Vec4b colorRGBA, cv::Mat & dataOut);

	void generateTwoColorMat(cv::Vec4b colorRGBA0, cv::Vec4b colorRGBA1, cv::Mat & dataOut);

	void generateThreeColorMat(cv::Vec4b colorRGBA0, cv::Vec4b colorRGBA1, cv::Vec4b colorRGBA2, cv::Mat & dataOut);

	void generateHSVMat(int iW, int iH, cv::Mat & outData);

	Texture::SharedPtr createColorTex(std::vector<cv::Vec4b> vecColorRGBA);

	Texture::SharedPtr createColorHSV(int iW, int iH, cv::Mat & outRGB);

	void CutCircleInMiddle(cv::Mat & dataIn, cv::Mat & dataOut);

	void resetBeautyParam();

	void resetShapeParam();

	void resetBodyShapeParam();

	int CalWstringWidth(const std::wstring & targetWstring, int fontH, int fontW, int fontWeight);

	typedef struct tagColorBag
	{
		Texture::SharedPtr pTex;
		std::vector<cv::Vec4b> vecColorRGBA;

		tagColorBag()
		{
			pTex = nullptr;
		}

		/* 注意当前的GL上下文，目前都是给ImgUI的 */
		tagColorBag(cv::Vec4b vec)
		{
			setColor(vec);
		}

		/* 后续支持下双色和三色的显示 */
		tagColorBag(cv::Vec4b vec0, cv::Vec4b vec1)
		{
			setColor(vec0, vec1);
		}

		tagColorBag(cv::Vec4b vec0, cv::Vec4b vec1, cv::Vec4b vec2)
		{
			setColor(vec0, vec1, vec2);
		}

		tagColorBag(cv::Vec4b vec, void * )
		{
			vecColorRGBA.push_back(vec);
			pTex = nullptr;
		}

		void setColor(cv::Vec4b vec)
		{
			vecColorRGBA.clear();
			vecColorRGBA.push_back(vec);
			pTex.reset();
			pTex = gui_tool::createColorTex(vecColorRGBA);
		}

		void setColor(cv::Vec4b vec0, cv::Vec4b vec1)
		{
			vecColorRGBA.push_back(vec0);
			vecColorRGBA.push_back(vec1);
			pTex.reset();
			pTex = gui_tool::createColorTex(vecColorRGBA);
		}

		void setColor(cv::Vec4b vec0, cv::Vec4b vec1, cv::Vec4b vec2)
		{
			vecColorRGBA.push_back(vec0);
			vecColorRGBA.push_back(vec1);
			vecColorRGBA.push_back(vec2);
			pTex.reset();
			pTex = gui_tool::createColorTex(vecColorRGBA);
		}

	}ColorBag;


	struct MyDocument
	{
		const char* Name;
		bool        Open;
		bool        PreviousOpen;
		bool        Dirty;
		bool        WantClose;

		MyDocument() { Name = NULL; Open = true;  PreviousOpen = false; Dirty = false; WantClose = false; }
		void DoOpen() { Open = true; }
		void DoQueueClose() { WantClose = true; }
		void DoForceClose() { Open = false; Dirty = false; }
		void DoSave() { Dirty = false; }
	};
}


#endif
