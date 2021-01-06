﻿
#include "Gui.h"
//#include "Config.h"	
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "stdio.h"
#include "fu_tool.h"
#include "GuiGS.h"
#include "GuiTabContent.h"
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "imgui/imgui_internal.h"
#include "GuiCustomMakeup.h"
#if __APPLE__
#include "fu_tool_mac.h"
#else
#include "VirtualCamera.h"
#endif 
using namespace gui_tool;

int oriWindowWidth = 0;
int oriWindowHeight = 0;
float scaleRatioW = 1.f;
float scaleRatioH = 1.f;
bool showUI = true;

int UIBridge::bundleCategory = BUNDLE_CATEGORY_NOMEAN;
int UIBridge::renderBundleCategory = -1;
vector<string> UIBridge::categoryBundles[BundleCategory::Count];
int UIBridge::faceType = 0;

bool UIBridge::showItemSelectWindow = false;
bool UIBridge::showItemTipsWindow = false;
bool UIBridge::showDegubInfoWindow = false;
bool UIBridge::showFilterSlider = false;
int UIBridge::showGreenScreen = false;
bool UIBridge::showCustomMakeup = false;


 uint32_t UIBridge::mFPS = 60;
 uint32_t UIBridge::mResolutionWidth = 1280;
 uint32_t UIBridge::mResolutionHeight = 720;
 uint32_t UIBridge::mRenderTime = 33;

int UIBridge::m_curFilterIdx = 2;  //默认粉嫩
int UIBridge::m_curRenderItem = -1; 
int UIBridge::m_curBindedItem = -1;
int UIBridge::m_curPlayingMusicItem = -1;
ImGuiID UIBridge::m_curRenderItemUIID = -1;
ImGuiID UIBridge::m_curRenderItemUIIDSec = -2;
string UIBridge::mCurRenderItemName = "";
double UIBridge::mLastTime=0.0;
int UIBridge::mEnableSkinDect = 1;
int UIBridge::mEnableHeayBlur = 0;
int UIBridge::mEnableExBlur = 0;
int UIBridge::mSelectedCamera = 0;
float UIBridge::mFaceBeautyLevel[MAX_BEAUTYFACEPARAMTER] = {0.0f};

float UIBridge::mFaceShapeLevel[MAX_FACESHAPEPARAMTER] = {0.0f};
float UIBridge::mFilterLevel[10] = { 100,100,40,100,100, 100,100,100,100,100};
float UIBridge::mMakeupLevel[10] = { 100,100,100,100,100, 100,100,100,100,100 };
float UIBridge::mBodyShapeLevel[MAX_BODY_SHAPE_PARAM] = { 0.0f };

bool UIBridge::mNeedIpcWrite =false;
bool UIBridge::mNeedPlayMP3 = false;
bool UIBridge::mNeedStopMP3 = false;

bool Gui::mIsOpenMiniWindow = true;
//{ "blur_level","color_level", "red_level", "eye_bright", "tooth_whiten" ,"remove_pouch_strength", "remove_nasolabial_folds_strength" };

const char * g_szMainFrameWinName = "MainFrameWindow";

bool doesFileExist(const string& filename)
{
#ifdef _WIN32
	DWORD attr = GetFileAttributesA(filename.c_str());
	return (attr != INVALID_FILE_ATTRIBUTES);
#endif
    
    return true;
}

bool isDirectoryExists(const string& filename)
{
#ifdef _WIN32
	DWORD attr = GetFileAttributesA(filename.c_str());
	return ((attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY));
#endif
    
    return true;
}

#ifndef _WIN32

static uint64_t GetTickCount()
{
    struct timespec tTemp = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &tTemp);
    uint64_t lRunMS = tTemp.tv_sec*1000 + tTemp.tv_nsec/1000000 ;

    return lRunMS;
}

#endif

int get_fps()
{
	static int fps = 0;
    
	static uint64_t lastTime = GetTickCount(); // ms
	static int frameCount = 0;

	++frameCount;

	uint64_t curTime = GetTickCount();
	if (curTime - lastTime > 1000)
	{
		fps = frameCount-1;
		frameCount = 0;
		lastTime = curTime;
	}
    
	return fps;
}

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{
	cout << width << "," << height << endl;
	scaleRatioW =(float)width/(float)oriWindowWidth ;
	scaleRatioH = (float)height / (float)oriWindowHeight;
}

#ifdef _WIN32
HWND Gui::hWindow;
#else
void * Gui::hWindow = NULL;
#endif

GLFWwindow* Gui::window;
Gui::UniquePtr Gui::create(uint32_t width, uint32_t height)
{
	UniquePtr pGui = UniquePtr(new Gui);
	
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return Gui::UniquePtr();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, 1);
	//glfwWindowHint(GLFW_DECORATED, false);//SetWindowPostion()
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	/*GLFWwindow**/ window = glfwCreateWindow(width, height, "FU Live Demo PC", NULL, NULL);
	if (!window)
	{
		cout << "Create Window Faild !!! Please Check GPU Card If Support OpenGL 3.2 CoreProfile";
		return nullptr;
	}

	glfwMakeContextCurrent(window);
#if __APPLE__
	hWindow = glfwGetCocoaWindow(window);
#else
    hWindow = glfwGetWin32Window(window);
#endif
	glfwSwapInterval(1); // Enable vsync
	gl3wInit();
	glfwSetWindowSizeCallback(window, window_size_callback);
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, 1.00f); 
	colors[ImGuiCol_Text] = ImColor(0, 0, 0, 255);
	colors[ImGuiCol_Button] = ImColor(252, 253, 255, 255);
	colors[ImGuiCol_ButtonHovered] = ImColor(152, 165, 245, 255); 
	colors[ImGuiCol_ButtonActive] = ImColor(112, 136, 237, 255);
	colors[ImGuiCol_ScrollbarBg] = ImColor(252, 253, 255, 255);
	colors[ImGuiCol_ScrollbarGrab] = ImColor(112, 136, 237, 255);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(112, 136, 237, 255);
	colors[ImGuiCol_PopupBg] = ImColor(252, 253, 255, 255);

	string fontFile = FuTool::GetFileFullPathFromeSearchPath("msyh.ttc");

	if (doesFileExist(fontFile))
	{
		io.Fonts->AddFontFromFileTTF(fontFile.c_str(), 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	}
	oriWindowWidth = width;
	oriWindowHeight = height;
	GLFWimage icons[1];
	Texture::SharedPtr tex = Texture::createTextureFromFile("LOGO.png", false);
    pGui->mouseControl = new MouseControl(GUIGS::previewRect);
	return pGui;
}

Gui::~Gui()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

static MyDocument GDocs[4];

static void ShowTabs(const char* title, bool* p_open, Nama::UniquePtr& nama)
{
	ImGui::Begin(title, p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

	if (GDocs[0].Name == NULL)
	{
		GDocs[0].Name = u8"美肤";
		GDocs[1].Name = u8"美型";
		GDocs[2].Name = u8"滤镜";
		GDocs[3].Name = u8"美体";
	}
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(120.f / 255.f, 136.f / 255.f, 234.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 1.f)); 
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(150.f / 255.f, 157.f / 255.f, 181.f / 255.f, 1.f));
	
	// Tabs
	ImGui::BeginTabBar("##tabs1",  ImGuiTabBarFlags_NoAnim | ImGuiTabBarFlags_SizingPolicyEqual);
	for (int doc_n = 0; doc_n < IM_ARRAYSIZE(GDocs); doc_n++)
	{
		MyDocument& doc = GDocs[doc_n];

		const bool selected = ImGui::TabItem(ImVec2(100 * scaleRatioW,30* scaleRatioH),doc.Name, &doc.Open, 0);

		if (!selected)
			continue;
		
		switch (doc_n)
		{
		case 0:
		{
			gui_tab_content::ShowTabBeautySkin(nama.get());
		}
		break;
		case 1:
		{
			gui_tab_content::ShowTabFaceBeauty(nama.get());
		}
		break;
		case 2:
		{
			gui_tab_content::ShowTabFilter(nama.get());
		}
		break;
		case 3:
		{
			gui_tab_content::ShowTabBodyBeauty(nama.get());
		}
		break;
		}
	}
	ImGui::EndTabBar();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	
	ImGui::End();
}

static void ShowAvatarTip()
{
	ImGui::Begin("avatarTip", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

	ImGui::Dummy(ImVec2(480 * scaleRatioW, 280 * scaleRatioH));

	LayoutImageSameLine(ImVec2(0, 0), ImVec2(20, 20), Texture::createTextureFromFile("icon_tips.png", false)->getTextureID(), u8"全身Avatar开启后美颜模块无法使用 如需编辑请先取消");

	ImGui::End();
}

static void ShowCustomMakeupTip(Nama * nama)
{
	
	GUICustomMakeup::Draw(nama);

}

static void ShowAvatarMenu(Nama * nama)
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::SetNextWindowPos(ImVec2(19 * scaleRatioW, 584 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300 * scaleRatioW, 80 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("avatarChange", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

	string strIconName = "";
	auto avaType = nama->GetAvatarType();
	switch (avaType)
	{
	case NamaExampleNameSpace::AVATAR_TYPE_FULLBODY:
		strIconName = "switch_whole_body.png";
		break;
	case NamaExampleNameSpace::AVATAR_TYPE_HALF:
		strIconName = "switch_half_body.png";
		break;
	default:
		break;
	}

	Texture::SharedPtr tex = Texture::createTextureFromFile(strIconName, false);
	//找不到图标
	if (!tex)
	{
		tex = Texture::createTextureFromFile("icon_Movebutton_nor.png", false);
	}

	if (ImGui::ImageButton(tex->getTextureID(), ImVec2(88 * scaleRatioW, 28 * scaleRatioH)))
	{
		nama->SwitchAvatar();
	}

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}



static void ShowAvatarMiniCameraView(GLuint & texId, float rotio, ImVec2 frameSize)
{

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::SetNextWindowPos(ImVec2(660 * scaleRatioW, 515 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(frameSize, ImGuiCond_Always);
	ImGui::Begin("avatarMiniCamera", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

	funShowImg(texId, rotio, frameSize, true);

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

static void ShowDebugMenu()
{
	if (UIBridge::showDegubInfoWindow)
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(82.f / 255.f, 88.f / 255.f, 112.f / 255.f, .78f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(82.f / 255.f, 88.f / 255.f, 112.f / 255.f, 0.f));
		ImGui::SetNextWindowPos(ImVec2(26 * scaleRatioW, 172 * scaleRatioH), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(154 * scaleRatioW, 98 * scaleRatioH), ImGuiCond_Always);
		ImGui::Begin("debugInfo##2223", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar /*| ImGuiWindowFlags_NoInputs*/);
		ImGui::TextColored(ImColor(255, 255, 255, 255), "FPS:%d", UIBridge::mFPS);
		ImGui::TextColored(ImColor(255, 255, 255, 255), "Resolution:%d*%d", UIBridge::mResolutionWidth, UIBridge::mResolutionHeight);
		ImGui::TextColored(ImColor(255, 255, 255, 255), "RenderTime:%dms", UIBridge::mRenderTime);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(82.f / 255.f, 88.f / 255.f, 112.f / 255.f, .0f));
		if (LayoutButton(ImVec2(100, 0), ImVec2(37, 24), u8"关闭"))
		{
			UIBridge::showDegubInfoWindow = false;
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::End();
		ImGui::PopStyleColor();
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(82.f / 255.f, 88.f / 255.f, 112.f / 255.f, .78f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(82.f / 255.f, 88.f / 255.f, 112.f / 255.f, .0f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(82.f / 255.f, 88.f / 255.f, 112.f / 255.f, 0.f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::SetNextWindowPos(ImVec2(26 * scaleRatioW, 172 * scaleRatioH), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(120 * scaleRatioW, 40 * scaleRatioH), ImGuiCond_Always);
		ImGui::Begin("debugInfo##23", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar /*| ImGuiWindowFlags_NoInputs*/);
		if (LayoutButton(ImVec2(0, 0), ImVec2(100, 25), u8"显示性能参数"))
		{
			if (!UIBridge::m_bSetGSInputSrc && UIBridge::showGreenScreen){  
				UIBridge::showDegubInfoWindow = false;
			}else{
				UIBridge::showDegubInfoWindow = true;
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
}

#define MAKEUP_CUSTOM_NAME ("demo_icon_customize.bundle")

static void ShowFloatMenuAR(Nama * nama)
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .70f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .0f));
	ImGui::SetNextWindowPos(ImVec2(19 * scaleRatioW, 584 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(888 * scaleRatioW, 90 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("itemSelect##1563", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar);
	for (int i = 0; i < BundleCategory::Count; i++)
	{
		if (UIBridge::categoryBundles[i].size() == 0)
		{
			if (i >= sizeof(gBundlePath) / sizeof(gBundlePath[0]))
			{
				break;
			}
			UIBridge::FindAllBundle(gBundlePath[i], UIBridge::categoryBundles[i]);
		}
	}

	if (UIBridge::bundleCategory < BundleCategory::Count && UIBridge::bundleCategory >= BundleCategory::Animoji)
	{
		for (int i = 0; i < UIBridge::categoryBundles[UIBridge::bundleCategory].size(); i++)
		{
			string itemName = UIBridge::categoryBundles[UIBridge::bundleCategory][i];

			string iconName = itemName.substr(0, itemName.find_last_of('.'));
			Texture::SharedPtr tex = Texture::createTextureFromFile(iconName + ".png", false);
			//找不到图标
			if (!tex)
			{
				tex = Texture::createTextureFromFile("icon_Movebutton_nor.png", false);
			}
			bool buttonClick;
			if (UIBridge::mCurRenderItemName == itemName)
			{
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH),ImVec2(0, 0),ImVec2(1, 1),-1,ImVec4(0, 0, 0, 0),ImVec4(1, 1, 1, 1),ImGui::ImGUI_Button_Operation_Type_Select);
			}else{
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH),ImVec2(0, 0),ImVec2(1, 1),-1,ImVec4(0, 0, 0, 0),ImVec4(1, 1, 1, 1),ImGui::ImGUI_Button_Operation_Type_Deselect);
			}
			if (buttonClick)
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

				if (itemName == MAKEUP_CUSTOM_NAME)
				{
					UIBridge::showCustomMakeup = true;
					nama->UnbindCurFixedMakeup();
				}
				else
				{
					if (UIBridge::showCustomMakeup)
					{
						GUICustomMakeup::Reset(nama);
						nama->DestroyAll();
					}
					
					UIBridge::showCustomMakeup = false;
					nama->SelectBundle(gBundlePath[UIBridge::bundleCategory] + "/" + itemName);
				}

				
			}
			ImGui::SameLine(0.f, 22.f * scaleRatioW);
		}
	}


	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

static void ShowFloatMenu(Nama * nama)
{
	if (!UIBridge::showGreenScreen)
	{
		ShowFloatMenuAR(nama);
	}
	else
	{
		GUIGS::ShowFloatMenuGS(nama);
	}
}

static void ShowArMenu()
{
	string *categoryNameArr = nullptr;
	string allCategory[] = { "list_icon_avatar_nor","list_icon_annimoji_nor","list_icon_Propmap_nor","list_icon_AR_nor",
		"list_icon_Expressionrecognition_nor",	"list_icon_Musicfilter_nor","list_icon_Bgsegmentation_nor",
		"list_icon_gesturerecognition_nor","list_icon_Hahamirror_nor","list_icon_makeup_nor","list_icon_hairdressing_nor","list_icon_photo_sticker_nor",
		u8"Avatar","Animoji",u8"道具贴图",u8"AR面具",
		u8"表情识别",u8"音乐滤镜",u8"人像分割",
		u8"手势识别",u8"哈哈镜",u8"美妆",u8"美发", u8"搞笑大头" };
	int amount = 12;
	categoryNameArr = allCategory;

	/* 这是个例外逻辑,把Avatar放在最前面，这样后面加起来正常的就没啥问题 */
#define INDEX_AVATAR (0)

	for (int i = 0; i < amount; i++)
	{
		/* bundleCategory对于于bundle载入路径的Array,前面多了个Avatar所以加1 */
		if ((UIBridge::bundleCategory + 1) == i)
		{
			if (LayoutImageButtonWithText(ImVec2(0.f, 27.f), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_propmap_collapse.png", false)->getTextureID(),
				Texture::createTextureFromFile("list_icon_propmap_collapse.png", false)->getTextureID(), categoryNameArr[amount + i].c_str()))
			{
				auto funSetUnSelectd = [&](int index) {
					UIBridge::bundleCategory = BUNDLE_CATEGORY_NOMEAN;
					UIBridge::showItemSelectWindow = false;

					if (INDEX_AVATAR == index)
					{
						Nama::mEnableAvatar = false;
					}
				};

				funSetUnSelectd(i);
			}
		}
		else
		{
			string iconFileName = categoryNameArr[i].substr(0, categoryNameArr[i].find_last_of("_")) + (1 ? "_hover.png" : "_nor.png");
			if (LayoutImageButtonWithText(ImVec2(0.f, 27.f), ImVec2(52, 52), Texture::createTextureFromFile(categoryNameArr[i] + ".png", false)->getTextureID(),
				Texture::createTextureFromFile(iconFileName, false)->getTextureID(), categoryNameArr[amount + i].c_str()))
			{
				auto funSetSelectd = [&](int index) {
					if (INDEX_AVATAR == index)
					{
						/* -1现在意味着AVATAR */
						UIBridge::bundleCategory = -1;
						UIBridge::showItemSelectWindow = false;
						Nama::mEnableAvatar = true;
					}
					else
					{
						/* bundleCategory对于于bundle载入路径的Array,前面多了个Avatar所以减1 */
						UIBridge::bundleCategory = index - 1;
						UIBridge::showItemSelectWindow = true;
						Nama::mEnableAvatar = false;
					}
				};

				funSetSelectd(i);
			}
		}
		ImGui::SameLine(0.f, 27.f * scaleRatioW);
	}
}

static void LayoutShowCamChoice(Nama * nama)
{
	if (nama->CameraList().size())
	{
		ImGui::PushItemWidth(244 * scaleRatioW);
		static string item_current = nama->CameraList()[0];
		if (ImGui::BeginCombo("##slect camera1", item_current.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < nama->CameraList().size(); n++)
			{
				bool is_selected = (item_current == nama->CameraList()[n]);
				if (ImGui::Selectable(nama->CameraList()[n].c_str(), is_selected))
				{
					item_current = nama->CameraList()[n];
					if (UIBridge::mSelectedCamera != n)
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
		if (ImGui::BeginCombo("##slect camera122", u8"未检测到摄像头")) // The second parameter is the label previewed before opening the combo.				
			ImGui::EndCombo();
		ImGui::PopItemWidth();
	}
}

/*
维护AR情况下的各种状态
*/
static void ReCoverArState(Nama * nama)
{
	nama->OpenCamera(UIBridge::mSelectedCamera);
}

/* 
第一次设置绿幕画面，给出一个默认位置
*/
static void ReSetGSInputSize(Nama * nama)
{
		FURect curRect = nama->getGSPreviewRect();
		float newRectX = 0.5-curRect.origin.x;
		nama->gsPreviewRect = {newRectX,curRect.origin.y,float(0.5)+newRectX,curRect.size.height};
		nama->changeGSPreviewRect(nama->gsPreviewRect);
}
/* 
维护绿幕状态下的状态
*/
static void ReCoverGSState(Nama * nama)
{
	/* 
	用户切换的时候不能进来改配置
	*/
	if (UIBridge::m_bSetGSInputSrc && UIBridge::m_bNeedShowGSCamChoice != true)
	{
		switch (UIBridge::m_gsState.inputType)
		{
		case GS_INPUT_TYPE_CAMERA:
		{
			if (UIBridge::m_gsState.iCmaeraID > 0)
			{
				nama->OpenCamera(UIBridge::m_gsState.iCmaeraID);
			}
		}
		break;
		case GS_INPUT_TYPE_FILE:
		{
			if (UIBridge::m_gsState.strFilePath.length() > 0)
			{
				nama->OpenCamera(UIBridge::m_gsState.strFilePath);
			}
		}
		break;
		default:
			break;
		}

	}
}

static void ShowMainMenu(Nama * nama)
{
	{
		ImGui::SetNextWindowPos(ImVec2(10 * scaleRatioW, 156 * scaleRatioH), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(914 * scaleRatioW, 664 * scaleRatioH), ImGuiCond_Always);
		ImGui::Begin("Window2##2", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::Dummy(ImVec2(888 * scaleRatioW, 480 * scaleRatioH));
		for (int i = 0; i < 7; i++)	ImGui::Spacing();
		
		if (UIBridge::showGreenScreen)
		{
			ReCoverGSState(nama);
			GUIGS::ShowGreenScreenMenu(nama);
		}
		else
		{
			ReCoverArState(nama);
			if (UIBridge::m_bShowingBodyBeauty) {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.30f);
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ShowArMenu();
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}else{
				ShowArMenu();
			}
		}
		
		
		ImGui::Dummy(ImVec2(1, 1));
		ImGui::Dummy(ImVec2(321 * scaleRatioW, 1)); ImGui::SameLine();
		ImGui::PushItemWidth(272 * scaleRatioW);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 6.0f);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(225.f / 255.f, 228.f / 255.f, 238.f / 255.f, 1.f));

		if (UIBridge::m_bSamplingColor) {
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.30f);
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		}
		if (ImGui::SliderString("##modeSlider", (char*)u8"AR功能", (char *)u8"绿幕抠像", (int*)&UIBridge::showGreenScreen, 0, 1))
		{
			
			if (UIBridge::showGreenScreen)
			{
				if(UIBridge::m_bSetGSInputSrc == false)
				{
					// 关闭默认相机输入源
					GUIGS::CloseCamera(nama);
					UIBridge::m_bNeedReChooseInputSrc = true;
				}
				UIBridge::showItemSelectWindow = true;
				Nama::mEnableAvatar = false;
				
				// 处理音乐滤镜,关闭音乐
				if (UIBridge::bundleCategory == BundleCategory::MusicFilter && UIBridge::mNeedPlayMP3) {
					nama->pauseCurrentMp3();
				}
			}
			else
			{
				UIBridge::showItemSelectWindow = (UIBridge::bundleCategory != BUNDLE_CATEGORY_NOMEAN);
				if (!nama->IsCameraPlaying() || nama->GetCameraCaptureType() == 2){
					nama->setDefaultFrame();
					nama->ReOpenCamera(UIBridge::mSelectedCamera);
				}
				
				// 处理音乐滤镜，重新开启音乐
				if (UIBridge::bundleCategory == BundleCategory::MusicFilter && UIBridge::mNeedPlayMP3 &&!UIBridge::m_bShowingBodyBeauty) {
					nama->resumeCurrentMp3();
				}
			}
			
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(1);
		ImGui::PopItemWidth();
		if (UIBridge::m_bSamplingColor) {
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		ImGui::End();
		
	}
}

static void ShowCameraChoice(Nama * nama)
{
	{
		ImGui::SetNextWindowPos(ImVec2(10 * scaleRatioW, 70 * scaleRatioH), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(914 * scaleRatioW, 76 * scaleRatioH), ImGuiCond_Always);
		ImGui::Begin("Window1##2", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::Dummy(ImVec2(21 * scaleRatioW, 11 * scaleRatioH));
		ImGui::Text(u8"选择摄像头:");

		ImGui::SameLine();
		
		if (UIBridge::showGreenScreen)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.30f);
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			//navigation window
			ImGui::PushItemWidth(244 * scaleRatioW);
			if (ImGui::BeginCombo("##slect camera4", u8"请从下方选择绿幕输入")) // The second parameter is the label previewed before opening the combo.				
				ImGui::EndCombo();
			ImGui::PopItemWidth();
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
			
		}
		else
		{
			LayoutShowCamChoice(nama);
		}

#ifdef _WIN32
		ImGui::SameLine();
		ImGui::Checkbox(u8"虚拟摄像头", &UIBridge::mNeedIpcWrite);
		ImGui::SameLine();
		ShowHelpMarker(u8"勾选后会开启虚拟摄像头功能，详细可参见帮助文档里内容");
		ImGui::SameLine(800 * scaleRatioW);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(225.f / 255.f, 228.f / 255.f, 238.f / 255.f, 1.f));

		if (ImGui::Button(u8"打开帮助文档"))
		{

			WinExec("notepad.exe README.md", SW_SHOW);

		}

		ImGui::PopStyleColor();
#endif
		ImGui::End();
	}
}

static void ShowTitle()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(1360 * scaleRatioW, 60 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("TitleWindow##21", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	string filename = "title.png";
	Texture::SharedPtr tex = Texture::createTextureFromFile(filename, false);
	float my_tex_w = (float)ImGui::GetContentRegionAvailWidth();
	float my_tex_h = (float)ImGui::GetContentRegionAvail().y;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::Image(tex->getTextureID(), ImVec2(my_tex_w, my_tex_h), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
	ImGui::PopStyleVar();
	ImGui::End();
}

void Gui::ShowMainWindow(Nama * nama)
{
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(58.f / 255.f, 61.f / 255.f, 82.f / 255.f, 1.f));
	float x = DEF_FRAME_SHOW_POS_X;
	float y = DEF_FRAME_SHOW_POS_Y;
	float w = DEF_FRAME_SHOW_WIDTH;
	float h = DEF_FRAME_SHOW_HEIGHT;
	ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(w,h), ImGuiCond_Always);
	ImGui::Begin(g_szMainFrameWinName, NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
	float frameWidth = (float)DEF_FRAME_SHOW_WIDTH;
	float frameHeight = (float)DEF_FRAME_SHOW_HEIGHT;
	
	UIBridge::mFPS = get_fps();
	//printf("UIBridge::mFPS: %d\n", UIBridge::mFPS);
	UIBridge::mRenderTime = 1000.f / (float)UIBridge::mFPS;
	UIBridge::mResolutionWidth = m_processedFrame.cols;
	UIBridge::mResolutionHeight = m_processedFrame.rows;
	
	GLuint texidNeedShow = UIBridge::m_bSamplingColor ? m_texIDOrignal : m_texIDNamaProcess;
	
	auto srcType = nama->GetCameraCaptureType();
	/* 相机输入需要镜像，文件不需要 */
	bool bNeedFlip = false;
	if(!UIBridge::showGreenScreen){
		bNeedFlip = srcType != GS_INPUT_TYPE_FILE;
	}
	funShowImg(texidNeedShow, ((float)m_processedFrame.cols) / m_processedFrame.rows, ImVec2(frameWidth, frameHeight), bNeedFlip);
	ImGui::End();
	ImGui::PopStyleColor();
	static FURect tmpRect;
	if (UIBridge::showGreenScreen && !UIBridge::m_bNeedReChooseInputSrc && UIBridge::m_bSetGSInputSrc) {
		ImVec2 dealt;
		if(mouseControl->draging(&dealt)){
			FURect namaGSPreviewRect = nama->gsPreviewRect;
			float direction = bNeedFlip ? -1.0: 1.0;
			float dealtXInNama = dealt.x / w;
			float dealtYInNama = dealt.y / h;
			float newNamaGSPreviewRectX = namaGSPreviewRect.origin.x + direction * dealtXInNama;
			float newNamaGSPreviewRectY = namaGSPreviewRect.origin.y + dealtYInNama;
			float newNamaGSPreviewRectW = namaGSPreviewRect.size.width + direction * dealtXInNama;
			float newNamaGSPreviewRectH = namaGSPreviewRect.size.height + dealtYInNama;
			
			if (newNamaGSPreviewRectX < 0.0) {
				newNamaGSPreviewRectX= 0.0;
				newNamaGSPreviewRectW = namaGSPreviewRect.size.width - namaGSPreviewRect.origin.x;
				
			}else if (newNamaGSPreviewRectW > 1.0)
			{
				newNamaGSPreviewRectW = 1;
				newNamaGSPreviewRectX = namaGSPreviewRect.origin.x + (1 - namaGSPreviewRect.size.width);
				
			}
			if (newNamaGSPreviewRectY < 0.0) {
				newNamaGSPreviewRectY= 0.0;
				newNamaGSPreviewRectH = namaGSPreviewRect.size.height - namaGSPreviewRect.origin.y;
				
			}else if (newNamaGSPreviewRectH > 1.0)
			{
				newNamaGSPreviewRectH = 1;
				newNamaGSPreviewRectY = namaGSPreviewRect.origin.y + (1 - namaGSPreviewRect.size.height);
			}else{
			}
			
			FURect newNamaGSPreviewRect = {newNamaGSPreviewRectX,newNamaGSPreviewRectY,
				newNamaGSPreviewRectW,newNamaGSPreviewRectH
			};
			nama->changeGSPreviewRect(newNamaGSPreviewRect);
			tmpRect = newNamaGSPreviewRect;
			FURect viewRect = mouseControl->viewRect;
			float newViewRectW = UIBridge::m_localVideoWidth;
			float newViewRectH = UIBridge::m_localVideoHeight;
			
			float newViewRectX = bNeedFlip ? x+(0.5-newNamaGSPreviewRectX)*w : x+newNamaGSPreviewRectX*w;
			float newViewRectY = y+newNamaGSPreviewRectY*h;
			
			FURect newViewRect = {newViewRectX,newViewRectY,newViewRectW,newViewRectH};
			mouseControl->callback(newViewRect);
			
		}else if(mouseControl->dragComplete(&dealt))
		{
			nama->setGSPreviewRect(tmpRect);
		}
	}
}


void Gui::UpdateFrame(Nama * nama)
{
	cv::Mat frameMat = nama->GetFrame();
	if (frameMat.data)
	{
		m_processedFrame.release();
		m_processedFrame = frameMat.clone();
		
			
		cv::cvtColor(frameMat, m_processedFrame, cv::COLOR_BGR2RGBA);

		bool bNeedOri = (Nama::mEnableAvatar && Gui::mIsOpenMiniWindow) || UIBridge::m_bSamplingColor;
		
		if (bNeedOri)
		{
			UpdateFrame2Tex(m_processedFrame, m_texIDOrignal);
		}

		float tempTime = GetTickCount();
		if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED))
		{
			nama->RenderItems(m_processedFrame);
		}

		if (UIBridge::showGreenScreen && !UIBridge::m_bSetGSInputSrc)
		{
			m_processedFrame.setTo(150);
		}else{
			cv::Mat bgra[4]; 
			cv::split(m_processedFrame, bgra);
			bgra[3] = 255.0;
			cv::merge(bgra, 4, m_processedFrame);
		}

		UpdateFrame2Tex(m_processedFrame, m_texIDNamaProcess);
        
#if _WIN32
        
		if (UIBridge::mNeedIpcWrite)
		{
			static bool is_create_vc = false;
			if (!is_create_vc)
			{
				createVirturalCamera(0);
				is_create_vc = true;
			}
			pushDataToVirturalCamera(m_processedFrame.data, m_processedFrame.cols, m_processedFrame.rows);
		}
        
#endif
        
	}
}
static void ShowTipStr(string tipStr){
	int tipStrWdith = 0;
#if _WIN32
	wstring tipWstr(tipStr.begin(),tipStr.end());
	tipStrWdith = CalWstringWidth(tipWstr, 13,0,1000);
#elif __APPLE__
	tipStrWdith = FuToolMac::culculatorTextWidth(tipStr.c_str(),18);
#endif

	int marginX = 20;
	int tipStrLength = tipStr.length();
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .70f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .0f));
	ImGui::SetNextWindowPos(ImVec2(366 * scaleRatioW, 472 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(tipStrWdith + marginX * 2, 30 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("itemTips##1563", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	ImGui::Dummy(ImVec2(marginX, 1)); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0, 0.0, 0.0, 1.0), tipStr.c_str());
	
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}
static void ShowTip(rapidjson::Document & doc)
{
	string tip;
	if (doc.HasParseError()) {
		rapidjson::ParseErrorCode code = doc.GetParseError();
		printf(" json parse error code: %d\n", code);
	}
	else
	{
		rapidjson::Value::ConstMemberIterator itr = doc.FindMember(UIBridge::mCurRenderItemName.c_str());
		if (itr != doc.MemberEnd()) {
			tip = itr->value.GetString();
			UIBridge::showItemTipsWindow = true;
		}
	}
	if (UIBridge::mLastTime > ImGui::GetTime())
	{
		if (UIBridge::showItemTipsWindow)
		{
			ShowTipStr(tip);
		}
	}
	else
	{
		UIBridge::showItemTipsWindow = false;
		UIBridge::mLastTime = 0.0;
	}
}

static void ShowTipLabel(string tip)
{
	if (UIBridge::mLastTime > ImGui::GetTime())
	{
			ShowTipStr(tip);
	}
	else
	{
		UIBridge::showItemTipsWindow = false;
		UIBridge::mLastTime = 0.0;
	}
}
#include "imgui_internal.h"

void Gui::ProcessGSSampleClick(Nama * nama)
{
	if (UIBridge::m_bSamplingColor)
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}

	if (UIBridge::m_bSamplingColor)
	{
		if (ImGui::IsMouseClicked(0))
		{
			ImVec2 pos = ImGui::GetMousePos();

			ImGuiWindow* window = ImGui::FindWindowByName(g_szMainFrameWinName);

			int CurW = DEF_FRAME_SHOW_WIDTH;
			int CurH = DEF_FRAME_SHOW_HEIGHT;
			
			//Imgui的窗口是在左上角的 
			bool bXIn = (pos.x > (window->Pos.x) && pos.x < (window->Pos.x + CurW));
			bool bYIn = (pos.y > (window->Pos.y) && pos.y < (window->Pos.y + CurH));
			if (bXIn && bYIn)
			{
				cv::Mat frameMat = nama->GetFrame();
				if (frameMat.data)
				{
					cv::Point point;
					point.x = (pos.x - window->Pos.x) / CurW * frameMat.cols;	
					point.y = (pos.y - window->Pos.y) / CurH * frameMat.rows;

					printf("keycolor pos find:(%d,%d) \r\n", point.x, point.y);

					auto dataBGR = frameMat.at<cv::Vec3b>(point.y, point.x);

					cv::Vec4b colorNeed = { dataBGR[2],dataBGR[1], dataBGR[0], 255 };
					nama->SetGSKeyColor(colorNeed);
					GUIGS::SetCurColorCircle(colorNeed);
					UIBridge::m_bSamplingColor = false;
				}
			}
		}
	}
}


void Gui::render(Nama::UniquePtr& nama)
{
	funGenTex(m_texIDNamaProcess);
	funGenTex(m_texIDOrignal);
	funGenTex(UIBridge::m_id4ChoiceCam);

	GUIGS::InitColorVec();
	GUIGS::ResetDefParam();
	resetBeautyParam();
	resetShapeParam();

	nama->UpdateGreenScreen();
	nama->UpdateBeauty();
	nama->SetCurrentShape(4);

	rapidjson::Document doc;
    string strFile = FuTool::GetFileFullPathFromeSearchPath("config.json");
	FILE* fp = fopen(strFile.c_str(), "rb");
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	doc.ParseStream(is);
	fclose(fp);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//Texture::AddSearchPath("");
		if (ImGui::IsKeyPressed(GLFW_KEY_F2))
		{
			showUI = !showUI;
		}

		ProcessGSSampleClick(nama.get());

		if (UIBridge::m_bResetPreviewRect)
		{
			ResetPreviewRect(nama);
			UIBridge::m_bResetPreviewRect = false;
		}
		//title window
		if (showUI)
		{
			ShowTitle();
		}
		
		{
			if (showUI)
			{
				
				ShowCameraChoice(nama.get());
				
				//main menu
				ShowMainMenu(nama.get());	
			}
			UpdateFrame(nama.get());
            
			//main window
			ShowMainWindow(nama.get());

			if (showUI)
			{
				ShowDebugMenu();

				/* 绿幕重选输入按钮 */
				if (UIBridge::showGreenScreen)
				{
					GUIGS::ShowReChooseInputSrc();
				}
				

				/* 悬浮选择道具 */
				if (UIBridge::showItemSelectWindow && !UIBridge::m_bSamplingColor)
				{
					ShowFloatMenu(nama.get());
				}

				if (Nama::mEnableAvatar)
				{
					ShowAvatarMenu(nama.get());

					/* 显示略影响性能 */
					if (Gui::mIsOpenMiniWindow)
					{
						float frameWidth = (float)DEF_FRAME_SHOW_WIDTH / 4.0f;
						float frameHeight = (float)DEF_FRAME_SHOW_HEIGHT / 4.0f;
						ShowAvatarMiniCameraView(m_texIDOrignal, ((float)UIBridge::mResolutionWidth) / UIBridge::mResolutionHeight, ImVec2(frameWidth, frameHeight));
					}
				}
			}
		}
		// 選擇了含有中文字符的 png 提示
		if (UIBridge::m_bLoadWrongNamePNGFile) {
			string tipStr = UIBridge::m_openLocalFileTip;
			ShowTipLabel(tipStr);
			if (UIBridge::mLastTime < ImGui::GetTime())
			{
				UIBridge::m_bLoadWrongNamePNGFile = false;
			}
		}
		// AR与美体功能不能共用提示
		else if(UIBridge::m_bShowingBodyBeauty && !UIBridge::showGreenScreen)
		{
			string tipStr = u8"AR功能跟美体模块无法共用";
			ShowTipLabel(tipStr);

		}else if (showUI)//道具提示
			{
				ShowTip(doc);
			}
		
		//right window
		if (showUI)
		{
			ImGui::SetNextWindowPos(ImVec2(934 * scaleRatioW, 70 * scaleRatioH), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(416 * scaleRatioW, 750 * scaleRatioH), ImGuiCond_Always);

			if (UIBridge::showGreenScreen)
			{
				GUIGS::ShowRightPanel(nama.get());
			}
			else
			{
				if (Nama::mEnableAvatar)
				{
					ShowAvatarTip();
				}
				else if (UIBridge::showCustomMakeup)
				{
					ShowCustomMakeupTip(nama.get());
				}else
				{
					ShowTabs("rightTabs", 0, nama);
				}
			}
		}
		
		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImVec4 clear_color = ImVec4(234.f / 255.f, 235.f / 255.f, 242.f / 255.f, 1.00f);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	auto lastColor = GUIGS::GetCurColorCircle();
	if (lastColor != g_color1)
	{
		nama->AddRecentColor(lastColor);
	}
	

	funDestroyTex(m_texIDNamaProcess);
	funDestroyTex(m_texIDOrignal);
	funDestroyTex(UIBridge::m_id4ChoiceCam);
}
void Gui::ResetPreviewRect(Nama::UniquePtr& nama){
	GUIGS::previewRect = {DEF_FRAME_SHOW_WIDTH - UIBridge::m_localVideoWidth + DEF_FRAME_SHOW_POS_X,
		DEF_FRAME_SHOW_POS_Y + DEF_FRAME_SHOW_HEIGHT - UIBridge::m_localVideoHeight,
		UIBridge::m_localVideoWidth,UIBridge::m_localVideoHeight};
	mouseControl->ReSet(GUIGS::previewRect);
	nama->gsPreviewRect = {1-UIBridge::m_localVideoWidth/DEF_FRAME_SHOW_WIDTH,
		1-UIBridge::m_localVideoHeight/DEF_FRAME_SHOW_HEIGHT,1,1};
	nama->changeGSPreviewRect(nama->gsPreviewRect);
}
void Gui::onWindowResize(uint32_t width, uint32_t height)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = (float)width;
	io.DisplaySize.y = (float)height;
}
