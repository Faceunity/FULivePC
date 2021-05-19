#ifndef _FL_TOOL_MAC_H_
#define _FL_TOOL_MAC_H_

#include <string>
#include <vector>
#include <functional>
#include "fu_common_def.h"

using namespace std;
struct FUCGSize {
    float width;
    float height;
};
class FuToolMac
{
public:
    static void addLockScreenEventObser(std::function<void()> f);
    static void addUnLockScreenEventObser(std::function<void()> f);
    // 检测相机权限，如果不存在，则获取一次，并返回结果
    static bool granteCameraAccess();
    // 提示用户打开相机权限
    static void tipToOpenPrivacyPanel();
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
