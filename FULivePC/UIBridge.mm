#import <Foundation/Foundation.h>
#include "UIBridge.h"
#include "fu_tool.h"
#include <vector>

/*
 将外部传递的相对路径组合成Resource.bundle的绝对路径，再找到下面所有的文件
 */
void UIBridge::FindAllBundle(std::string folder,std::vector<std::string> &files)
{
    std::string fullFolder = FuTool::GetFileFullPathFromeSearchPath(folder.c_str());
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
}


