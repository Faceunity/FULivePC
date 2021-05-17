
#include "Texture.h"
#include "fu_tool.h"
#include "fu_tool_mac.h"
#include <opencv2/opencv.hpp>
#include "GuiTool.h"

std::map<std::string, Texture::SharedPtr> Texture::mTextureMap;
std::vector<std::string> Texture::m_searchPath;

std::map<std::string, Texture::SharedPtr> Texture::mUnLoadTextureMap;
std::map<std::string, Texture::SharedPtr> Texture::mLoadingTextureMap;

void Texture::create(uint32_t width, uint32_t height, unsigned char* pixels)
{
	mTextureID = kMaxPossible;														  // Upload texture to graphics system
	this->pixels = pixels;
	m_width = width;
	m_height = height;
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);
	return;
}

Texture::SharedPtr Texture::create2D(uint32_t width, uint32_t height, uint32_t arraySize, uint32_t mipLevels, const void * pInitData)
{
	return SharedPtr();
}

Texture::SharedPtr Texture::genError(const std::string& errMsg, const std::string& filename)
{
	std::string err = "Error when loading image file " + filename + '\n' + errMsg + '.';
	logError(err.c_str());
	return nullptr;
}

bool Texture::AddSearchPath(const char *path)
{
	std::vector<std::string>::iterator itr = m_searchPath.begin();
	while (itr != m_searchPath.end())
	{
		if (!(*itr).compare(path))
		{
			return true;
		}
		itr++;
	}
	m_searchPath.push_back(path);
	return true;
}

bool Texture::RemoveSearchPath(const char *path)
{
	std::vector<std::string>::iterator src = m_searchPath.begin();

	while (src != m_searchPath.end()) {
		if (!(*src).compare(path)) {
			m_searchPath.erase(src);
			return true;
		}
		src++;
	}
	return true;
}


std::string Texture::GetFileFullPathFromeSearchPath(const char * name)
{
	FILE *fp = NULL;
	// loop N times up the hierarchy, testing at each level
	std::string upPath;
	std::string fullPath;
	for (int32_t i = 0; i < 10; i++) {
		std::vector<std::string>::iterator src = m_searchPath.begin();
		bool looping = true;
		while (looping) {
			fullPath.assign(upPath);  // reset to current upPath.
			if (src != m_searchPath.end()) {
				fullPath.append(*src);
				fullPath.append("/res/");
				src++;
			}
			else {
				fullPath.append("res/");
				looping = false;
			}
			fullPath.append(name);

#ifdef DEBUG
#endif
            
#ifdef _WIN32
			if ((fopen_s(&fp, fullPath.c_str(), "rb") == 0) || (fp != NULL))
#else
            fp = fopen(fullPath.c_str(), "rb");
            if(fp)
#endif
			{
				fclose(fp);
				return fullPath;
			}
				
		}

		upPath.append("../");
	}
	//return nullptr;
	return "";
}

Texture::SharedPtr Texture::createUnLoadTextureFromFullPath(const std::string& fullpath, bool bCircle)
{
	if (fullpath.size() == 0)
	{
		return SharedPtr(genError("Image Name unknown", fullpath));
	}
	if (mUnLoadTextureMap.find(fullpath) != mUnLoadTextureMap.end())
	{
		return mUnLoadTextureMap[fullpath];
	}
#ifdef __APPLE__
	std::string filePath = FuToolMac::GetCurrentAppPath() + "//" + fullpath;
#else
	std::string filePath = fullpath;
#endif
	
	cv::Mat icon = cv::imread(filePath.c_str(), cv::IMREAD_UNCHANGED);
	if (icon.empty())
	{
		cv::VideoCapture capture;
		cv::Mat imggif;
		imggif = capture.open(filePath.c_str());
		if (!capture.isOpened())
		{
			return SharedPtr(genError("Image Name unknown", fullpath));
		}
		while (capture.read(imggif))
		{
			icon = imggif.clone();
		}
		capture.release();
		
		if ((icon.empty()))
		{
			return SharedPtr(genError("Image Name unknown", fullpath));
		}
	}
	cv::cvtColor(icon, icon, cv::COLOR_BGRA2RGBA);

	cv::Mat outData;
	if (bCircle)
	{
		gui_tool::CutCircleInMiddle(icon, outData);
}
	else
	{
		outData = icon;
	}

#ifdef _WIN32
	std::string unLoadfullpath = GetFileFullPathFromeSearchPath("boutique_stickers_icon_download.png");
#else
	std::string unLoadfullpath = GetFileFullPathFromeSearchPath("boutique_stickers_icon_download.png");
#endif

	cv::Mat downIcon = cv::imread(unLoadfullpath.c_str(), cv::IMREAD_UNCHANGED);
	cv::cvtColor(downIcon, downIcon, cv::COLOR_BGR2RGBA);
	downIcon.copyTo(outData(cv::Rect(outData.cols - downIcon.cols, outData.rows - downIcon.rows, downIcon.cols, downIcon.rows)));
	//cv::cvtColor(icon, icon, cv::COLOR_BGR2RGBA);
	//cv::imshow("image", icon);
	//cv::waitKey();

	auto texture = createTextureFromData(outData.cols, outData.rows, outData.data);
	mUnLoadTextureMap[fullpath] = texture;

	return texture;
}

Texture::SharedPtr Texture::createLoadingTextureFromFullPath(const std::string& fullpath, bool bCircle)
{
	if (fullpath.size() == 0)
	{
		return SharedPtr(genError("Image Name unknown", fullpath));
	}
	if (mLoadingTextureMap.find(fullpath) != mLoadingTextureMap.end())
	{
		return mLoadingTextureMap[fullpath];
	}
#ifdef __APPLE__
	std::string filePath = FuToolMac::GetCurrentAppPath() + "//" + fullpath;
#else
	std::string filePath = fullpath;
#endif

	cv::Mat icon = cv::imread(filePath.c_str(), cv::IMREAD_UNCHANGED);
	if (icon.empty())
	{
		cv::VideoCapture capture;
		cv::Mat imggif;
		imggif = capture.open(filePath.c_str());
		if (!capture.isOpened())
		{
			return SharedPtr(genError("Image Name unknown", fullpath));
		}
		while (capture.read(imggif))
		{
			icon = imggif.clone();
		}
		capture.release();

		if ((icon.empty()))
		{
			return SharedPtr(genError("Image Name unknown", fullpath));
		}
	}
	cv::cvtColor(icon, icon, cv::COLOR_BGRA2RGBA);

	cv::Mat outData;
	if (bCircle)
	{
		gui_tool::CutCircleInMiddle(icon, outData);
	}
	else
	{
		outData = icon;
	}

#ifdef _WIN32
	std::string unLoadfullpath = GetFileFullPathFromeSearchPath("btnLoad.png");
#else
	std::string unLoadfullpath = GetFileFullPathFromeSearchPath("btnLoad.png");
#endif

	cv::Mat downIcon = cv::imread(unLoadfullpath.c_str(), cv::IMREAD_UNCHANGED);
	cv::cvtColor(downIcon, downIcon, cv::COLOR_BGR2RGBA);
	downIcon.copyTo(outData(cv::Rect(outData.cols - downIcon.cols, outData.rows - downIcon.rows, downIcon.cols, downIcon.rows)));
	//cv::cvtColor(icon, icon, cv::COLOR_BGR2RGBA);
	//cv::imshow("image", icon);
	//cv::waitKey();

	auto texture = createTextureFromData(outData.cols, outData.rows, outData.data);
	mUnLoadTextureMap[fullpath] = texture;

	return texture;
}


Texture::SharedPtr Texture::createTextureFromFullPath(const std::string& fullpath, bool bCircle)
{
	SharedPtr pTexture = SharedPtr(new Texture);
	if (fullpath.size() == 0)
	{
		return SharedPtr(genError("Image Name unknown", fullpath));
	}
	if (mTextureMap.find(fullpath) != mTextureMap.end())
	{
		return mTextureMap[fullpath];
	}
	
#ifdef __APPLE__
    std::string filePath = FuToolMac::GetCurrentAppPath() + "//" + fullpath;
#else
    std::string filePath = fullpath;
#endif
	cv::Mat icon = cv::imread(filePath.c_str(), cv::IMREAD_UNCHANGED);
	if (icon.empty())
	{
		cv::VideoCapture capture;
		cv::Mat imggif;
		imggif = capture.open(filePath.c_str());
		if (!capture.isOpened())
		{
			return SharedPtr(genError("Image Load Failed", fullpath));
		}
		while (capture.read(imggif))
		{
			icon = imggif.clone();
		}
		capture.release();

		if ((icon.empty()))
		{
			return SharedPtr(genError("Image Load Failed", fullpath));
		}
	}


	cv::Mat outData;
	if (bCircle)
	{
		gui_tool::CutCircleInMiddle(icon, outData);
	}
	else
	{
		outData = icon;
	}

	cv::cvtColor(outData, outData, cv::COLOR_BGRA2RGBA);
	pTexture->create(outData.cols, outData.rows, outData.data);

	mTextureMap[fullpath] = pTexture;
	return pTexture;
}

Texture::SharedPtr Texture::createTextureFromFile(const std::string filename, bool generateMips)
{
	SharedPtr pTexture = SharedPtr(new Texture);
    if (filename.size() == 0)
    {
        return SharedPtr(genError("Image Name unknown", filename));
    }
	
    //这边默认所有素材名称都不相同,只是win32需要全路径
#ifdef _WIN32
	std::string fullpath = GetFileFullPathFromeSearchPath(filename.c_str());
#else
    std::string fullpath = filename;
#endif
    
	if (mTextureMap.find(fullpath)!= mTextureMap.end())
	{
		return mTextureMap[fullpath];
	}
    
    auto pBmp = FuTool::getBitmapFromFile(fullpath);
	if(!pBmp)
    {
        return SharedPtr(genError("Image Load Failed", filename));
    }
    
	pTexture->create(pBmp->mWidth, pBmp->mHeight, pBmp->mpData);
    delete pBmp;
    
	mTextureMap[fullpath] = pTexture;
	return pTexture;
}

Texture::SharedPtr Texture::createTextureFromData(uint32_t width, uint32_t height, unsigned char* pixels)
{
	SharedPtr pTexture = SharedPtr(new Texture);
	pTexture->create(width, height, pixels);
	return pTexture;
}

Texture::~Texture()
{
}
