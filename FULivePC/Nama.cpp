/**
* FU SDK使用者可以将拿到处理后的frame图像与自己的原有项目对接
* 请FU SDK使用者直接参考示例放至代码至对应位置
*
* FU SDK与camera无耦合，不关心数据的来源，只要图像内容正确且和宽高吻合即可
*
* Created by liujia on 2018/1/3 mybbs2200@gmail.com.
*/
#include "Camera.h"
#include "Nama.h"
#include "Config.h"	
#include "Gui/Gui.h"

#include <funama.h>				//nama SDK 的头文件
#include <authpack.h>			//nama SDK 的key文件
#pragma comment(lib, "nama.lib")//nama SDK 的lib文件
//音乐播放
#include "Sound/MP3.h"
std::map<int,Mp3*> mp3Map;
using namespace NamaExampleNameSpace;
bool Nama::mHasSetup = false;
static HGLRC new_context;

std::string Nama::mFilters[6] = { "origin", "delta", "electric", "slowlived", "tokyo", "warm" };

Nama::UniquePtr Nama::create(uint32_t width, uint32_t height)
{
	UniquePtr pNama = UniquePtr(new Nama);
	pNama->Init(width, height);
	return pNama;
}

Nama::Nama()
{
	mFrameID = 0;
	mMaxFace = 1;
	mIsBeautyOn = true;
	mBeautyHandles = 0;
	mCapture = std::tr1::shared_ptr<CCameraDS>(new CCameraDS);
}

Nama::~Nama()
{
	if (true == mHasSetup)
	{
		fuDestroyAllItems();//Note: 切忌使用一个已经destroy的item
		fuOnDeviceLost();//Note: 这个调用销毁nama创建的OpenGL资源
	}
	//fuSetup整个程序只需要运行一次，销毁某个子窗口时只需要调用上述两个函数。 
	//Tips:如果其他窗口还会用这些资源，那么资源创建应该在父窗口。程序运行期间一直持有这些资源.

	std::map<int, Mp3*>::iterator it;
	for (it = mp3Map.begin(); it != mp3Map.end(); it++)
	{
		it->second->Cleanup();
		delete it->second;
		UIBridge::mNeedPlayMP3 = false;
	}
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
		fuOnCameraChange();//Note: 重置人脸检测的信息
	}
	return true;
}

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

void InitOpenGL()
{
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

	//hglrc就是创建出的OpenGL context
	printf("hw=%08x hgldc=%08x spf=%d ret=%d hglrc=%08x\n",
		hw, hgldc, spf, ret, hglrc);
}

bool Nama::CheckGLContext()
{
	int add0, add1, add2, add3;
	add0 = (int)wglGetProcAddress("glGenFramebuffersARB");
	add1 = (int)wglGetProcAddress("glGenFramebuffersOES");
	add2 = (int)wglGetProcAddress("glGenFramebuffersEXT");
	add3 = (int)wglGetProcAddress("glGenFramebuffers");
	printf("gl ver test (%s:%d): %08x %08x %08x %08x\n", __FILE__, __LINE__,add0, add1, add2, add3);
	return add0 | add1 | add2 | add3;
}


bool Nama::Init(uint32_t& width, uint32_t& height)
{
	HGLRC context = wglGetCurrentContext();
	HWND wnd = (HWND)Gui::hWindow;
	new_context = wglCreateContext(GetDC(wnd));
	wglMakeCurrent(GetDC(wnd), new_context);
	mCapture->initCamera(width, height,UIBridge::mSelectedCamera);
	mFrameWidth = width;
	mFrameHeight = height;
	if (false == mHasSetup)
	{
		//读取nama数据库，初始化nama
		std::vector<char> v3data;
		if (false == LoadBundle(g_fuDataDir + g_v3Data, v3data))
		{
			std::cout << "Error:缺少数据文件。" << g_fuDataDir + g_v3Data << std::endl;
			return false;
		}
		//CheckGLContext();
		fuSetup(reinterpret_cast<float*>(&v3data[0]), v3data.size(), NULL, g_auth_package, sizeof(g_auth_package));

		std::vector<char> anim_model_data;
		if (false == LoadBundle(g_fuDataDir + g_anim_model, anim_model_data))
		{
			std::cout << "Error:缺少数据文件。" << g_fuDataDir + g_anim_model << std::endl;
			return false;
		}
		// 表情系数稳定
		fuLoadAnimModel(reinterpret_cast<float*>(&anim_model_data[0]), anim_model_data.size());

		std::vector<char> ardata_ex_data;
		if (false == LoadBundle(g_fuDataDir + g_ardata_ex, ardata_ex_data))
		{
			std::cout << "Error:缺少数据文件。" << g_fuDataDir + g_ardata_ex << std::endl;
			return false;
		}
		// AR
		fuLoadExtendedARData(reinterpret_cast<float*>(&ardata_ex_data[0]), ardata_ex_data.size());

		std::vector<char> tongue_model_data;
		if (false == LoadBundle(g_fuDataDir + g_tongue, tongue_model_data))
		{
			std::cout << "Error:缺少数据文件。" << g_fuDataDir + g_tongue << std::endl;
			return false;
		}
		// 舌头识别
		fuLoadTongueModel(reinterpret_cast<float*>(&tongue_model_data[0]), tongue_model_data.size());

		std::vector<char> fxaa_data;
		if (false == LoadBundle(g_fuDataDir + g_fxaa, fxaa_data))
		{
			std::cout << "Error:缺少数据文件。" << g_fuDataDir + g_fxaa << std::endl;
			return false;
		}
		mFxaaHandles = fuCreateItemFromPackage(fxaa_data.data(), fxaa_data.size());
		fuSetExpressionCalibration(1);

		mHasSetup = true;
	}
	else
	{
		fuOnDeviceLost();
		mHasSetup = false;
	}

	//读取美颜道具，设置美颜参数
	{
		std::vector<char> propData;
		if (false == LoadBundle(g_fuDataDir + g_faceBeautification, propData))
		{
			std::cout << "load face beautification data failed." << std::endl;
			return false;
		}
		std::cout << "load face beautification data." << std::endl;

		mBeautyHandles = fuCreateItemFromPackage(&propData[0], propData.size());	
	}
	fuSetDefaultOrientation(0);	
	wglMakeCurrent(GetDC(wnd), context);
	return true;
}

int Nama::IsTracking()
{
	return fuIsTracking();
}

void Nama::SwitchBeauty(bool bValue)
{
	mIsBeautyOn = bValue;
}

void Nama::SetCurrentShape(int index)
{
	if (false == mIsBeautyOn || mBeautyHandles == 0)return;
	if (0 <= index <= 4)
	{
		int res = fuItemSetParamd(mBeautyHandles, "face_shape", index);
	}
}

void Nama::UpdateFilter(int index)
{
	if (false == mIsBeautyOn || mBeautyHandles == 0)return;

	fuItemSetParams(mBeautyHandles, "filter_name", &mFilters[index][0]);
}

void Nama::UpdateBeauty()
{
	if (mBeautyHandles == 0)
	{
		printf("m_beautyHandles is NULL\n");
		return;
	}
	if (false == mIsBeautyOn)return;
	
	for (int i=0;i<MAX_BEAUTYFACEPARAMTER;i++)
	{		
		if (i==0)//磨皮
		{
			fuItemSetParamd(mBeautyHandles, const_cast<char*>(faceBeautyParamName[i].c_str()), UIBridge::mFaceBeautyLevel[i] *6.0/ 100.f);
		} 
		else
		{
			fuItemSetParamd(mBeautyHandles, const_cast<char*>(faceBeautyParamName[i].c_str()), UIBridge::mFaceBeautyLevel[i] / 100.f);
		}		
	}
	std::string faceShapeParamName[] = { "cheek_thinning","eye_enlarging", "intensity_chin", "intensity_forehead", "intensity_nose","intensity_mouth" };
	for (int i=0;i<MAX_FACESHAPEPARAMTER;i++)
	{
		fuItemSetParamd(mBeautyHandles, const_cast<char*>(faceShapeParamName[i].c_str()), UIBridge::mFaceShapeLevel[i]/100.0f);		
	}
	fuItemSetParamd(mBeautyHandles, "skin_detect", UIBridge::mEnableSkinDect);	
	fuItemSetParamd(mBeautyHandles, "heavy_blur", UIBridge::mEnableHeayBlur);
	fuItemSetParamd(mBeautyHandles, "face_shape_level", 1);
	fuItemSetParamd(mBeautyHandles, "filter_level", UIBridge::mFilterLevel[UIBridge::m_curFilterIdx]/100.0f);
}

bool Nama::SelectBundle(std::string bundleName)
{
	int bundleID = -1;	
	//停止播放音乐
	std::map<int, Mp3*>::iterator it;
	for (it = mp3Map.begin(); it != mp3Map.end(); it++)
	{
		long long current = 0;
		it->second->SetPositions(&current, NULL, true);
		it->second->Stop();
		UIBridge::mNeedPlayMP3 = false;
	}
	//如果未加载道具，则加载
	if (0 == mBundlesMap[bundleName])
	{
		std::vector<char> propData;
		if (false == LoadBundle(bundleName, propData))
		{
			std::cout << "load prop data failed." << std::endl;
			UIBridge::m_curRenderItem = -1;
			return false;
		}
		std::cout << "load prop data." << std::endl;
		//Map大小进行控制
		if (mBundlesMap.size() > MAX_NAMA_BUNDLE_NUM)
		{
			fuDestroyItem(mBundlesMap.begin()->second);
			mBundlesMap.erase(mBundlesMap.begin());
			printf("cur map size : %d \n", mBundlesMap.size());
		}

		bundleID = fuCreateItemFromPackage(&propData[0], propData.size());
		mBundlesMap[bundleName] = bundleID;		
		//加载并播放音乐
		if (UIBridge::bundleCategory == BundleCategory::MusicFilter)
		{
			std::string itemName = UIBridge::mCurRenderItemName.substr(0, UIBridge::mCurRenderItemName.find_last_of("."));
			if (mp3Map.find(bundleID) == mp3Map.end())
			{
				Mp3 *mp3 = new Mp3;
				mp3->Load("../../assets/items/MusicFilter/" + itemName + ".mp3");				
				mp3Map[bundleID] = mp3;
			}			
			mp3Map[bundleID]->Play();
			UIBridge::mNeedPlayMP3 = true;
		}
	}
	else
	{
		bundleID = mBundlesMap[bundleName];		
		if (UIBridge::bundleCategory == BundleCategory::MusicFilter)
		{
			mp3Map[bundleID]->Play();
			UIBridge::mNeedPlayMP3 = true;
		}
	}
		
	if (UIBridge::m_curRenderItem == bundleID)
	{
		UIBridge::m_curRenderItem = -1;		
	}
	else
	{
		UIBridge::m_curRenderItem = bundleID;
		UIBridge::renderBundleCategory = UIBridge::bundleCategory;
	}
	if (UIBridge::bundleCategory == PortraitDrive )
	{
		mMaxFace = 1;
	}
	else
	{
		mMaxFace = 4;
	}
	return true;
}
//渲染函数
void Nama::RenderItems(uchar* frame)
{
	HGLRC context = wglGetCurrentContext();
	HWND wnd = (HWND)Gui::hWindow;
	wglMakeCurrent(GetDC(wnd), new_context);
	//此处判断供第一次集成查错用，可以在确认OpenGL环境正确的情况下删掉
	//if (CheckGLContext() == false)
	//{
	//	InitOpenGL();
	//}
	
	fuSetMaxFaces(mMaxFace);
	if (UIBridge::bundleCategory == MusicFilter)
	{
		if (UIBridge::mNeedPlayMP3)
		{
			if (mp3Map.find(UIBridge::m_curRenderItem) != mp3Map.end())
			{
				fuItemSetParamd(UIBridge::m_curRenderItem, "music_time", mp3Map[UIBridge::m_curRenderItem]->GetCurrentPosition() / 1e4);
				mp3Map[UIBridge::m_curRenderItem]->CirculationPlayCheck();
			}
		}
		if(UIBridge::mNeedStopMP3)
		{
			std::map<int, Mp3*>::iterator it = mp3Map.begin();
			for(;it != mp3Map.end();it++)
			{
				it->second->Stop();				
			}
			UIBridge::m_curRenderItem = -1;
			UIBridge::mNeedStopMP3 = false;
		}
	}

	//int handle[3] = { m_beautyHandles, UIBridge::m_curRenderItem ,m_fxaaHandles };
	if (UIBridge::renderBundleCategory == Animoji)
	{
		fuItemSetParamd(UIBridge::m_curRenderItem, "{\"thing\":\"<global>\",\"param\":\"follow\"} ", 1);
	}
	else
	{
		fuItemSetParamd(UIBridge::m_curRenderItem, "{\"thing\":\"<global>\",\"param\":\"follow\"} ", 0);		
	}
	int handle[] = { mBeautyHandles, UIBridge::m_curRenderItem };
	int handleSize = sizeof(handle) / sizeof(handle[0]);
	//支持的格式有FU_FORMAT_BGRA_BUFFER 、 FU_FORMAT_NV21_BUFFER 、FU_FORMAT_I420_BUFFER 、FU_FORMAT_RGBA_BUFFER		
	fuRenderItemsEx2(FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame), FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame),
		mFrameWidth, mFrameHeight, mFrameID, handle, handleSize, NAMA_RENDER_FEATURE_FULL, NULL);
	
	if (fuGetSystemError())
	{
		printf("%s \n", fuGetSystemErrorString(fuGetSystemError()));
	}
	++mFrameID;
	wglMakeCurrent(GetDC(wnd), context);
	return;
}
//只调用nama里的美颜模块
uchar* Nama::RenderEx(uchar* frame)
{
	fuBeautifyImage(FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame),
		FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame),
		mFrameWidth, mFrameHeight, mFrameID, &mBeautyHandles, 1);

	++mFrameID;

	return frame;
}

//绘制人脸特征点
void Nama::DrawLandmarks(uchar* frame)
{
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

namespace NamaExampleNameSpace {

	size_t FileSize(std::ifstream& file)
	{
		std::streampos oldPos = file.tellg();
		file.seekg(0, std::ios::beg);
		std::streampos beg = file.tellg();
		file.seekg(0, std::ios::end);
		std::streampos end = file.tellg();
		file.seekg(oldPos, std::ios::beg);
		return static_cast<size_t>(end - beg);
	}

	bool LoadBundle(const std::string& filepath, std::vector<char>& data)
	{
		std::ifstream fin(filepath, std::ios::binary);
		if (false == fin.good())
		{
			fin.close();
			return false;
		}
		size_t size = FileSize(fin);
		if (0 == size)
		{
			fin.close();
			return false;
		}
		data.resize(size);
		fin.read(reinterpret_cast<char*>(&data[0]), size);

		fin.close();
		return true;
	}
}