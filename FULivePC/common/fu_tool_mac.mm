#include "fu_tool_mac.h"
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <AppKit/AppKit.h>

std::vector<std::string> FuToolMac::getVideoDevices()
{
    std::vector<std::string> vecRet;
    
    NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice * device in devices) {
        vecRet.push_back([[device localizedName] UTF8String]);
    }
    
    return vecRet;
}

Bitmap * FuToolMac::getBitmapFromAsset(std::string name)
{
    Bitmap * pBmp = NULL;
    
    do {
        NSString * picPath = [NSString stringWithUTF8String:name.c_str()];
        NSString * picName = [picPath stringByDeletingPathExtension];
        NSImage * img = [NSImage imageNamed:picName]; //这个不能加后缀，否则打不开
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
  这里的filepath是Resource下的相对路径
 */
std::string FuToolMac::GetFileFullPathFromBundle(const char * path)
{
    std::string strFullPath = "";
    
    NSString * nstrFullPath = [[MyLibBUNDLE resourcePath] stringByAppendingPathComponent: [NSString stringWithUTF8String:path]];
    strFullPath = [nstrFullPath UTF8String];
    
    return strFullPath;
}

