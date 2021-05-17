#ifndef _FL_TOOL_H_
#define _FL_TOOL_H_

#include <string>
#include <set>
#include <vector>
#include "fu_common_def.h"
using namespace std;

class FuTool
{
public:

	static vector<string> FuTool::splitpath(
		const string& str
		, const set<char> delimiters);
	/*
	查询某个固定文件夹下的某个文件的全路径
	*/
	static string GetFileFullPathFromeSearchPath(const char * name, const char * InFolderName = "res");

	static Bitmap * getBitmapFromFile(string file);

	static vector<string> getVideoDevices();

	static bool LoadBundleByFullPath(const string& filepath, vector<char>& data);

	static bool LoadBundle(const string& filepath, vector<char>& data);

	static void getFileNames(string path, vector<string>& files, string suffix);
	static void getFiles(string path, vector<string>& files, string suffix);
	static vector<string> getFilesInFolder(string folder, string suffix);
	static int extractIntFromPath(string path);
	static string GetRelativePath(string fullpath, string dependpath);
	static int getLocalVideoRotation(string& videoFilePath);
	static bool getLocalVideoResolution(string& videoPath, float* videoWidth, float* videoHeight);
	static string convert2utf8(const char * strIn);
	static string convert2local8bit(const char * strIn);
public:
	static vector<string> m_searchPath;
private:
#pragma mark ---------------------   macosx 特殊 处理 方法   ---------------------------------
	typedef int FUSearchFileReturnType;
	static void getFiles_macos(string path, vector< string>& files, string suffix, FUSearchFileReturnType returnType);

};

#endif