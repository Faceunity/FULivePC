#include "fu_tool_mac.h"
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <AppKit/AppKit.h>
#include "fu_tool.h"
vector<string> FuToolMac::getVideoDevices()
{
    vector<string> vecRet;
    
    NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice * device in devices) {
        vecRet.push_back([[device localizedName] UTF8String]);
    }
    
    return vecRet;
}

#define MyPicBUNDLE_NAME @ "ResPic.bundle"
#define MyPicBUNDLE_PATH [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:MyPicBUNDLE_NAME]
#define MyPicBUNDLE [NSBundle bundleWithPath: MyPicBUNDLE_PATH]

string FuToolMac::GetCurrentAppPath()
{
    NSString* path = @"";
    NSString* str_app_full_file_name = [[NSBundle mainBundle] bundlePath];

    NSRange range = [str_app_full_file_name rangeOfString:@"/" options:NSBackwardsSearch];

    if (range.location != NSNotFound)
    {
        path = [str_app_full_file_name substringToIndex:range.location];
        path = [path stringByAppendingFormat:@"%@",@"/"];

    }

    return [path UTF8String];
}

string FuToolMac::Convert2utf8(const char * path)
{
   NSString * nstr =  [NSString stringWithUTF8String:path];
   string str = [nstr UTF8String];
   return str;
}

/*
  这里的filepath是ResPic.bundle下的相对路径
 */
string FuToolMac::GetFileFullPathFromResPicBundle(const char * path)
{
    string strFullPath = "";
    
    NSString * nstrFullPath = [[MyPicBUNDLE resourcePath] stringByAppendingPathComponent: [NSString stringWithUTF8String:path]];
    strFullPath = [nstrFullPath UTF8String];
    
    return strFullPath;
}
Bitmap * FuToolMac::getBitmapFromAsset(string name)
{
    Bitmap * pBmp = NULL;
    
    do {
        NSString * picPath = [NSString stringWithUTF8String:name.c_str()];
        NSString * picBundlePath =  [[MyPicBUNDLE resourcePath] stringByAppendingPathComponent: picPath];
        NSImage * img = [[NSImage alloc] initWithContentsOfFile: picBundlePath];
        if (!img) {
            break;
        }
        
        NSData * tiffData = [img TIFFRepresentation];
        NSBitmapImageRep * bitmap;
        bitmap = [NSBitmapImageRep imageRepWithData:tiffData];
        NSColorSpace * colorSpace = [bitmap colorSpace];
        if ([colorSpace colorSpaceModel] != NSColorSpaceModelRGB) { //正常情况下assets里面读出来的都是RGBA的
            break;
        }
        
        NSInteger iHeight = [bitmap pixelsHigh];
        NSInteger iWide = [bitmap pixelsWide];
        NSInteger ibitPerPixel = [bitmap bitsPerPixel];
        NSBitmapFormat format = [bitmap bitmapFormat]; //大小端还有阿尔法的位置
        if(ibitPerPixel != 32 && ibitPerPixel != 24) //既不是RGBA也不是RGB 稍微检查下，基本上不会出问题，毕竟是自己素材的
        {
            break;
        }
        
        pBmp = new Bitmap;
        pBmp->mpData = new uint8_t[iHeight * iWide * 4];
        pBmp->mHeight = iHeight;
        pBmp->mWidth = iWide;
        unsigned char * pRawData = [bitmap bitmapData];
        
        switch (ibitPerPixel) {
            case 24:
            {
                //无阿尔法通道的，把通道数值设置成不透明
                memset(pBmp->mpData, 0xff, iHeight * iWide * 4);
                for (int i = 0; i < iHeight; i++) {
                    for (int j = 0; j < iWide; j++) {
                        memcpy(pBmp->mpData + i*iWide*4 + j*4, pRawData + i*iWide*3 + j*3, 3);
                    }
                }
            }
                break;
            case 32:
            {
                memcpy(pBmp->mpData, pRawData, iHeight * iWide * 4);
            }
                break;
            default:
                break;
        }
        
        
    } while (0);
    
    return pBmp;
}

#define MyLibBUNDLE_NAME @ "Resource.bundle"
#define MyLibBUNDLE_PATH [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:MyLibBUNDLE_NAME]
#define MyLibBUNDLE [NSBundle bundleWithPath: MyLibBUNDLE_PATH]


/*
  这里的filepath是Resource.bundle下的相对路径
 */
string FuToolMac::GetFileFullPathFromResourceBundle(const char * path)
{
    string strFullPath = "";
    
    NSString * nstrFullPath = [[MyLibBUNDLE resourcePath] stringByAppendingPathComponent: [NSString stringWithUTF8String:path]];
    strFullPath = [nstrFullPath UTF8String];
    
    return strFullPath;
}

/// 获取相对于 MyLibBUNDLE_PATH 的 相对路径
/// @param path 绝对路径
string FuToolMac::GetRelativePathDependResourceBundle(const char * fullpath)
{
	string dependpath = MyLibBUNDLE_PATH.UTF8String;
	return FuTool::GetRelativePath(fullpath, dependpath);
}



//#import "FUObject-C-Interface.h"
/// 使用OC方法，来选取需要的文件，相对于Qt方法更加稳定
/// @param dirPath 将要选取文件的文件夹路径
/// @param types 文件类型 “bundle”等的集合
/// @param seletedFilesPathsPtr 已经选取的文件路径数组指针
/// @param allowsMultipleSelection 是否允许多选
/// @return true 表示选取文件成功，false 表示选取文件失败
bool FuToolMac::importFilesInObjectC(const char *dirPath,vector<const char *> types,vector<const char *> *seletedFilesPathsPtr,bool allowsMultipleSelection) {
	// Get the main window for the document.
	
	// Create and configure the panel.
	NSOpenPanel* panel = [NSOpenPanel openPanel];
	[panel setCanChooseDirectories:NO];
	[panel setAllowsMultipleSelection:allowsMultipleSelection];
	[panel setMessage:@"Import one or more files or directories."];
	NSMutableArray *typeArr = [NSMutableArray array];
	for(std::vector<const char *>::iterator it = types.begin(); it != types.end(); ++it) {
		[typeArr addObject:[NSString stringWithUTF8String:*it]];
	}
	panel.allowedFileTypes = typeArr;
	//   Display the panel attached to the document's window.
	NSString * dirPath_Str = [NSString stringWithUTF8String:dirPath];
	panel.directoryURL = [NSURL URLWithString: dirPath_Str];
	if([panel runModal] == NSOKButton){
		NSArray* urls = [panel URLs];
		[urls enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
			NSURL * selectedURL = obj;
			NSString * selectedFile_Str = selectedURL.path;
			NSString * selectedFile_Str_deal = [selectedFile_Str stringByReplacingOccurrencesOfString:@"file://" withString:@""];  // stringByReplacingOccurrencesOfString:@"%20" withString:@" "];
			//vector<const char *> seletedFilesPaths = *seletedFilesPathsPtr;
			const char* selectedFile_Str_char = [selectedFile_Str_deal UTF8String];
			(*seletedFilesPathsPtr).push_back(selectedFile_Str_char);
		}];
		return true;
	}else{
		return false;
	}
}


/// 计算给定字符串的尺寸
/// @param string 传入的字符数
/// @param fontSize 字体大小
FUCGSize FuToolMac::culculatorTextSize(const char *string,float fontSize) { 
	NSString * ocString = [NSString stringWithUTF8String:string];
	NSFont *font = [NSFont systemFontOfSize:fontSize];
	CGSize ocSize = [ocString boundingRectWithSize:CGSizeMake(MAXFLOAT, MAXFLOAT) options:(NSStringDrawingUsesLineFragmentOrigin|NSStringDrawingUsesFontLeading) attributes: @{NSFontAttributeName:font} context:nil].size;
	FUCGSize fuCGSize = {ocSize.width,ocSize.height};
	return fuCGSize;
}
float FuToolMac::culculatorTextWidth(const char *string,float fontSize) { 
	return culculatorTextSize(string, fontSize).width;
}




