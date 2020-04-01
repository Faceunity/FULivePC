#include "fu_tool.h"
#include <vector>
#include "freeImage.h"
#include <sstream>
#include <fstream>
#include <iostream>

#ifdef __APPLE__
#include "fu_tool_mac.h"
#endif

#pragma  comment(lib,"FreeImage.lib")

std::vector<std::string> FuTool::m_searchPath;

static FILE * openFile(const char * fullPath)
{
	FILE * fp = NULL;
#ifdef _WIN32
	fopen_s(&fp, fullPath, "rb");
#else
	fp = fopen(fullPath, "rb");
#endif
	return fp;
}

std::string FuTool::GetFileFullPathFromeSearchPath(const char * name,const char * InFolderName)
{
#ifdef _WIN32
    FILE *fp = NULL;
    
    fp = openFile(name);
    if(fp){
        fclose(fp);
        return name;
    }
    
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
                fullPath.append("/");
                fullPath.append(InFolderName);
                fullPath.append("/");
                src++;
            }
            else {
                //sprintf_s(fullPath, "%sassets/%s", upPath, filePath);
                fullPath.append(InFolderName);
                fullPath.append("/");
                looping = false;
            }
            fullPath.append(name);

#ifdef DEBUG
           // fprintf(stderr, "Trying to open %s\n", fullPath.c_str());
#endif
            
            fp = openFile(fullPath.c_str());
            if(fp)
            {
                fclose(fp);
                return fullPath;
            }
                
        }

        upPath.append("../");
    }
    
    return "";
#else
    return FuToolMac::GetFileFullPathFromBundle(name);
#endif
}

std::vector<std::string> FuTool::getVideoDevices()
{
#ifdef __APPLE__
    return FuToolMac::getVideoDevices();
#else
    //暂时不从camera那边挪过来了
    std::vector<std::string> VecNone;
    return VecNone;
#endif
}

#ifdef _WIN32
/* From FreeImage */
template <class T> void INPLACESWAP(T& a, T& b) {
    a ^= b; b ^= a; a ^= b;
}
static BOOL SwapRedBlue32(FIBITMAP* dib) {
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

static Bitmap * getBitmapFromFileWithFreeImage(std::string fullpath)
{
    Bitmap * pBmp = NULL;
    
    FREE_IMAGE_FORMAT fifFormat = FIF_UNKNOWN;
    
    do
    {
        fifFormat = FreeImage_GetFileType(fullpath.c_str(), 0);
        if (fifFormat == FIF_UNKNOWN)
        {
            // Can't get the format from the file. Use file extension
            fifFormat = FreeImage_GetFIFFromFilename(fullpath.c_str());

            if (fifFormat == FIF_UNKNOWN)
            {
                break;
            }
        }

        // Check the the library supports loading this image Type
        if (FreeImage_FIFSupportsReading(fifFormat) == false)
        {
            break;
        }

        // Read the DIB
        FIBITMAP* pDib = FreeImage_Load(fifFormat, fullpath.c_str());
        if (pDib == nullptr)
        {
            break;
        }

        // create the bitmap
        pBmp = new Bitmap();
        pBmp->mHeight = FreeImage_GetHeight(pDib);
        pBmp->mWidth = FreeImage_GetWidth(pDib);

        if (pBmp->mHeight == 0 || pBmp->mWidth == 0 || FreeImage_GetBits(pDib) == nullptr)
        {
            break;
        }
        
        uint32_t bpp = FreeImage_GetBPP(pDib);

        // Convert the image to RGBX image
        if (bpp == 24)
        {
            printf("Converting 24-bit texture to 32-bit");
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
        
    }while(0);
    
    return pBmp;
}

#endif

Bitmap * FuTool::getBitmapFromFile(std::string fullpath)
{
#ifdef _WIN32
    return getBitmapFromFileWithFreeImage(fullpath);
#else
    return FuToolMac::getBitmapFromAsset(fullpath);
#endif
}

#include "Config.h"

static size_t FileSize(std::ifstream& file)
{
    std::streampos oldPos = file.tellg();
    file.seekg(0, std::ios::beg);
    std::streampos beg = file.tellg();
    file.seekg(0, std::ios::end);
    std::streampos end = file.tellg();
    file.seekg(oldPos, std::ios::beg);
    return static_cast<size_t>(end - beg);
}

static bool LoadBundleInner(const std::string& filepath, std::vector<char>& data)
{
    
    std::string strRealPath = "";
    strRealPath = FuTool::GetFileFullPathFromeSearchPath(filepath.c_str(),g_fuDataDir.c_str());
    
    std::ifstream fin(strRealPath, std::ios::binary);
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

bool FuTool::LoadBundle(const std::string& filepath, std::vector<char>& data)
{
    return LoadBundleInner(filepath,data);
}


