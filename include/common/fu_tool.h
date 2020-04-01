#ifndef _FL_TOOL_H_
#define _FL_TOOL_H_

#include <string>
#include <vector>
#include "fu_common_def.h"

class FuTool
{
public:
    /*
    查询某个固定文件夹下的某个文件的全路径
    */
    static std::string GetFileFullPathFromeSearchPath(const char * name, const char * InFolderName = "res");
    
    static Bitmap * getBitmapFromFile(std::string fullpath);
    
    static std::vector<std::string> getVideoDevices();
    
    static bool LoadBundle(const std::string& filepath, std::vector<char>& data);
    
public:
    static std::vector<std::string> m_searchPath;
};

#endif

