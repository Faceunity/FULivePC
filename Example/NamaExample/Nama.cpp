/**
* FU SDK使用者可以将拿到处理后的frame图像与自己的原有项目对接
* 请FU SDK使用者直接参考示例放至代码至对应位置
*
* FU SDK与camera无耦合，不关心数据的来源，只要图像内容正确且和宽高吻合即可
*
* Created by liujia on 2017/3/24 mybbs2200@gmail.com.
*/
#include "CameraDS.h"
#include "Nama.h"
#include "GlobalValue.h"
#include <fstream>
#include <iostream>

#include <funama.h>
#include <authpack.h>

#pragma comment(lib, "nama.lib")

#include "CrashReport/CrashReport.h"
bool NE::Nama::m_hasSetup = false;

namespace NE
{
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

std::string NE::Nama::_filters[6] = { "nature", "delta", "electric", "slowlived", "tokyo", "warm" };

NE::Nama::Nama()
	:m_frameID(0),
	m_cap(0),
	m_curBundleIdx(0),
	m_mode(PROP),
	m_isBeautyOn(true),
	m_isDrawProp(1),
	m_frameWidth(0),
	m_frameHeight(0),
	m_curFilterIdx(0),
	m_beautyHandles(0),
	m_gestureHandles(0),
	m_propHandles(0),
	m_curColorLevel(1.0f),
	m_curBlurLevel(5.0f),
	m_curCheekThinning(1.0f),
	m_curEyeEnlarging(1.0f),
	m_face_shape(0),
	m_redLevel(0.6f),
	m_faceShapeLevel(1.0f)
{
}

NE::Nama::~Nama()
{	
	if (true== m_hasSetup)
	{
		fuDestroyAllItems();//Note: 切忌使用一个已经destroy的item
	}	
}

void NE::Nama::Init(const int width, const int height)
{
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	m_frameWidth = width;
	m_frameHeight = height;
	m_cap = std::tr1::shared_ptr<CCameraDS>(new CCameraDS);
	int cameraCount = m_cap->CameraCount();
	int chooseCamera = 0;
	if (cameraCount > 1)
	{		
		std::cout << "camera count:" << cameraCount << std::endl << "please input CameraID:[0,1,2...] ";
		std::cin >> chooseCamera;
		while (chooseCamera > cameraCount - 1 || chooseCamera < 0)
		{
			std::cout << "camera count:" << cameraCount << std::endl << "please input CameraID:[0,1,2...] ";
			std::cin >> chooseCamera;
		}
	}	
	if (false == m_cap->OpenCamera(chooseCamera, false, m_frameWidth, m_frameHeight))
	{
		std::cout << "缺少摄像头，推荐使用 Logitech C920，然后安装官方驱动。" << std::endl ;
		Sleep(3000);
		exit(1);
	}

	if (false == m_hasSetup)
	{
		std::vector<char> v3data;
		if (false == NE::LoadBundle(g_fuDataDir + g_v3Data, v3data))
		{
			exit(1);
		}		
		fuSetup(reinterpret_cast<float*>(&v3data[0]), NULL, g_auth_package, sizeof(g_auth_package));
		m_hasSetup = true;
	}
	else
	{
		fuOnDeviceLost();
	}

	{
		std::vector<char> propData;
		if (false == NE::LoadBundle(g_fuDataDir + g_faceBeautification, propData))
		{
			std::cout << "load face beautification data failed." << std::endl;
			exit(1);
		}
		std::cout << "load face beautification data." << std::endl;

		m_beautyHandles = fuCreateItemFromPackage(&propData[0], propData.size());


		//"nature", "delta", "electric", "slowlived", "tokyo", "warm"等参数的设置
		fuItemSetParams(m_beautyHandles, "filter_name", &_filters[m_curFilterIdx][0]);
		fuItemSetParamd(m_beautyHandles, "is_opengl_es", 0);
		fuItemSetParamd(m_beautyHandles, "color_level", m_curColorLevel);
		fuItemSetParamd(m_beautyHandles, "blur_level", m_curBlurLevel);
		fuItemSetParamd(m_beautyHandles, "cheek_thinning", m_curCheekThinning);
		fuItemSetParamd(m_beautyHandles, "eye_enlarging", m_curEyeEnlarging);
		fuItemSetParamd(m_beautyHandles, "face_shape_level", m_faceShapeLevel);
		fuItemSetParamd(m_beautyHandles, "red_level", m_redLevel);
		fuItemSetParamd(m_beautyHandles, "face_shape", m_face_shape);
	}
	{
		std::vector<char> gestureData;
		if (false == NE::LoadBundle(g_fuDataDir + g_gestureRecongnition, gestureData))
		{
			std::cout << "load gesture recognition data failed." << std::endl;
			exit(1);
		}
		std::cout << "load gesture recognition data." << std::endl;

		m_gestureHandles = fuCreateItemFromPackage(&gestureData[0], gestureData.size());
	}
	m_propHandles.resize(g_propCount);

	m_curBundleIdx = -1;
	NextBundle();
}

void NE::Nama::SwitchRenderMode()
{
	static MODE nextMode[] = { LANDMARK , PROP };
	m_mode = nextMode[m_mode];
}

void NE::Nama::SwitchBeauty()
{
	m_isBeautyOn = !m_isBeautyOn;
}

void NE::Nama::PreBundle()
{
	if (!m_isDrawProp)
	{
		return;
	}
	--m_curBundleIdx;
	if (m_curBundleIdx < 0)
	{
		m_curBundleIdx += m_propHandles.size();
	}

	CreateBundle(m_curBundleIdx);
}

void NE::Nama::NextBundle()
{
	if (!m_isDrawProp)
	{
		return;
	}
	++m_curBundleIdx;
	m_curBundleIdx %= m_propHandles.size();

	CreateBundle(m_curBundleIdx);
}

void NE::Nama::NextShape()
{
	if (false == m_isBeautyOn)
	{
		return;
	}
	++m_face_shape;
	m_face_shape %= 3;
	int res = fuItemSetParamd(m_beautyHandles, "face_shape", m_face_shape);
	std::cout << "face_shape: " << m_face_shape << std::endl;
}

void NE::Nama::UpdateFilter()
{
	if (false == m_isBeautyOn)
	{
		return;
	}
	fuItemSetParams(m_beautyHandles, "filter_name", &_filters[m_curFilterIdx][0]);
}

void NE::Nama::UpdateBeauty()
{
	if (false == m_isBeautyOn)
	{
		return;
	}
	fuItemSetParamd(m_beautyHandles, "color_level", m_curColorLevel);
	fuItemSetParamd(m_beautyHandles, "blur_level", m_curBlurLevel);
	fuItemSetParamd(m_beautyHandles, "cheek_thinning", m_curCheekThinning);
	fuItemSetParamd(m_beautyHandles, "eye_enlarging", m_curEyeEnlarging);
	fuItemSetParamd(m_beautyHandles, "face_shape_level", m_faceShapeLevel);
	fuItemSetParamd(m_beautyHandles, "red_level", m_redLevel);
	//fuItemSetParamd(m_beautyHandles, "face_shape", m_face_shape);
}
std::tr1::shared_ptr<unsigned char> NE::Nama::RevertFrameBuffer(std::tr1::shared_ptr<unsigned char> frame)
{
	int size = m_frameWidth*m_frameHeight * 4;
	auto temp_frame = std::tr1::shared_ptr<unsigned char>(new unsigned char[size]);

	for (int i = 0; i < m_frameHeight; i++)
	{
		auto ptr = frame.get() + i*m_frameWidth * 4 + m_frameWidth * 4;
		auto qptr = temp_frame.get() + i*m_frameWidth * 4;
		for (int j = 0; j < m_frameWidth; j++)
		{
			qptr[0] = ptr[0];
			qptr[1] = ptr[1];
			qptr[2] = ptr[2];
			qptr[3] = ptr[3];
			ptr -= 4;
			qptr += 4;
		}
	}

	return temp_frame;
}

void NE::Nama::ScissorFrameBuffer(std::tr1::shared_ptr<unsigned char> frame)
{
	int size = m_frameWidth*m_frameHeight * 4;
	for (int i = 0; i < m_frameHeight; i++)
	{
		auto ptr = frame.get() + i*m_frameWidth * 4 + m_frameWidth * 4;
		auto qptr = frame.get() + i*m_frameWidth * 4;
		for (int j = 0; j < m_frameWidth * 0.31640625f; j++)
		{
			qptr[0] = 0;
			qptr[1] = 0;
			qptr[2] = 0;
			qptr[3] = 0;
			qptr += 4;
			ptr[0] = 0;
			ptr[1] = 0;
			ptr[2] = 0;
			ptr[3] = 0;
			ptr -= 4;
		}
	}
}

//同时调用nama里的所有模块
std::tr1::shared_ptr<unsigned char> NE::Nama::Render()
{
	std::tr1::shared_ptr<unsigned char> frame = m_cap->QueryFrame();
	//ScissorFrameBuffer(frame);
	frame = RevertFrameBuffer(frame);
	
	
	switch (m_mode)
	{
	case PROP:
		if (1 == m_isBeautyOn && 1 == m_isDrawProp)
		{
			int handle[2] = { m_beautyHandles, m_propHandles[m_curBundleIdx] };
			fuRenderItems(0, reinterpret_cast<int*>(frame.get()), m_frameWidth, m_frameHeight, m_frameID, handle, 2);
		}
		else if (1 == m_isDrawProp && 0 == m_isBeautyOn)
		{			
			fuRenderItems(0, reinterpret_cast<int*>(frame.get()), m_frameWidth, m_frameHeight, m_frameID, &m_propHandles[m_curBundleIdx], 1);
		}
		else if (1 == m_isBeautyOn && 0 == m_isDrawProp)
		{
			fuRenderItems(0, reinterpret_cast<int*>(frame.get()), m_frameWidth, m_frameHeight, m_frameID, &m_beautyHandles, 1);
		}
		//fuRenderItems(0, reinterpret_cast<int*>(frame.get()), m_frameWidth, m_frameHeight, m_frameID, &m_gestureHandles, 1);
		break;
	case LANDMARK:
		fuRenderItems(0, reinterpret_cast<int*>(frame.get()), m_frameWidth, m_frameHeight, m_frameID, NULL, 0);
		DrawLandmarks(frame);
		break;
	default:
		break;
	}

	++m_frameID;
	frame = RevertFrameBuffer(frame);
	return frame;
}
//只调用nama里的美颜模块
std::tr1::shared_ptr<unsigned char> NE::Nama::RenderEx()
{
	std::tr1::shared_ptr<unsigned char> frame = m_cap->QueryFrame();

	fuBeautifyImage(FU_FORMAT_BGRA_BUFFER, reinterpret_cast<int*>(frame.get()),
		FU_FORMAT_BGRA_BUFFER, reinterpret_cast<int*>(frame.get()),
		m_frameWidth, m_frameHeight, m_frameID, &m_beautyHandles, 1);

	++m_frameID;

	return frame;
}
//加载全部道具，初始化稍慢
void NE::Nama::CreateBundle()
{
	for (int i(0); i != g_propCount; ++i)
	{
		CreateBundle(i);
	}
}
//按需加载道具，但是在切换的时候会卡顿一下
void NE::Nama::CreateBundle(const int idx)
{
	if (0 == m_propHandles[idx])
	{
		std::vector<char> propData;
		if (false == NE::LoadBundle(g_fuDataDir + g_propName[idx], propData))
		{
			std::cout << "load prop data failed." << std::endl;
			exit(1);
		}
		std::cout << "load prop data." << std::endl;

		m_propHandles[idx] = fuCreateItemFromPackage(&propData[0], propData.size());
	}
}

void NE::Nama::DrawLandmarks(std::tr1::shared_ptr<unsigned char> frame)
{
	float landmarks[150];
	float trans[3];
	float rotat[4];
	int ret = 0;

	ret = fuGetFaceInfo(0, "translation", trans, sizeof(trans) / sizeof(trans[0]));
	ret = fuGetFaceInfo(0, "rotation", rotat, sizeof(rotat) / sizeof(rotat[0]));
	m_curTranslation = ConvertToString(trans[0]) + "," + ConvertToString(trans[1])
		+ "," + ConvertToString(trans[2]);
	m_curRotation = ConvertToString(rotat[0]) + "," + ConvertToString(rotat[1]) + ","
		+ ConvertToString(rotat[2]) + "," + ConvertToString(rotat[3]);

	if (m_isDrawPoints)
	{
		ret = fuGetFaceInfo(0, "landmarks", landmarks, sizeof(landmarks) / sizeof(landmarks[0]));
		for (int i(0); i != 75; ++i)
		{
			DrawPoint(frame, static_cast<int>(landmarks[2 * i]), static_cast<int>(landmarks[2 * i + 1]));
		}
	}
}

void NE::Nama::DrawPoint(std::tr1::shared_ptr<unsigned char> frame, int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	const int offsetX[] = { -1, 0, 1 , -1, 0, 1 , -1, 0, 1 };
	const int offsetY[] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
	const int count = sizeof(offsetX) / sizeof(offsetX[0]);

	unsigned char* data = frame.get();
	for (int i(0); i != count; ++i)
	{
		int xx = x + offsetX[i];
		int yy = y + offsetY[i];
		if (0 > xx || xx > m_frameWidth || 0 > yy || yy > m_frameHeight)
		{
			continue;
		}

		data[yy * 4 * m_frameWidth + xx * 4 + 0] = b;
		data[yy * 4 * m_frameWidth + xx * 4 + 1] = g;
		data[yy * 4 * m_frameWidth + xx * 4 + 2] = r;
	}

}
