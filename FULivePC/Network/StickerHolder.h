#ifndef _STICKER_HOLDER_H__
#define _STICKER_HOLDER_H__
#include <string>
#include <vector>
#include <curl/curl.h>
#include <atomic>
#include <memory>

#ifdef __APPLE__
#include<sys/stat.h>
#include<sys/types.h>
#include <unistd.h>
#include "fu_tool_mac.h"
#else
#include <direct.h>
#endif

enum eStickerFlagAdapter
{
	ADAPTER_FLAG_SINGLE_PEOPLE = 0x01,
	ADAPTER_FLAG_MAKEUP_FLIP = 0x02,
	ADAPTER_FLAG_IS_NEED_CLICK = 0x04,
	ADAPTER_FLAG_IS_NEED_3DFLIP = 0x08,
};

class BundleRes
{
public:
	BundleRes(std::string strId, std::string strBundleName, std::string strIconName, std::string strIconUrl)
		: mId(strId)
		, mBundleName(strBundleName)
		, mIconName(strIconName)
		, mIconUrl(strIconUrl)
	{
		std::string strDir = "bundleRes";

#ifdef __APPLE__
		mkdir(strDir.c_str(),0777);
		strDir += "/";
		strDir += strId;
		mkdir(strDir.c_str(),0777);
#else
		mkdir(strDir.c_str());
		strDir += "/";
		strDir += strId;
		mkdir(strDir.c_str());
#endif

		mIconDir = strDir + "/" + mIconName; 
		mBundleDir = strDir + "/" + mBundleName;

		std::string strPath;
#ifdef __APPLE__
		strPath = FuToolMac::GetCurrentAppPath() + "//" + strPath;
#endif
		strPath += mBundleDir;
		FILE* pFile = fopen(strPath.c_str(), "r");
		if (pFile)
		{
			mBundleIsDownload = true;
			fclose(pFile);
		}
		else
		{
			mBundleIsDownload = false;
		}
		
		mIsDownloading = false;
		mHasNotBinded = false;
	};
	~BundleRes() 
	{

	};

	bool IsOnlyOnePeople() {

		return (mAdapterFlag & ADAPTER_FLAG_SINGLE_PEOPLE) != 0;

	}

	std::string mId;
	std::string mBundleName;

	std::string mIconName;
	std::string mIconUrl;

	std::string mIconDir;
	std::string mBundleDir;

	int32_t mAdapterFlag = 0;

	std::atomic_bool mBundleIsDownload;
	std::atomic_bool mIsDownloading;

	std::atomic_bool mHasNotBinded;
};

class StikcerHolder
{
public:
	StikcerHolder();
	virtual ~StikcerHolder();

	void RequestTags();

	std::string RequestBundleUrl(std::string strId);

	void RequestTools();


	bool Post(const std::string& strHost, const std::string& strMethod, const std::string& strArgument, std::string& strResponse);

	bool Get(const std::string& strHost, const std::string& strMethod, const std::string& strArgument, std::string& strResponse);

	static int WriteCallback(void* pBuffer, size_t nSize, size_t nMemByte, FILE* hFile);

	bool DownLoadFile(std::string strUrl, std::string strPath);
private:
	CURL* mCurl = nullptr;

public:
	std::vector<std::string> mTags;
	std::vector<std::vector<std::shared_ptr<BundleRes>>> mTagBundleList;
};




#endif // _STICKER_HOLDER_H__
