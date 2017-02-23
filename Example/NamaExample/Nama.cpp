#include "CameraDS.h"
#include "Nama.h"
#include "GlobalValue.h"
#include <fstream>
#include <iostream>

#include <funama.h>
#include <authpack.h>

#pragma comment(lib, "nama.lib")

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

NE::Nama::Nama()
	:m_frameID(0),
	m_cap(0),
	m_curBundleIdx(0),
	m_mode(PROP),
	m_frameWidth(0),
	m_frameHeight(0)
{
}

void NE::Nama::Init(const int width, const int height)
{
	m_frameWidth = width;
	m_frameHeight = height;
	m_cap = std::tr1::shared_ptr<CCameraDS>(new CCameraDS);
	if (false == m_cap->OpenCamera(0, false, m_frameWidth, m_frameHeight))
	{
		exit(1);
	}

	std::vector<char> v3data;
	if(false == NE::LoadBundle(g_fuDataDir + g_v3Data, v3data))
	{
		exit(1);
	}

	fuSetup(reinterpret_cast<float*>(&v3data[0]), NULL, g_auth_package, sizeof(g_auth_package));

	m_propHandles.resize(g_propCount);

	m_curBundleIdx = -1;
	NextBundle();
}

void NE::Nama::SwitchRenderMode()
{
	static MODE nextMode[] = { LANDMARK , PROP };
	m_mode = nextMode[m_mode];
}

void NE::Nama::PreBundle()
{
	--m_curBundleIdx;
	if (m_curBundleIdx < 0)
	{
		m_curBundleIdx += m_propHandles.size();
	}

	CreateBundle(m_curBundleIdx);
}

void NE::Nama::NextBundle()
{
	++m_curBundleIdx;
	m_curBundleIdx %= m_propHandles.size();

	CreateBundle(m_curBundleIdx);
}

std::tr1::shared_ptr<unsigned char> NE::Nama::NextFrame()
{
	std::tr1::shared_ptr<unsigned char> frame = m_cap->QueryFrame();
	switch (m_mode)
	{
	case PROP:
		fuRenderItems(0, reinterpret_cast<int*>(frame.get()), m_frameWidth, m_frameHeight, m_frameID, &m_propHandles[m_curBundleIdx], 1);
		break;
	case LANDMARK:
		fuRenderItems(0, reinterpret_cast<int*>(frame.get()), m_frameWidth, m_frameHeight, m_frameID, NULL, 0);
		DrawLandmarks(frame);
		break;
	default:
		break;
	}
	
	return frame;
}

void NE::Nama::CreateBundle()
{
	for (int i(0); i != g_propCount; ++i)
	{
		CreateBundle(i);
	}
}

void NE::Nama::CreateBundle(const int idx)
{
	if(0 == m_propHandles[idx])
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
	double landmarks[150];
	int ret = fuGetFaceInfo(0, "landmarks", landmarks, sizeof(landmarks) / sizeof(landmarks[0]));
	for (int i(0); i != 75; ++i)
	{
		DrawPoint(frame, static_cast<int>(landmarks[2 * i]), static_cast<int>(landmarks[2 * i + 1]));
	}
}

void NE::Nama::DrawPoint(std::tr1::shared_ptr<unsigned char> frame, int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	const int offsetX[] = { -1, 0, 1 , -1, 0, 1 , -1, 0, 1 };
	const int offsetY[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};

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
