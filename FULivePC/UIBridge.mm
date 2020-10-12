#import <Foundation/Foundation.h>
#include "UIBridge.h"
#include "fu_tool.h"
#include <vector>
#include "fu_tool_mac.h"
#include "Config.h"
/*
 将外部传递的相对路径组合成Resource.bundle的绝对路径，再找到下面所有的文件
 */
void UIBridge::FindAllBundle(string folder,vector<string> &files)
{
    string fullFolder = FuTool::GetFileFullPathFromeSearchPath(folder.c_str());
    NSFileManager *manager = [NSFileManager defaultManager];

    NSString *home = [NSString stringWithUTF8String:fullFolder.data()]; //see if UTF8

    NSDirectoryEnumerator *direnum = [manager enumeratorAtPath:home];
    NSString *filename;
    while (filename = [direnum nextObject]) {
        if ([[filename pathExtension] isEqualTo:@"bundle"]) {
            //NSString *path = [NSString stringWithFormat:@"%@%@%@",home,@"/",filename];
           
            files.push_back([filename UTF8String]);
        }
    }
	sort(files.begin(), files.end());
}

void UIBridge::FindAllCommonPIC(string folder, vector<string> &files)
{
    FuTool::getFiles(folder, files, ".jpg");

    FuTool::getFiles(folder, files, ".JPG");

    FuTool::getFiles(folder, files, ".png");

    FuTool::getFiles(folder, files, ".mp4");

    FuTool::getFiles(folder, files, ".mov");
}
/// 从macos app bundle 下的 Resources/Resource.bundle 中的指定文件夹下获取图片或者视频的资源全路径
/// @param folder  Resources/Resource.bundle中的文件夹
/// @param files 接收图片或视频资源全路径的数组
void UIBridge::FindAllCommonPICFromResourceBundle(string folder, vector<string> &files){
	string fullDirPath = FuToolMac::GetFileFullPathFromResourceBundle(folder.c_str());
    FuTool::getFiles(fullDirPath, files, ".jpg");

    FuTool::getFiles(fullDirPath, files, ".JPG");

    FuTool::getFiles(fullDirPath, files, ".png");

    FuTool::getFiles(fullDirPath, files, ".mp4");

    FuTool::getFiles(fullDirPath, files, ".mov");
}


/// 从macos app bundle 下的 Resources/Resource.bundle 中的指定文件夹下获取图片或者视频的资源名称
/// @param folder  Resources/Resource.bundle中的文件夹
/// @param names 接收图片或视频资源名称的数组
void UIBridge::FindAllCommonPICNameFromResourceBundle(string folder, vector<string> &names){
	string fullDirPath = FuToolMac::GetFileFullPathFromResourceBundle(folder.c_str());
    FuTool::getFileNames(fullDirPath, names, ".jpg");

    FuTool::getFileNames(fullDirPath, names, ".JPG");

    FuTool::getFileNames(fullDirPath, names, ".png");

    FuTool::getFileNames(fullDirPath, names, ".mp4");

    FuTool::getFileNames(fullDirPath, names, ".mov");
	sort(names.begin(), names.end());
}


/// 根据相对于 macos app bundle 下的 Resources/Resource.bundle 下的相对路径，返回文件的全路径
/// @param path 相对于 macos app bundle 下的 Resources/Resource.bundle 下的相对路径
string UIBridge::GetFileFullPathFromResourceBundle(const char * path)
{
	return FuToolMac::GetFileFullPathFromResourceBundle(path);
}

