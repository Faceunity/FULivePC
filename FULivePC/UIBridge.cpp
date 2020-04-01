#include "UIBridge.h"
#include <vector>
#ifdef _WIN32
#include <windows.h>
#include "limits.h"
#endif // _WIN32


static void Wchar_tToString(std::string& szDst, wchar_t *wchar)
{
    wchar_t * wText = wchar;
    DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用
    char *psText; // psText为char*的临时数组，作为赋值给std::string的中间变量
    psText = new char[dwNum];
    WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte的再次运用
    szDst = psText;// std::string赋值
    delete[]psText;// psText的清除
}

static void IteratorFolder(const char* lpPath, std::vector<std::string> &fileList) {
    char szFind[MAX_PATH];
    WIN32_FIND_DATA FindFileData;
    strcpy(szFind, lpPath);
    strcat(szFind, "\\*.*");
    int len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)szFind, -1, NULL, 0);
    wchar_t * wszUtf8 = new wchar_t[len + 1];
    memset(wszUtf8, 0, len * 2 + 2);
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)szFind, -1, (LPWSTR)wszUtf8, len);
    HANDLE hFind = ::FindFirstFileW(wszUtf8, &FindFileData);
    if (INVALID_HANDLE_VALUE == hFind)    return;
    while (true) {
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (FindFileData.cFileName[0] != '.')
            {
                std::string folderName;
                Wchar_tToString(folderName, FindFileData.cFileName);
                fileList.push_back(folderName);
            }
        }
        else
        {
            //std::cout << FindFileData.cFileName << std::endl;
            std::string str;
            Wchar_tToString(str, FindFileData.cFileName);
            if (str.find(".bundle")!= std::string::npos)
            {
                fileList.push_back(str);
            }
        }
        if (!FindNextFile(hFind, &FindFileData))    break;
    }
    FindClose(hFind);
}

void UIBridge::FindAllBundle(std::string folder,std::vector<std::string> &files)
{
    IteratorFolder(folder.c_str(),files);
}


