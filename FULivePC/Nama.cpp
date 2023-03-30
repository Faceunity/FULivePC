#include "Camera.h"
#include "FuController.h"
#include "Nama.h"
#include "Config.h"	
#include "Gui.h"
#include "fu_tool.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include <CNamaSDK.h>				//nama SDK
#include <authpack.h>			//nama SDK
#include "GuiTool.h"
#include "GuiGS.h"
#include "BgSegUpdate.h"

using namespace NamaExampleNameSpace;
using namespace std;
bool Nama::mHasSetup = false;


NamaState Nama::mNamaAppState;
NamaState Nama::mNamaAppStateBackAR;
NamaState Nama::mNamaAppStateBackGS;

#ifdef _WIN32

#include "Sound/MP3.h"
extern HGLRC new_context;
#else
#include "NaMaGLContext.h"
#include "fu_tool_mac.h"
#include "Sound/MP3_mac.h"
extern void* new_context;
#endif

map<int, Mp3*> mp3Map;

string Nama::mFilters[6] = { "origin", "bailiang1", "fennen1", "xiaoqingxin1", "lengsediao1", "nuansediao1" };
std::unique_ptr<Nama> Nama::pNama = nullptr;

void Nama::create( bool enable)
{
	pNama = UniquePtr(new Nama);
	mNamaAppState.EnableNama = enable;
	mNamaAppStateBackGS.EnableNama = enable;
	pNama->Init();
	pNama->gsPreviewRect = {0.5,0.5,1.0,1.0};
	pNama->changeGSPreviewRect(pNama->gsPreviewRect);
}

Nama::Nama()
{
	mFrameID = 0;
	mIsBeautyOn = true;
	mBeautyHandles = 0;
	mMakeUpHandle = -1;
}

bool Nama::Release() {
	if (true == mHasSetup)
	{
		fuDestroyAllItems();
		fuOnDeviceLost();
		fuDestroyLibData();
		return true;
	}
	return false;
}

Nama::~Nama()
{
	SaveRencentColorToFile();
	map<int, Mp3*>::iterator it;
	for (it = mp3Map.begin(); it != mp3Map.end(); it++)
	{
		it->second->Cleanup();
		delete it->second;
		UIBridge::mNeedPlayMP3 = false;
	}
}

#ifdef _WIN32

PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),
	1u,
	PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW,
	PFD_TYPE_RGBA,
	32u,
	0u, 0u, 0u, 0u, 0u, 0u,
	8u,
	0u,
	0u,
	0u, 0u, 0u, 0u,
	24u,
	8u,
	0u,
	PFD_MAIN_PLANE,
	0u,
	0u, 0u };

#endif

void InitOpenGL()
{
#ifdef _WIN32
	HWND hw = CreateWindowExA(
		0, "EDIT", "", ES_READONLY,
		0, 0, 1, 1,
		NULL, NULL,
		GetModuleHandleA(NULL), NULL);
	HDC hgldc = GetDC(hw);
	int spf = ChoosePixelFormat(hgldc, &pfd);
	int ret = SetPixelFormat(hgldc, spf, &pfd);
	HGLRC hglrc = wglCreateContext(hgldc);
	wglMakeCurrent(hgldc, hglrc);
	printf("hw=%08x hgldc=%08x spf=%d ret=%d hglrc=%08x\n",
		hw, hgldc, spf, ret, hglrc);

#endif

	//Init GL and Window
}

bool Nama::CheckGLContext()
{

	int add0, add1, add2, add3;

#ifdef _WIN32
	add0 = (int)wglGetProcAddress("glGenFramebuffersARB");
	add1 = (int)wglGetProcAddress("glGenFramebuffersOES");
	add2 = (int)wglGetProcAddress("glGenFramebuffersEXT");
	add3 = (int)wglGetProcAddress("glGenFramebuffers");
#else
	add0 = (int)glfwGetProcAddress("glGenFramebuffersARB");
	add1 = (int)glfwGetProcAddress("glGenFramebuffersOES");
	add2 = (int)glfwGetProcAddress("glGenFramebuffersEXT");
	add3 = (int)glfwGetProcAddress("glGenFramebuffers");
#endif

	const GLubyte* OpenGLVersion = glGetString(GL_VERSION);

	printf("gl ver test (%s:%d):version:%s  %08x %08x %08x %08x\n", __FILE__, __LINE__, OpenGLVersion, add0, add1, add2, add3);
	return add0 | add1 | add2 | add3;

}

bool Nama::Init()
{
	if (false == mHasSetup && true == mNamaAppState.EnableNama)
	{
		CheckGLContext();

		fuSetLogLevel(FU_LOG_LEVEL_ERROR);

		fuSetup(nullptr, 0, nullptr, g_auth_package, sizeof(g_auth_package));
		// setup without license, only render 1000 frames.
		//fuSetup(nullptr, 0, nullptr, nullptr, 0);

		printf("Nama version:%s \n", fuGetVersion());
		vector<char> tongue_model_data;
		if (false == FuTool::LoadBundle(g_tongue, tongue_model_data))
		{
			cout << "Error: fail load tongue model" << g_tongue << endl;
		}
		else {
			fuLoadTongueModel(reinterpret_cast<float*>(&tongue_model_data[0]), tongue_model_data.size());
		}

		vector<char> ai_model_data;
		if (false == FuTool::LoadBundle(g_ai_faceprocessor, ai_model_data))
		{
			cout << "Error:fail load faceprocessor model" << g_ai_faceprocessor << endl;
		}
		else {
			fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_model_data[0]), ai_model_data.size(), FUAITYPE::FUAITYPE_FACEPROCESSOR);
		}

		vector<char> ai_human_model_data;
		if (false == FuTool::LoadBundle(g_ai_humanprocessor, ai_human_model_data))
		{
			cout << "Error: fail load humanprocessor model" << g_ai_humanprocessor << endl;
		}
		else {
			fuSetHumanSegMode(FUAIHUMAN_SEG_GPU_COMMON);
			//fuPreprocessAIModelFromPackage(reinterpret_cast<float*>(&ai_human_model_data[0]), ai_human_model_data.size(), FUAITYPE::FUAITYPE_HUMAN_PROCESSOR);
			fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_human_model_data[0]), ai_human_model_data.size(), FUAITYPE::FUAITYPE_HUMAN_PROCESSOR);
		}
		vector<char> ai_gesture_model_data;
		if (false == FuTool::LoadBundle(g_ai_hand_processor, ai_gesture_model_data))
		{
			cout << "Error: fail load gesture model" << g_ai_hand_processor << endl;
		}
		else {
			fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_gesture_model_data[0]), ai_gesture_model_data.size(), FUAITYPE::FUAITYPE_HANDGESTURE);

		}
		mModuleCode = fuGetModuleCode(0);
		mModuleCode1 = fuGetModuleCode(1);
		{
			vector<char> propData;
			if (false == FuTool::LoadBundle(g_faceBeautification, propData))
			{
				cout << "load face beautification data failed." << endl;
			} 
			else {
				cout << "load face beautification data." << endl;
				mBeautyHandles = fuCreateItemFromPackage(&propData[0], propData.size());
			}
		}


		if (CheckModuleCode(Makeup))
		{
			vector<char> propData;
			if (false == FuTool::LoadBundle(g_Makeup, propData))
			{
				cout << "load face makeup data failed." << endl;
			}
			else {
				cout << "load face makeup data." << endl;
				mMakeUpHandle = fuCreateItemFromPackage(&propData[0], propData.size());
			}
			//fuItemSetParamd(mMakeUpHandle, "is_clear_makeup", 1);
		}

		{
			vector<char> propData;
			if (false == FuTool::LoadBundle(g_bodySlim, propData))
			{
				cout << "load body slim data failed." << endl;
			}
			else {
				cout << "load body slim data." << endl;
				mBodyShapeHandle = fuCreateItemFromPackage(&propData[0], propData.size());
			}
		}

		{
			vector<char> propData;
			if (false == FuTool::LoadBundle(g_greenscreen, propData))
			{
				cout << "load gs data failed." << endl;
			}
			else {
				cout << "load gs data." << endl;
				mGSHandle = fuCreateItemFromPackage(&propData[0], propData.size());
			}
		}

		{
			vector<char> propData;
			if (false == FuTool::LoadBundle(g_fxaa, propData))
			{
				cout << "load fx data failed." << endl;
			}
			else {
				cout << "load fx data." << endl;
				mFxaaHandles = fuCreateItemFromPackage(&propData[0], propData.size());
			}
		}
		
		fuItemSetParamd(mBodyShapeHandle, "Debug", 0.0);

		fuItemSetParamd(mMakeUpHandle, "machine_level", 1.0);
		//fuSetDefaultOrientation(0);
		float fValue = 0.5f;
		fuSetFaceTrackParam((void*)"mouth_expression_more_flexible", &fValue);

		fuSetMaxFaces(4);

		InitController();
		mHasSetup = true;
	}
	else
	{
		if (mNamaAppState.EnableNama) fuOnDeviceLost();
		mHasSetup = false;
	}

	return true;
}

bool Nama::InitController()
{
	if (m_Controller != nullptr)
		return false;
	m_Controller = std::make_shared<FuController>();
	m_Controller->InitController(g_control, g_control_cfg);
	m_Controller->InitFXAA(g_fxaa);
	return true;
}

void Nama::LoadAvatarHandTrackBundle()
{
	if (m_Controller == nullptr)
		return;
	//Bind Animation For Hand
	auto files = FuTool::getFilesInFolder(g_pose_track_folder, "bundle");
	for (auto & file : files)
	{
#if __APPLE__
		// 这里macosx 为绝对路径，我们需要获取相对于 Resource.bundle的相对路径
		file = FuToolMac::GetRelativePathDependResourceBundle(file.c_str());
#endif
		m_Controller->BindBundle(file);
		avatarBundles.emplace_back(file);
	}
}

void Nama::LoadAvatarBundles(const std::vector<std::string>& bundleNames)
{
	if (m_Controller == nullptr)
		return;
	for (auto& i : bundleNames) {
		m_Controller->BindBundle(i);
	}
	avatarBundles = bundleNames;
	Nama::mNamaAppState.RenderAvatar = true;
	UIBridge::m_curRenderItem = -1;
}

void Nama::UnLoadAvatar()
{
	if (m_Controller == nullptr)
		return;
	for (auto& i : avatarBundles) {
		m_Controller->UnBindBundle(i);
	}
	avatarBundles.clear();
	Nama::mNamaAppState.RenderAvatar = false;
	Nama::mNamaAppState.RenderAvatarBear = false;
}

void Nama::UnLoadMakeup() 
{
	if (UIBridge::bundleCategory != BundleCategory::Makeup && mMakeUpHandle != -1)
	{
		fuUnbindItems(mMakeUpHandle, &UIBridge::m_curRenderItem, 1);
		UIBridge::m_curBindedItem = -1;
		UIBridge::m_curRenderItem = -1;
	}
}

void Nama::ReloadItems() {
#ifdef _WIN32
	HGLRC context = wglGetCurrentContext();
	HWND wnd = (HWND)Gui::hOffscreenWindow;
	wglMakeCurrent(GetDC(wnd), new_context);
#else
	void* curCtx = NaMaGetCurrentGLCtx();
	NaMaMakeAsCurrentCtx(new_context);
#endif
	UnLoadAvatar();
	DestroyAll();
	fuDestroyAllItems();
	fuOnDeviceLost();
	mMakeUpHandle = -1;
	UIBridge::m_curRenderItem = -1;
	UIBridge::m_curPlayingMusicItem = -1;
	m_CMakeupMap.clear();
	m_CMakeupTypeMap.clear();
	{
		vector<char> propData;
		if (false == FuTool::LoadBundle(g_assetDir + g_faceBeautification, propData))
		{
			cout << "load face beautification data failed." << endl;
		}
		else {
			cout << "load face beautification data." << endl;
			mBeautyHandles = fuCreateItemFromPackage(&propData[0], propData.size());
		}
	}

	{
		vector<char> propData;
		if (false == FuTool::LoadBundle(g_assetDir + g_bodySlim, propData))
		{
			cout << "load body slim data failed." << endl;
		}
		else {
			cout << "load body slim data." << endl;
			mBodyShapeHandle = fuCreateItemFromPackage(&propData[0], propData.size());
		}
	}

	{
		vector<char> propData;
		if (false == FuTool::LoadBundle(g_assetDir + g_greenscreen, propData))
		{
			cout << "load gs data failed." << endl;
		}
		else {
			cout << "load gs data." << endl;
			mGSHandle = fuCreateItemFromPackage(&propData[0], propData.size());
		}
	}

	{
		vector<char> propData;
		if (false == FuTool::LoadBundle(g_assetDir + g_fxaa, propData))
		{
			cout << "load fx data failed." << endl;
		}
		else {
			cout << "load fx data." << endl;
			mFxaaHandles = fuCreateItemFromPackage(&propData[0], propData.size());
		}
	}

	float fValue = 0.5f;
	fuSetFaceTrackParam((void*)"mouth_expression_more_flexible", &fValue);

	fuSetMaxFaces(4);

	m_Controller->InitController(g_assetDir + g_control, g_assetDir + g_control_cfg);
	m_Controller->InitFXAA(g_assetDir + g_fxaa);
	UpdateFilter(UIBridge::m_curFilterIdx);
	UpdateBeauty();
	UpdateGreenScreen();
	SetGSKeyColor(GUIGS::GetCurColorCircle());
	changeGSPreviewRect(gsPreviewRect);
	if (UIBridge::mCurRenderGSSAItemName != "NONE") {
		if (UIBridge::mCurRenderGSSAItemName == GSSAFEAREA_USERFILE_PIC) {
#if _WIN32
			cv::Mat mat = cv::imread(UIBridge::mCurRenderGSSAItemName, cv::IMREAD_REDUCED_COLOR_4);
#elif __APPLE__
			cv::Mat mat = cv::imread(FuToolMac::GetDocumentPath() + "/" + GSSAFEAREA_USERFILE_PIC);
#endif
			cv::cvtColor(mat, mat, cv::COLOR_BGR2RGBA);
			UpdateGSSA(mat);
		}
		else {
			string imagePath;
#if _WIN32
			imagePath = gGSSAPic + "/" + UIBridge::mCurRenderGSSAItemName;
#elif __APPLE__
			imagePath = UIBridge::GetFileFullPathFromResourceBundle((gGSSAPic + "/" + UIBridge::mCurRenderGSSAItemName + ".jpg").c_str());
#endif 
			cv::Mat mat = cv::imread(imagePath);
			cv::cvtColor(mat, mat, cv::COLOR_BGR2RGBA);
			UpdateGSSA(mat);
		}
	}
#ifdef _WIN32
	wglMakeCurrent(GetDC(wnd), context);
#else
	NaMaMakeAsCurrentCtx(curCtx);
#endif
}

int Nama::IsTracking()
{
	if (false == mNamaAppState.EnableNama)
	{
		return 0;
	}
	return fuIsTracking();
}

void Nama::SwitchBeauty(bool bValue)
{
	mIsBeautyOn = bValue;
}

void Nama::SetCurrentShape(int index)
{
	if (false == mNamaAppState.EnableNama || false == mIsBeautyOn || mBeautyHandles == 0)return;
	if (0 <= index <= 4)
	{
		int res = fuItemSetParamd(mBeautyHandles, "face_shape", index);
	}
}

int Nama::CreateMakeupBundle(string bundleName)
{
	int fakeBundleID = 0;

	if (false == mNamaAppState.EnableNama || false == mIsBeautyOn || mMakeUpHandle == -1)return 0;
	using namespace std;
	using namespace rapidjson;

	auto pos = mMakeupsMap.find(bundleName);
	if (pos == mMakeupsMap.end())
	{
		rapidjson::Document doc;

		rapidjson::Document& dd = doc;
		string jsonpath = bundleName + "/makeup.json";

		FILE* fp = fopen(jsonpath.c_str(), "rb");
		if (!fp)
		{
			return 0;
		}
		char readBuffer[65536];
		FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		doc.ParseStream(is);
		fclose(fp);

		vector<MakeupParam> paramArr;
		for (rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); itr++)
		{
			MakeupParam param;
			Value jKey;
			Value jValue;
			Document::AllocatorType allocator;
			jKey.CopyFrom(itr->name, allocator);
			jValue.CopyFrom(itr->value, allocator);
			if (jKey.IsString())
			{
				param.name = jKey.GetString();
			}
			if (jValue.IsArray())
			{
				param.type = MakeupParamType::MArray;
				for (auto& v : jValue.GetArray())
				{
					param.colorArr.push_back(v.GetFloat());
				}
			}
			if (jValue.IsString())
			{
				param.type = MakeupParamType::MString;
				param.tex = jValue.GetString();
			}
			if (jValue.IsInt())
			{
				param.type = MakeupParamType::MInt;
				param.lip_type = jValue.GetInt();

			}
			if (jValue.IsFloat())
			{
				param.type = MakeupParamType::MFloat;
				param.brow_warp = jValue.GetFloat();
			}
			if (jValue.IsDouble())
			{
				param.type = MakeupParamType::MFloat;
				param.brow_warp = jValue.GetDouble();
			}
			paramArr.push_back(param);
		}
		mMakeupsMap[bundleName] = paramArr;
		fakeBundleID = mMakeupsMap.size() + 666;
	}

	return fakeBundleID;
}

void Nama::SelectMakeupBundle(string bundleName)
{
	if (bundleName.size() == 0)
	{
		//reset params
		fuItemSetParamd(mMakeUpHandle, "is_makeup_on", 0);
	}
	fuItemSetParamd(mMakeUpHandle, "is_makeup_on", 1);
	auto paramArr = mMakeupsMap[bundleName];

	for (auto param : paramArr)
	{
		switch (param.type)
		{
		case MakeupParamType::MArray:
			fuItemSetParamdv(mMakeUpHandle, const_cast<char*>(param.name.c_str()), (double*)param.colorArr.data(), param.colorArr.size());
			break;
		case MakeupParamType::MFloat:
			fuItemSetParamd(mMakeUpHandle, const_cast<char*>(param.name.c_str()), param.brow_warp);
			break;
		case MakeupParamType::MInt:
			fuItemSetParamd(mMakeUpHandle, const_cast<char*>(param.name.c_str()), param.lip_type);
			break;
		case MakeupParamType::MString:
		{
			Texture::SharedPtr pTexture = Texture::createTextureFromFile(bundleName + "/" + param.tex, false);
			if (pTexture)
			{
				fuCreateTexForItem(mMakeUpHandle, const_cast<char*>(param.name.c_str()), pTexture->getData(), pTexture->m_width, pTexture->m_height);
			}
			else
			{
				printf("Load makeup texture %s failed!\n", (bundleName + "/" + param.tex).c_str());
			}
			break;
		}
		default:
			break;
		}
	}
}

void Nama::UpdateFilter(int index)
{
	if (false == mNamaAppState.EnableNama || false == mIsBeautyOn || mBeautyHandles == 0)return;

	fuItemSetParams(mBeautyHandles, "filter_name", &mFilters[index][0]);
	fuItemSetParamd(mBeautyHandles, "filter_level", UIBridge::mFilterLevel[index] / 100.0f);
}


void Nama::UpdateBeauty()
{
	if (false == mNamaAppState.EnableNama || mBeautyHandles == 0)
	{
		return;
	}
	if (false == mIsBeautyOn)return;
	for (int i = 2; i < MAX_BEAUTYFACEPARAMTER; i++)
	{
		if (i == 2)
		{
			fuItemSetParamd(mBeautyHandles, const_cast<char*>(g_faceBeautyParamName[i].c_str()), UIBridge::mFaceBeautyLevel[i] * 6.0 / 100.f);
		}
		else
		{
			fuItemSetParamd(mBeautyHandles, const_cast<char*>(g_faceBeautyParamName[i].c_str()), UIBridge::mFaceBeautyLevel[i] / 100.f);
		}
	}

	for (int i = 0; i < MAX_FACESHAPEPARAMTER; i++)
	{
		if (g_faceShapeParamShowFlag[i] == FACE_SHAPE_SHOW_FLAG_MIDDLE)
		{
			UIBridge::mFaceShapeLevel[i] += 50;
		}
		fuItemSetParamd(mBeautyHandles, const_cast<char*>(g_faceShapeParamName[i].c_str()), UIBridge::mFaceShapeLevel[i] / 100.0f);
		/*if (i == 12)
		{
			wchar_t szBuf[2048] = { 0 };
			wsprintf(szBuf,L"liufei set data %d", (int)UIBridge::mFaceShapeLevel[i]);
			OutputDebugString(szBuf);

		}*/


		if (g_faceShapeParamShowFlag[i] == FACE_SHAPE_SHOW_FLAG_MIDDLE)
		{
			UIBridge::mFaceShapeLevel[i] -= 50;
		}
	}
	fuItemSetParamd(mBeautyHandles, "skin_detect", UIBridge::mFaceBeautyLevel[0]);
	map<int, int> blurType = { {0,2},{1,0},{2,1},{3,3} };
	fuItemSetParamd(mBeautyHandles, "blur_type", blurType[UIBridge::mFaceBeautyLevel[1]]);
	fuItemSetParamd(mBeautyHandles, "face_shape_level", 1);
	if (!(UIBridge::showLightMakeupTip && !UIBridge::showGreenScreen)) {
		fuItemSetParamd(mBeautyHandles, "filter_level", UIBridge::mFilterLevel[UIBridge::m_curFilterIdx] / 100.0f);
	}
}

#include "rapidjson.h"
#include "stringbuffer.h"
#include "prettywriter.h"

void Nama::LoadRencentColorFromFile()
{

	ifstream in(gGSColorConfig.c_str());
	if (!in.is_open()) {
		fprintf(stderr, "fail to read json file: %s\n", gGSColorConfig.data());
		return;
	}

	string json_content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	in.close();

	rapidjson::Document dom;
	if (!dom.Parse(json_content.c_str()).HasParseError())
	{
		if (dom.HasMember("colors") && dom["colors"].IsArray())
		{
			const rapidjson::Value& arr = dom["colors"];

			for (int i = 0; i < arr.Size(); ++i) {

				if (arr[i].IsArray() && arr[i].Size() == 3)
				{
					if (arr[i][0].IsInt() && arr[i][1].IsInt() && arr[i][2].IsInt())
					{
						cv::Vec4b data;
						data[3] = 255;
						data[2] = arr[i][2].GetInt();
						data[1] = arr[i][1].GetInt();
						data[0] = arr[i][0].GetInt();

						gui_tool::ColorBag color = gui_tool::ColorBag(data);
						m_queueRencetColor.push(color);
					}
				}

			}
		}
	}
}

#define  MAX_COLOR_QUEUE (10)

/* 关闭的时候把当前的Color丢进保存 */
void Nama::AddRecentColor(cv::Vec4b data)
{

	gui_tool::ColorBag color = gui_tool::ColorBag(data, nullptr);

	if (m_queueRencetColor.size() > MAX_COLOR_QUEUE)
	{
		m_queueRencetColor.pop();
		m_queueRencetColor.push(color);
	}
	else
	{
		m_queueRencetColor.push(color);
	}
}

void Nama::SaveRencentColorToFile()
{
	rapidjson::StringBuffer buf;
	//rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf); // it can word wrap

	writer.StartObject();

	writer.Key("colors");
	writer.StartArray();

	auto tmpQueue = m_queueRencetColor;

	while (!tmpQueue.empty())
	{
		auto data = tmpQueue.front();

		writer.StartArray();

		writer.Int(data.vecColorRGBA[0][0]);
		writer.Int(data.vecColorRGBA[0][1]);
		writer.Int(data.vecColorRGBA[0][2]);

		writer.EndArray();

		tmpQueue.pop();
	}


	writer.EndArray();

	writer.EndObject();


	const char* json_content = buf.GetString();

	ofstream outfile;
	outfile.open(gGSColorConfig.data());
	if (!outfile.is_open()) {
		fprintf(stderr, "fail to open file to write: %s\n", gGSColorConfig.data());
	}

	outfile << json_content << endl;
	outfile.close();

}

/*
在UI绘制线程Context内调用
*/
vector<gui_tool::ColorBag> Nama::GetRencentColor()
{
	if (m_queueRencetColor.size() == 0)
	{
		LoadRencentColorFromFile();
	}

	vector<gui_tool::ColorBag> data;
	auto tmpQueue = m_queueRencetColor;

	while (!tmpQueue.empty())
	{
		data.push_back(tmpQueue.front());
		tmpQueue.pop();
	}

	return data;
}

void Nama::SetGSKeyColor(cv::Vec4b data)
{
	if (mGSHandle == -1)
	{
		return;
	}

	double key_color[] = { data[0], data[1], data[2] };

	printf("key color find:%d,%d,%d \r\n", (int)data[0], (int)data[1], (int)data[2]);

	fuItemSetParamdv(mGSHandle, "key_color", key_color, 3);
}

void NamaExampleNameSpace::Nama::GetGSParamd(vector<int>& data)
{
	data.push_back(fuItemGetParamd(mGSHandle, "chroma_thres") * 100);
	data.push_back(fuItemGetParamd(mGSHandle, "chroma_thres_T") * 100);
	data.push_back(fuItemGetParamd(mGSHandle, "alpha_L") * 100);
}

void Nama::UpdateGreenScreen()
{
	if (mGSHandle == -1)
	{
		return;
	}

	for (int i = 0; i < MAX_GREEN_SCREEN_PARAM; i++)
	{
		fuItemSetParamd(mGSHandle, const_cast<char*>(g_GSParamName[i].c_str()), UIBridge::mGSParam[i] / 100.f);
	}
#if 0
	fuItemSetParamd(mGSHandle,"start_x", 0);
	fuItemSetParamd(mGSHandle,"start_y", 0);
	fuItemSetParamd(mGSHandle,"end_x", 1);
	fuItemSetParamd(mGSHandle,"end_y", 1);
#endif
}


void Nama::UpdateGSBg(cv::Mat & dataRGBA)
{
	if (mGSHandle == -1)
	{
		return;
	}

	//DWORD time1 = GetTickCount();
	fuCreateTexForItem(mGSHandle, "tex_bg", dataRGBA.data, dataRGBA.cols, dataRGBA.rows);
	/*DWORD time2 = GetTickCount();

	printf("liufei cost time:%d \r\n", time2 - time1);*/
}

void Nama::UpdateGSSA(cv::Mat& dataRGBA) {
	fuItemSetParamd(mGSHandle, "is_use_template", 1);
	fuCreateTexForItem(mGSHandle, "tex_template", dataRGBA.data, dataRGBA.cols, dataRGBA.rows);
}

void Nama::NonuseGSSA() {
	fuItemSetParamd(mGSHandle, "is_use_template", 0);
}


//更新背景分割的背景数据
void Nama::UpdateSegBg(cv::Mat & dataRGBA)
{
	if (UIBridge::bundleCategory != BundleCategory::BackgroundSegmentation || UIBridge::m_curRenderItem == -1)
	{
		return;
	}

	fuItemSetParamd(UIBridge::m_curRenderItem, "bg_align_type", 1);
	
	fuCreateTexForItem(UIBridge::m_curRenderItem, "tex_bg_seg", dataRGBA.data, dataRGBA.cols, dataRGBA.rows);

}


bool Nama::IsBodyShapeOpen()
{
	bool bRet = false;
	for (int i = 0; i < MAX_BODY_SHAPE_PARAM; i++)
	{
		if (UIBridge::mBodyShapeLevel[i] != 0)
		{
			bRet = true;
			break;
		}
	}

	return bRet;
}

void Nama::UpdateBodyShape()
{
	if (false == mNamaAppState.EnableNama || mBodyShapeHandle == 0)
	{
		return;
	}

	for (int i = 0; i < MAX_BODY_SHAPE_PARAM; i++)
	{
		fuItemSetParamd(mBodyShapeHandle, const_cast<char*>(g_bodyShapeParamName[i].c_str()), UIBridge::mBodyShapeLevel[i] / 100.0f);
	}
}
/// 暂停当前MP3
void Nama::pauseCurrentMp3(){
	mp3Map[UIBridge::m_curPlayingMusicItem]->Pause();
}
void Nama::resumeCurrentMp3(){
	mp3Map[UIBridge::m_curPlayingMusicItem]->Play();
}

void Nama::SetCMDoubles(const std::string& key, double* values, uint32_t count)
{
	if (mMakeUpHandle > 0)
	{
		fuItemSetParamdv(mMakeUpHandle, key.data(), values, count);
	}
}

void Nama::SetCMDouble(const std::string& key, double v)
{
	if (mMakeUpHandle > 0)
	{
		fuItemSetParamd(mMakeUpHandle, key.data(), v);
	}
}

void Nama::SetCurDouble(const std::string& key, double v)
{
	if (UIBridge::m_curRenderItem > 0)
	{
		fuItemSetParamd(UIBridge::m_curRenderItem, key.data(), v);
	}
}

void Nama::ClearAllCM()
{
	if (mMakeUpHandle > 0)
	{
		for (auto & v : m_CMakeupTypeMap)
		{
			if (v.second > 0) {
				fuUnbindItems(mMakeUpHandle, &v.second, 1);
			}
		}
	}
}

int Nama::SelectCustomMakeupBundle(string bundleName, string strType)
{
	if (false == mNamaAppState.EnableNama)
	{
		return false;
	}

	ChangeCleanFlag(false);
	fuSetMaxFaces(4);
	int bundleID = -1;

	if (0 == m_CMakeupMap[bundleName])
	{
		/*if (!CheckModuleCode(UIBridge::bundleCategory))
		{
			cout << "no right to use." << endl;
			return false;
		}*/
		vector<char> propData;
		if (false == FuTool::LoadBundle(bundleName, propData))
		{
			cout << "load prop data failed." << endl;
			UIBridge::m_curRenderItem = -1;
			return false;
		}
		cout << "load prop data." << endl;

		if (m_CMakeupMap.size() > MAX_NAMA_CM_BUNDLE_NUM)
		{
			int arrayBind[] = { m_CMakeupMap.begin()->second };
			fuUnbindItems(mMakeUpHandle, arrayBind, 1);
			fuDestroyItem(m_CMakeupMap.begin()->second);
			m_CMakeupMap.erase(m_CMakeupMap.begin());
			printf("cur map size : %d \n", m_CMakeupMap.size());
		}

		bundleID = fuCreateItemFromPackage(&propData[0], propData.size());
		m_CMakeupMap[bundleName] = bundleID;
	}

	if (UIBridge::bundleCategory == BundleCategory::Makeup &&
		m_CMakeupMap[bundleName] > 0)
	{
		auto itor = m_CMakeupTypeMap.find(strType);
		int ret = -1;
		if (itor != m_CMakeupTypeMap.end())
		{
			ret = fuUnbindItems(mMakeUpHandle, &itor->second, 1);
		}

		fuBindItems(mMakeUpHandle, &m_CMakeupMap[bundleName], 1);
		m_CMakeupTypeMap[strType] = m_CMakeupMap[bundleName];

		UIBridge::renderBundleCategory = BundleCategory::Makeup;
	}

	Nama::mNamaAppState.RenderAvatar = false;
	Nama::mNamaAppState.RenderAvatarBear = false;

	return m_CMakeupMap[bundleName];
}

void Nama::DestroyAll()
{
	ClearAllCM();

	if (UIBridge::bundleCategory == BundleCategory::Makeup && UIBridge::m_curBindedItem != -1 && mMakeUpHandle != -1)
	{
		fuUnbindItems(mMakeUpHandle, &UIBridge::m_curBindedItem, 1);
	}

	for (auto & data: mBundlesMap)
	{
		if (data.second > 0) {
			fuDestroyItem(data.second);
		}
	}

	UIBridge::m_curBindedItem = -1;
	mBundlesMap.clear();
}

int Nama::GetLastNamaError()
{
	return fuGetSystemError();
}


void Nama::UnbindCurFixedMakeup()
{
	if (UIBridge::bundleCategoryLast == BundleCategory::ItemJingpin) {
		ReloadItems();
	}
	if (UIBridge::bundleCategoryLast == BundleCategory::StyleRecommendation) {
		UIBridge::mStyleRecommendationIndex = 0;
		gui_tool::loadStyleParam();
		UpdateBeauty();
	}
	if (UIBridge::bundleCategory == BundleCategory::Makeup)
	{
		DestroyAll();
		UIBridge::m_curRenderItem = -1;
		if (mMakeUpHandle == -1) {
			vector<char> propData;
			if (false == FuTool::LoadBundle(g_Makeup, propData))
			{
				cout << "load face makeup data failed." << endl;
			}
			else {
				cout << "load face makeup data." << endl;
				mMakeUpHandle = fuCreateItemFromPackage(&propData[0], propData.size());
				fuItemSetParamd(mMakeUpHandle, "machine_level", 1.0);
			}
		}
	}
}

void Nama::ChangeCleanFlag(bool bOpen)
{
	if (mMakeUpHandle)
	{
		double v = bOpen ? 1.0 : 0.0;
		SetCMDouble("is_clear_makeup", v);
	}
}

bool Nama::SelectBundle(string bundleName, int maxFace)
{
	if (false == mNamaAppState.EnableNama)
	{
		return false;
	}
	if ((UIBridge::bundleCategoryLast == BundleCategory::ItemJingpin &&
		UIBridge::bundleCategory != BundleCategory::ItemJingpin) ||
		UIBridge::bundleCategoryLast == BundleCategory::Makeup &&
		UIBridge::bundleCategory != BundleCategory::Makeup) {
		ReloadItems();
	}
	if ((UIBridge::bundleCategoryLast == BundleCategory::StyleRecommendation &&
		UIBridge::bundleCategory != BundleCategory::StyleRecommendation)) {
		UIBridge::mStyleRecommendationIndex = 0;
		gui_tool::loadStyleParam();
		UpdateBeauty();
	}
	UIBridge::bundleCategoryLast = UIBridge::bundleCategory;
	ChangeCleanFlag(true);

	int bundleID = -1;
	
	map<int, Mp3*>::iterator it;
	for (it = mp3Map.begin(); it != mp3Map.end(); it++)
	{
		long long current = 0;
		it->second->SetPositions(&current, NULL, true);
		it->second->Stop();
		UIBridge::m_curPlayingMusicItem = -1;
		UIBridge::mNeedPlayMP3 = false; 
	}
	if (0 == mBundlesMap[bundleName])
	{
		/*if (!CheckModuleCode(UIBridge::bundleCategory))
		{
			cout << "no right to use." << endl;
			return false;
		}*/
		vector<char> propData;
		std::string bundlePath;

        if (false == FuTool::LoadBundle(bundleName, propData))
		{
            if(false == FuTool::LoadBundleByFullPath(bundlePath, propData))
            {
                cout << "load prop data failed." << endl;
                UIBridge::m_curRenderItem = -1;
                return false;
            }
		}
		cout << "load prop data." << endl;
		
		/*if (mBundlesMap.size() > MAX_NAMA_BUNDLE_NUM)
		{
			fuDestroyItem(mBundlesMap.begin()->second);
			mBundlesMap.erase(mBundlesMap.begin());
			printf("cur map size : %d \n", mBundlesMap.size());
		}*/
		DestroyAll();

		bundleID = fuCreateItemFromPackage(&propData[0], propData.size());
		mBundlesMap[bundleName] = bundleID;
		
		if (UIBridge::bundleCategory == BundleCategory::GestureRecognition) {
			fuItemSetParamd(bundleID, "rotMode", 0);
			if (bundleName.find("ctrl_flower") != std::string::npos) {
				fuItemSetParamd(bundleID, "particleDirMode", 0);
			}
		}

		if (UIBridge::bundleCategory == BundleCategory::Makeup)
		{
			if (!UIBridge::newMakeupType) {
				if (mMakeUpHandle == -1) {
					vector<char> propData;
					if (false == FuTool::LoadBundle(g_Makeup, propData))
					{
						cout << "load face makeup data failed." << endl;
					}
					else {
						cout << "load face makeup data." << endl;
						mMakeUpHandle = fuCreateItemFromPackage(&propData[0], propData.size());
						fuItemSetParamd(mMakeUpHandle, "machine_level", 1.0);
					}
				}
				fuBindItems(mMakeUpHandle, &bundleID, 1);
				UIBridge::m_curBindedItem = bundleID;
			}
			else {
				if (mMakeUpHandle > 0) {
					fuDestroyItem(mMakeUpHandle);
					mMakeUpHandle = -1;
				}
				fuItemSetParamd(bundleID, "machine_level", 1.0);
			}
		}
		// 处理 美发
		if (UIBridge::bundleCategory == BundleCategory::BeautyHair) {
			int index = FuTool::extractIntFromPath(bundleName) - 1;
			fuItemSetParamd(bundleID, "Index", index);
		}
		
		if (UIBridge::bundleCategory == BundleCategory::MusicFilter)
		{
			string itemName = UIBridge::mCurRenderItemName.substr(0, UIBridge::mCurRenderItemName.find_last_of("."));
			if (mp3Map.find(bundleID) == mp3Map.end())
			{
				Mp3 *mp3 = new Mp3;
#if _WIN32
				mp3->Load("assets/items/MusicFilter/" + itemName + ".mp3");
#elif __APPLE__
				string mp3Path = FuToolMac::GetFileFullPathFromResourceBundle(("items/MusicFilter/" + itemName + ".mp3").c_str());
				mp3->Load(mp3Path);
#endif
				mp3Map[bundleID] = mp3;
				
			}
			mp3Map[bundleID]->Play();
			UIBridge::m_curPlayingMusicItem = bundleID;
			UIBridge::mNeedPlayMP3 = true;
		}
		if (UIBridge::bundleCategory == BundleCategory::StyleRecommendation) {
			StyleRecommendationParam tempSRP = UIBridge::mStyleParamList.at(UIBridge::mStyleRecommendationIndex);
			fuItemSetParamd(bundleID, "machine_level", 1.0);
			fuItemSetParamd(bundleID, "filter_level", float(tempSRP.mFilterLevel) / 100);
			fuItemSetParamd(bundleID, "makeup_intensity", float(tempSRP.mMakeUpIntensity) / 100);
		}
	}
	else
	{
		bundleID = mBundlesMap[bundleName];
		
		if (UIBridge::bundleCategory == BundleCategory::Makeup)
		{
			if (UIBridge::m_curBindedItem != -1)
			{
				fuUnbindItems(mMakeUpHandle, &UIBridge::m_curBindedItem, 1);
			}
			if (!UIBridge::newMakeupType) {
				fuBindItems(mMakeUpHandle, &bundleID, 1);
				UIBridge::m_curBindedItem = bundleID;
			}
			else {
				fuItemSetParamd(bundleID, "machine_level", 1.0);
			}
		}
		
		if (UIBridge::bundleCategory == BundleCategory::MusicFilter)
		{
			mp3Map[bundleID]->Play();
			UIBridge::m_curPlayingMusicItem = bundleID;
			UIBridge::mNeedPlayMP3 = true;
		}
	}

	if (UIBridge::m_curRenderItem == bundleID && UIBridge::bundleCategory != BundleCategory::LightMakeup)
	{
		
		if (UIBridge::bundleCategory == BundleCategory::Makeup)
		{
			fuUnbindItems(mMakeUpHandle, &UIBridge::m_curRenderItem, 1);
			UIBridge::m_curBindedItem = -1;
		}
		UIBridge::m_curRenderItem = -1;
	}
	else
	{
		// 如果不是美妆，首先解除美妆效果
		if (UIBridge::bundleCategory != BundleCategory::Makeup && mMakeUpHandle != -1 && UIBridge::m_curRenderItem != -1)
		{
			fuUnbindItems(mMakeUpHandle, &UIBridge::m_curRenderItem, 1);
			UIBridge::m_curBindedItem = -1;
		}
		UIBridge::m_curRenderItem = bundleID;
		UIBridge::renderBundleCategory = UIBridge::bundleCategory;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//旧版本统一设定参数
	fuItemSetParamd(UIBridge::m_curRenderItem, "is3DFlipH", 1);
	fuItemSetParamd(UIBridge::m_curRenderItem, "isFlipTrack", 1);
	//PC 就只有横屏
	fuItemSetParamd(UIBridge::m_curRenderItem, "rotationMode", 2);

	if (UIBridge::bundleCategory == Animoji)
	{
		maxFace = 1;
	}else if (UIBridge::bundleCategory ==BackgroundSegmentation)
	{
		vector<string> humanSegmentations = {"boyfriend1","boyfriend2","boyfriend3"};
		bool isHumanSegmentation = false;
		std::vector<string>::iterator itr = humanSegmentations.begin();
		while (itr != humanSegmentations.end())
		{
			if (bundleName.find(*itr) != std::string::npos)
			{
				isHumanSegmentation = true;
				break;
			}
			itr++;
			
		}
		if (isHumanSegmentation) 
			maxFace = 1;
		else 
			maxFace = 4;
	}
	else if (UIBridge::bundleCategory == BigHead)
	{
		maxFace = 1;
	}

	if (UIBridge::renderBundleCategory == Animoji)
	{
		fuItemSetParamd(UIBridge::m_curRenderItem, "{\"thing\":\"<global>\",\"param\":\"follow\"} ", 1);
	}
	else
	{
		fuItemSetParamd(UIBridge::m_curRenderItem, "{\"thing\":\"<global>\",\"param\":\"follow\"} ", 0);
	}

	UIBridge::lastMaxFace = maxFace;
	if (UIBridge::m_curRenderItem == -1) {
		fuSetMaxFaces(4);
	}
	else {
		fuSetMaxFaces(maxFace);
	}

	Nama::mNamaAppState.RenderAvatar = false;
	Nama::mNamaAppState.RenderAvatarBear = false;

	return true;
}

bool Nama::CheckModuleFaceInfo() {
	if (!IsInited())
	{
		return false;
	}
	int passCode = fuGetModuleCode(1);
	int needCode = define_arr_ext[DEFINE_GET_FACEINFO];
	bool bOK = false;
	if ((passCode & needCode) == needCode) {
		bOK = true;
	}
	return bOK;
}

bool Nama::CheckModuleBodyInfo() {
	if (!IsInited())
	{
		return false;
	}
	int passCode = fuGetModuleCode(1);
	int needCode = define_arr_ext[DEFINE_BODY_TRACK];
	bool bOK = false;
	if ((passCode & needCode) == needCode) {
		bOK = true;
	}
	return bOK;
}

bool Nama::CheckModuleCode(int category)
{
	if (!IsInited() || category == BUNDLE_CATEGORY_NOMEAN)
	{
		return true;
	}

	int passCode = fuGetModuleCode(g_checkIndex[category]);
	int needCode = g_checkID[category];
	bool bOK = false;
	if ((passCode & needCode) == needCode) {
		bOK = true;
	}

	return bOK;
}

bool Nama::CheckModuleCodeSide(int categorySide)
{
	if (!IsInited() || categorySide == BUNDLE_CATEGORY_NOMEAN)
	{
		return true;
	}
	int passCode = fuGetModuleCode(g_checkSideIndex[categorySide]);
	int needCode = g_checkSideID[categorySide];
	bool bOK = false;
	if ((passCode & needCode) == needCode) {
		bOK = true;
	}

	return bOK;
}

void Nama::RenderDefNama(cv::Mat& picInput, int rotType)
{

	if (UIBridge::mNeedPlayMP3)
	{
		if (mp3Map.find(UIBridge::m_curRenderItem) != mp3Map.end())
		{
			fuItemSetParamd(UIBridge::m_curRenderItem, "music_time", mp3Map[UIBridge::m_curRenderItem]->GetCurrentPosition() / 1e4);
			mp3Map[UIBridge::m_curRenderItem]->CirculationPlayCheck();
		}
	}
	if (UIBridge::mNeedStopMP3)
	{
		map<int, Mp3*>::iterator it = mp3Map.begin();
		for (; it != mp3Map.end(); it++)
		{
			it->second->Stop();
		}
		UIBridge::m_curRenderItem = -1;
		UIBridge::mNeedStopMP3 = false;
		UIBridge::mNeedPlayMP3 = false;
	}

	{
		vector<int> renderList;

		// AR 和 美体互斥
		if (UIBridge::m_bShowingBodyBeauty)
		{
			renderList.push_back(mBodyShapeHandle);
			renderList.push_back(mBeautyHandles);
		}
		else {
			renderList.push_back(mBeautyHandles);
			if (UIBridge::renderBundleCategory == BundleCategory::Makeup)
			{
				renderList.push_back(UIBridge::m_curRenderItem);
				if (!UIBridge::newMakeupType)
				{
					renderList.push_back(mMakeUpHandle);
				}
			}
			else
			{
				renderList.push_back(UIBridge::m_curRenderItem);
			}
		}

		renderList.push_back(mFxaaHandles);

		BgSegUpdate::instance().Update(this);

		fuSetInputCameraBufferMatrix((TRANSFORM_MATRIX)rotType);
		fuSetInputCameraTextureMatrix((TRANSFORM_MATRIX)rotType);

		fuRender(FU_FORMAT_RGBA_BUFFER,
			(int*)(picInput.data),
			FU_FORMAT_RGBA_BUFFER,
			(int*)(picInput.data),
			picInput.cols,
			picInput.rows,
			mFrameID,
			renderList.data(),
			(int32_t)renderList.size(),
			NAMA_RENDER_FEATURE_FULL,
			NULL);

		if (UIBridge::bundleCategory == BundleCategory::GestureRecognition) {
			if (fuHandDetectorGetResultNumHands() == 0) {
				UIBridge::m_openLocalFileTip = u8"未检测到手势";
				UIBridge::m_bLoadWrongNamePNGFile = true;
				UIBridge::mLastTimeExtra = ImGui::GetTime() + 1.0;
			}
		}
		else if (UIBridge::bundleCategory == BundleCategory::BackgroundSegmentation ||
			UIBridge::bundleCategory == BundleCategory::GreenScreen || 
			UIBridge::m_bShowingBodyBeauty)
		{
			if (CheckModuleBodyInfo()) {
				if (fuHumanProcessorGetNumResults() == 0) {
					UIBridge::m_openLocalFileTip = u8"未检测到人体";
					UIBridge::m_bLoadWrongNamePNGFile = true;
					UIBridge::mLastTimeExtra = ImGui::GetTime() + 1.0;
				}
			}
			else {
				static bool once = false;
				if (!once) {
					UIBridge::m_openLocalFileTip = u8"没有获取人体信息权限";
					UIBridge::m_bLoadWrongNamePNGFile = true;
					UIBridge::mLastTimeExtra = ImGui::GetTime() + 10.0;

				}
			}
		}
		else {
			if (CheckModuleFaceInfo()) {
				if (fuFaceProcessorGetNumResults() == 0) {
					UIBridge::m_openLocalFileTip = u8"未检测到人脸";
					UIBridge::m_bLoadWrongNamePNGFile = true;
					UIBridge::mLastTimeExtra = ImGui::GetTime() + 1.0;
				}
			}
			else {
				static bool once = false;
				if (!once) {
					UIBridge::m_openLocalFileTip = u8"没有获取人脸信息权限";
					UIBridge::m_bLoadWrongNamePNGFile = true;
					UIBridge::mLastTimeExtra = ImGui::GetTime() + 10.0;
				}
			}
		}
	}

	if (fuGetSystemError())
	{
		printf("%s \n", fuGetSystemErrorString(fuGetSystemError()));
	}
	++mFrameID;
}


void Nama::RenderP2A(cv::Mat & picBg, int rotType)
{
	if (m_Controller == nullptr)
		return;

	fuSetInputCameraBufferMatrix((TRANSFORM_MATRIX)rotType);
	fuSetInputCameraTextureMatrix((TRANSFORM_MATRIX)rotType);
	m_Controller->RenderBundlesBuffer(picBg.data, picBg, mFrameID);
	if (CheckModuleBodyInfo()) {
		if (fuHumanProcessorGetNumResults() == 0) {
			UIBridge::m_openLocalFileTip = u8"未检测到人体";
			UIBridge::m_bLoadWrongNamePNGFile = true;
			UIBridge::mLastTimeExtra = ImGui::GetTime() + 1.0;
		}
	}
	else {
		static bool once = false;
		if (!once) {
			UIBridge::m_openLocalFileTip = u8"没有获取人体信息权限";
			UIBridge::m_bLoadWrongNamePNGFile = true;
			UIBridge::mLastTimeExtra = ImGui::GetTime() + 10.0;
		}
	}
}

void Nama::changeGSPreviewRect(FURect rect)
{
	fuItemSetParamd(mGSHandle, "start_x", rect.origin.x);
	fuItemSetParamd(mGSHandle, "start_y", rect.origin.y);
	fuItemSetParamd(mGSHandle, "end_x", rect.size.width);
	fuItemSetParamd(mGSHandle, "end_y", rect.size.height);
}

void Nama::setGSPreviewRect(FURect rect)
{
	gsPreviewRect = rect;
	fuItemSetParamd(mGSHandle, "start_x", gsPreviewRect.origin.x);
	fuItemSetParamd(mGSHandle, "start_y", gsPreviewRect.origin.y);
	fuItemSetParamd(mGSHandle, "end_x", gsPreviewRect.size.width);
	fuItemSetParamd(mGSHandle, "end_y", gsPreviewRect.size.height);
}
FURect Nama::getGSPreviewRect()
{
	return gsPreviewRect;
}
void Nama::RenderGS(cv::Mat & picInput, int rotType)
{
	if (mGSHandle != -1)
	{
		fuSetMaxFaces(4);
		vector<int> vecRender;
		vecRender.push_back(mGSHandle);
		vecRender.push_back(mBeautyHandles);
        
		fuSetInputCameraBufferMatrix((TRANSFORM_MATRIX)rotType);
		fuSetInputCameraTextureMatrix((TRANSFORM_MATRIX)rotType);
		fuRender(FU_FORMAT_RGBA_BUFFER,
			(void*)(picInput.data),
			FU_FORMAT_RGBA_BUFFER,
			(void*)(picInput.data),
			picInput.cols,
			picInput.rows,
			mFrameID,
			vecRender.data(),
			(int32_t)vecRender.size(),
			NAMA_RENDER_FEATURE_FULL,
			NULL);
	}

	if (fuGetSystemError())
	{
		printf("%s \n", fuGetSystemErrorString(fuGetSystemError()));
	}
	++mFrameID;
}


void Nama::RenderItems(cv::Mat & picMat)
{
	if (UIBridge::showGreenScreen)
	{
		TRANSFORM_MATRIX type = (CCameraManage::getInstance()->mCapture->getCaptureType() == CAPTURE_CAMERA) ? TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL: TRANSFORM_MATRIX::CCROT0;
		RenderGS(picMat, TRANSFORM_MATRIX::CCROT0);
	}
	else
	{
		if (UIBridge::m_bShowingBodyBeauty) {
			RenderDefNama(picMat, TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
		}else if (mNamaAppState.RenderAvatarBear) {
			RenderDefNama(picMat, TRANSFORM_MATRIX::CCROT0);
			RenderP2A(picMat, TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
		}else if (mNamaAppState.RenderAvatar)
			RenderP2A(picMat, TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
		else
			RenderDefNama(picMat, TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
	}

	return;
}


void Nama::DrawLandmarks(uchar* frame)
{
	if (false == mNamaAppState.EnableNama) return;
	float landmarks[150];
	float trans[3];
	float rotat[4];
	int ret = 0;

	ret = fuGetFaceInfo(0, "landmarks", landmarks, sizeof(landmarks) / sizeof(landmarks[0]));
	for (int i(0); i != 75; ++i)
	{
		DrawPoint(frame, static_cast<int>(landmarks[2 * i]), static_cast<int>(landmarks[2 * i + 1]));
	}

}

void Nama::DrawPoint(uchar* frame, int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	const int offsetX[] = { -1, 0, 1 , -1, 0, 1 , -1, 0, 1 };
	const int offsetY[] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
	const int count = sizeof(offsetX) / sizeof(offsetX[0]);

	unsigned char* data = frame;
	for (int i(0); i != count; ++i)
	{
		int xx = x + offsetX[i];
		int yy = y + offsetY[i];
		if (0 > xx || xx >= mFrameWidth || 0 > yy || yy >= mFrameHeight)
		{
			continue;
		}

		data[yy * 4 * mFrameWidth + xx * 4 + 0] = b;
		data[yy * 4 * mFrameWidth + xx * 4 + 1] = g;
		data[yy * 4 * mFrameWidth + xx * 4 + 2] = r;
	}

}

BodyTrackType Nama::GetBodyTrackType()
{
	return m_bodyTrackType;
}

void Nama::SwitchBodyTrackType()
{
	switch (m_bodyTrackType)
	{
	case BodyTrackType::FullBody:
		SetBodyTrackType(BodyTrackType::HalfBody);
		break;
	case BodyTrackType::HalfBody:
		SetBodyTrackType(BodyTrackType::FullBody);
		break;
	default:
		break;
	}
}

void Nama::SetBodyTrackType(BodyTrackType type)
{
	m_bodyTrackType = type;
	switch (type)
	{
	case BodyTrackType::None:
		break;
	case BodyTrackType::HalfBody:
		m_Controller->SetAvatar3DScene(BodyTrackType::HalfBody);
		break;
	case BodyTrackType::FullBody:
		m_Controller->SetAvatar3DScene(BodyTrackType::FullBody);
		break;
	default:
		break;
	}
}

void NamaExampleNameSpace::Nama::setLightMakeupTex(string path, string value)
{
	cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED);
	cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA);
	fuCreateTexForItem(UIBridge::m_curRenderItem, value.data(), image.data, image.cols, image.rows);
}

void NamaExampleNameSpace::Nama::setLightMakeupParam(LightMakeupParam param)
{
	fuItemSetParamd(UIBridge::m_curRenderItem, "is_makeup_on", 1.0);
#if __APPLE__
	// 这里macosx 为绝对路径，我们需要获取相对于图片的相对路径
	param.blusherPath = UIBridge::GetFileFullPathFromResourceBundle(param.blusherPath.c_str());
	param.eyeshadowPath = UIBridge::GetFileFullPathFromResourceBundle(param.eyeshadowPath.c_str());
	param.eyebrowPath = UIBridge::GetFileFullPathFromResourceBundle(param.eyebrowPath.c_str());
	param.lipColorPath = UIBridge::GetFileFullPathFromResourceBundle(param.lipColorPath.c_str());
#endif
	setLightMakeupTex(param.blusherPath, "tex_blusher");
	fuItemSetParamd(UIBridge::m_curRenderItem, "makeup_intensity_blusher", param.blusher);
	setLightMakeupTex(param.eyeshadowPath, "tex_eye");
	fuItemSetParamd(UIBridge::m_curRenderItem, "makeup_intensity_eye", param.eyeshadow);
	setLightMakeupTex(param.eyebrowPath, "tex_brow");
	fuItemSetParamd(UIBridge::m_curRenderItem, "makeup_intensity_eyeBrow", param.eyebrow);
	ifstream in(param.lipColorPath.c_str());
	if (!in.is_open()) {
		fprintf(stderr, "fail to read json file: %s\n", param.lipColorPath.data());
		return;
	}
	string json_content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	in.close();
	rapidjson::Document dom;
	if (!dom.Parse(json_content.c_str()).HasParseError())
	{
		if (dom.HasMember("rgba") && dom["rgba"].IsArray())
		{
			const rapidjson::Value& arr = dom["rgba"];
			std::vector<double> color;
			for (int i = 0; i < arr.Size(); ++i) {
				color.push_back(arr[i].GetDouble());
			}
			fuItemSetParamdv(UIBridge::m_curRenderItem, "makeup_lip_color", color.data(), color.size());
		}
	}

	fuItemSetParamd(UIBridge::m_curRenderItem, "makeup_intensity", param.intensity);
	fuItemSetParams(mBeautyHandles, "filter_name", param.filterName.c_str());
	fuItemSetParamd(mBeautyHandles, "filter_level", param.filterLevel);
}

void NamaExampleNameSpace::Nama::setHumanSegScene(int type)
{
	//fuSetHumanSegScene(FUAIHUMANSEGSCENETYPE(type));
	vector<char> ai_human_model_data;
	if (FuTool::LoadBundle(g_ai_humanprocessor, ai_human_model_data))
	{	
		fuReleaseAIModel(FUAITYPE::FUAITYPE_HUMAN_PROCESSOR);
		fuSetHumanSegMode(FUAIHUMANSEGMODE(type));
		fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_human_model_data[0]), ai_human_model_data.size(), FUAITYPE::FUAITYPE_HUMAN_PROCESSOR);
	}
}

void NamaExampleNameSpace::Nama::setMaxFaces(int face)
{
	fuSetMaxFaces(face);
}
