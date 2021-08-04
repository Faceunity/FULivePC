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

#include "Config.h"

enum eStickerFlagAdapter
{
	ADAPTER_FLAG_SINGLE_PEOPLE = 0x01,
	ADAPTER_FLAG_MAKEUP_FLIP = 0x02,
	ADAPTER_FLAG_IS_NEED_CLICK = 0x04,
	ADAPTER_FLAG_IS_NEED_3DFLIP = 0x08,
};

class BundleRes	//ItemRes
{
public:
	std::string mId;
	std::string mItemName;
	std::vector<std::string> mBundleNames;
	std::string mIconName;

	std::string mIconDir;
	std::vector<std::string> mBundleDirs;
	std::atomic_bool mBundleIsDownload;
	std::atomic_bool mIsDownloading;
	std::atomic_bool mHasNotBinded;

	std::string mIconUrl;
	int32_t mAdapterFlag = 0;
	std::string mCategory = "";


	std::string mConfigJson = "";
	ItemFlag mItemFlag;
	ItemCategory mCategory2;
	BodyTrackConfig mBodyTrackConfig;


	BundleRes(std::string strId, std::string itemName, std::vector<std::string> strBundleNames, std::string strIconName)
		: mId(strId)
		, mItemName(itemName)
		, mBundleNames(strBundleNames)
		, mIconName(strIconName)
	{

#ifdef __APPLE__

        mIconDir = FuToolMac::GetDocumentPath() + "/" + mIconName;
		for (auto& name : mBundleNames)
			mBundleDirs.emplace_back(FuToolMac::GetDocumentPath() + "/" + name);
        
#else
        std::string strDir = "bundleRes";
		mkdir(strDir.c_str());
		strDir += "/";
		strDir += strId;
		mkdir(strDir.c_str());
        
        mIconDir = strDir + "/" + mIconName;
		for (auto& name : mBundleNames)
			mBundleDirs.emplace_back(strDir + "/" + name);
        
#endif

		mBundleIsDownload = true;
		for (auto& dir : mBundleDirs) {
			FILE* pFile = fopen(dir.c_str(), "r");
			if (pFile)
				fclose(pFile);
			else
			{
				mBundleIsDownload = false;
				break;
			}
		}
		
		mIsDownloading = false;
		mHasNotBinded = false;
	};

	bool IsOnlyOnePeople() {
		return (mAdapterFlag & ADAPTER_FLAG_SINGLE_PEOPLE) != 0;
	}
	bool IsAvatar();
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
