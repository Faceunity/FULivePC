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

using namespace NamaExampleNameSpace;

bool Nama::mHasSetup = false;
bool Nama::mEnableNama = false;
bool Nama::mEnableAvatar = false;


#ifdef _WIN32

#include "Sound/MP3.h"

static HGLRC new_context;

#elif __APPLE__
#include "fu_tool_mac.h"
#include "Sound/MP3_mac.h"
#include "NaMaGLContext.h"

static void * new_context;

#endif

map<int, Mp3*> mp3Map;

string Nama::mFilters[6] = { "origin", "bailiang1", "fennen1", "xiaoqingxin1", "lengsediao1", "nuansediao1" };

Nama::UniquePtr Nama::create(uint32_t width, uint32_t height, bool enable)
{
	UniquePtr pNama = UniquePtr(new Nama);
	mEnableNama = enable;
	pNama->Init(width, height);
	pNama->gsPreviewRect = {0.5,0.5,1.0,1.0};
	pNama->changeGSPreviewRect(pNama->gsPreviewRect);
	return pNama;
}

Nama::Nama()
{

	mFrameID = 0;
	mMaxFace = 1;
	mIsBeautyOn = true;
	mBeautyHandles = 0;
	mMakeUpHandle = 0;
#ifdef _WIN32
	mCapture = tr1::shared_ptr<CCameraDS>(new CCameraDS);
#else
	mCapture = shared_ptr<CCameraDS>(new CCameraDS);
#endif
}

Nama::~Nama()
{
	SaveRencentColorToFile();

	if (true == mHasSetup)
	{
		fuDestroyAllItems();
		fuOnDeviceLost();
		fuDestroyLibData();

		if (m_pCtrl)
		{
			delete m_pCtrl;
			m_pCtrl = nullptr;
		}
	}


	map<int, Mp3*>::iterator it;
	for (it = mp3Map.begin(); it != mp3Map.end(); it++)
	{
		it->second->Cleanup();
		delete it->second;
		UIBridge::mNeedPlayMP3 = false;
	}

}

vector<string> Nama::CameraList()
{
	return mCapture->getDeviceNameList();
}
cv::Mat Nama::GetOriginFrame()
{
	return mCapture->getFrame();
}

cv::Mat Nama::GetFrame()
{
	cv::Mat frameMat = mCapture->getFrame();
	cv::Mat tmpMat;
	if(!UIBridge::showGreenScreen){
		tmpMat = frameMat;
	}
	else{
		auto srcType =  mCapture->getCaptureType();
		bool bNeedFlip = srcType != GS_INPUT_TYPE_FILE;
		
		if (bNeedFlip)
			cv::flip(frameMat, tmpMat, 1);
		else {   // 本地视频文件
			switch (UIBridge::m_localVideoRotation) {
				case 0:
					tmpMat = frameMat;
					break;
				case 90:
					cv::rotate(frameMat, tmpMat, cv::ROTATE_90_CLOCKWISE);
					break;
				case 180:
					tmpMat = frameMat;
					break;
				case 270:
					tmpMat = frameMat;
					break;
					
				default:
					tmpMat = frameMat;
					break;
			}	
		}
	}
	return tmpMat;
}

bool Nama::ReOpenCamera(int camID)
{
	if (mCapture->isInit())
	{
		mCapture->closeCamera();
		mCapture->initCamera(mCapture->rs_width, mCapture->rs_height, camID);
		mFrameWidth = mCapture->m_dstFrameSize.width;
		mFrameHeight = mCapture->m_dstFrameSize.height;
		fuOnCameraChange();
	}
	return true;
}

bool Nama::ReOpenCamera(string strVideoPath)
{
	if (mCapture->isInit())
	{
		mCapture->closeCamera();
		mCapture->InitCameraFile(mCapture->rs_width, mCapture->rs_height, strVideoPath);
		mFrameWidth = mCapture->m_dstFrameSize.width;
		mFrameHeight = mCapture->m_dstFrameSize.height;
		fuOnCameraChange();
	}
	return true;
}

void Nama::CloseCurCamera()
{
	if (mCapture->isInit())
	{
		mCapture->closeCamera();
	}
}

void Nama::OpenCamera(int iCamID)
{
	int CamID = mCapture->getCaptureCameraID();
	auto curCaptureType = mCapture->getCaptureType();

	if ((CamID == iCamID && !mCapture->isInit()) || CamID != iCamID || curCaptureType != CAPTURE_CAMERA)
	{
		mCapture->closeCamera();
		mCapture->initCamera(mCapture->rs_width, mCapture->rs_height, iCamID);
		mFrameWidth = mCapture->m_dstFrameSize.width;
		mFrameHeight = mCapture->m_dstFrameSize.height;
		fuOnCameraChange();
	}
}

void Nama::OpenCamera(string strVideoPath)
{
	auto strPath = mCapture->getFilePath();
	auto curCaptureType = mCapture->getCaptureType();

	if ((strPath == strVideoPath && !mCapture->isInit()) || strVideoPath != strPath || curCaptureType != CAPTURE_FILE)
	{
		mCapture->closeCamera();
		mCapture->InitCameraFile(mCapture->rs_width, mCapture->rs_height, strVideoPath);
		mFrameWidth = mCapture->m_dstFrameSize.width;
		mFrameHeight = mCapture->m_dstFrameSize.height;
		fuOnCameraChange();
	}
}

bool Nama::IsCameraPlaying()
{
	return mCapture->isPlaying();
}

bool Nama::IsCameraInit()
{
	return mCapture->isInit();
}

int Nama::GetCameraCaptureType()
{
	return mCapture->getCaptureType();
}
cv::Size Nama::getCameraResolution()
{
	return mCapture->getCameraResolution();
}
int Nama::GetCameraID()
{
	return mCapture->getCaptureCameraID();
}

/// 手动设置最后一帧为默认画面
void Nama::setDefaultFrame()
{
	mCapture->setDefaultFrame();
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

bool Nama::Init(uint32_t& width, uint32_t& height)
{
#ifdef _WIN32
	HGLRC context = wglGetCurrentContext();
	HWND wnd = (HWND)Gui::hWindow;
	new_context = wglCreateContext(GetDC(wnd));
	wglMakeCurrent(GetDC(wnd), new_context);
#else
	void * curCtx = NaMaGetCurrentGLCtx();
	new_context = NaMaCreateGLCtx(Gui::hWindow);
	NaMaMakeAsCurrentCtx(new_context);
#endif

	mCapture->initCamera(width, height, UIBridge::mSelectedCamera);
	mFrameWidth = mCapture->m_dstFrameSize.width;
	mFrameHeight = mCapture->m_dstFrameSize.height;
	if (false == mHasSetup && true == mEnableNama)
	{
		CheckGLContext();

		fuSetLogLevel(FU_LOG_LEVEL_INFO);

		fuSetup(nullptr, 0, nullptr, g_auth_package, sizeof(g_auth_package));
		// setup without license, only render 1000 frames.
		//fuSetup(nullptr, 0, nullptr, nullptr, 0);

		printf("Nama version:%s \n", fuGetVersion());
		vector<char> tongue_model_data;
		if (false == FuTool::LoadBundle(g_tongue, tongue_model_data))
		{
			cout << "Error: fail load tongue model" << g_tongue << endl;
			return false;
		}

		fuLoadTongueModel(reinterpret_cast<float*>(&tongue_model_data[0]), tongue_model_data.size());

		vector<char> ai_model_data;
		if (false == FuTool::LoadBundle(g_ai_faceprocessor, ai_model_data))
		{
			cout << "Error:fail load faceprocessor model" << g_ai_faceprocessor << endl;
			return false;
		}
		fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_model_data[0]), ai_model_data.size(), FUAITYPE::FUAITYPE_FACEPROCESSOR);

		vector<char> ai_beseg_green_model_data;
		if (false == FuTool::LoadBundle(g_ai_bgseg_green, ai_beseg_green_model_data))
		{
			cout << "Error: fail load bgseg green model" << g_ai_bgseg_green << endl;
			return false;
		}
		fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_beseg_green_model_data[0]), ai_beseg_green_model_data.size(), FUAITYPE::FUAITYPE_BACKGROUNDSEGMENTATION_GREEN);

		vector<char> ai_hairseg_model_data;
		if (false == FuTool::LoadBundle(g_ai_hairseg, ai_hairseg_model_data))
		{
			cout << "Error: fail load hair seg model" << g_ai_hairseg << endl;
			return false;
		}
		fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_hairseg_model_data[0]), ai_hairseg_model_data.size(), FUAITYPE::FUAITYPE_HAIRSEGMENTATION);


		vector<char> ai_human_model_data;
		if (false == FuTool::LoadBundle(g_ai_humanprocessor, ai_human_model_data))
		{
			cout << "Error: fail load humanprocessor model" << g_ai_humanprocessor << endl;
			return false;
		}
		fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_human_model_data[0]), ai_human_model_data.size(), FUAITYPE::FUAITYPE_HUMAN_PROCESSOR);

		vector<char> ai_gesture_model_data;
		if (false == FuTool::LoadBundle(g_ai_hand_processor, ai_gesture_model_data))
		{
			cout << "Error: fail load gesture model" << g_ai_hand_processor << endl;
			return false;
		}
		fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_gesture_model_data[0]), ai_gesture_model_data.size(), FUAITYPE::FUAITYPE_HANDGESTURE);



		mModuleCode = fuGetModuleCode(0);
		mModuleCode1 = fuGetModuleCode(1);
		{
			vector<char> propData;
			if (false == FuTool::LoadBundle(g_faceBeautification, propData))
			{
				cout << "load face beautification data failed." << endl;
				return false;
			}
			cout << "load face beautification data." << endl;

			mBeautyHandles = fuCreateItemFromPackage(&propData[0], propData.size());
		}


		if (CheckModuleCode(Makeup))
		{
			vector<char> propData;
			if (false == FuTool::LoadBundle(g_Makeup, propData))
			{
				cout << "load face makeup data failed." << endl;
				return false;
			}
			cout << "load face makeup data." << endl;

			mMakeUpHandle = fuCreateItemFromPackage(&propData[0], propData.size());
			//fuItemSetParamd(mMakeUpHandle, "is_clear_makeup", 1);
		}

		{
			vector<char> propData;
			if (false == FuTool::LoadBundle(g_bodySlim, propData))
			{
				cout << "load body slim data failed." << endl;
				return false;
			}
			cout << "load body slim data." << endl;

			mBodyShapeHandle = fuCreateItemFromPackage(&propData[0], propData.size());
		}

		{
			vector<char> propData;
			if (false == FuTool::LoadBundle(g_greenscreen, propData))
			{
				cout << "load gs data failed." << endl;
				return false;
			}
			cout << "load gs data." << endl;

			mGSHandle = fuCreateItemFromPackage(&propData[0], propData.size());
		}

		//fuSetDefaultOrientation(0);
		float fValue = 0.5f;
		fuSetFaceTrackParam((void*)"mouth_expression_more_flexible", &fValue);

		fuSetMaxFaces(4);

		InitAvatarController();

		mHasSetup = true;
	}
	else
	{
		if (mEnableNama) fuOnDeviceLost();
		mHasSetup = false;
	}

#ifdef _WIN32
	wglMakeCurrent(GetDC(wnd), context);
#else
	NaMaMakeAsCurrentCtx(curCtx);
#endif

	return true;
}

bool Nama::InitAvatarController()
{
	m_pCtrl = new FuController;
	m_pCtrl->InitController(g_control, g_control_cfg);
	m_pCtrl->InitFXAA(g_fxaa_flipx);
	m_pCtrl->BindBundleToController(g_fakeman);
	m_pCtrl->BindBundleToController(g_avatar_def_bg);

	//Bind Animation For Hand
	auto files = FuTool::getFilesInFolder(g_pose_track_folder, "bundle"); 
	for (auto & file : files)
	{
#if __APPLE__
		// 这里macosx 为绝对路径，我们需要获取相对于 Resource.bundle的相对路径
		file = FuToolMac::GetRelativePathDependResourceBundle(file.c_str());
#endif
		m_pCtrl->BindBundleToController(file);
	}
	return true;
}

int Nama::IsTracking()
{
	if (false == mEnableNama)
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
	if (false == mEnableNama || false == mIsBeautyOn || mBeautyHandles == 0)return;
	if (0 <= index <= 4)
	{
		int res = fuItemSetParamd(mBeautyHandles, "face_shape", index);
	}
}

int Nama::CreateMakeupBundle(string bundleName)
{
	int fakeBundleID = 0;

	if (false == mEnableNama || false == mIsBeautyOn || mMakeUpHandle == 0)return 0;
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
	if (false == mEnableNama || false == mIsBeautyOn || mBeautyHandles == 0)return;

	fuItemSetParams(mBeautyHandles, "filter_name", &mFilters[index][0]);
}

void Nama::UpdateBeauty()
{
	if (false == mEnableNama || mBeautyHandles == 0)
	{
		return;
	}
	if (false == mIsBeautyOn)return;

	for (int i = 0; i < MAX_BEAUTYFACEPARAMTER; i++)
	{
		if (i == 0)
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
	fuItemSetParamd(mBeautyHandles, "skin_detect", UIBridge::mEnableSkinDect);
	map<int, int> blurType = { {0,2},{1,0},{2,1} };
	fuItemSetParamd(mBeautyHandles, "blur_type", blurType[UIBridge::mEnableHeayBlur]);
	fuItemSetParamd(mBeautyHandles, "face_shape_level", 1);
	fuItemSetParamd(mBeautyHandles, "filter_level", UIBridge::mFilterLevel[UIBridge::m_curFilterIdx] / 100.0f);
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
	if (false == mEnableNama || mBodyShapeHandle == 0)
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

void Nama::ClearAllCM()
{
	if (mMakeUpHandle > 0)
	{
		for (auto & v : m_CMakeupTypeMap)
		{
			fuUnbindItems(mMakeUpHandle, &v.second, 1);
		}
	}
}

int Nama::SelectCostumMakeupBundle(string bundleName, string strType)
{
	if (false == mEnableNama)
	{
		return false;
	}

	ChangeCleanFlag(false);

	int bundleID = -1;

	if (0 == m_CMakeupMap[bundleName])
	{
		if (!CheckModuleCode(UIBridge::bundleCategory))
		{
			cout << "no right to use." << endl;
			return false;
		}
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

	if (UIBridge::bundleCategory == BundleCategory::Makeup && m_CMakeupMap[bundleName] > 0)
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

	return m_CMakeupMap[bundleName];
}

void Nama::DestroyAll()
{
	ClearAllCM();

	if (UIBridge::bundleCategory == BundleCategory::Makeup && UIBridge::m_curBindedItem != -1)
	{
		fuUnbindItems(mMakeUpHandle, &UIBridge::m_curBindedItem, 1);
	}

	for (auto & data: mBundlesMap)
	{
		fuDestroyItem(data.second);
	}

	UIBridge::m_curBindedItem = -1;
	mBundlesMap.clear();
}

void Nama::UnbindCurFixedMakeup()
{
	if (UIBridge::bundleCategory == BundleCategory::Makeup)
	{
		if (UIBridge::m_curBindedItem != -1)
		{
			fuUnbindItems(mMakeUpHandle, &UIBridge::m_curBindedItem, 1);
			UIBridge::m_curBindedItem = -1;
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

bool Nama::SelectBundle(string bundleName)
{
	if (false == mEnableNama)
	{
		return false;
	}
	
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
		
		if (!CheckModuleCode(UIBridge::bundleCategory))
		{
			cout << "no right to use." << endl;
			return false;
		}
		vector<char> propData;
		if (false == FuTool::LoadBundle(bundleName, propData))
		{
			cout << "load prop data failed." << endl;
			UIBridge::m_curRenderItem = -1;
			return false;
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
			fuBindItems(mMakeUpHandle, &bundleID, 1);
			UIBridge::m_curBindedItem = bundleID;
		}
		else
		{
			//		fuItemSetParamd(mMakeUpHandle, "is_makeup_on", 0);
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
			fuBindItems(mMakeUpHandle, &bundleID, 1);
			UIBridge::m_curBindedItem = bundleID;
		}
		
		if (UIBridge::bundleCategory == BundleCategory::MusicFilter)
		{
			mp3Map[bundleID]->Play();
			UIBridge::m_curPlayingMusicItem = bundleID;
			UIBridge::mNeedPlayMP3 = true;
		}
	}
	
	if (UIBridge::m_curRenderItem == bundleID)
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
		if (UIBridge::bundleCategory != BundleCategory::Makeup)
		{
			fuUnbindItems(mMakeUpHandle, &UIBridge::m_curRenderItem, 1);
			UIBridge::m_curBindedItem = -1;
		}
		UIBridge::m_curRenderItem = bundleID;
		UIBridge::renderBundleCategory = UIBridge::bundleCategory;
	}

	////////////////////////////////////////////////////////////////////////////////////////

	if (UIBridge::bundleCategory == Animoji)
	{
		mMaxFace = 1;
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
			mMaxFace = 1;
		else 
			mMaxFace = 4;
	}
	else if (UIBridge::bundleCategory == BigHead)
	{
		mMaxFace = 1;
		
	}
	else{
		mMaxFace = 4;
	}
	if (UIBridge::renderBundleCategory == Animoji)
	{
		fuItemSetParamd(UIBridge::m_curRenderItem, "{\"thing\":\"<global>\",\"param\":\"follow\"} ", 1);
	}
	else
	{
		fuItemSetParamd(UIBridge::m_curRenderItem, "{\"thing\":\"<global>\",\"param\":\"follow\"} ", 0);
	}
	fuSetMaxFaces(mMaxFace);
	return true;
}

bool Nama::CheckModuleCode(int category)
{
	return true;
}

void Nama::RenderDefNama(uchar * frame)
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
		}else{
			renderList.push_back(mBeautyHandles);
			
			if (UIBridge::renderBundleCategory == BundleCategory::Makeup)
			{
				renderList.push_back(UIBridge::m_curRenderItem);
				renderList.push_back(mMakeUpHandle);
			}
			else
			{
				renderList.push_back(UIBridge::m_curRenderItem);
			}
		}
		//֧FU_FORMAT_BGRA_BUFFER  FU_FORMAT_NV21_BUFFER FU_FORMAT_I420_BUFFER FU_FORMAT_RGBA_BUFFER		
		fuRenderItemsEx2(FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame), FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame),
						 mFrameWidth, mFrameHeight, mFrameID, renderList.data(), renderList.size(), NAMA_RENDER_FEATURE_FULL, NULL);
	}

	if (fuGetSystemError())
	{
		printf("%s \n", fuGetSystemErrorString(fuGetSystemError()));
	}
	++mFrameID;
}


void Nama::RenderP2A(cv::Mat & picBg)
{
	m_pCtrl->RenderBundlesBuffer(picBg.data, picBg, mFrameID);
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
void Nama::RenderGS(cv::Mat & picInput)
{
	if (mGSHandle != -1)
	{

		vector<int> vecRender;
		vecRender.push_back(mGSHandle);
		vecRender.push_back(mBeautyHandles);
        
		fuRenderItemsEx2(FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(picInput.data), FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(picInput.data),
			picInput.cols, picInput.rows, mFrameID, vecRender.data(), vecRender.size(), NAMA_RENDER_FEATURE_FULL, NULL);
	}

	if (fuGetSystemError())
	{
		printf("%s \n", fuGetSystemErrorString(fuGetSystemError()));
	}
	++mFrameID;
}


void Nama::RenderItems(cv::Mat & picMat)
{
#ifdef _WIN32
	HGLRC context = wglGetCurrentContext();
	HWND wnd = (HWND)Gui::hWindow;
	wglMakeCurrent(GetDC(wnd), new_context);
#else
	void * curCtx = NaMaGetCurrentGLCtx();
	NaMaMakeAsCurrentCtx(new_context);
#endif
	
	if (UIBridge::showGreenScreen)
	{

		RenderGS(picMat);

	}
	else
	{
		if (!mEnableAvatar)
		{
			RenderDefNama(picMat.data);
		}
		else
		{
			RenderP2A(picMat);
		}
	}

#ifdef _WIN32
	wglMakeCurrent(GetDC(wnd), context);
#else
	NaMaMakeAsCurrentCtx(curCtx);
#endif

	return;
}


void Nama::DrawLandmarks(uchar* frame)
{
	if (false == mEnableNama) return;
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

AvatarType Nama::GetAvatarType()
{
	return m_avaType;
}

void Nama::SwitchAvatar()
{
	switch (m_avaType)
	{
	case NamaExampleNameSpace::AVATAR_TYPE_FULLBODY:
		SetAvatarType(AVATAR_TYPE_HALF);
		break;
	case NamaExampleNameSpace::AVATAR_TYPE_HALF:
		SetAvatarType(AVATAR_TYPE_FULLBODY);
		break;
	default:
		break;
	}
}

void SetPos(FuController* pCtrl, double * pdArray)
{
	pCtrl->SetParams("target_position", pdArray, 3);
	pCtrl->SetParam("reset_all", 1.0);
}

/* 实际使用的时候可以通过其他方式调整模型位置,主要为了显示 */
const double g_FullArray[] = { 0.0, 50.0, -900 };
const double g_HalfArray[] = { 0.0, 0.0, -300 };

void Nama::SetAvatarType(AvatarType type)
{
	if (!m_pCtrl)
	{
		return;
	}

	m_avaType = type;
	switch (type)
	{
	case NamaExampleNameSpace::AVATAR_TYPE_NONE:
		break;
	case NamaExampleNameSpace::AVATAR_TYPE_FULLBODY:
		m_pCtrl->CheckHalfBody(false);
		SetPos(m_pCtrl, (double*)g_FullArray);
		break;
	case NamaExampleNameSpace::AVATAR_TYPE_HALF:
		m_pCtrl->CheckHalfBody(true);
		SetPos(m_pCtrl, (double*)g_HalfArray);
		break;
	default:
		break;
	}
}
