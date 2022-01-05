#include "Camera.h"
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
#include "GuiSticker.h"
#include "FuController.h"
#include "GuiBgSeg.h"

#if __APPLE__
#include "fu_tool_mac.h"
#include "NaMaGLContext.h"
void* new_context;
#else
#include "VirtualCamera.h"
HGLRC new_context;
#endif 

using namespace gui_tool;

int oriWindowWidth = 0;
int oriWindowHeight = 0;
float scaleRatioW = 1.f;
float scaleRatioH = 1.f;
bool showUI = true;

int UIBridge::bundleCategory = BUNDLE_CATEGORY_NOMEAN;
int UIBridge::bundleCategoryLast = BUNDLE_CATEGORY_NOMEAN;
int UIBridge::renderBundleCategory = -1;
vector<string> UIBridge::categoryBundles[BundleCategory::Count];
int UIBridge::faceType = 0;

bool UIBridge::showItemSelectWindow = false;
bool UIBridge::showItemTipsWindow = false;
bool UIBridge::showDegubInfoWindow = false;
bool UIBridge::showFilterSlider = false;
int UIBridge::showGreenScreen = false;
bool UIBridge::showCustomMakeup = false;
bool UIBridge::newMakeupType = false;
bool UIBridge::showLightMakeupTip = false;
LightMakeupParam UIBridge::m_lightMakeupParam;

volatile uint32_t UIBridge::mFPS = 60;
uint32_t UIBridge::mResolutionWidth = 1280;
uint32_t UIBridge::mResolutionHeight = 720;
uint32_t UIBridge::mRenderTime = 33;

int UIBridge::m_curFilterIdx = 0;
int UIBridge::m_curRenderItem = -1;
int UIBridge::m_curBindedItem = -1;
int UIBridge::m_curPlayingMusicItem = -1;
ImGuiID UIBridge::m_curRenderItemUIID = -1;
ImGuiID UIBridge::m_curRenderItemUIIDSec = -2;
string UIBridge::mCurRenderItemName = "";
double UIBridge::mLastTime = 0.0;
int UIBridge::mEnableSkinDect = 1;
int UIBridge::mEnableHeayBlur = 3;
int UIBridge::mEnableExBlur = 0;
int UIBridge::mSelectedCamera = 0;
float UIBridge::mFaceBeautyLevel[MAX_BEAUTYFACEPARAMTER] = { 0.0f };

float UIBridge::mFaceShapeLevel[MAX_FACESHAPEPARAMTER] = { 0.0f };
float UIBridge::mFilterLevel[10] = { 40,40,40,40,40, 40,40,40,40,40 };
float UIBridge::mMakeupLevel[10] = { 100,100,100,100,100, 100,100,100,100,100 };
float UIBridge::mBodyShapeLevel[MAX_BODY_SHAPE_PARAM] = { 0.0f };

bool UIBridge::mNeedIpcWrite = false;
bool UIBridge::mNeedPlayMP3 = false;
bool UIBridge::mNeedStopMP3 = false;

bool Gui::mIsOpenMiniWindow = true;
bool Gui::mIsEnableHumanFollowMode = false;

//{ "blur_level","color_level", "red_level", "eye_bright", "tooth_whiten" ,"remove_pouch_strength", "remove_nasolabial_folds_strength" };

const char* g_szMainFrameWinName = "MainFrameWindow";
const FURect g_showViewRect = {
	DEF_FRAME_SHOW_POS_X,
	DEF_FRAME_SHOW_POS_Y,
	DEF_FRAME_SHOW_WIDTH,
	DEF_FRAME_SHOW_HEIGHT
};

static void ShowTipStr(string tipStr);
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

uint64_t GetTickCount()
{
	struct timespec tTemp = { 0, 0 };
	clock_gettime(CLOCK_MONOTONIC, &tTemp);
	uint64_t lRunMS = tTemp.tv_sec * 1000 + tTemp.tv_nsec / 1000000;

	return lRunMS;
}

#endif

int get_fps(bool bADDFrame)
{
	static int fps = 0;

	static uint64_t lastTime = GetTickCount(); // ms
	static int frameCount = 0;
	++frameCount;

	uint64_t curTime = GetTickCount();
	if (curTime - lastTime > 1000)
	{
		fps = frameCount - 1;
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
	scaleRatioW = (float)width / (float)oriWindowWidth;
	scaleRatioH = (float)height / (float)oriWindowHeight;

	glfwSetWindowSize(Gui::offscreen_window, width, height);
}

static void window_iconify_callback(GLFWwindow* window, int isIcon)
{
	//HWND wnd = (HWND)Gui::hWindow;
	//HDC hdc = ::GetDC(wnd);
	//cout << "window iconify " << isIcon << " dc " << hdc << std::endl;
}

#ifdef _WIN32
HWND Gui::hWindow;
HWND Gui::hOffscreenWindow;
#else
void* Gui::hWindow = NULL;
void* Gui::hOffscreenWindow = NULL;
#endif

void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s source=0x%x, type = 0x%x, id = %d, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		source, type, id, severity, message);
}

GLFWwindow* Gui::window;
GLFWwindow* Gui::offscreen_window;
Gui::UniquePtr Gui::create(uint32_t width, uint32_t height)
{
	GUISticker::LoadResource();
	GUIBgSeg::LoadResource();
	GUIGS::LoadResource();
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

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	offscreen_window = glfwCreateWindow(width, height, "", NULL, window);
	glfwMakeContextCurrent(offscreen_window);
	//glfwSwapInterval(1); // Enable vsync

	glfwMakeContextCurrent(window);
#if __APPLE__
	hWindow = glfwGetCocoaWindow(window);
	hOffscreenWindow = glfwGetCocoaWindow(offscreen_window);
#else
	hWindow = glfwGetWin32Window(window);
	hOffscreenWindow = glfwGetWin32Window(offscreen_window);
#endif
	//glfwSwapInterval(1); // Enable vsync
	gl3wInit();
	glfwSetWindowSizeCallback(window, window_size_callback);

#if 0
	glfwSetWindowIconifyCallback(window, window_iconify_callback);

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
#endif

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
	pGui->m_mouseControl = new MouseControl(GUIGS::previewRect);
	pGui->m_mouseControlSec = new MouseControl(g_showViewRect);
	return pGui;
}

Gui::~Gui()
{
	CCameraManage::getInstance()->mCapture->closeCamera();
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(offscreen_window);
	glfwDestroyWindow(window);
	glfwTerminate();
}


static void ShowTipLabel(string tip);

static MyDocument GDocs[4];

static void ShowTabs(const char* title, bool* p_open, Nama* nama)
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
	ImGui::BeginTabBar("##tabs1", ImGuiTabBarFlags_NoAnim | ImGuiTabBarFlags_SizingPolicyEqual);
	for (int doc_n = 0; doc_n < IM_ARRAYSIZE(GDocs); doc_n++)
	{
		MyDocument& doc = GDocs[doc_n];

		ImGui::PushID(&doc);

		const bool selected = ImGui::TabItem(ImVec2(100 * scaleRatioW, 30 * scaleRatioH), doc.Name, &doc.Open, 0);

		if (!selected)
		{
			ImGui::PopID();
			continue;
		}


		switch (doc_n)
		{
		case 0:
		{
			if (!nama->CheckModuleCodeSide(BeautifyFaceSkin))
			{
				UIBridge::mLastTime = ImGui::GetTime() + 2.0;
				string tipStr = u8"美颜权限不足";
				ShowTipLabel(tipStr);
			}
			gui_tab_content::ShowTabBeautySkin(nama);
		}
		break;
		case 1:
		{
			if (!nama->CheckModuleCodeSide(BeautifyFaceShape))
			{
				UIBridge::mLastTime = ImGui::GetTime() + 2.0;
				string tipStr = u8"美颜权限不足";
				ShowTipLabel(tipStr);
			}
			gui_tab_content::ShowTabFaceBeauty(nama);
		}
		break;
		case 2:
		{
			if (!nama->CheckModuleCodeSide(BeautifyFilter))
			{
				UIBridge::mLastTime = ImGui::GetTime() + 2.0;
				string tipStr = u8"滤镜权限不足";
				ShowTipLabel(tipStr);
			}
			gui_tab_content::ShowTabFilter(nama);
		}
		break;
		case 3:
		{
			if (!nama->CheckModuleCodeSide(BeautifyBody))
			{
				UIBridge::mLastTime = ImGui::GetTime() + 2.0;
				string tipStr = u8"美体权限不足";
				ShowTipLabel(tipStr);
			}
			gui_tab_content::ShowTabBodyBeauty(nama);
		}
		break;
		}

		ImGui::PopID();
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

static void ShowCustomMakeupTip(Nama* nama)
{

	GUICustomMakeup::Draw(nama);

}

BodyTrackConfig bodyTrackConfig;
static void ApplyBodyTrackConfig(Nama* nama) {
	auto& params = nama->GetBodyTrackType() == BodyTrackType::FullBody ? bodyTrackConfig.FullBody : bodyTrackConfig.HalfBody;
	nama->m_Controller->EnableFaceProcessor(params.EnableFaceProcessor);
	nama->m_Controller->UseRetargetRootScale(params.UseRetargetRootScale, params.UseRetargetRootScaleValue);
	nama->m_Controller->SetAvatarAnimFilterParams(params.AnimFilterParams_n_buffer_frames, params.AnimFilterParams_pos_w, params.AnimFilterParams_angle_w);
	nama->m_Controller->SetAvatarGlobalOffset(params.GlobalOffset_x, params.GlobalOffset_y, params.GlobalOffset_z);
	nama->m_Controller->SetAvatarScale(params.Scale);
	nama->m_Controller->SetPos(params.Pos_x, params.Pos_y, params.Pos_z);
	//nama->m_Controller->EnableHumanFollowMode(params.EnableHumanFollowMode);
	nama->m_Controller->SetAvatarTranslationScale(params.TrackMoveRange_x, params.TrackMoveRange_y, params.TrackMoveRange_z);
}

static void ShowAvatarMenu(Nama* nama)
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::SetNextWindowPos(ImVec2(19 * scaleRatioW, 530 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300 * scaleRatioW, 40 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("avatarChange", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

	string strIconName = "";
	switch (nama->GetBodyTrackType())
	{
	case BodyTrackType::FullBody:
		strIconName = "switch_whole_body.png";
		break;
	case BodyTrackType::HalfBody:
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
		nama->SwitchBodyTrackType();
		ApplyBodyTrackConfig(nama);
	}

	//ImGui::Checkbox("MiniWindow", &Gui::mIsOpenMiniWindow);
	//if (ImGui::Checkbox("HumanFollowMode", &Gui::mIsEnableHumanFollowMode)) {
	//	nama->m_Controller->EnableHumanFollowMode(Gui::mIsEnableHumanFollowMode);
	//}

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

static void ShowAvatarMiniCameraView(GLuint& texId, float rotio, ImVec2 frameSize)
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
			if (!UIBridge::m_bSetGSInputSrc && UIBridge::showGreenScreen) {
				UIBridge::showDegubInfoWindow = false;
			}
			else {
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

static void ShowFloatMenuAR(Nama* nama)
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .70f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .0f));
	ImGui::SetNextWindowPos(ImVec2(19 * scaleRatioW, 580 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(876 * scaleRatioW, 95 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("itemSelect##1563", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar);


	auto bundleCategory = UIBridge::bundleCategory;

	if (bundleCategory == BundleCategory::Avatar) {
		auto& path = gBundlePath[BundleCategory::Avatar];
		{
			ImGui::PushID(0);
			string itemName = "fakeman";
			string iconName = Texture::GetPicPathFromResFolder("fakeman.png");
			Texture::SharedPtr tex = Texture::createTextureFromFullPath(iconName, true);
			if (!tex)
				tex = Texture::createTextureFromFile("icon_Movebutton_nor.png", false);
			bool buttonClick;
			if (UIBridge::mCurRenderItemName == itemName)
			{
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Select);
			}
			else {
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Deselect);
			}
			if (buttonClick)
			{
				if (UIBridge::mCurRenderItemName != itemName)
				{
					UIBridge::mCurRenderItemName = itemName;
					UIBridge::mLastTime = ImGui::GetTime() + 2.0;
					UIBridge::showItemTipsWindow = false;

					nama->UnLoadAvatar();
					std::vector<std::string> mBundleDirs;
					mBundleDirs.emplace_back(path + "fakeman.bundle");
					mBundleDirs.emplace_back(path + "default_bg.bundle");
					nama->LoadAvatarBundles(mBundleDirs);
					nama->LoadAvatarHandTrackBundle();

					BodyTrackConfig c;
					c.DefaultType = BodyTrackType::FullBody;

					c.FullBody.EnableFaceProcessor = false;
					//c.FullBody.EnableHumanFollowMode = true;
					c.FullBody.UseRetargetRootScale = true;
					c.FullBody.UseRetargetRootScaleValue = 0;
					c.FullBody.AnimFilterParams_n_buffer_frames = 5;
					c.FullBody.AnimFilterParams_pos_w = 0;
					c.FullBody.AnimFilterParams_angle_w = 0;
					c.FullBody.GlobalOffset_x = 0;
					c.FullBody.GlobalOffset_y = 0;
					c.FullBody.GlobalOffset_z = 0;
					c.FullBody.Scale = 1.0f;
					c.FullBody.Pos_x = 0;
					c.FullBody.Pos_y = 50;
					c.FullBody.Pos_z = -900;
					c.FullBody.TrackMoveRange_x = 0.0f;
					c.FullBody.TrackMoveRange_y = 0.0f;
					c.FullBody.TrackMoveRange_z = 0.0f;

					c.HalfBody.EnableFaceProcessor = false;
					//c.HalfBody.EnableHumanFollowMode = false;
					c.HalfBody.UseRetargetRootScale = true;
					c.HalfBody.UseRetargetRootScaleValue = 0;
					c.HalfBody.AnimFilterParams_n_buffer_frames = 5;
					c.HalfBody.AnimFilterParams_pos_w = 0;
					c.HalfBody.AnimFilterParams_angle_w = 0;
					c.HalfBody.GlobalOffset_x = 0;
					c.HalfBody.GlobalOffset_y = 0;
					c.HalfBody.GlobalOffset_z = 0;
					c.HalfBody.Scale = 1.0f;
					c.HalfBody.Pos_x = 0;
					c.HalfBody.Pos_y = 0;
					c.HalfBody.Pos_z = -300;
					c.HalfBody.TrackMoveRange_x = 0.0f;
					c.HalfBody.TrackMoveRange_y = 0.0f;
					c.HalfBody.TrackMoveRange_z = 0.0f;

					bodyTrackConfig = c;
					ApplyBodyTrackConfig(nama);

					Gui::mIsOpenMiniWindow = true;
					Gui::mIsEnableHumanFollowMode = false;
					nama->m_Controller->EnableHumanFollowMode(Gui::mIsEnableHumanFollowMode);
					Nama::mNamaAppState.EnableAvatarUI = true;
				}
				else
				{
					UIBridge::mCurRenderItemName = "NONE";
					UIBridge::mLastTime = 0.0;
					UIBridge::showItemTipsWindow = false;
					UIBridge::mNeedStopMP3 = true;

					nama->UnLoadAvatar();
				}
			}

			ImGui::SameLine(0.f, 22.f * scaleRatioW);
			ImGui::PopID();
		}
		//{
		//	ImGui::PushID(1);
		//	string itemName = "xiong";
		//	string iconName = Texture::GetPicPathFromResFolder("xiong.png");
		//	Texture::SharedPtr tex = Texture::createTextureFromFullPath(iconName, true);
		//	if (!tex)
		//		tex = Texture::createTextureFromFile("icon_Movebutton_nor.png", false);
		//	bool buttonClick;
		//	if (UIBridge::mCurRenderItemName == itemName)
		//	{
		//		buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Select);
		//	}
		//	else {
		//		buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Deselect);
		//	}
		//	if (buttonClick)
		//	{
		//		if (UIBridge::mCurRenderItemName != itemName)
		//		{
		//			UIBridge::mCurRenderItemName = itemName;
		//			UIBridge::mLastTime = ImGui::GetTime() + 2.0;
		//			UIBridge::showItemTipsWindow = false;

		//			nama->UnLoadAvatar();
		//			std::vector<std::string> mBundleDirs;
		//			mBundleDirs.emplace_back(path + "xiong.bundle");
		//			mBundleDirs.emplace_back(path + "xiong_light.bundle");
		//			mBundleDirs.emplace_back(path + "xiong_weixiao_ani00.bundle");
		//			nama->LoadAvatarBundles(mBundleDirs);

		//			BodyTrackConfig c;
		//			c.DefaultType = BodyTrackType::FullBody;

		//			c.FullBody.EnableFaceProcessor = false;
		//			//c.FullBody.EnableHumanFollowMode = true;
		//			c.FullBody.UseRetargetRootScale = true;
		//			c.FullBody.UseRetargetRootScaleValue = 0;
		//			c.FullBody.AnimFilterParams_n_buffer_frames = 10;
		//			c.FullBody.AnimFilterParams_pos_w = 0;
		//			c.FullBody.AnimFilterParams_angle_w = 0;
		//			c.FullBody.GlobalOffset_x = 0;
		//			c.FullBody.GlobalOffset_y = 0;
		//			c.FullBody.GlobalOffset_z = 0;
		//			c.FullBody.Scale = 1.0f;
		//			c.FullBody.Pos_x = 70;
		//			c.FullBody.Pos_y = 50;
		//			c.FullBody.Pos_z = -1100;
		//			c.FullBody.TrackMoveRange_x = 0.9f;
		//			c.FullBody.TrackMoveRange_y = 0.9f;
		//			c.FullBody.TrackMoveRange_z = 0.1f;

		//			c.HalfBody.EnableFaceProcessor = false;
		//			//c.HalfBody.EnableHumanFollowMode = false;
		//			c.HalfBody.UseRetargetRootScale = true;
		//			c.HalfBody.UseRetargetRootScaleValue = 0;
		//			c.HalfBody.AnimFilterParams_n_buffer_frames = 10;
		//			c.HalfBody.AnimFilterParams_pos_w = 0;
		//			c.HalfBody.AnimFilterParams_angle_w = 0;
		//			c.HalfBody.GlobalOffset_x = 0;
		//			c.HalfBody.GlobalOffset_y = 0;
		//			c.HalfBody.GlobalOffset_z = 0;
		//			c.HalfBody.Scale = 1.0f;
		//			c.HalfBody.Pos_x = 0;
		//			c.HalfBody.Pos_y = 60;
		//			c.HalfBody.Pos_z = -300;
		//			c.HalfBody.TrackMoveRange_x = 0.0f;
		//			c.HalfBody.TrackMoveRange_y = 0.0f;
		//			c.HalfBody.TrackMoveRange_z = 0.0f;

		//			bodyTrackConfig = c;
		//			nama->SetBodyTrackType(BodyTrackType::FullBody);
		//			ApplyBodyTrackConfig(nama);

		//			Gui::mIsOpenMiniWindow = false;
		//			Gui::mIsEnableHumanFollowMode = false;
		//			nama->m_Controller->EnableHumanFollowMode(Gui::mIsEnableHumanFollowMode);
		//			Nama::mNamaAppState.EnableAvatarUI = false;
		//		}
		//		else
		//		{
		//			UIBridge::mCurRenderItemName = "NONE";
		//			UIBridge::mLastTime = 0.0;
		//			UIBridge::showItemTipsWindow = false;
		//			UIBridge::mNeedStopMP3 = true;

		//			nama->UnLoadAvatar();
		//		}
		//	}

		//	ImGui::SameLine(0.f, 24.f * scaleRatioW);
		//	ImGui::PopID();
		//}

	}
	else if (bundleCategory < BundleCategory::Count)
	{
		if (UIBridge::categoryBundles[BundleCategory::LightMakeup].size() == 0)
		{
			UIBridge::categoryBundles[BundleCategory::LightMakeup].push_back("light_makeup_peachblossom.");
			UIBridge::categoryBundles[BundleCategory::LightMakeup].push_back("light_makeup_grapefruit.");
			UIBridge::categoryBundles[BundleCategory::LightMakeup].push_back("light_makeup_clear.");
			UIBridge::categoryBundles[BundleCategory::LightMakeup].push_back("light_makeup_boyfriend.");
		}
		for (int i = BundleCategory::Avatar + 1; i < BundleCategory::Count; i++)
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
		for (int i = 0; i < UIBridge::categoryBundles[bundleCategory].size(); i++)
		{
			ImGui::PushID(i);

			string itemName = UIBridge::categoryBundles[bundleCategory][i];

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
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(57 * scaleRatioW, 57 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Select);
			}
			else {
				buttonClick = ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(57 * scaleRatioW, 57 * scaleRatioH), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImGui::ImGUI_Button_Operation_Type_Deselect);
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

				if (itemName == "light_makeup_peachblossom." || 
					itemName == "light_makeup_grapefruit." ||
					itemName == "light_makeup_clear." || 
					itemName == "light_makeup_boyfriend.") {
					itemName = LIGHT_MAKEUP_NAME;
				}
				if (itemName == MAKEUP_CUSTOM_NAME)
				{
					UIBridge::showCustomMakeup = true;
					UIBridge::newMakeupType = false;
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
					if (i >= 1 && i <= 4) {
						UIBridge::newMakeupType = true;
						nama->SelectBundle(gBundlePath[bundleCategory] + "/" + itemName);
					}
					else {
						UIBridge::newMakeupType = false;
						nama->SelectBundle(gBundlePath[bundleCategory] + "/" + itemName);
					}
				}
				if (UIBridge::mCurRenderItemName!= "NONE" && 
					(iconName.compare("light_makeup_peachblossom") == 0 ||
					iconName.compare("light_makeup_grapefruit") == 0 || 
					iconName.compare("light_makeup_clear") == 0 || 
					iconName.compare("light_makeup_boyfriend") == 0)) {
					UIBridge::showLightMakeupTip = true;
					//设置轻美妆参数
					if (iconName.compare("light_makeup_peachblossom") == 0) {
						UIBridge::m_lightMakeupParam.blusherPath = g_assetDir + "items/LightMakeup/blusher/mu_blush_01.png";
						UIBridge::m_lightMakeupParam.blusher = 0.9;
						UIBridge::m_lightMakeupParam.eyeshadowPath = g_assetDir + "items/LightMakeup/eyeshadow/mu_eyeshadow_01.png";
						UIBridge::m_lightMakeupParam.eyeshadow = 0.9;
						UIBridge::m_lightMakeupParam.eyebrowPath = g_assetDir + "items/LightMakeup/eyebrow/mu_eyebrow_01.png";
						UIBridge::m_lightMakeupParam.eyebrow = 0.5;
						UIBridge::m_lightMakeupParam.lipColorPath = g_assetDir + "items/LightMakeup/lipstick/mu_lip_01.json";
						UIBridge::m_lightMakeupParam.intensity = 0.9;
						UIBridge::m_lightMakeupParam.filterName = "fennen3";
						UIBridge::m_lightMakeupParam.filterLevel = 1.0;
					}
					else if (iconName.compare("light_makeup_grapefruit") == 0) {
						UIBridge::m_lightMakeupParam.blusherPath = g_assetDir + "items/LightMakeup/blusher/mu_blush_23.png";
						UIBridge::m_lightMakeupParam.blusher = 1.0;
						UIBridge::m_lightMakeupParam.eyeshadowPath = g_assetDir + "items/LightMakeup/eyeshadow/mu_eyeshadow_21.png";
						UIBridge::m_lightMakeupParam.eyeshadow = 0.75;
						UIBridge::m_lightMakeupParam.eyebrowPath = g_assetDir + "items/LightMakeup/eyebrow/mu_eyebrow_19.png";
						UIBridge::m_lightMakeupParam.eyebrow = 0.6;
						UIBridge::m_lightMakeupParam.lipColorPath = g_assetDir + "items/LightMakeup/lipstick/mu_lip_21.json";
						UIBridge::m_lightMakeupParam.intensity = 0.8;
						UIBridge::m_lightMakeupParam.filterName = "lengsediao4";
						UIBridge::m_lightMakeupParam.filterLevel = 0.7;
					}
					else if (iconName.compare("light_makeup_clear") == 0) {
						UIBridge::m_lightMakeupParam.blusherPath = g_assetDir + "items/LightMakeup/blusher/mu_blush_22.png";
						UIBridge::m_lightMakeupParam.blusher = 0.9;
						UIBridge::m_lightMakeupParam.eyeshadowPath = g_assetDir + "items/LightMakeup/eyeshadow/mu_eyeshadow_20.png";
						UIBridge::m_lightMakeupParam.eyeshadow = 0.65;
						UIBridge::m_lightMakeupParam.eyebrowPath = g_assetDir + "items/LightMakeup/eyebrow/mu_eyebrow_18.png";
						UIBridge::m_lightMakeupParam.eyebrow = 0.45;
						UIBridge::m_lightMakeupParam.lipColorPath = g_assetDir + "items/LightMakeup/lipstick/mu_lip_20.json";
						UIBridge::m_lightMakeupParam.intensity = 0.8;
						UIBridge::m_lightMakeupParam.filterName = "xiaoqingxin1";
						UIBridge::m_lightMakeupParam.filterLevel = 0.8;
					}
					else if (iconName.compare("light_makeup_boyfriend") == 0) {
						UIBridge::m_lightMakeupParam.blusherPath = g_assetDir + "items/LightMakeup/blusher/mu_blush_20.png";
						UIBridge::m_lightMakeupParam.blusher = 0.8;
						UIBridge::m_lightMakeupParam.eyeshadowPath = g_assetDir + "items/LightMakeup/eyeshadow/mu_eyeshadow_18.png";
						UIBridge::m_lightMakeupParam.eyeshadow = 0.9;
						UIBridge::m_lightMakeupParam.eyebrowPath = g_assetDir + "items/LightMakeup/eyebrow/mu_eyebrow_16.png";
						UIBridge::m_lightMakeupParam.eyebrow = 0.65;
						UIBridge::m_lightMakeupParam.lipColorPath = g_assetDir + "items/LightMakeup/lipstick/mu_lip_18.json";
						UIBridge::m_lightMakeupParam.intensity = 1.0;
						UIBridge::m_lightMakeupParam.filterName = "xiaoqingxin3";
						UIBridge::m_lightMakeupParam.filterLevel = 0.9;
					}
					nama->setLightMakeupParam(UIBridge::m_lightMakeupParam);
				}
				else {
					//设置滤镜
					UIBridge::showLightMakeupTip = false;
					nama->UpdateFilter(UIBridge::m_curFilterIdx);
				}
			}
			ImGui::SameLine(0.f, 22.f * scaleRatioW);

			ImGui::PopID();
		}
	}


	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

static void ShowFloatMenu(Nama* nama)
{
	if (!UIBridge::showGreenScreen)
	{
		switch (UIBridge::bundleCategory)
		{
		case BundleCategory::ItemJingpin:
		{
			GUISticker::ShowStickerPannel(nama);
		}
		break;

		case BundleCategory::BackgroundSegmentation:
		{
			GUIBgSeg::ShowBgSegPannel(nama);
		}
		break;

		default:
			/* 通用显示逻辑 */
			ShowFloatMenuAR(nama);
			break;
		}
	}
	else
	{
		GUISticker::mSelectSticker = true;
		GUIGS::ShowFloatMenuGS(nama);
	}
}

static void ShowArMenu(Nama* nama)
{
	string* categoryNameArr = nullptr;
	string allCategory[] = { "list_icon_avatar_nor","list_icon_annimoji_nor","list_icon_Propmap_nor","list_icon_Jinpin_nor","list_icon_AR_nor",
		"list_icon_Expressionrecognition_nor",	"list_icon_Musicfilter_nor","list_icon_Bgsegmentation_nor",
		"list_icon_gesturerecognition_nor","list_icon_Hahamirror_nor","list_icon_makeup_nor","list_icon_lightmakeup_nor","list_icon_hairdressing_nor","list_icon_photo_sticker_nor",
		u8"Avatar","Animoji",u8"道具贴纸",u8"精品贴纸",u8"AR面具",
		u8"表情识别",u8"音乐滤镜",u8"人像分割",
		u8"手势识别",u8"哈哈镜",u8"美妆",u8"轻美妆",u8"美发", u8"搞笑大头" };
	int amount = 14;
	categoryNameArr = allCategory;

	/* 这是个例外逻辑,把Avatar放在最前面，这样后面加起来正常的就没啥问题 */
	int i = 0;
#if __APPLE__
	i = 1;
#endif 
	for (; i < amount; i++)
	{
		/* bundleCategory对于于bundle载入路径的Array,前面多了个Avatar所以加1 */
		if (UIBridge::bundleCategory == i)
		{
			if (LayoutImageButtonWithText(ImVec2(0.f, 27.f), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_propmap_collapse.png", false)->getTextureID(),
				Texture::createTextureFromFile("list_icon_propmap_collapse.png", false)->getTextureID(), categoryNameArr[amount + i].c_str()))
			{
				auto funSetUnSelectd = [&](int index) {
					UIBridge::bundleCategory = BUNDLE_CATEGORY_NOMEAN;
					UIBridge::showItemSelectWindow = false;

					if (BundleCategory::Avatar == index)
					{
						Nama::mNamaAppState.EnableAvatar = false;
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
					GUISticker::mSelectSticker = true;
					if (BundleCategory::Avatar == index)
					{
						UIBridge::bundleCategory = index;
						UIBridge::showItemSelectWindow = true;
						Nama::mNamaAppState.EnableAvatar = true;
					}
					else
					{
						UIBridge::bundleCategory = index;
						UIBridge::showItemSelectWindow = true;
						Nama::mNamaAppState.EnableAvatar = false;
					}

				};

				funSetSelectd(i);
			}
		}
		ImGui::SameLine(0.f, 27.f * scaleRatioW);
	}
}

static void LayoutShowCamChoice(Nama* nama)
{
	vector<string> CameraList = CCameraManage::getInstance()->CameraList();
	if (CameraList.size())
	{
		ImGui::PushItemWidth(244 * scaleRatioW);
		static string item_current = CameraList[0];
		if (ImGui::BeginCombo("##slect camera1", item_current.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < CameraList.size(); n++)
			{
				bool is_selected = (item_current == CameraList[n]);
				if (ImGui::Selectable(CameraList[n].c_str(), is_selected))
				{
					item_current = CameraList[n];
					if (UIBridge::mSelectedCamera != n)
					{
						UIBridge::mSelectedCamera = n;
						CCameraManage::getInstance()->ReOpenCamera(n);
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
static void ReCoverArState(Nama* nama)
{
	CCameraManage::getInstance()->OpenCamera(UIBridge::mSelectedCamera);
}

/*
第一次设置绿幕画面，给出一个默认位置
*/
static void ReSetGSInputSize(Nama* nama)
{
	FURect curRect = nama->getGSPreviewRect();
	float newRectX = 0.5 - curRect.origin.x;
	nama->gsPreviewRect = { newRectX,curRect.origin.y,float(0.5) + newRectX,curRect.size.height };
	nama->changeGSPreviewRect(nama->gsPreviewRect);
}
/*
维护绿幕状态下的状态
*/
static void ReCoverGSState(Nama* nama)
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
				CCameraManage::getInstance()->OpenCamera(UIBridge::m_gsState.iCmaeraID);
			}
		}
		break;
		case GS_INPUT_TYPE_FILE:
		{
			if (UIBridge::m_gsState.strFilePath.length() > 0)
			{
				CCameraManage::getInstance()->OpenCamera(UIBridge::m_gsState.strFilePath);
			}
		}
		break;
		default:
			break;
		}

	}
}

static void ShowMainMenu(Nama* nama)
{
	{
		ImGui::SetNextWindowPos(ImVec2(10 * scaleRatioW, 156 * scaleRatioH), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(914 * scaleRatioW, 664 * scaleRatioH), ImGuiCond_Always);
		ImGui::Begin("Window2##2", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::Dummy(ImVec2(888 * scaleRatioW, 480 * scaleRatioH));
		for (int i = 0; i < 7; i++)	ImGui::Spacing();

		if (UIBridge::showGreenScreen)
		{
			Nama::mNamaAppStateBackGS = Nama::mNamaAppState;
			ReCoverGSState(nama);
			GUIGS::ShowGreenScreenMenu(nama);
		}
		else
		{
			Nama::mNamaAppStateBackAR = Nama::mNamaAppState;
			ReCoverArState(nama);
			if (UIBridge::m_bShowingBodyBeauty) {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.30f);
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ShowArMenu(nama);
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			else {
				ShowArMenu(nama);
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
		if (ImGui::SliderString("##modeSlider", (char*)u8"AR功能", (char*)u8"绿幕抠像", (int*)&UIBridge::showGreenScreen, 0, 1))
		{

			if (UIBridge::showGreenScreen)
			{
				Nama::mNamaAppState = Nama::mNamaAppStateBackGS;
				nama->UpdateFilter(UIBridge::m_curFilterIdx);
				if (UIBridge::m_bSetGSInputSrc == false)
				{
					// 关闭默认相机输入源
					GUIGS::CloseCamera(nama);
					UIBridge::m_bNeedReChooseInputSrc = true;
				}
				else {
					//重新播放视频
					string videoPath;
#if _WIN32
					videoPath = gGSBgPic + "/" + UIBridge::mCurRenderGSItemName;
#elif __APPLE__
					videoPath = UIBridge::GetFileFullPathFromResourceBundle((gGSBgPic + "/" + UIBridge::mCurRenderGSItemName + ".mp4").c_str());
#endif 
					GUIGS::ChangeGreenScreenBg(videoPath);
				}
				UIBridge::showItemSelectWindow = true;
				Nama::mNamaAppState.EnableAvatar = false;

				// 处理音乐滤镜,关闭音乐
				if (UIBridge::bundleCategory == BundleCategory::MusicFilter && UIBridge::mNeedPlayMP3) {
					nama->pauseCurrentMp3();
				}
			}
			else
			{
				Nama::mNamaAppState = Nama::mNamaAppStateBackAR;
				GUIGS::CloseGreenScreenBg();
				UIBridge::showItemSelectWindow = !UIBridge::m_bShowingBodyBeauty && (UIBridge::bundleCategory != BUNDLE_CATEGORY_NOMEAN);
				/*if (!nama->IsCameraPlaying() || nama->GetCameraCaptureType() == 2){
					nama->setDefaultFrame();
					nama->ReOpenCamera(UIBridge::mSelectedCamera);
				}*/

				// 处理音乐滤镜，重新开启音乐
				if (UIBridge::bundleCategory == BundleCategory::MusicFilter && UIBridge::mNeedPlayMP3 && !UIBridge::m_bShowingBodyBeauty) {
					nama->resumeCurrentMp3();
				}
				//轻美妆重新设置轻美妆滤镜
				if (UIBridge::bundleCategory == BundleCategory::LightMakeup && UIBridge::showLightMakeupTip && !UIBridge::m_bShowingBodyBeauty) {
					nama->setLightMakeupParam(UIBridge::m_lightMakeupParam);
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

static void ShowCameraChoice(Nama* nama)
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

void Gui::tipToGrantCameraAcess(Nama* nama)
{
#ifndef _WIN32
	ImGui::SetNextWindowPos(ImVec2(240 * scaleRatioW, 250 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(400 * scaleRatioW, 300 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("WindowGSI##3", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	RectLayoutDesc desc;
	desc.pos = ImVec2(160, 0);
	desc.size = ImVec2(52, 52);

	ImVec2 textPos = ImVec2(60, 0);

	LayoutImage(desc, Texture::createTextureFromFile("icon_prompt.png", false)->getTextureID(), textPos,
		u8"请在设置里面为 FULivePC 打开相机权限！");

	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(120.f / 255.f, 136.f / 255.f, 234.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(150.f / 255.f, 157.f / 255.f, 181.f / 255.f, 1.f));

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

	if (LayoutButton(ImVec2(120, 60), ImVec2(120, 40), u8"确定"))
	{
		FuToolMac::tipToOpenPrivacyPanel();
	}


	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(4);

	ImGui::End();
#endif // !_WIN32

}

void Gui::ShowMainWindow(Nama* nama)
{
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(58.f / 255.f, 61.f / 255.f, 82.f / 255.f, 1.f));
	float x = DEF_FRAME_SHOW_POS_X;
	float y = DEF_FRAME_SHOW_POS_Y;
	float w = DEF_FRAME_SHOW_WIDTH;
	float h = DEF_FRAME_SHOW_HEIGHT;
	ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);
	ImGui::Begin(g_szMainFrameWinName, NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
	float frameWidth = (float)DEF_FRAME_SHOW_WIDTH;
	float frameHeight = (float)DEF_FRAME_SHOW_HEIGHT;

	UIBridge::mFPS = get_fps(true);
	//printf("UIBridge::mFPS: %d\n", UIBridge::mFPS);
	UIBridge::mRenderTime = 1000.f / (float)UIBridge::mFPS;
	UIBridge::mResolutionWidth = m_processedFrame.cols;
	UIBridge::mResolutionHeight = m_processedFrame.rows;

	GLuint texidNeedShow = UIBridge::m_bSamplingColor ? m_texIDOrignal : m_texIDNamaProcess;

	auto srcType = CCameraManage::getInstance()->GetCameraCaptureType();

	/* 相机输入需要镜像，文件不需要 */
	bool bNeedFlip = false;
	//if (UIBridge::showGreenScreen) {
	//	bNeedFlip = srcType != GS_INPUT_TYPE_FILE && !UIBridge::m_bSamplingColor;
	//}

	funShowImg(texidNeedShow, ((float)m_processedFrame.cols) / m_processedFrame.rows, ImVec2(frameWidth, frameHeight), bNeedFlip);
	ImGui::End();
	ImGui::PopStyleColor();
	static FURect tmpRect;
	if (UIBridge::showItemSelectWindow && UIBridge::bundleCategory != BUNDLE_CATEGORY_NOMEAN) {
		FURect newViewRect = { 
			DEF_FRAME_SHOW_POS_X,
			DEF_FRAME_SHOW_POS_Y,
			DEF_FRAME_SHOW_WIDTH, 
			DEF_FRAME_SHOW_HEIGHT - 80 * scaleRatioH 
		};
		m_mouseControlSec->changeRect(newViewRect);
	}else {
		FURect newViewRect = {
			DEF_FRAME_SHOW_POS_X,
			DEF_FRAME_SHOW_POS_Y,
			DEF_FRAME_SHOW_WIDTH,
			DEF_FRAME_SHOW_HEIGHT
		};
		m_mouseControlSec->changeRect(newViewRect);
	}
	if (UIBridge::showGreenScreen && !UIBridge::m_bNeedReChooseInputSrc && UIBridge::m_bSetGSInputSrc) {
		ImVec2 dealt;
		if (m_mouseControl->draging(&dealt)) {
			FURect namaGSPreviewRect = nama->gsPreviewRect;
			float direction = bNeedFlip ? -1.0 : 1.0;
			float dealtXInNama = dealt.x / w;
			float dealtYInNama = dealt.y / h;
			float newNamaGSPreviewRectX = namaGSPreviewRect.origin.x + direction * dealtXInNama;
			float newNamaGSPreviewRectY = namaGSPreviewRect.origin.y + dealtYInNama;
			float newNamaGSPreviewRectW = namaGSPreviewRect.size.width + direction * dealtXInNama;
			float newNamaGSPreviewRectH = namaGSPreviewRect.size.height + dealtYInNama;

			if (newNamaGSPreviewRectX < 0.0) {
				newNamaGSPreviewRectX = 0.0;
				newNamaGSPreviewRectW = namaGSPreviewRect.size.width - namaGSPreviewRect.origin.x;

			}
			else if (newNamaGSPreviewRectW > 1.0)
			{
				newNamaGSPreviewRectW = 1;
				newNamaGSPreviewRectX = namaGSPreviewRect.origin.x + (1 - namaGSPreviewRect.size.width);

			}
			if (newNamaGSPreviewRectY < 0.0) {
				newNamaGSPreviewRectY = 0.0;
				newNamaGSPreviewRectH = namaGSPreviewRect.size.height - namaGSPreviewRect.origin.y;

			}
			else if (newNamaGSPreviewRectH > 1.0)
			{
				newNamaGSPreviewRectH = 1;
				newNamaGSPreviewRectY = namaGSPreviewRect.origin.y + (1 - namaGSPreviewRect.size.height);
			}
			else {
			}

			FURect newNamaGSPreviewRect = { newNamaGSPreviewRectX,newNamaGSPreviewRectY,
				newNamaGSPreviewRectW,newNamaGSPreviewRectH
			};
			nama->changeGSPreviewRect(newNamaGSPreviewRect);
			tmpRect = newNamaGSPreviewRect;
			FURect viewRect = m_mouseControl->viewRect;
			float newViewRectW = UIBridge::m_localVideoWidth;
			float newViewRectH = UIBridge::m_localVideoHeight;

			float newViewRectX = bNeedFlip ? x + (0.5 - newNamaGSPreviewRectX) * w : x + newNamaGSPreviewRectX * w;
			float newViewRectY = y + newNamaGSPreviewRectY * h;

			FURect newViewRect = { newViewRectX,newViewRectY,newViewRectW,newViewRectH };
			m_mouseControl->callback(newViewRect);

		}
		else if (m_mouseControl->dragComplete(&dealt))
		{
			nama->setGSPreviewRect(tmpRect);
		}
	}

#ifndef _WIN32
	if (!FuToolMac::granteCameraAccess()) {
		tipToGrantCameraAcess(nama);
	}
#endif


	ImVec2 pos;
	if (m_mouseControlSec->isSelected(&pos))
	{
		nama->SetCurDouble("is_click", 1.0);
		nama->SetCurDouble("mouse_down", 1.0);
	}
}


void Gui::UpdateFrame(Nama* nama)
{
	cv::Mat frameMat = CCameraManage::getInstance()->GetFrame();

	if (frameMat.data)
	{
		m_processedFrame.release();
		m_processedFrame = frameMat;

		cv::cvtColor(frameMat, m_processedFrame, cv::COLOR_BGR2RGBA);

		bool bNeedOri = (Nama::mNamaAppState.EnableAvatar && Nama::mNamaAppState.EnableAvatarUI && Gui::mIsOpenMiniWindow) || UIBridge::m_bSamplingColor;


		if (bNeedOri)
		{
			UpdateFrame2Tex(m_processedFrame, m_texIDOrignal);
		}

		//if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED))
		{
#ifdef _WIN32
			HGLRC context = wglGetCurrentContext();
			HWND wnd = (HWND)Gui::hOffscreenWindow;
			wglMakeCurrent(GetDC(wnd), new_context);
#else
			void* curCtx = NaMaGetCurrentGLCtx();
			NaMaMakeAsCurrentCtx(new_context);
#endif
			nama->RenderItems(m_processedFrame);
			CCameraManage::getInstance()->SetNewFrame(false);
			glFinish();
#ifdef _WIN32
			wglMakeCurrent(GetDC(wnd), context);
#else
			NaMaMakeAsCurrentCtx(curCtx);
#endif
		}

		if (UIBridge::showGreenScreen && !UIBridge::m_bSetGSInputSrc)
		{
			m_processedFrame.setTo(150);
		}
		else {
			cv::Mat bgra[4];
			cv::split(m_processedFrame, bgra);
			bgra[3] = 255.0;
			cv::merge(bgra, 4, m_processedFrame);
		}
		UpdateFrame2Tex(m_processedFrame, m_texIDNamaProcess);


#ifndef PERFORMANCE_OPEN
		//更新Texture操作需要同步，否则某些机器会出现撕裂
		glFinish();

#endif

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
static void ShowTipStr(string tipStr) {

	//#if _WIN32
	//	wstring tipWstr(tipStr.begin(),tipStr.end());
	//	tipStrWdith = CalWstringWidth(tipWstr, 13,0,1000);
	//#elif __APPLE__
	//	tipStrWdith = FuToolMac::culculatorTextWidth(tipStr.c_str(),18);
	//#endif
		//一般就一行
	const ImVec2 line_size = ImGui::CalcTextSize(tipStr.data(), tipStr.data() + tipStr.length() - 1, false);

	int marginX = 50;
	int tipStrLength = tipStr.length();
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .70f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .0f));
	ImGui::SetNextWindowPos(ImVec2(366 * scaleRatioW, 472 * scaleRatioH), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(line_size.x + 25, 30 * scaleRatioH), ImGuiCond_Always);
	ImGui::Begin("itemTips##1563", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

	ImGui::TextUnformatted(tipStr.data());

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

static void ShowTip(rapidjson::Document& doc)
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

void Gui::ProcessGSSampleClick(Nama* nama)
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
				cv::Mat frameMat = CCameraManage::getInstance()->GetFrame();
				if (frameMat.data)
				{
					cv::Point point;
					point.x = (pos.x - window->Pos.x) / CurW * frameMat.cols;
					point.y = (pos.y - window->Pos.y) / CurH * frameMat.rows;

					printf("keycolor pos find:(%d,%d) \r\n", point.x, point.y);

					cv::Vec4b colorNeed = {};

					if (frameMat.channels() == 3)
					{
						auto dataBGR = frameMat.at<cv::Vec3b>(point.y, point.x);

						colorNeed = { dataBGR[2],dataBGR[1], dataBGR[0], 255 };
					}
					else if (frameMat.channels() == 4)
					{
						auto dataBGRA = frameMat.at<cv::Vec4b>(point.y, point.x);

						colorNeed = { dataBGRA[2],dataBGRA[1], dataBGRA[0], 255 };
					}

					nama->SetGSKeyColor(colorNeed);
					GUIGS::SetCurColorCircle(colorNeed);
					UIBridge::m_bSamplingColor = false;
				}
			}
		}
	}
}


void Gui::init(int width, int height, bool enableNama)
{
#ifdef _WIN32
	HGLRC context = wglGetCurrentContext();
	HWND wnd = (HWND)Gui::hOffscreenWindow;
	new_context = wglCreateContext(GetDC(wnd));
	wglMakeCurrent(GetDC(wnd), new_context);
#else
	void* curCtx = NaMaGetCurrentGLCtx();
	new_context = NaMaCreateGLCtx(Gui::hOffscreenWindow);
	NaMaMakeAsCurrentCtx(new_context);
#endif

	CCameraManage::getInstance()->mCapture->initCamera(width, height, UIBridge::mSelectedCamera);
#if __APPLE__
	//   FuToolMac::addLockScreenEventObser(nama->lock_screen_not_macosx());
	FuToolMac::addUnLockScreenEventObser(CCameraManage::getInstance()->unlock_screen_not_macosx());
#endif
	Nama::create(enableNama);
	Nama::get()->mFrameWidth = CCameraManage::getInstance()->mCapture->m_dstFrameSize.width;
	Nama::get()->mFrameHeight = CCameraManage::getInstance()->mCapture->m_dstFrameSize.height;
#ifdef _WIN32
	wglMakeCurrent(GetDC(wnd), context);
#else
	NaMaMakeAsCurrentCtx(curCtx);
#endif
}

void Gui::render(Nama* nama)
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
	int frameId = 0;
	while (!glfwWindowShouldClose(window))
	{

		glfwMakeContextCurrent(window);

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


		ProcessGSSampleClick(nama);
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

				ShowCameraChoice(nama);

				//main menu
				ShowMainMenu(nama);
			}
			glfwMakeContextCurrent(offscreen_window);
			UpdateFrame(nama);
			glfwMakeContextCurrent(window);

			//main window
			ShowMainWindow(nama);
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
					ShowFloatMenu(nama);
				}

				if (Nama::mNamaAppState.EnableAvatar && Nama::mNamaAppState.EnableAvatarUI)
				{
					ShowAvatarMenu(nama);

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
		int err = nama->GetLastNamaError();
		if (err == 20 || err == 21)
		{
			string tipStr = u8"证书过期，请联系FaceUnity技术支持";
			ShowTipStr(tipStr);
		}
		else if (UIBridge::m_bLoadWrongNamePNGFile) {
			string tipStr = UIBridge::m_openLocalFileTip;
			ShowTipLabel(tipStr);
			if (UIBridge::mLastTime < ImGui::GetTime())
			{
				UIBridge::m_bLoadWrongNamePNGFile = false;
			}
		}
		// AR与美体功能不能共用提示
		else if (UIBridge::m_bShowingBodyBeauty && !UIBridge::showGreenScreen)
		{
			string tipStr = u8"AR功能跟美体模块无法共用";
			ShowTipLabel(tipStr);

		}
		else if (!nama->CheckModuleCode(UIBridge::bundleCategory)) {
			string tipStr = u8"道具权限不足，请联系FaceUnity技术支持";
			ShowTipStr(tipStr);
		}
		else if (UIBridge::m_bShowingSafeAreaTip)
		{
			string tipStr = u8"白色区域为安全区域,不参与绿幕抠像";
			ShowTipLabel(tipStr);
			UIBridge::m_bShowingSafeAreaTip = UIBridge::showItemTipsWindow;
		}
		else if (showUI)//道具提示
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
				GUIGS::ShowRightPanel(nama);
			}
			else
			{
				if (Nama::mNamaAppState.EnableAvatar)
				{
					ShowAvatarTip();
				}
				else if (UIBridge::showCustomMakeup)
				{
					ShowCustomMakeupTip(nama);
				}
				else
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
		frameId++;
	}

	printf("frame total %d\n", frameId);

	auto lastColor = GUIGS::GetCurColorCircle();
	if (lastColor != g_color1)
	{
		nama->AddRecentColor(lastColor);
	}
    nama->Release();

	funDestroyTex(m_texIDNamaProcess);
	funDestroyTex(m_texIDOrignal);
	funDestroyTex(UIBridge::m_id4ChoiceCam);
}
void Gui::ResetPreviewRect(Nama* nama) {
	GUIGS::previewRect = { DEF_FRAME_SHOW_WIDTH - UIBridge::m_localVideoWidth + DEF_FRAME_SHOW_POS_X,
		DEF_FRAME_SHOW_POS_Y + DEF_FRAME_SHOW_HEIGHT - UIBridge::m_localVideoHeight,
		UIBridge::m_localVideoWidth,UIBridge::m_localVideoHeight };
	m_mouseControl->ReSet(GUIGS::previewRect);
	nama->gsPreviewRect = { 1 - UIBridge::m_localVideoWidth / DEF_FRAME_SHOW_WIDTH,
		1 - UIBridge::m_localVideoHeight / DEF_FRAME_SHOW_HEIGHT,1,1 };
	nama->changeGSPreviewRect(nama->gsPreviewRect);
}
void Gui::onWindowResize(uint32_t width, uint32_t height)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = (float)width;
	io.DisplaySize.y = (float)height;
}
