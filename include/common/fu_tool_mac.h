#ifndef _FL_TOOL_MAC_H_
#define _FL_TOOL_MAC_H_

#include <string>
#include <vector>
#include "fu_common_def.h"
using namespace std;
struct FUCGSize {
    float width;
    float height;
};
class FuToolMac
{
public:
    static vector<string> getVideoDevices();
	static string GetFileFullPathFromResPicBundle(const char * path);
    static Bitmap * getBitmapFromAsset(string name);
    
    static string GetFileFullPathFromResourceBundle(const char * path);
	static string GetRelativePathDependResourceBundle(const char * fullpath);
	static bool importFilesInObjectC(const char *dirPath,vector<const char *> types,vector<const char *> *seletedFilesPathsPtr,bool allowsMultipleSelection);
    static float culculatorTextWidth(const char *string,float fontSize);
    static string Convert2utf8(const char * path);
    
    static string GetCurrentAppPath();
private:
	static FUCGSize culculatorTextSize(const char *string,float fontSize);
	
};

#endif
