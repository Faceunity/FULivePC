#include "Camera.h"
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

#ifdef _WIN32

#include "Sound/MP3.h"
std::map<int,Mp3*> mp3Map;
static HGLRC new_context;

#else

#include "NaMaGLContext.h"
static void * new_context;

#endif

std::string Nama::mFilters[6] = { "origin", "bailiang1", "fennen1", "xiaoqingxin1", "lengsediao1", "nuansediao1" };

std::map<int, int> modules = { {Animoji,16},{ItemSticker,2},{ARMask,32},{ChangeFace,128},
{ExpressionRecognition,2048},{MusicFilter,131072},{BackgroundSegmentation,256},
{GestureRecognition,512},{MagicMirror,65536},{PortraitDrive,32768},{Makeup,524288},
{Hair,524288},{ChangeFaceEx,8388608},{ExpressionGif,16777216}, {Facebeauty,1} ,{LightMakeup,0} ,{Facepup,0} };

std::map<int, int> modules1 = { {Animoji,0},{ItemSticker,0},{ARMask,0},{ChangeFace,0},
{ExpressionRecognition,0},{MusicFilter,0},{BackgroundSegmentation,0},
{GestureRecognition,0},{MagicMirror,0},{PortraitDrive,0},{Makeup,0},
{Hair,0},{ChangeFaceEx,0},{ExpressionGif,0}, {Facebeauty,0} ,{LightMakeup,8} ,{Facepup,16} };

Nama::UniquePtr Nama::create(uint32_t width, uint32_t height,bool enable)
{
	UniquePtr pNama = UniquePtr(new Nama);
	mEnableNama = enable;
	pNama->Init(width, height);
	
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
	mCapture = std::tr1::shared_ptr<CCameraDS>(new CCameraDS);
#else
    mCapture = std::shared_ptr<CCameraDS>(new CCameraDS);
#endif
}

Nama::~Nama()
{
	if (true == mHasSetup)
	{
		fuDestroyAllItems();
		fuOnDeviceLost();
		fuDestroyLibData();
	}

#ifdef _WIN32
    
	std::map<int, Mp3*>::iterator it;
	for (it = mp3Map.begin(); it != mp3Map.end(); it++)
	{
		it->second->Cleanup();
		delete it->second;
		UIBridge::mNeedPlayMP3 = false;
	}
    
#endif
}

std::vector<std::string> Nama::CameraList()
{
	return mCapture->getDeviceNameList();
}

cv::Mat Nama::GetFrame()
{
	return mCapture->getFrame();
}

bool Nama::ReOpenCamera(int camID)
{
	if (mCapture->isInit())
	{
		mCapture->closeCamera();
		mCapture->initCamera(mCapture->rs_width, mCapture->rs_height,camID);
		fuOnCameraChange();
	}
	return true;
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
    
	mCapture->initCamera(width, height,UIBridge::mSelectedCamera);
	mFrameWidth = mCapture->m_dstFrameSize.width;
	mFrameHeight = mCapture->m_dstFrameSize.height;
	if (false == mHasSetup && true ==  mEnableNama)
	{	
        CheckGLContext();
		
		fuSetLogLevel(FU_LOG_LEVEL_ERROR);
        
		fuSetup(nullptr, 0, nullptr, g_auth_package, sizeof(g_auth_package));
		// setup without license, only render 1000 frames.
		//fuSetup(nullptr, 0, nullptr, nullptr, 0);

		printf("Nama version:%s \n", fuGetVersion());
		std::vector<char> tongue_model_data;
		if (false == FuTool::LoadBundle(g_tongue, tongue_model_data))
		{
			std::cout << "Error: fail load tongue model" << g_tongue << std::endl;
			return false;
		}

		fuLoadTongueModel(reinterpret_cast<float*>(&tongue_model_data[0]), tongue_model_data.size());

		std::vector<char> ai_model_data;
		if (false == FuTool::LoadBundle(g_ai_faceprocessor, ai_model_data))
		{
			std::cout << "Error:fail load faceprocessor model" << g_ai_faceprocessor << std::endl;
			return false;
		}		
		fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_model_data[0]), ai_model_data.size(), FUAITYPE::FUAITYPE_FACEPROCESSOR);

		std::vector<char> ai_beseg_green_model_data;
		if (false == FuTool::LoadBundle(g_ai_bgseg_green, ai_beseg_green_model_data))
		{
			std::cout << "Error: fail load bgseg green model" << g_ai_bgseg_green << std::endl;
			return false;
		}
		fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_beseg_green_model_data[0]), ai_beseg_green_model_data.size(), FUAITYPE::FUAITYPE_BACKGROUNDSEGMENTATION_GREEN);

		std::vector<char> ai_beseg_model_data;
		if (false == FuTool::LoadBundle(g_ai_bgseg, ai_beseg_model_data))
		{
			std::cout << "Error: fail load bgseg model" << g_ai_bgseg << std::endl;
			return false;
		}
		fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_beseg_model_data[0]), ai_beseg_model_data.size(), FUAITYPE::FUAITYPE_BACKGROUNDSEGMENTATION);

		std::vector<char> ai_hairseg_model_data;
		if (false == FuTool::LoadBundle(g_ai_hairseg, ai_hairseg_model_data))
		{
			std::cout << "Error: fail load hair seg model" << g_ai_hairseg << std::endl;
			return false;
		}
		fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_hairseg_model_data[0]), ai_hairseg_model_data.size(), FUAITYPE::FUAITYPE_HAIRSEGMENTATION);

		std::vector<char> fxaa_data;
		if (false == FuTool::LoadBundle(g_fxaa, fxaa_data))
		{
			std::cout << "Error: Set Bundle" << g_fxaa << std::endl;
			return false;
		}
		mFxaaHandles = fuCreateItemFromPackage(fxaa_data.data(), fxaa_data.size());
		//fuSetExpressionCalibration(1);

		mModuleCode = fuGetModuleCode(0);
		mModuleCode1 = fuGetModuleCode(1);
		if(CheckModuleCode(Facebeauty))
		{
			std::vector<char> propData;
			if (false == FuTool::LoadBundle(g_faceBeautification, propData))
			{
				std::cout << "load face beautification data failed." << std::endl;
				return false;
			}
			std::cout << "load face beautification data." << std::endl;

			mBeautyHandles = fuCreateItemFromPackage(&propData[0], propData.size());
		}

		
		if( CheckModuleCode(Makeup))
		{
			std::vector<char> propData;
			if (false == FuTool::LoadBundle(g_Makeup, propData))
			{
				std::cout << "load face makeup data failed." << std::endl;
				return false;
			}
			std::cout << "load face makeup data." << std::endl;

			mMakeUpHandle = fuCreateItemFromPackage(&propData[0], propData.size());
			fuItemSetParamd(mMakeUpHandle, "is_clear_makeup", 1);
		}
		//fuSetDefaultOrientation(0);
		float fValue = 0.5f;
		fuSetFaceTrackParam("mouth_expression_more_flexible", &fValue);

		fuSetMaxFaces(4);

		mHasSetup = true;
	}
	else
	{
		if(mEnableNama) fuOnDeviceLost();
		mHasSetup = false;
	}

#ifdef _WIN32
	wglMakeCurrent(GetDC(wnd), context);
#else
    NaMaMakeAsCurrentCtx(curCtx);
#endif
    
	return true;
}

int Nama::IsTracking()
{
	if (false == mEnableNama )
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

int Nama::CreateMakeupBundle(std::string bundleName)
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
		std::string jsonpath = bundleName + "/makeup.json";

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

void Nama::SelectMakeupBundle(std::string bundleName)
{
	if (bundleName.size() == 0)
	{
		//reset params
		fuItemSetParamd(mMakeUpHandle, "is_makeup_on", 0);
	}
	fuItemSetParamd(mMakeUpHandle, "is_makeup_on", 1);
	auto paramArr =mMakeupsMap[bundleName];
	
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
			fuItemSetParamd(mMakeUpHandle, const_cast<char*>(param.name.c_str()),param.lip_type );
			break;
		case MakeupParamType::MString:
		{
			Texture::SharedPtr pTexture = Texture::createTextureFromFile(bundleName+"/"+param.tex, false);
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
	
	for (int i=0;i<MAX_BEAUTYFACEPARAMTER;i++)
	{		
		if (i==0)
		{
			fuItemSetParamd(mBeautyHandles, const_cast<char*>(g_faceBeautyParamName[i].c_str()), UIBridge::mFaceBeautyLevel[i] *6.0/ 100.f);
		} 
		else
		{
			fuItemSetParamd(mBeautyHandles, const_cast<char*>(g_faceBeautyParamName[i].c_str()), UIBridge::mFaceBeautyLevel[i] / 100.f);
		}		
	}
	
	for (int i=0;i<MAX_FACESHAPEPARAMTER;i++)
	{
		if (g_faceShapeParamShowFlag[i] == FACE_SHAPE_SHOW_FLAG_MIDDLE)
		{
			UIBridge::mFaceShapeLevel[i] += 50;
		}
		fuItemSetParamd(mBeautyHandles, const_cast<char*>(g_faceShapeParamName[i].c_str()), UIBridge::mFaceShapeLevel[i]/100.0f);

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
	std::map<int, int> blurType = { {0,2},{1,0},{2,1} };
	fuItemSetParamd(mBeautyHandles, "blur_type", blurType[UIBridge::mEnableHeayBlur]);
	fuItemSetParamd(mBeautyHandles, "face_shape_level", 1);
	fuItemSetParamd(mBeautyHandles, "filter_level", UIBridge::mFilterLevel[UIBridge::m_curFilterIdx]/100.0f);
}

bool Nama::SelectBundle(std::string bundleName)
{
	if (false == mEnableNama )
	{
		return false;
	}

	int bundleID = -1;	
	
#ifdef _WIN32
	std::map<int, Mp3*>::iterator it;
	for (it = mp3Map.begin(); it != mp3Map.end(); it++)
	{
		long long current = 0;
		it->second->SetPositions(&current, NULL, true);
		it->second->Stop();
		UIBridge::mNeedPlayMP3 = false;
	}
#endif
	
	if (0 == mBundlesMap[bundleName])
	{
		
		if ( !CheckModuleCode(UIBridge::bundleCategory) )
		{
			std::cout << "no right to use." << std::endl;
			return false;
		}
		std::vector<char> propData;
		if (false == FuTool::LoadBundle(bundleName, propData))
		{
			std::cout << "load prop data failed." << std::endl;
			UIBridge::m_curRenderItem = -1;
			return false;
		}
		std::cout << "load prop data." << std::endl;
		
		if (mBundlesMap.size() > MAX_NAMA_BUNDLE_NUM)
		{
			fuDestroyItem(mBundlesMap.begin()->second);
			mBundlesMap.erase(mBundlesMap.begin());
			printf("cur map size : %d \n", mBundlesMap.size());
		}

		bundleID = fuCreateItemFromPackage(&propData[0], propData.size());
		mBundlesMap[bundleName] = bundleID;

		if (UIBridge::bundleCategory == BundleCategory::Makeup)
		{
			if (UIBridge::m_curBindedItem != -1)
			{
				fuUnbindItems(mMakeUpHandle, &UIBridge::m_curBindedItem, 1);
			}
			fuBindItems(mMakeUpHandle, &bundleID, 1);
			UIBridge::m_curBindedItem = bundleID;
		}
		else
		{
			//fuItemSetParamd(mMakeUpHandle, "is_makeup_on", 0);
		}

		if (UIBridge::bundleCategory == BundleCategory::MusicFilter)
		{
#ifdef _WIN32
			std::string itemName = UIBridge::mCurRenderItemName.substr(0, UIBridge::mCurRenderItemName.find_last_of("."));
			if (mp3Map.find(bundleID) == mp3Map.end())
			{
				Mp3 *mp3 = new Mp3;
				mp3->Load("assets/items/MusicFilter/" + itemName + ".mp3");
				mp3Map[bundleID] = mp3;
			}
			mp3Map[bundleID]->Play();
			UIBridge::mNeedPlayMP3 = true;
#endif
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
		else
		{
			//fuItemSetParamd(mMakeUpHandle, "is_makeup_on", 0);
		}
		if (UIBridge::bundleCategory == BundleCategory::MusicFilter)
		{
#ifdef _WIN32
			mp3Map[bundleID]->Play();
			UIBridge::mNeedPlayMP3 = true;
#endif
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
		UIBridge::m_curRenderItem = bundleID;
		UIBridge::renderBundleCategory = UIBridge::bundleCategory;
	}
	if (UIBridge::bundleCategory == PortraitDrive || UIBridge::bundleCategory == Animoji)
	{
		mMaxFace = 1;
	}
	else
	{
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
	return true;
}

bool Nama::CheckModuleCode(int category)
{	
	return true;//(mModuleCode & modules[category])  || (mModuleCode1 & modules1[category]);
}

void Nama::RenderItems(uchar* frame)
{
#ifdef _WIN32
	HGLRC context = wglGetCurrentContext();
	HWND wnd = (HWND)Gui::hWindow;
	wglMakeCurrent(GetDC(wnd), new_context);
#else
    void * curCtx = NaMaGetCurrentGLCtx();
    NaMaMakeAsCurrentCtx(new_context);
#endif
	
	if (true == mEnableNama )
	{
		//fuSetMaxFaces(mMaxFace);

#ifdef _WIN32
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
			std::map<int, Mp3*>::iterator it = mp3Map.begin();
			for (; it != mp3Map.end(); it++)
			{
				it->second->Stop();
			}
			UIBridge::m_curRenderItem = -1;
			UIBridge::mNeedStopMP3 = false;
		}
#endif

		if (UIBridge::showMakeUpWindow)
		{
			int handle[] = { mBeautyHandles, mMakeUpHandle }; //  mNewFaceTracker(old use)
			int handleSize = sizeof(handle) / sizeof(handle[0]);
			fuRenderItemsEx2(FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame), FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame),
				mFrameWidth, mFrameHeight, mFrameID, handle, handleSize, NAMA_RENDER_FEATURE_FULL, NULL);
		}
		else
		{
			int handle[] = { mBeautyHandles, UIBridge::m_curRenderItem };
			int handleSize = sizeof(handle) / sizeof(handle[0]);
			//֧FU_FORMAT_BGRA_BUFFER  FU_FORMAT_NV21_BUFFER FU_FORMAT_I420_BUFFER FU_FORMAT_RGBA_BUFFER		
			fuRenderItemsEx2(FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame), FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame),
				mFrameWidth, mFrameHeight, mFrameID, handle, handleSize, NAMA_RENDER_FEATURE_FULL, NULL);
		}

		if (fuGetSystemError())
		{
			printf("%s \n", fuGetSystemErrorString(fuGetSystemError()));
		}
		++mFrameID;
	}

#ifdef _WIN32
	wglMakeCurrent(GetDC(wnd), context);
#else
    NaMaMakeAsCurrentCtx(curCtx);
#endif
    
	return;
}

uchar* Nama::RenderEx(uchar* frame)
{
#ifdef _WIN32
    
	HGLRC context = wglGetCurrentContext();
	HWND wnd = (HWND)Gui::hWindow;
	wglMakeCurrent(GetDC(wnd), new_context);
#else
    void * curCtx = NaMaGetCurrentGLCtx();
    NaMaMakeAsCurrentCtx(new_context);
#endif
    
	if (true == mEnableNama)
	{
		fuBeautifyImage(FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame),
			FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame),
			mFrameWidth, mFrameHeight, mFrameID, &mBeautyHandles, 1);

		++mFrameID;
	}
    
#ifdef _WIN32
	wglMakeCurrent(GetDC(wnd), context);
#else
    NaMaMakeAsCurrentCtx(curCtx);
#endif
    
	return frame;
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
