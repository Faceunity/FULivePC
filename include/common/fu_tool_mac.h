#ifndef _FL_TOOL_MAC_H_
#define _FL_TOOL_MAC_H_

#include <string>
#include <vector>
#include "fu_common_def.h"

class FuToolMac
{
public:
    static std::vector<std::string> getVideoDevices();
    
    static Bitmap * getBitmapFromAsset(std::string name);
    
    static std::string GetFileFullPathFromBundle(const char * path);
};

#endif
