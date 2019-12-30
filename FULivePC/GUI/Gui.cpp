
#include "Gui.h"
//#include "Config.h"	
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "rapidjson\document.h"
#include "rapidjson\filereadstream.h"
#include "stdio.h"
int oriWindowWidth = 0;
int oriWindowHeight = 0;
float scaleRatioW = 1.f;
float scaleRatioH = 1.f;
bool showUI = true;
extern HANDLE hMutex;

void resetBeautyParam()
{
	UIBridge::mEnableSkinDect = 1;
	UIBridge::mEnableHeayBlur = 0;
	UIBridge::mEnableExBlur = 0;
	UIBridge::mFaceBeautyLevel[0] = 70;
	UIBridge::mFaceBeautyLevel[1] = 50;
	UIBridge::mFaceBeautyLevel[2] = 50;
	UIBridge::mFaceBeautyLevel[3] = 70;
	UIBridge::mFaceBeautyLevel[4] = 0;
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

	UIBridge::mFaceShapeLevel[6] = 0;
	UIBridge::mFaceShapeLevel[7] = 0;
	UIBridge::mFaceShapeLevel[8] = 0;
}

bool doesFileExist(const std::string& filename)
{
	DWORD attr = GetFileAttributesA(filename.c_str());
	return (attr != INVALID_FILE_ATTRIBUTES);
}

bool isDirectoryExists(const std::string& filename)
{
	DWORD attr = GetFileAttributesA(filename.c_str());
	return ((attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY));
}

int get_fps()
{
	static int fps = 0;
	static int lastTime = GetTickCount(); // ms
	static int frameCount = 0;

	++frameCount;

	int curTime = GetTickCount();
	if (curTime - lastTime > 1000) // 取固定时间间隔为1秒
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
	std::cout << width << "," << height << std::endl;
	//ImGuiStyle::ScaleAllSizes(1.0f);
	scaleRatioW =(float)width/(float)oriWindowWidth ;
	scaleRatioH = (float)height / (float)oriWindowHeight;
}
HWND Gui::hWindow;
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
	glfwMakeContextCurrent(window);
	hWindow = glfwGetWin32Window(window);
	glfwSwapInterval(1); // Enable vsync
	gl3wInit();
	glfwSetWindowSizeCallback(window, window_size_callback);
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

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

	//std::string fontFile = "c:\\Windows\\Fonts\\msyh.ttc";
	std::string fontFile = "../../res/msyh.ttc";

	if (doesFileExist(fontFile))
	{
		io.Fonts->AddFontFromFileTTF(fontFile.c_str(), 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	}
	oriWindowWidth = width;
	oriWindowHeight = height;
	//pGui->onWindowResize(width, height);
	GLFWimage icons[1];
	Texture::SharedPtr tex = Texture::createTextureFromFile("LOGO.png", false);
	//icons[0].pixels = tex.getData();
	//glfwSetWindowIcon(window, 1, icons);

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

static void ShowHelpMarker(const char* desc)
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

static bool ImageButtonWithLabel(const char* text)
{
	ImGui::BeginGroup();
	//ImGui::ImageButton()
	ImGui::Text("%s",text);
	ImGui::EndGroup();
}

static bool LayoutButton(const ImVec2& pos,const ImVec2& size, const char* label)
{
	ImGui::BeginGroup();
	ImGui::Dummy(ImVec2(1,pos.y * scaleRatioH));
	ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine();
	bool ret = ImGui::Button(label, ImVec2(size.x * scaleRatioW ,size.y * scaleRatioH));
	ImGui::EndGroup();
	return ret;
}

static bool LayoutSelectable(const ImVec2& pos, const ImVec2& size, const char* label, bool selected)
{
	ImGui::BeginGroup();
	ImGui::Dummy(ImVec2(1, pos.y * scaleRatioH));
	ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine();
	bool ret = ImGui::SelectableBorder(label,selected,0,ImVec2(size.x * scaleRatioW,size.y * scaleRatioH));
	ImGui::EndGroup();
	return ret;
}

static bool LayoutSlider(const ImVec2& pos, const ImVec2& size, const char* label,float* v,float v_min,float v_max)
{
	ImGui::BeginGroup();
	ImGui::Dummy(ImVec2(1, pos.y * scaleRatioH));
	ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine();
	ImGui::PushItemWidth(size.x * scaleRatioW);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
	//bool ret = ImGui::SliderRoundScalar(label, ImGuiDataType_Float, v, &v_min, &v_max,"%.f");
	bool ret = ImGui::SliderFloat(label, v, v_min, v_max, "%.f");
	ImGui::PopStyleVar();
	ImGui::PopItemWidth();
	ImGui::EndGroup();
	return ret;
}

static void LayoutImage(const ImVec2& pos, const ImVec2& size, ImTextureID user_texture_id, const char* label)
{
	ImGui::BeginGroup();
	ImGui::Dummy(ImVec2(1, pos.y * scaleRatioH));
	ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine();
	ImGui::Image(user_texture_id,ImVec2(size.x * scaleRatioW,size.y * scaleRatioW));
	ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine(); ImGui::Text(label);
	ImGui::EndGroup();	
}

static bool LayoutImageButtonWithText(const ImVec2& pos, const ImVec2& size, ImTextureID user_texture_id, ImTextureID user_texture_id2, const char* label)
{
	ImGui::BeginGroup();
	bool ret = ImGui::ImageRoundButton2(user_texture_id, user_texture_id2, ImVec2(size.x * scaleRatioW,size.y * scaleRatioH));/* ImGui::SameLine(0.f, 27.f);*/
	ImGui::Spacing();
	ImGui::Dummy(ImVec2(pos.x * scaleRatioW, 1)); ImGui::SameLine(); ImGui::Text(label);
	ImGui::EndGroup();
	return ret;
}

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

static MyDocument GDocs[3];

static void ShowTabs(const char* title, bool* p_open, Nama::UniquePtr& nama)
{
	ImGui::Begin(title, p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

	if (GDocs[0].Name == NULL)
	{
		GDocs[0].Name = u8"美肤";
		GDocs[1].Name = u8"美型";
		GDocs[2].Name = u8"滤镜";
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
		//ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,ImVec2(0.0f,5.0f));
		//ImGui::PushItemWidth(52);
		const bool selected = ImGui::TabItem(ImVec2(135 * scaleRatioW,30* scaleRatioH),doc.Name, &doc.Open, 0);
		
		//ImGui::PopItemWidth();
		//ImGui::PopStyleVar();
		//ImGui::PopStyleVar();

		if (!selected)
			continue;
		
		switch (doc_n)
		{
		case 0:
		{
			ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 0.f));
			if (UIBridge::mEnableSkinDect)
			{
				LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_skinbeauty_open.png", false)->getTextureID(), u8"精准美肤");
			}
			else
			{
				LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_skinbeauty_Close.png", false)->getTextureID(), u8"精准美肤");
			}
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(225.f / 255.f, 228.f / 255.f, 238.f / 255.f, 1.f));
			//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
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
			std::string sliderNameArr[10] = { "list_icon_Grindingskin_open","list_icon_Skinwhitening_open", "list_icon_Ruddy_open",
				"list_icon_Brighteye_open","list_iconBeautifulteeth_open",
				u8"   磨皮", u8"   美白",u8"   红润", u8"   亮眼", u8"   美牙" };

			for (int i = 0; i < 5; i++)
			{
				if (UIBridge::mFaceBeautyLevel[i] == 0)
				{
					std::string closeIconFile = sliderNameArr[i].substr(0, sliderNameArr[i].find_last_of('_')) + "_Close";
					LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile(closeIconFile + ".png", false)->getTextureID(), sliderNameArr[5 + i].c_str());
				}
				else
				{
					LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile(sliderNameArr[i] + ".png", false)->getTextureID(), sliderNameArr[5 + i].c_str());
				}
				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

				if (LayoutSlider(ImVec2(22, 22), ImVec2(252, 10), ("##slider221" + std::to_string(i)).c_str(), &UIBridge::mFaceBeautyLevel[i], 0, 100))
				{
					nama->UpdateBeauty();
				}
				//ImGui:: SameLine(); ImGui::InputInt(("##input221" + std::to_string(i)).c_str(), (int*)&UIBridge::mFaceBeautyLevel[i]);
				ImGui::PopStyleVar();
			}
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
			//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
			if (LayoutButton(ImVec2(145, 38), ImVec2(126, 40), u8"恢复默认"))
			{
				resetBeautyParam();
				nama->UpdateBeauty();
			}
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			//ImGui::PopStyleColor();


			ImGui::PopStyleColor();
		}
		break;
		case 1:
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
			std::string faceShapeIconNameArr[9] = { "list_icon_Thinface_open", "list_icon_Bigeye_open",
				"list_icon_chin_open", "list_icon_forehead_open", "list_icon_Thinnose_open","list_icon_Mouthtype_open",
				"list_icon_v_open","list_icon_narrow_face_open","list_icon_little_face_open" };
			std::string faceShapeNameArr[9] = { u8"   瘦脸" ,u8"   大眼" ,u8"   下巴",u8"   额头" ,u8"   瘦鼻",u8"   嘴型",u8"   V脸",u8"   窄脸",u8"   小脸" };
			ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(224.f / 255.f, 227.f / 255.f, 238.f / 255.f, 1.f));
			ImGui::BeginChild("faceShape##2223",ImVec2(400,550) );
			ImGui::PopStyleColor();
			for (int i = 0; i < 9; i++)
			{
				if (UIBridge::faceType != 0 && i > 1)
				{
					break;
				}
				if (UIBridge::mFaceShapeLevel[i] == 0)
				{
					std::string closeIconFile = faceShapeIconNameArr[i].substr(0, faceShapeIconNameArr[i].find_last_of('_')) + "_Close";
					LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile(closeIconFile + ".png", false)->getTextureID(), faceShapeNameArr[i].c_str());
				}
				else
				{
					LayoutImage(ImVec2(22, 0), ImVec2(52, 52), Texture::createTextureFromFile(faceShapeIconNameArr[i] + ".png", false)->getTextureID(), faceShapeNameArr[i].c_str());
				}
				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
				if (i == 2 || i == 3 || i == 5)
				{
					if (LayoutSlider(ImVec2(22, 22), ImVec2(252, 10), (std::string("##slider23") + std::to_string(i)).c_str(), &UIBridge::mFaceShapeLevel[i], -50, 50))
					{
						nama->UpdateBeauty();
					}
				}
				else
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
		break;
		case 2:
		{
			ImGui::Dummy(ImVec2(1, 10 * scaleRatioH));			
			std::string filterNameArr[6] = { "list_image_origin", "list_image_bailiang1", "list_image_fennen1", "list_image_xiaoqingxin1", "list_image_lengsediao1", "list_image_nuansediao1" };
			for (int i = 0; i < 6; i++)
			{
				if (ImGui::ImageButton(Texture::createTextureFromFile(filterNameArr[i] + ".png", false)->getTextureID(), ImVec2(106 * scaleRatioW, 106 * scaleRatioH)))
				{
					nama->UpdateFilter(i);
					UIBridge::m_curFilterIdx = i;
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

void Gui::render(Nama::UniquePtr& nama)
{
	GLuint textureID = -1;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	resetBeautyParam();
	resetShapeParam();
	nama->UpdateBeauty();
	nama->SetCurrentShape(4);

	if (!ipcBridge.open(IPC_FILE_NAME, IpcBridge::OpenMode::Write))
	{
		printf("open ipc file failed!");
	}

	rapidjson::Document doc;
	FILE* fp = fopen("../../res/config.json", "rb"); // 非 Windows 平台使用 "r"
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
		//title window
		if (showUI)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(1360 * scaleRatioW, 60 * scaleRatioH), ImGuiCond_Always);
			ImGui::Begin("TitleWindow##21", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);			
			std::string filename = "title.png";
			Texture::SharedPtr tex = Texture::createTextureFromFile(filename, false);
			float my_tex_w = (float)ImGui::GetContentRegionAvailWidth();
			float my_tex_h = (float)ImGui::GetContentRegionAvail().y;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::Image(tex->getTextureID(), ImVec2(my_tex_w, my_tex_h), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
			ImGui::PopStyleVar();
			ImGui::End();
		}
		//navigation window
		if (showUI)
		{
			ImGui::SetNextWindowPos(ImVec2(10 * scaleRatioW, 70 * scaleRatioH), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(914 * scaleRatioW, 76 * scaleRatioH), ImGuiCond_Always);
			ImGui::Begin("Window1##2", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::Dummy(ImVec2(21 * scaleRatioW, 11 * scaleRatioH));
			ImGui::Text(u8"选择摄像头:");

			ImGui::SameLine();
			if (nama->CameraList().size())
			{
				ImGui::PushItemWidth(244 * scaleRatioW);
				static std::string item_current = nama->CameraList()[0];
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
				if(ImGui::BeginCombo("##slect camera122", u8"未检测到摄像头")) // The second parameter is the label previewed before opening the combo.				
					ImGui::EndCombo();				
				ImGui::PopItemWidth();
			}

			ImGui::SameLine();
			ImGui::Checkbox(u8"虚拟摄像头", &UIBridge::mNeedIpcWrite);
			ImGui::SameLine();
			ShowHelpMarker(u8"勾选后会开启虚拟摄像头功能，详细可参见帮助文档里内容");
			ImGui::SameLine(800 * scaleRatioW);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(225.f / 255.f, 228.f / 255.f, 238.f / 255.f, 1.f));
			if (ImGui::Button(u8"打开帮助文档"))
			{
				WinExec("notepad.exe ../../README.md", SW_SHOW);
			}
			ImGui::PopStyleColor();
			ImGui::End();
		}
		//frame window
		{
			if (showUI)
			{
				ImGui::SetNextWindowPos(ImVec2(10 * scaleRatioW, 156 * scaleRatioH), ImGuiCond_Always);
				ImGui::SetNextWindowSize(ImVec2(914 * scaleRatioW, 664 * scaleRatioH), ImGuiCond_Always);
				ImGui::Begin("Window2##2", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);
				ImGui::Dummy(ImVec2(888 * scaleRatioW, 500 * scaleRatioH));
				for (int i = 0; i < 7; i++)	ImGui::Spacing();

				{
					std::string *categoryNameArr = nullptr;
					std::string allCategory[]= { "list_icon_annimoji_nor","list_icon_Propmap_nor","list_icon_AR_nor", "list_icon_Changeface_nor",
						"list_icon_Expressionrecognition_nor",	"list_icon_Musicfilter_nor","list_icon_Bgsegmentation_nor",
						"list_icon_gesturerecognition_nor","list_icon_Hahamirror_nor","list_icon_Portraitdrive_nor",
						"Animoji",u8"道具贴图",u8"AR面具",u8"  换脸",
						u8"表情识别",u8"音乐滤镜",u8"背景分割",
						u8"手势识别",u8" 哈哈镜",u8"人像驱动", };
					int amount = 10;
					categoryNameArr = allCategory;
					std::string makeupCategory[] = {"list_icon_propmap_collapse_nor",u8"美妆"};
					if (UIBridge::showMakeUpWindow)
					{
						categoryNameArr = makeupCategory;
						amount =1;
						ImGui::Dummy(ImVec2(431 * scaleRatioW, 1)); ImGui::SameLine();
					}
					
					for (int i = 0; i < amount; i++)
					{
						if (i == 3)continue;//sb product logic
						if (UIBridge::bundleCategory == i || UIBridge::bundleCategory == BundleCategory::Makeup)
						{
							if (LayoutImageButtonWithText(ImVec2(0.f, 27.f), ImVec2(52, 52), Texture::createTextureFromFile("list_icon_propmap_collapse.png", false)->getTextureID(),
								Texture::createTextureFromFile("list_icon_propmap_collapse.png", false)->getTextureID(), categoryNameArr[amount + i].c_str()))
							{
								UIBridge::bundleCategory = -1;
								UIBridge::showItemSelectWindow = false;
							}
						}
						else
						{
							std::string iconFileName = categoryNameArr[i].substr(0, categoryNameArr[i].find_last_of("_")) + (1 ? "_hover.png" : "_nor.png");
							if (LayoutImageButtonWithText(ImVec2(0.f, 27.f), ImVec2(52, 52), Texture::createTextureFromFile(categoryNameArr[i] + ".png", false)->getTextureID(),
								Texture::createTextureFromFile(iconFileName, false)->getTextureID(), categoryNameArr[amount + i].c_str()))
							{//显示道具选择
								UIBridge::bundleCategory = i;
								if (UIBridge::showMakeUpWindow)
								{
									UIBridge::bundleCategory = BundleCategory::Makeup;
								}
								UIBridge::showItemSelectWindow = true;
							}
						}
						ImGui::SameLine(0.f, 27.f * scaleRatioW);
					}
				}
				ImGui::Dummy(ImVec2(1,1));
				ImGui::Dummy(ImVec2(321 * scaleRatioW, 1)); ImGui::SameLine();
				ImGui::PushItemWidth(272 * scaleRatioW);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 6.0f);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(225.f / 255.f, 228.f / 255.f, 238.f / 255.f, 1.f));				
				//ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 0.f));
				if (ImGui::SliderString("##modeSlider", u8"AR功能", u8"美妆", (int*)&UIBridge::showMakeUpWindow, 0, 1))
				{
					if (UIBridge::showMakeUpWindow)
					{
						UIBridge::showItemSelectWindow = true;
						UIBridge::bundleCategory = BundleCategory::Makeup;
					}
					else
					{
						UIBridge::showItemSelectWindow = false;
						UIBridge::bundleCategory = -1;
					}
				}
				ImGui::PopStyleVar(2);
				ImGui::PopStyleColor(1);
				ImGui::PopItemWidth();
				ImGui::End();
				
			}
		
			//Nama渲染
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(58.f / 255.f, 61.f / 255.f, 82.f / 255.f, 1.f));
			ImGui::SetNextWindowPos(ImVec2(10 * scaleRatioW, 156 * scaleRatioH), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(888 * scaleRatioW, 500 * scaleRatioH), ImGuiCond_Always);
			ImGui::Begin("frame##4", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);			
			float frameWidth = (float)888.f * scaleRatioW;
			float frameHeight = (float)500.f * scaleRatioH;
			ImVec2 frameUV_LB = ImVec2(1, 0);
			ImVec2 frameUV_RT = ImVec2(0, 1);
			//WaitForSingleObject(hMutex, INFINITE);
			cv::Mat frameMat = nama->GetFrame();			
			static cv::Mat processedFrame = frameMat.clone();
			//ReleaseMutex(hMutex);
			cv::cvtColor(frameMat, processedFrame, cv::COLOR_BGR2RGBA);
			//float tempTime = GetTickCount();
			if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED))
			{
				nama->RenderItems(processedFrame.data);
				//nama->DrawLandmarks(processedFrame.data);
			}			
			//printf("RenderItems cost %f \n", GetTickCount()- tempTime);			
			{
				cv::Mat bgra[4];
				cv::split(processedFrame, bgra);
				bgra[3] = 255.0f;
				cv::merge(bgra, 4, processedFrame);
			}
			if (UIBridge::mNeedIpcWrite)
			{
				cv::cvtColor(processedFrame, processedFrame, cv::COLOR_RGBA2BGRA);
				size_t frameSize = ipcBridge.write(MEDIASUBTYPE_RGB32, processedFrame.cols, processedFrame.rows, processedFrame.data);
				cv::cvtColor(processedFrame, processedFrame, cv::COLOR_BGRA2RGBA);
			}
			UIBridge::mFPS = get_fps();
			//printf("UIBridge::mFPS: %d\n", UIBridge::mFPS);
			UIBridge::mRenderTime = 1000.f / (float)UIBridge::mFPS;
			UIBridge::mResolutionWidth = processedFrame.cols;
			UIBridge::mResolutionHeight = processedFrame.rows;
			glBindTexture(GL_TEXTURE_2D, textureID);
			glPixelStorei(GL_UNPACK_ROW_LENGTH, processedFrame.step / processedFrame.elemSize());
			glPixelStorei(GL_UNPACK_ALIGNMENT, (processedFrame.step & 3) ? 1 : 4);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, processedFrame.cols, processedFrame.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, processedFrame.data);
			glBindTexture(GL_TEXTURE_2D, 0);
			float rotio = (float)UIBridge::mResolutionWidth / (float)UIBridge::mResolutionHeight;
			if (rotio<1.7f)//确保宽高比1.777显示
			{
				float UVSub = (1.0f - 1.0f / 1.776f) / 4.0f;
				frameUV_LB = ImVec2(1.f, UVSub);
				frameUV_RT = ImVec2(0.f, 1.f-UVSub);
			}
			ImGui::Image((void *)(intptr_t)textureID, ImVec2(frameWidth, frameHeight), frameUV_LB, frameUV_RT, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
			ImGui::End();
			ImGui::PopStyleColor();
			//FPS信息显示
			if (showUI)
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
						UIBridge::showDegubInfoWindow = true;
					}
					ImGui::End();
					ImGui::PopStyleVar();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
				}
			}
		
			if (showUI)
			{
				//悬浮选择道具
				if (UIBridge::showItemSelectWindow)
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
							if (i> sizeof(gBundlePath) / sizeof(gBundlePath[0]))
							{
								break;
							}
							UIBridge::FindAllBundle(gBundlePath[i], UIBridge::categoryBundles[i]);
						}
					}
					
					for (int i = 0; i < UIBridge::categoryBundles[UIBridge::bundleCategory].size(); i++)
					{
						std::string itemName = UIBridge::categoryBundles[UIBridge::bundleCategory][i];

						std::string iconName = itemName.substr(0, itemName.find_last_of('.'));
						Texture::SharedPtr tex = Texture::createTextureFromFile(iconName + ".png", false);
						//找不到图标
						if (!tex)
						{
							tex = Texture::createTextureFromFile("icon_Movebutton_nor.png", false);
						}

						if (ImGui::ImageRoundButton(UIBridge::m_curRenderItemUIID, tex->getTextureID(), ImVec2(56 * scaleRatioW, 56 * scaleRatioH)))
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
							nama->SelectBundle(gBundlePath[UIBridge::bundleCategory] + "/" + itemName);
						}
						ImGui::SameLine(0.f, 22.f * scaleRatioW);
					}
					ImGui::End();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
				}
			}
		}
		//道具提示
		if (showUI)		
		{
			std::string tip;
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
					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .70f));
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(252.f / 255.f, 253.f / 255.f, 255.f / 255.f, .0f));
					ImGui::SetNextWindowPos(ImVec2(366 * scaleRatioW, 472 * scaleRatioH), ImGuiCond_Always);
					ImGui::SetNextWindowSize(ImVec2(156 * scaleRatioW, 30 * scaleRatioH), ImGuiCond_Always);
					ImGui::Begin("itemTips##1563", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
					ImGui::Dummy(ImVec2(30, 1)); ImGui::SameLine();
					ImGui::TextColored(ImVec4(0.0, 0.0, 0.0, 1.0), tip.c_str());
					ImGui::End();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
				}
			}
			else
			{
				UIBridge::showItemTipsWindow = false;
				UIBridge::mLastTime = 0.0;
			}
		}
		
		//right window
		if (showUI)
		{
			ImGui::SetNextWindowPos(ImVec2(934 * scaleRatioW, 70 * scaleRatioH), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(416 * scaleRatioW, 750 * scaleRatioH), ImGuiCond_Always);
			ShowTabs("rightTabs", 0, nama);
		}
		//ImGui::ShowDemoWindow();
		
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
}

void Gui::onWindowResize(uint32_t width, uint32_t height)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = (float)width;
	io.DisplaySize.y = (float)height;
	//std::cout << width << "," << height << std::endl;
	//
}
