#include "fu_tool.h"
#include <vector>
#include "freeImage.h"
#include <sstream>
#include <fstream>
#include <iostream>
extern "C" {
#include "libavformat/avformat.h"
}
#if _WIN32
#include <io.h>
#pragma  comment(lib,"FreeImage.lib")
#elif __APPLE__
#include "fu_tool_mac.h"
#include <dirent.h>
#include <limits.h>       //For PATH_MAX
#endif




#pragma mark ---------------------   macosx 特殊 处理 方法   ---------------------------------
#if __APPLE__
typedef enum {
	FUSearchFileReturnFullPath,   // /Users/lee/Desktop/NAMA/FULivePC/res/title.png
	FUSearchFileReturnName,       // title
	FUSearchFileReturnLastPathComponent //title.png
}FUSearchFileReturnType;
#endif


vector<string> FuTool::splitpath(
	const string& str
	, const set<char> delimiters)
{
	vector<string> result;

	char const* pch = str.c_str();
	char const* start = pch;
	for (; *pch; ++pch)
	{
		if (delimiters.find(*pch) != delimiters.end())
		{
			if (start != pch)
			{
				string str(start, pch);
				result.push_back(str);
			}
			else
			{
				result.push_back("");
			}
			start = pch + 1;
		}
	}
	result.push_back(start);

	return result;
}


vector<string> FuTool::m_searchPath;

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

string FuTool::GetFileFullPathFromeSearchPath(const char * name,const char * InFolderName)
{
#ifdef _WIN32
    FILE *fp = NULL;
    
    fp = openFile(name);
    if(fp){
        fclose(fp);
        return name;
    }
    
    // loop N times up the hierarchy, testing at each level
    string upPath;
    string fullPath;
    for (int32_t i = 0; i < 10; i++) {
        vector<string>::iterator src = m_searchPath.begin();
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
    return FuToolMac::GetFileFullPathFromResourceBundle(name);
#endif
}

vector<string> FuTool::getVideoDevices()
{
#ifdef __APPLE__
    return FuToolMac::getVideoDevices();
#else
    //暂时不从camera那边挪过来了
    vector<string> VecNone;
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

static Bitmap * getBitmapFromFileWithFreeImage(string fullpath)
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

Bitmap * FuTool::getBitmapFromFile(string file)
{
#ifdef _WIN32
    return getBitmapFromFileWithFreeImage(file);
#else
    return FuToolMac::getBitmapFromAsset(file);
#endif
}

#include "Config.h"

static size_t FileSize(ifstream& file)
{
    streampos oldPos = file.tellg();
    file.seekg(0, ios::beg);
    streampos beg = file.tellg();
    file.seekg(0, ios::end);
    streampos end = file.tellg();
    file.seekg(oldPos, ios::beg);
    return static_cast<size_t>(end - beg);
}

static bool LoadBundleInner(const string& filepath, vector<char>& data)
{
    
    string strRealPath = "";
    strRealPath = FuTool::GetFileFullPathFromeSearchPath(filepath.c_str(),g_fuDataDir.c_str());
    
    ifstream fin(strRealPath, ios::binary);
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

/// 加载bundle
/// @param filepath 相对路径 ，macosx 上为相对于 Resource.bundle 文件
/// @param data 加载成功后返回的数据
bool FuTool::LoadBundle(const string& filepath, vector<char>& data)
{
    return LoadBundleInner(filepath,data);
}

/// 在一个文件夹下获取指定后缀名的文件名数组
/// @param path 文件夹路径
/// @param files 接收文件名的数组，接收的是文件名
/// @param suffix 指定的文件后缀
void FuTool::getFileNames(string path, vector<string>& files, string suffix)
{
#if _WIN32
#elif __APPLE__
	getFiles_macos(path, files, suffix, FUSearchFileReturnName);
#endif
}
/// 在一个文件夹下获取指定后缀名的文件数组
/// @param path 文件夹路径
/// @param files 接收路径的数组，接收的是全路径
/// @param suffix 指定的文件后缀
void FuTool::getFiles(string path, vector<string>& files, string suffix)
{
#if _WIN32
    //文件句柄  
    intptr_t   hFile = 0;
    //文件信息  
    struct _finddata_t fileinfo;
    string p;
    p.assign(path).append("/*.").append(suffix);
    
    auto charLast = path.at(path.length() - 1);
    if (charLast != '/')
    {
        path += '/';
    }
    
    if ((hFile = _findfirst(p.c_str(), &fileinfo)) != -1)
    {
        do
        {
            //加入列表
            files.push_back(path+fileinfo.name);
            
        } while (_findnext(hFile, &fileinfo) == 0);
        
        _findclose(hFile);
    }
#elif __APPLE__
	 getFiles_macos(path, files, suffix, FUSearchFileReturnFullPath);
#endif
}



/// 获取文件夹下的所有文件数组，数组单元为 folder + filename + suffix 的 形式 
/// @param folder 文件夹路径，相对路径或者绝对路径都可以，能访问到即可
/// @param suffix 后缀
vector<string> FuTool::getFilesInFolder(string folder, string suffix)
{
	vector<string> fileNames;
#if _WIN32
	getFiles(folder, fileNames, suffix);
#elif __APPLE__
	string fullDirPath = FuToolMac::GetFileFullPathFromResourceBundle(folder.c_str());
    FuTool::getFiles_macos(fullDirPath, fileNames, suffix,FUSearchFileReturnFullPath);
#endif
	return fileNames;
}

// 从路径中提取路径包含的序号，仅适用于 *_05.*类型的路径，
// 例如 items/BeautyHair/hair_normal_05.bundle =》 05
// 随着需求的增加，后面会补充这个方法
int FuTool::extractIntFromPath(string path){
	// 获取 items/BeautyHair/hair_normal_05.bundle =》 05
	string filename = path;
	
	// Remove directory if present.
	// Do this before extension removal incase directory has a period character.
	const size_t last_slash_idx = filename.find_last_of("_");
	if (string::npos != last_slash_idx)
	{
		filename.erase(0, last_slash_idx + 1);
	}			
	// Remove extension if present.
	const size_t period_idx = filename.rfind('.');
	if (string::npos != period_idx)
	{
		filename.erase(period_idx);
	}
	int index = atoi(filename.c_str());
	return index;
}
/// 根据 dependpath 从 fullpath 获取相对路径 
/// 如 fullpath =/Users/lee/Desktop/NAMA/FULivePC/bin/mac/FULivePC.app/Contents/Resources/Resource.bundle/PoseTrack/anim_gun.bundle  
/// dependpath = Resource.bundle
/// => relativePath = PoseTrack/anim_gun.bundle
/// @param fullpath 全路径
/// @param dependpath 依据的路径
/// @return 如果没有成功，则返回  fullpath
string FuTool::GetRelativePath(string fullpath,string dependpath){
	cout << "fullpath: " << fullpath << '\n';
	size_t found = fullpath.rfind(dependpath);
	string relativePath;
	if (found == string::npos)
	{
		relativePath = fullpath;
	}else{
		relativePath = fullpath.substr(found + dependpath.size());
	}
	cout << "relativePath: " << relativePath << '\n';
	return relativePath;
}
bool FuTool::getLocalVideoResolution(string& videoPath,float* videoWidth,float* videoHeight){
	int rotation = 0;
	AVFormatContext *fmt_ctx = NULL;
	AVDictionaryEntry *tag = NULL;
	
	int ret;
	av_register_all();
	int videoStream = -1;
	bool isSuccess = false;
	if ((ret = avformat_open_input(&fmt_ctx, videoPath.c_str(), NULL, NULL)))
		rotation = 0;
	if (fmt_ctx) {
		for (int i = 0; i < fmt_ctx->nb_streams; i++) {
			printf("fmt_ctx->streams[i]->codec->codec_type------------::%d\n", fmt_ctx->streams[i]->codec->codec_type);
			if (fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO && videoStream < 0) {
				videoStream = i;
				AVDictionary* streamsMetadata = (AVDictionary*)fmt_ctx->streams[i]->metadata;
				while ((tag = av_dict_get(streamsMetadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
					printf("getLocalVideoRotation-------::%s------::%s\n",tag->key,tag->value);
					if (!strcmp(tag->key, "rotate")) {
						rotation = atoi(tag->value);
						break;
					}
					
				}
				
				AVDictionary* metadata = (AVDictionary*)fmt_ctx->metadata;
				while ((tag = av_dict_get(metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
					printf("getLocalVideoRotation---metadata----::%s------::%s\n",tag->key,tag->value);
					//					if (!strcmp(tag->key, "rotate")) {
					//						rotation = atoi(tag->value);
					//						break;
					//					}
					
				}
			}
			
			
			
			break;
		}
		if (videoStream >= 0) {

			// Get a pointer to the codec context for the video stream
			AVCodecContext* pCodecCtx = fmt_ctx->streams[videoStream]->codec;

			printf("\n");
			printf("Width: %d\n", pCodecCtx->width);
			printf("Height: %d\n", pCodecCtx->height);
			*videoWidth = pCodecCtx->width;
			*videoHeight = pCodecCtx->height;
			isSuccess = true;
		}
	}
	avformat_free_context(fmt_ctx);
	return isSuccess;
}
int FuTool::getLocalVideoRotation(string& videoPath){
	int rotation = 0;
	AVFormatContext *fmt_ctx = NULL;
	AVDictionaryEntry *tag = NULL;
	int ret;
	av_register_all();
	int videoStream = -1;
	if ((ret = avformat_open_input(&fmt_ctx, videoPath.c_str(), NULL, NULL)))
		rotation = 0;
	if (fmt_ctx) {
		for (int i = 0; i < fmt_ctx->nb_streams; i++) {
			if (fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO && videoStream < 0) {
				videoStream = i;
				AVDictionary* streamsMetadata = (AVDictionary*)fmt_ctx->streams[i]->metadata;
				while ((tag = av_dict_get(streamsMetadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
					printf("getLocalVideoRotation-------::%s------::%s\n",tag->key,tag->value);
					if (!strcmp(tag->key, "rotate")) {
						rotation = atoi(tag->value);
						break;
					}
					
				}
			}
			break;
		}
	}
	avformat_free_context(fmt_ctx);
	return rotation;
}
#if __APPLE__

#pragma mark ---------------------   macosx 特殊 处理 方法   ---------------------------------
/// macos 平台 下，获取文件夹下所有文件的路径或者文件名
/// @param path 文件夹路径
/// @param files 返回的文件路径数组或者文件名数组
/// @param suffix 要获取的文件类型后缀
/// @param returnType 在查询到文件夹后，需要返回给 files 数组的 path 类型
void FuTool::getFiles_macos(string path, vector<string>& files, string suffix,FUSearchFileReturnType returnType){
	DIR* dirFile = opendir( path.c_str());
	// 处理后缀
	if(suffix.at(0) != '.')
	{
		suffix.insert(suffix.begin(), '.');
	}
	char buf[PATH_MAX + 1]; 
	if ( dirFile ) 
	{
		struct dirent* hFile;
		errno = 0;
		while (( hFile = readdir( dirFile )) != NULL ) 
		{
			if ( !strcmp( hFile->d_name, "."  )) continue;
			if ( !strcmp( hFile->d_name, ".." )) continue;
			
			// in linux hidden files all start with '.'
			if ( hFile->d_name[0] == '.' ) continue;
			
			// 比较后缀，加入数组
			if ( strstr( hFile->d_name, suffix.c_str() ))
			{
				
				//printf( "found an type file: %s", hFile->d_name );
				//                realpath(hFile->d_name, buf);
				//                printf ("[%s]\n", buf);
				//加入列表
				switch (returnType) {
					case FUSearchFileReturnFullPath:
						if (path.back() != '/') {
							path += '/';
						}
						files.push_back(path + hFile->d_name);
						break;
					case FUSearchFileReturnName:
					{
						string d_name = hFile->d_name;
						size_t lastindex = d_name.find_last_of("."); 
						string rawname = d_name.substr(0, lastindex); 
						files.push_back(rawname);
					}
						break;
					case FUSearchFileReturnLastPathComponent:
						files.push_back(hFile->d_name);
						break;
					default:
						break;
				}
				
			}
		} 
		closedir( dirFile );
	}
}
#endif
