
#include "Texture.h"
#pragma  comment(lib,"FreeImage.lib")
std::map<std::string, Texture::SharedPtr> Texture::mTextureMap;
std::vector<std::string> Texture::m_searchPath;

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
				//sprintf_s(fullPath, "%s%s/assets/%s", upPath, *src, filePath);
				fullPath.append(*src);
				fullPath.append("/res/");
				src++;
			}
			else {
				//sprintf_s(fullPath, "%sassets/%s", upPath, filePath);
				fullPath.append("res/");
				looping = false;
			}
			fullPath.append(name);

#ifdef DEBUG
			fprintf(stderr, "Trying to open %s\n", fullPath.c_str());
#endif
			if ((fopen_s(&fp, fullPath.c_str(), "rb") == 0) || (fp != NULL))
			{
				fclose(fp);
				return fullPath;
				//return (void*)fp;
			}
				
		}

		upPath.append("../");
	}
	//return nullptr;
	return "";
}
/* From FreeImage */
template <class T> void INPLACESWAP(T& a, T& b) {
	a ^= b; b ^= a; a ^= b;
}
BOOL SwapRedBlue32(FIBITMAP* dib) {
	if (FreeImage_GetImageType(dib) != FIT_BITMAP) {
		return FALSE;
	}

	const unsigned bytesperpixel = FreeImage_GetBPP(dib) / 8;
	if (bytesperpixel > 4 || bytesperpixel < 3) {
		return FALSE;
	}

	const unsigned height = FreeImage_GetHeight(dib);
	const unsigned pitch = FreeImage_GetPitch(dib);
	const unsigned lineSize = FreeImage_GetLine(dib);

	BYTE* line = FreeImage_GetBits(dib);
	for (unsigned y = 0; y < height; ++y, line += pitch) {
		for (BYTE* pixel = line; pixel < line + lineSize; pixel += bytesperpixel) {
			INPLACESWAP(pixel[0], pixel[2]);
		}
	}

	return TRUE;
}
Texture::SharedPtr Texture::createTextureFromFile(const std::string filename, bool generateMips)
{
	SharedPtr pTexture = SharedPtr(new Texture);
	FREE_IMAGE_FORMAT fifFormat = FIF_UNKNOWN;
	if (filename.size() == 0)
	{
		return SharedPtr(genError("Image Name unknown", filename));
	}
	std::string fullpath = GetFileFullPathFromeSearchPath(filename.c_str());

	if (mTextureMap.find(fullpath)!= mTextureMap.end())
	{
		return mTextureMap[fullpath];
	}
	fifFormat = FreeImage_GetFileType(fullpath.c_str(), 0);
	if (fifFormat == FIF_UNKNOWN)
	{
		// Can't get the format from the file. Use file extension
		fifFormat = FreeImage_GetFIFFromFilename(fullpath.c_str());

		if (fifFormat == FIF_UNKNOWN)
		{
			return SharedPtr(genError("Image Type unknown", filename));
		}
	}

	// Check the the library supports loading this image Type
	if (FreeImage_FIFSupportsReading(fifFormat) == false)
	{
		return SharedPtr(genError("Library doesn't support the file format", filename));
	}

	// Read the DIB
	FIBITMAP* pDib = FreeImage_Load(fifFormat, fullpath.c_str());
	if (pDib == nullptr)
	{
		return SharedPtr(genError("Can't read image file", filename));
	}

	// create the bitmap
	auto pBmp = new Bitmap();
	pBmp->mHeight = FreeImage_GetHeight(pDib);
	pBmp->mWidth = FreeImage_GetWidth(pDib);

	if (pBmp->mHeight == 0 || pBmp->mWidth == 0 || FreeImage_GetBits(pDib) == nullptr)
	{
		return SharedPtr(genError("Invalid image", filename));
	}

	uint32_t bpp = FreeImage_GetBPP(pDib);

	// Convert the image to RGBX image
	if (bpp == 24)
	{
		logWarning("Converting 24-bit texture to 32-bit");
		bpp = 32;
		auto pNew = FreeImage_ConvertTo32Bits(pDib);
		FreeImage_Unload(pDib);
		pDib = pNew;
	}

	uint32_t bytesPerPixel = bpp / 8;

	pBmp->mpData = new uint8_t[pBmp->mHeight * pBmp->mWidth * bytesPerPixel];
	SwapRedBlue32(pDib);
	FreeImage_ConvertToRawBits(pBmp->mpData, pDib, pBmp->mWidth * bytesPerPixel, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, true);
	FreeImage_Unload(pDib);
	
	pTexture->create(pBmp->mWidth, pBmp->mHeight, pBmp->mpData);
	mTextureMap[fullpath] = pTexture;
	return pTexture;
}

Texture::~Texture()
{
	
}
