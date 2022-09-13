#ifndef _GUI_GM_H_
#define _GUI_GM_H_

#include "GuiTool.h"
#include "Nama.h"
#include "Texture.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "fu_shared_obj.h"
#include "Nama.h"
#include <memory>

using namespace NamaExampleNameSpace;
using namespace gui_tool;


typedef struct tagEyeBrowType
{
	int32_t iType;
	std::string strIconPath;
	std::string strName;
}EyeBrowType;

typedef struct tagCMEyeBrowConfig
{
	std::string strBundlePath;
	std::vector<EyeBrowType> vecType;
	std::vector<std::shared_ptr<gui_tool::ColorBag> > vecColor;
}CMEyeBrowConfig;

typedef struct tagLipType
{
	std::string strBundlePath;
	int32_t iType;
	std::string strIconPath;
	std::string strName;
}LipType;

typedef struct tagCMLipConfig
{
	std::vector<LipType> vecType;
	std::vector<std::shared_ptr<gui_tool::ColorBag> > vecColor;
}CMLipConfig;

//Color Bag有多重颜色的情况，要支持双色，三色的显示方式
typedef struct tagCMNormalConfig
{
	std::string strBundlePath;
	std::string strIconPath;
	std::string strName;
	std::vector<std::shared_ptr<gui_tool::ColorBag> > vecColor;

}CMNormalConfig;


class CMBasePage:public ISharedObject
{
	Declare_FUObject(CMBasePage, ISharedObject);
public:
	
	CMBasePage();
	virtual ~CMBasePage();

	virtual void SetNama(Nama * pNama)
	{
		m_pNama = pNama;
	}

	virtual Nama * GetNama()
	{
		return m_pNama;
	}

	virtual bool InitFrom(const rapidjson::Value & json) = 0;
	virtual void ShowUI() = 0;
	virtual std::string GetDescName() const = 0;
	virtual void Reset() = 0;
	virtual void ShowIntensityUI() = 0;
	virtual void SetIntensity() = 0;

protected:
	Nama * m_pNama = nullptr;
	float m_fIntensity = 100.0f;
};

class CMEyeBrow:public CMBasePage
{
	Declare_FUObject(CMEyeBrow, CMBasePage);
public:
	CMEyeBrow();
	~CMEyeBrow();

	virtual bool InitFrom(const rapidjson::Value & json) override;
	virtual void ShowUI() override;
	virtual std::string GetDescName() const override
	{
		return "眉毛";
	}

	virtual void Reset() override;
	virtual void ShowIntensityUI() override;
	virtual void SetIntensity() override;

private:
	std::vector<std::shared_ptr<CMEyeBrowConfig> > m_vecData;
	int m_iCurType = -1;
	std::shared_ptr<ColorBag> m_curBag = nullptr;
};

typedef std::function<bool(std::shared_ptr<ColorBag> choicebag, void * pUserData)>  FunCBShowUI;

class CMNormalNode
{
public:

	CMNormalNode();
	virtual ~CMNormalNode();

	virtual void ShowUINormal(FunCBShowUI funCB, Nama * pNama, void * pUserData);
	virtual void ResetNormal();

protected:

	std::vector< std::shared_ptr<CMNormalConfig> > m_vecData;
	std::shared_ptr<CMNormalConfig> m_curConfig = nullptr;
	std::shared_ptr<ColorBag> m_curBag = nullptr;

};

class CMEyeShadow :public CMBasePage, public CMNormalNode
{
	Declare_FUObject(CMEyeShadow, CMBasePage);
public:
	CMEyeShadow();
	~CMEyeShadow();

	virtual bool InitFrom(const rapidjson::Value & json) override;
	virtual void ShowUI() override;
	virtual std::string GetDescName() const override
	{
		return "眼影";
	}

	virtual void Reset() override;
	virtual void ShowIntensityUI() override;
	virtual void SetIntensity() override;

private:
	
};

class CMEyeLash :public CMBasePage, public CMNormalNode
{
	Declare_FUObject(CMEyeLash, CMBasePage);
public:
	CMEyeLash();
	~CMEyeLash();

	virtual bool InitFrom(const rapidjson::Value & json) override;
	virtual void ShowUI() override;
	virtual std::string GetDescName() const override
	{
		return "睫毛";
	}

	virtual void Reset() override;
	virtual void ShowIntensityUI() override;
	virtual void SetIntensity() override;

private:

};

class CMEyeLiner :public CMBasePage, public CMNormalNode
{
	Declare_FUObject(CMEyeLiner, CMBasePage);
public:
	CMEyeLiner();
	~CMEyeLiner();

	virtual bool InitFrom(const rapidjson::Value & json) override;
	virtual void ShowUI() override;
	virtual std::string GetDescName() const override
	{
		return "眼线";
	}

	virtual void Reset() override;
	virtual void ShowIntensityUI() override;
	virtual void SetIntensity() override;

private:

};

class CMFoundation :public CMBasePage, public CMNormalNode
{
	Declare_FUObject(CMFoundation, CMBasePage);
public:
	CMFoundation();
	~CMFoundation();

	virtual bool InitFrom(const rapidjson::Value & json) override;
	virtual void ShowUI() override;
	virtual std::string GetDescName() const override
	{
		return "粉底";
	}
	
	virtual void Reset() override;
	virtual void ShowIntensityUI() override;
	virtual void SetIntensity() override;

private:

};

class CMHighLight :public CMBasePage, public CMNormalNode
{
	Declare_FUObject(CMHighLight, CMBasePage);
public:
	CMHighLight();
	~CMHighLight();

	virtual bool InitFrom(const rapidjson::Value & json) override;
	virtual void ShowUI() override;
	virtual std::string GetDescName() const override
	{
		return "高光";
	}

	virtual void Reset() override;
	virtual void ShowIntensityUI() override;
	virtual void SetIntensity() override;

private:

};


class CMBlush :public CMBasePage, public CMNormalNode
{
	Declare_FUObject(CMBlush, CMBasePage);
public:
	CMBlush();
	~CMBlush();

	virtual bool InitFrom(const rapidjson::Value & json) override;
	virtual void ShowUI() override;
	virtual std::string GetDescName() const override
	{
		return "腮红";
	}

	virtual void Reset() override;
	virtual void ShowIntensityUI() override;
	virtual void SetIntensity() override;

private:

};

class CMContour :public CMBasePage, public CMNormalNode
{
	Declare_FUObject(CMContour, CMBasePage);
public:
	CMContour();
	~CMContour();

	virtual bool InitFrom(const rapidjson::Value & json) override;
	virtual void ShowUI() override;
	virtual std::string GetDescName() const override
	{
		return "阴影";
	}

	virtual void Reset() override;
	virtual void ShowIntensityUI() override;
	virtual void SetIntensity() override;

private:

};

class CMLip :public CMBasePage
{
	Declare_FUObject(CMLip, CMBasePage);
public:
	CMLip();
	~CMLip();

	virtual bool InitFrom(const rapidjson::Value & json) override;
	virtual void ShowUI() override;
	virtual std::string GetDescName() const override
	{
		return "口红";
	}

	virtual void Reset() override;
	virtual void ShowIntensityUI() override;
	virtual void SetIntensity() override;

private:
	std::vector<std::shared_ptr<CMLipConfig> > m_vecData;
	int m_iCurType = -1;
	std::shared_ptr<ColorBag> m_curBag = nullptr;
};


class CMEyePupil :public CMBasePage, public CMNormalNode
{
	Declare_FUObject(CMEyePupil, CMBasePage);
public:
	CMEyePupil();
	~CMEyePupil();

	virtual bool InitFrom(const rapidjson::Value & json) override;
	virtual void ShowUI() override;
	virtual std::string GetDescName() const override
	{
		return "美瞳";
	}

	virtual void Reset() override;
	virtual void ShowIntensityUI() override;
	virtual void SetIntensity() override;

private:

};


class GUICustomMakeup
{
public:
	GUICustomMakeup();
	~GUICustomMakeup();

	static bool InitFromConfig(Nama * pNama);
	static void Draw(Nama * pNama);
	static void Reset(Nama * pNama);

private:

private:
	static std::map<std::string, std::shared_ptr<CMBasePage> > m_pages;
	static std::shared_ptr<CMBasePage> m_curChoosed;
	static bool m_bInited;
};

#endif
