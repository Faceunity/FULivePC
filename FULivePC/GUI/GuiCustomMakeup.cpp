#include "GuiCustomMakeup.h"
#include <iostream>
#include "UIBridge.h"
#include "fu_tool.h"

using namespace std;

extern float scaleRatioW;
extern float scaleRatioH;

static const ImVec4 normalColor{ 1.0f, 1.0f, 1.0f, 1.0f };
static const ImVec4 hoverColor{ 119.f / 255.f ,135.f / 255.f ,233.f / 255.f, 1.0f };
static const ImVec4 disableColor{ 149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.0f };

Implement_Abstract_FUObject(CMBasePage);

CMBasePage::CMBasePage()
{
}

CMBasePage::~CMBasePage()
{
}

////////////////////////////////////////////////////


static bool initNormal(const rapidjson::Value& json,
	std::vector< std::shared_ptr<CMNormalConfig> >& data)
{
	bool bRet = false;

	bool Flag1 = false, Flag2 = false, Flag3 = false;

	if (json.IsArray())
	{
		for (int i = 0; i < json.Size(); ++i) {

			auto config = std::make_shared<CMNormalConfig>();
			const auto& dictData = json[i];

			if (dictData.HasMember("bundle"))
			{
				config->strBundlePath = dictData["bundle"].GetString();
				Flag1 = true;
			}

			if (dictData.HasMember("icon"))
			{
				config->strIconPath = dictData["icon"].GetString();
				Flag2 = true;
			}

			if (dictData.HasMember("name"))
			{
				config->strName = dictData["name"].GetString();
			}
			else
			{
				config->strName = "";
			}

			if (dictData.HasMember("colors") && dictData["colors"].IsArray())
			{
				const auto& array = dictData["colors"];
				for (int j = 0; j < array.Size(); j++)
				{
					//这个不定长可以能1组2组3组
					if (array[j].IsArray() && array[j].Size() >= 4)
					{
						cv::Vec4b color[3];
						bool flag[3] = { false };

						for (int m = 0; m < array[j].Size(); m += 4)
						{
							for (int k = m, z = 0; z < 4; k++, z++)
							{
								int v = (255 * array[j][k].GetFloat());
								color[m / 4][z] = v % 256;
								flag[m / 4] = true;
							}
						}

						std::shared_ptr<ColorBag> bag = nullptr;

						if (flag[2] == true)
						{
							bag = std::make_shared<ColorBag>(color[0], color[1], color[2]);
						}
						else if (flag[1] == true)
						{
							bag = std::make_shared<ColorBag>(color[0], color[1]);
						}
						else if (flag[0] == true)
						{
							bag = std::make_shared<ColorBag>(color[0]);
						}

						config->vecColor.push_back(bag);

						Flag3 = true;
					}
				}
			}

			data.push_back(config);

		}
	}

	bRet = Flag1 && Flag3;
	return bRet;
}

CMNormalNode::CMNormalNode()
{


}

CMNormalNode::~CMNormalNode()
{

}

void CMNormalNode::ShowUINormal(FunCBShowUI funCB, Nama* pNamaA, void* pUserData)
{
	CMBasePage* pPage = (CMBasePage*)pUserData;
	Nama* pNama = pPage->GetNama();
	string strDecKey = pPage->GetDescName();

	ImGui::BeginGroup();

	ImGui::Dummy(ImVec2(1, 20 * scaleRatioH));
	ImGui::Dummy(ImVec2(20 * scaleRatioW, 1)); ImGui::SameLine();
	bool newlineflag = false;
	for (size_t i = 0; i < m_vecData.size(); i++)
	{
		ImGui::PushID(m_vecData[i].get());


		static ImVec4 currentColor;

		if (m_curConfig == m_vecData[i]) {
			// 已选中，显示高亮
			currentColor = hoverColor;
		}
		else {
			// 未选中，默认
			currentColor = normalColor;
		}

		ImGui::PushStyleColor(ImGuiCol_Button, currentColor);

		if (LayoutRectImageButtonWithText(ImVec2(6.f, 27.f), ImVec2(76, 76), Texture::createTextureFromFile(m_vecData[i]->strIconPath, false)->getTextureID(), m_vecData[i]->strName.data()))
		{
			if (pPage->m_fIntensity == 0.0f) {
				pPage->m_fIntensity = 100.f;
			}
			pNama->SelectCustomMakeupBundle(gBundlePath[UIBridge::bundleCategory] + "/subs/" + m_vecData[i]->strBundlePath + ".bundle", strDecKey);
			pPage->SetIntensity();
			m_curConfig = m_vecData[i];
			m_curBag = nullptr; //需要重新选择颜色
		}

		ImGui::PopStyleColor();
		if (i >=4 ) {
			newlineflag = true;
		}
		if (i == 3) {
			ImGui::Dummy(ImVec2(1, 20 * scaleRatioH));
			ImGui::Dummy(ImVec2(20 * scaleRatioW, 1)); ImGui::SameLine();
		}
		else {
			ImGui::SameLine(0.f, 15.f * scaleRatioW);
		}

		ImGui::PopID();
	}

	if (newlineflag) {
		ImGui::NewLine();
		ImGui::Dummy(ImVec2(1, 20 * scaleRatioH));
		ImGui::Dummy(ImVec2(20 * scaleRatioW, 1)); ImGui::SameLine();
	}
	else {
		ImGui::Dummy(ImVec2(1, 1));
		ImGui::Dummy(ImVec2(20 * scaleRatioW, 20 * scaleRatioH)); ImGui::SameLine();
	}


	if (m_curConfig)
	{
		for (size_t i = 0; i < m_curConfig->vecColor.size(); i++)
		{
			auto bag = m_curConfig->vecColor[i];

			ImGui::PushID(bag.get());

			static ImVec4 currentCBColor;

			if (m_curBag == bag) {
				// 已选中，显示高亮
				currentCBColor = hoverColor;
			}
			else {
				// 未选中，默认
				currentCBColor = normalColor;
			}
			ImGui::PushStyleColor(ImGuiCol_Button, currentCBColor);

			if (LayoutImageButtonWithText(ImVec2(0.f, 27.f), ImVec2(40, 40), bag->pTex->getTextureID(),
				bag->pTex->getTextureID(), ""))
			{
				funCB(bag, pUserData);
				m_curBag = bag;
			}

			ImGui::PopStyleColor();

			ImGui::SameLine(0, 24 * scaleRatioW);

			ImGui::PopID();
		}

		//未选择就默认赋值
		if (!m_curBag && m_curConfig->vecColor.size() > 0)
		{
			funCB(m_curConfig->vecColor[0], pUserData);
			m_curBag = m_curConfig->vecColor[0];
		}
	}


	ImGui::Dummy(ImVec2(0, 0));
	ImGui::Dummy(ImVec2(40 * scaleRatioW, 0));
	ImGui::SameLine();

	ImGui::SetCursorPosY(610 * scaleRatioH);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 30);
	if (m_curBag == nullptr) {
		if (m_curConfig == nullptr) {
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		}
		else {
			if (m_curConfig->vecColor.size() > 0) {
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			}
		}
	}
	pPage->ShowIntensityUI();
	if (m_curBag == nullptr) {
		if (m_curConfig == nullptr) {
			ImGui::PopItemFlag();
		}
		else {
			if (m_curConfig->vecColor.size() > 0) {
				ImGui::PopItemFlag();
			}
		}
	}

	ImGui::EndGroup();
}

void CMNormalNode::ResetNormal()
{
	m_curBag = nullptr;
	m_curConfig = nullptr;
}

////////////////////////////////////////////////////

////////////////////////////////////////////////////

static std::vector<double> GetDoubles(cv::Vec4b & cvC)
{
	std::vector<double> ret;
	ret.push_back((double)cvC[0] / 255.0f);
	ret.push_back((double)cvC[1] / 255.0f);
	ret.push_back((double)cvC[2] / 255.0f);
	ret.push_back((double)cvC[3] / 255.0f);

	return ret;
}

////////////////////////////////////////////////////

Implement_FUObject(CMEyeBrow);

CMEyeBrow::CMEyeBrow()
{
}

CMEyeBrow::~CMEyeBrow()
{
}

bool CMEyeBrow::InitFrom(const rapidjson::Value & json)
{
	bool bRet = false;

	bRet = initNormal(json, m_vecData);

	return bRet;
}

/* 眉毛就一个配置 */
void CMEyeBrow::ShowUI()
{
	auto funCB = [](std::shared_ptr<ColorBag> choicebag, void* pUserData)-> bool {

		CMEyeLash* pThis = (CMEyeLash*)pUserData;

		Nama* pNama = pThis->GetNama();
		auto strType = pThis->GetDescName();
		std::vector<double> colors0 = GetDoubles(choicebag->vecColorRGBA[0]);
		pNama->SetCMDoubles("makeup_eyeBrow_color", colors0.data(), colors0.size());

		return true;
	};

	ShowUINormal(funCB, m_pNama, this);
}
void CMEyeBrow::Reset()
{
	CMNormalNode::ResetNormal();
}

void CMEyeBrow::ShowIntensityUI()
{
	char szPoints[1024] = { 0 };
	sprintf(szPoints, "%p", this);
	if (LayoutSlider(ImVec2(0, 22), ImVec2(320, 10), ("##slider221" + string(szPoints)).c_str(), ("##slidertext221" + std::string(szPoints)).c_str(), &m_fIntensity, 0, 100))
	{
		SetIntensity();
	}
}

void CMEyeBrow::SetIntensity()
{
	m_pNama->SetCMDouble("makeup_intensity_eyeBrow", m_fIntensity / 100);
}

Implement_FUObject(CMEyeShadow);

CMEyeShadow::CMEyeShadow()
{

}

CMEyeShadow::~CMEyeShadow()
{

}

bool CMEyeShadow::InitFrom(const rapidjson::Value & json)
{
	bool bRet = false;

	bRet = initNormal(json, m_vecData);

	return bRet;
}




void CMEyeShadow::ShowUI()
{
	auto funCB = [](std::shared_ptr<ColorBag> choicebag, void * pUserData)-> bool {
		
		CMEyeShadow * pThis = (CMEyeShadow*)pUserData;

		Nama * pNama = pThis->GetNama();
		
		auto strType = pThis->GetDescName();

		for (int i = 0; i < choicebag->vecColorRGBA.size(); i++)
		{
			std::vector<double> colors0 = GetDoubles(choicebag->vecColorRGBA[i]);

			char szName[1024] = { 0 };
			if (i > 0)
			{
				sprintf(szName, "makeup_eye_color%d", i + 1);
			}
			else
			{
				sprintf(szName, "makeup_eye_color");
			}
			

			pNama->SetCMDoubles(szName, colors0.data(), colors0.size());
		}
		
		return true;
	};

	ShowUINormal(funCB, m_pNama, this);
}

void CMEyeShadow::Reset()
{
	CMNormalNode::ResetNormal();
}

void CMEyeShadow::ShowIntensityUI()
{
	char szPoints[1024] = { 0 };
	sprintf(szPoints, "%p", this);
	if (LayoutSlider(ImVec2(0, 22), ImVec2(320, 10), ("##slider221" + string(szPoints)).c_str(), ("##slidertext221" + std::string(szPoints)).c_str(), &m_fIntensity, 0, 100))
	{
		SetIntensity();
	}
}

void CMEyeShadow::SetIntensity()
{
	m_pNama->SetCMDouble("makeup_intensity_eye", m_fIntensity / 100);
}

//////////////////////////////////////////////////////

Implement_FUObject(CMEyeLash);

CMEyeLash::CMEyeLash()
{

}

CMEyeLash::~CMEyeLash()
{

}

bool CMEyeLash::InitFrom(const rapidjson::Value & json)
{
	bool bRet = false;

	bRet = initNormal(json, m_vecData);

	return bRet;
}

void CMEyeLash::ShowUI()
{
	auto funCB = [](std::shared_ptr<ColorBag> choicebag, void * pUserData)-> bool {

		CMEyeLash * pThis = (CMEyeLash*)pUserData;

		Nama * pNama = pThis->GetNama();
		auto strType = pThis->GetDescName();
		std::vector<double> colors0 = GetDoubles(choicebag->vecColorRGBA[0]);
		pNama->SetCMDoubles("makeup_eyelash_color", colors0.data(), colors0.size());

		return true;
	};

	ShowUINormal(funCB, m_pNama, this);
}

void CMEyeLash::Reset()
{
	CMNormalNode::ResetNormal();
}

void CMEyeLash::ShowIntensityUI()
{
	char szPoints[1024] = { 0 };
	sprintf(szPoints, "%p", this);
	if (LayoutSlider(ImVec2(0, 22), ImVec2(320, 10), ("##slider221" + string(szPoints)).c_str(), ("##slidertext221" + std::string(szPoints)).c_str(), &m_fIntensity, 0, 100))
	{
		SetIntensity();
	}
}

void CMEyeLash::SetIntensity()
{
	m_pNama->SetCMDouble("makeup_intensity_eyelash", m_fIntensity / 100);
}

//////////////////////////////////////////////////////////

Implement_FUObject(CMEyeLiner);

CMEyeLiner::CMEyeLiner()
{

}

CMEyeLiner::~CMEyeLiner()
{

}

bool CMEyeLiner::InitFrom(const rapidjson::Value & json)
{
	bool bRet = false;

	bRet = initNormal(json, m_vecData);

	return bRet;
}

void CMEyeLiner::ShowUI()
{
	auto funCB = [](std::shared_ptr<ColorBag> choicebag, void * pUserData)-> bool {

		CMEyeLiner * pThis = (CMEyeLiner*)pUserData;

		Nama * pNama = pThis->GetNama();
		auto strType = pThis->GetDescName();
		std::vector<double> colors0 = GetDoubles(choicebag->vecColorRGBA[0]);
		pNama->SetCMDoubles("makeup_eyeLiner_color", colors0.data(), colors0.size());

		return true;
	};

	ShowUINormal(funCB, m_pNama, this);
}

void CMEyeLiner::Reset()
{
	CMNormalNode::ResetNormal();
}

void CMEyeLiner::ShowIntensityUI()
{
	char szPoints[1024] = { 0 };
	sprintf(szPoints, "%p", this);
	if (LayoutSlider(ImVec2(0, 22), ImVec2(320, 10), ("##slider221" + string(szPoints)).c_str(), ("##slidertext221" + std::string(szPoints)).c_str(), &m_fIntensity, 0, 100))
	{
		SetIntensity();
	}
}

void CMEyeLiner::SetIntensity()
{
	m_pNama->SetCMDouble("makeup_intensity_eyeLiner", m_fIntensity / 100);
}

//////////////////////////////////////////////////////////

Implement_FUObject(CMFoundation);

CMFoundation::CMFoundation()
{

}

CMFoundation::~CMFoundation()
{

}

bool CMFoundation::InitFrom(const rapidjson::Value & json)
{
	bool bRet = false;

	bRet = initNormal(json, m_vecData);

	return bRet;
}

void CMFoundation::ShowUI()
{
	auto config = m_vecData[0];

	ImGui::BeginGroup();

	ImGui::Dummy(ImVec2(1, 20 * scaleRatioH));
	ImGui::Dummy(ImVec2(20 * scaleRatioW, 1)); ImGui::SameLine();
	ImGui::Dummy(ImVec2(0, 0));
	ImGui::Dummy(ImVec2(20 * scaleRatioW, 1)); ImGui::SameLine();


	for (size_t i = 0; i < config->vecColor.size(); i++)
	{
		auto bag = config->vecColor[i];

		ImGui::PushID(bag.get());

		static ImVec4 currentCBColor;

		if (m_curBag == bag) {
			// 已选中，显示高亮
			currentCBColor = hoverColor;
		}
		else {
			// 未选中，默认
			currentCBColor = normalColor;
		}
		ImGui::PushStyleColor(ImGuiCol_Button, currentCBColor);

		if (LayoutImageButtonWithText(ImVec2(0.f, 27.f), ImVec2(40, 40), bag->pTex->getTextureID(),
			bag->pTex->getTextureID(), ""))
		{
			m_pNama->SelectCustomMakeupBundle(gBundlePath[UIBridge::bundleCategory] + "/subs/" + config->strBundlePath + ".bundle", GetDescName());
			std::vector<double> colors0 = GetDoubles(bag->vecColorRGBA[0]);
			m_pNama->SetCMDoubles("makeup_foundation_color", colors0.data(), colors0.size());
			SetIntensity();
			m_curBag = bag;
		}

		ImGui::PopStyleColor();

		ImGui::SameLine(0, 24 * scaleRatioW);

		ImGui::PopID();

		if (!m_curBag && config->vecColor.size() > 0)
		{
			if (m_fIntensity == 0.0f) {
				m_fIntensity = 100.f;
			}
			m_curBag = config->vecColor[0];
			m_pNama->SelectCustomMakeupBundle(gBundlePath[UIBridge::bundleCategory] + "/subs/" + config->strBundlePath + ".bundle", GetDescName());
			std::vector<double> colors0 = GetDoubles(m_curBag->vecColorRGBA[0]);
			m_pNama->SetCMDoubles("makeup_foundation_color", colors0.data(), colors0.size());
			SetIntensity();
		}
	}

	ImGui::Dummy(ImVec2(0, 0));
	ImGui::Dummy(ImVec2(40 * scaleRatioW, 0));
	ImGui::SameLine();

	ImGui::SetCursorPosY(610 * scaleRatioH);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 30);
	if (m_curBag == nullptr) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	}
	this->ShowIntensityUI();
	if (m_curBag == nullptr) {
		ImGui::PopItemFlag();
	}
	ImGui::EndGroup();

}

void CMFoundation::Reset()
{
	CMNormalNode::ResetNormal();
}

void CMFoundation::ShowIntensityUI()
{
	char szPoints[1024] = { 0 };
	sprintf(szPoints, "%p", this);
	if (LayoutSlider(ImVec2(0, 22), ImVec2(320, 10), ("##slider221" + string(szPoints)).c_str(), ("##slidertext221" + std::string(szPoints)).c_str(), &m_fIntensity, 0, 100))
	{
		SetIntensity();
	}
}

void CMFoundation::SetIntensity()
{
	m_pNama->SetCMDouble("makeup_intensity_foundation", m_fIntensity / 100);
}

//////////////////////////////////////////////////////////

Implement_FUObject(CMHighLight);

CMHighLight::CMHighLight()
{

}

CMHighLight::~CMHighLight()
{

}

bool CMHighLight::InitFrom(const rapidjson::Value & json)
{
	bool bRet = false;

	bRet = initNormal(json, m_vecData);

	return bRet;
}

void CMHighLight::ShowUI()
{
	auto funCB = [](std::shared_ptr<ColorBag> choicebag, void * pUserData)-> bool {

		CMHighLight * pThis = (CMHighLight*)pUserData;

		Nama * pNama = pThis->GetNama();
		auto strType = pThis->GetDescName();
		std::vector<double> colors0 = GetDoubles(choicebag->vecColorRGBA[0]);
		pNama->SetCMDoubles("makeup_highlight_color", colors0.data(), colors0.size());

		return true;
	};

	ShowUINormal(funCB, m_pNama, this);
}

void CMHighLight::Reset()
{
	CMNormalNode::ResetNormal();
}

void CMHighLight::ShowIntensityUI()
{
	char szPoints[1024] = { 0 };
	sprintf(szPoints, "%p", this);
	if (LayoutSlider(ImVec2(0, 22), ImVec2(320, 10), ("##slider221" + string(szPoints)).c_str(), ("##slidertext221" + std::string(szPoints)).c_str(), &m_fIntensity, 0, 100))
	{
		SetIntensity();
	}
}

void CMHighLight::SetIntensity()
{
	m_pNama->SetCMDouble("makeup_intensity_highlight", m_fIntensity / 100);
}

//////////////////////////////////////////////////////////

Implement_FUObject(CMBlush);

CMBlush::CMBlush()
{

}

CMBlush::~CMBlush()
{

}

bool CMBlush::InitFrom(const rapidjson::Value & json)
{
	bool bRet = false;

	bRet = initNormal(json, m_vecData);

	return bRet;
}

void CMBlush::ShowUI()
{
	auto funCB = [](std::shared_ptr<ColorBag> choicebag, void * pUserData)-> bool {
		CMBlush * pThis = (CMBlush*)pUserData;

		Nama * pNama = pThis->GetNama();
		auto strType = pThis->GetDescName();
		std::vector<double> colors0 = GetDoubles(choicebag->vecColorRGBA[0]);
		pNama->SetCMDoubles("makeup_blusher_color", colors0.data(), colors0.size());

		return true;
	};
	ShowUINormal(funCB, m_pNama, this);
}

void CMBlush::Reset()
{
	CMNormalNode::ResetNormal();
}

void CMBlush::ShowIntensityUI()
{
	char szPoints[1024] = { 0 };
	sprintf(szPoints, "%p", this);
	if (LayoutSlider(ImVec2(0, 22), ImVec2(320, 10), ("##slider221" + string(szPoints)).c_str(), ("##slidertext221" + std::string(szPoints)).c_str(), &m_fIntensity, 0, 100))
	{
		SetIntensity();
	}
}

void CMBlush::SetIntensity()
{
	m_pNama->SetCMDouble("makeup_intensity_blusher", m_fIntensity / 100);
}

//////////////////////////////////////////////////////////

Implement_FUObject(CMContour);

CMContour::CMContour()
{

}

CMContour::~CMContour()
{

}

bool CMContour::InitFrom(const rapidjson::Value & json)
{
	bool bRet = false;

	bRet = initNormal(json, m_vecData);

	return bRet;
}

void CMContour::ShowUI()
{
	auto funCB = [](std::shared_ptr<ColorBag> choicebag, void * pUserData)-> bool {

		CMContour * pThis = (CMContour*)pUserData;

		Nama * pNama = pThis->GetNama();
		auto strType = pThis->GetDescName();
		std::vector<double> colors0 = GetDoubles(choicebag->vecColorRGBA[0]);
		
		pNama->SetCMDoubles("makeup_shadow_color", colors0.data(), colors0.size());

		return true;
	};

	ShowUINormal(funCB, m_pNama, this);
}

void CMContour::Reset()
{
	CMNormalNode::ResetNormal();
}

void CMContour::ShowIntensityUI()
{
	char szPoints[1024] = { 0 };
	sprintf(szPoints, "%p", this);
	if (LayoutSlider(ImVec2(0, 22), ImVec2(320, 10), ("##slider221" + string(szPoints)).c_str(), ("##slidertext221" + std::string(szPoints)).c_str(), &m_fIntensity, 0, 100))
	{
		SetIntensity();
	}
}

void CMContour::SetIntensity()
{
	m_pNama->SetCMDouble("makeup_intensity_shadow", m_fIntensity / 100);
}

//////////////////////////////////////////////////////////

Implement_FUObject(CMLip);

CMLip::CMLip()
{

}

CMLip::~CMLip()
{

}

bool CMLip::InitFrom(const rapidjson::Value & json)
{
	bool bRet = false;

	bool Flag1 = false, Flag2 = false, Flag3 = false;

	if (json.IsArray())
	{
		auto config = std::make_shared<CMLipConfig>();
		for (int i = 0; i < json.Size(); ++i) {
			const auto & dictData = json[i];
			LipType bt;
			if (dictData.HasMember("bundle"))
			{
				bt.strBundlePath = dictData["bundle"].GetString();
				Flag1 = true;
			}

			if (dictData.HasMember("icon"))
			{
				bt.strIconPath = dictData["icon"].GetString();
			}

			if (dictData.HasMember("name"))
			{
				bt.strName = dictData["name"].GetString();
			}

			if (dictData.HasMember("type"))
			{
				bt.iType = dictData["type"].GetInt();
			}
			config->vecType.push_back(bt);
			Flag2 = true;
			if (config->vecColor.size() == 0) {
				if (dictData.HasMember("colors") && dictData["colors"].IsArray())
				{
					const auto& arrayData = dictData["colors"];
					for (int j = 0; j < arrayData.Size(); j++)
					{
						if (arrayData[j].IsArray() && arrayData[j].Size() == 4)
						{
							cv::Vec4b color;

							for (int m = 0; m < 4; m++)
							{
								int v = (255 * arrayData[j][m].GetFloat());
								color[m] = v % 256;
							}

							auto bag = std::make_shared<ColorBag>(color);
							config->vecColor.push_back(bag);

							Flag3 = true;
						}
					}
				}
			}

			m_vecData.push_back(config);

		}
	}

	bRet = Flag1 && Flag2 && Flag3;
	return bRet;
}

#define YAO_CHUN (1)
#define SHUI_RUN (7)

void CMLip::ShowUI()
{
	auto config = m_vecData[0];

	ImGui::BeginGroup();

	ImGui::Dummy(ImVec2(1, 20 * scaleRatioH));
	ImGui::Dummy(ImVec2(20 * scaleRatioW, 1)); ImGui::SameLine();

	for (size_t i = 0; i < config->vecType.size(); i++)
	{
		ImGui::PushID(&config->vecType[i]);


		static ImVec4 currentColor;

		if (m_iCurType == config->vecType[i].iType) {
			// 已选中，显示高亮
			currentColor = hoverColor;
		}
		else {
			// 未选中，默认
			currentColor = normalColor;
		}
		ImGui::PushStyleColor(ImGuiCol_Button, currentColor);

		if (LayoutRectImageButtonWithText(ImVec2(6.f, 27.f), ImVec2(76, 76), Texture::createTextureFromFile(config->vecType[i].strIconPath, false)->getTextureID(), config->vecType[i].strName.data()))
		{
			if (m_fIntensity == 0.0f) { 
				m_fIntensity = 100.f;
			}
			m_pNama->SelectCustomMakeupBundle(gBundlePath[UIBridge::bundleCategory] + "/subs/" + config->vecType[i].strBundlePath + ".bundle", GetDescName());
			if (config->vecType[i].iType == YAO_CHUN)
			{
				m_pNama->SetCMDouble("lip_type", 0);
				m_pNama->SetCMDouble("is_two_color", 1);

				double zeroArray[4] = { 0.0,0.0,0.0,0.0 };
				m_pNama->SetCMDoubles("makeup_lip_color2", zeroArray, 4);
			}
			else
			{
				m_pNama->SetCMDouble("lip_type", config->vecType[i].iType);
				m_pNama->SetCMDouble("is_two_color", 0);
			}
			
			SetIntensity();
			m_curBag = nullptr;
			m_iCurType = config->vecType[i].iType;
			if (m_iCurType == SHUI_RUN)
			{
				std::vector<double> colors0 = GetDoubles(config->vecColor[0]->vecColorRGBA[0]);
				m_pNama->SetCMDoubles("makeup_lip_color_v2", colors0.data(), colors0.size());
			}	
		}

		ImGui::PopStyleColor();

		if (i == 3) {
			ImGui::Dummy(ImVec2(1, 20 * scaleRatioH));
			ImGui::Dummy(ImVec2(20 * scaleRatioW, 1)); ImGui::SameLine();
		}
		else {
			ImGui::SameLine(0.f, 15.f * scaleRatioW);
		}

		ImGui::PopID();
	}

	ImGui::NewLine();
	ImGui::Dummy(ImVec2(1, 20 * scaleRatioH));
	ImGui::Dummy(ImVec2(20 * scaleRatioW, 1)); ImGui::SameLine();


	if (m_iCurType > -1)
	{
		for (size_t i = 0; i < config->vecColor.size(); i++)
		{
			auto bag = config->vecColor[i];

			ImGui::PushID(bag.get());

			static ImVec4 currentCBColor;

			if (m_curBag == bag) {
				// 已选中，显示高亮
				currentCBColor = hoverColor;
			}
			else {
				// 未选中，默认
				currentCBColor = normalColor;
			}
			ImGui::PushStyleColor(ImGuiCol_Button, currentCBColor);

			if (LayoutImageButtonWithText(ImVec2(0.f, 27.f), ImVec2(40, 40), bag->pTex->getTextureID(),
				bag->pTex->getTextureID(), ""))
			{
				std::vector<double> colors0 = GetDoubles(bag->vecColorRGBA[0]);
				if (m_iCurType == SHUI_RUN)
				{
					m_pNama->SetCMDoubles("makeup_lip_color_v2", colors0.data(), colors0.size());
				}
				else {
					m_pNama->SetCMDoubles("makeup_lip_color", colors0.data(), colors0.size());
				}
				m_curBag = bag;
			}

			ImGui::PopStyleColor();

			ImGui::SameLine(0, 24 * scaleRatioW);

			ImGui::PopID();
		}

		//未选择就默认赋值
		if (!m_curBag && config->vecColor.size() > 0)
		{
			m_curBag = config->vecColor[0];
			std::vector<double> colors0 = GetDoubles(m_curBag->vecColorRGBA[0]);
			m_pNama->SetCMDoubles("makeup_lip_color", colors0.data(), colors0.size());
		}
	}

	ImGui::Dummy(ImVec2(0, 0));
	ImGui::Dummy(ImVec2(40 * scaleRatioW, 0));
	ImGui::SameLine();

	ImGui::SetCursorPosY(610 * scaleRatioH);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 30);
	if (m_curBag == nullptr) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	}
	this->ShowIntensityUI();
	if (m_curBag == nullptr) {
		ImGui::PopItemFlag();
	}
	ImGui::EndGroup();
}

void CMLip::Reset()
{
	m_iCurType = -1;
	m_curBag = nullptr;
}

void CMLip::ShowIntensityUI()
{
	char szPoints[1024] = { 0 };
	sprintf(szPoints, "%p", this);
	if (LayoutSlider(ImVec2(0, 22), ImVec2(320, 10), ("##slider221" + string(szPoints)).c_str(), ("##slidertext221" + std::string(szPoints)).c_str(), &m_fIntensity, 0, 100))
	{
		SetIntensity();
	}
}

void CMLip::SetIntensity()
{
	m_pNama->SetCMDouble("makeup_intensity_lip", m_fIntensity / 100);
}

//////////////////////////////////////////////////////////

Implement_FUObject(CMEyePupil);

CMEyePupil::CMEyePupil()
{

}

CMEyePupil::~CMEyePupil()
{

}

bool CMEyePupil::InitFrom(const rapidjson::Value & json)
{
	bool bRet = false;

	bRet = initNormal(json, m_vecData);

	return bRet;
}

void CMEyePupil::ShowUI()
{
	auto funCB = [](std::shared_ptr<ColorBag> choicebag, void * pUserData)-> bool {

		CMEyePupil * pThis = (CMEyePupil*)pUserData;

		Nama * pNama = pThis->GetNama();
		auto strType = pThis->GetDescName();

		if (choicebag->vecColorRGBA.size() > 0)
		{
			std::vector<double> colors0 = GetDoubles(choicebag->vecColorRGBA[0]);

			pNama->SetCMDoubles("makeup_pupil_color", colors0.data(), colors0.size());
		}

		return true;
	};

	ShowUINormal(funCB, m_pNama, this);
}

void CMEyePupil::Reset()
{
	CMNormalNode::ResetNormal();
}

void CMEyePupil::ShowIntensityUI()
{
	char szPoints[1024] = { 0 };
	sprintf(szPoints, "%p", this);
	if (LayoutSlider(ImVec2(0, 22), ImVec2(320, 10), ("##slider221" + string(szPoints)).c_str(), ("##slidertext221" + std::string(szPoints)).c_str(), &m_fIntensity, 0, 100))
	{
		SetIntensity();
	}
}

void CMEyePupil::SetIntensity()
{
	m_pNama->SetCMDouble("makeup_intensity_pupil", m_fIntensity / 100);
}

//////////////////////////////////////////////////////////

std::map<std::string, std::shared_ptr<CMBasePage> > GUICustomMakeup::m_pages;
bool GUICustomMakeup::m_bInited = false;
std::shared_ptr<CMBasePage>  GUICustomMakeup::m_curChoosed = nullptr;

GUICustomMakeup::GUICustomMakeup()
{
}

GUICustomMakeup::~GUICustomMakeup()
{
}


//gCustomCMConfig
bool GUICustomMakeup::InitFromConfig(Nama * pNama)
{
	if (m_bInited)
	{
		return true;
	}
    
    string strRealPath = "";
    strRealPath = FuTool::GetFileFullPathFromeSearchPath(gCustomCMConfig.c_str());
	ifstream in(strRealPath.c_str());
	if (!in.is_open()) {
		fprintf(stderr, "fail to read json file: %s\n", gCustomCMConfig.data());
		return false;
	}

	string json_content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	in.close();

	rapidjson::Document dom;
	if (!dom.Parse(json_content.c_str()).HasParseError())
	{
		for (rapidjson::Value::ConstMemberIterator itr = dom.MemberBegin(); itr != dom.MemberEnd(); itr++)
		{
			rapidjson::Value jKey;
			rapidjson::Value jValue;
			rapidjson::Document::AllocatorType allocator;
			jKey.CopyFrom(itr->name, allocator);
			jValue.CopyFrom(itr->value, allocator);
			if (jKey.IsString())
			{
				std::string strName = jKey.GetString();
				auto ptr = MakeSharedObject<CMBasePage>(strName);
				if (ptr && ptr->InitFrom(jValue))
				{
					ptr->SetNama(pNama);
					m_pages.insert(std::make_pair(strName, ptr));
				}
			}
		}
	}

	m_bInited = true;
	return true;
}

void GUICustomMakeup::Reset(Nama * pNama)
{
	//m_curChoosed = nullptr;

	if (pNama)
	{
		pNama->SetCMDouble("is_two_color", 0); //清除咬唇的设定
		pNama->ChangeCleanFlag(true);
		pNama->ClearAllCM();
	}
	
	for (auto & data : m_pages)
	{
		data.second->Reset();
		data.second->m_fIntensity = 0.0f;
	}
}

void GUICustomMakeup::Draw(Nama * pNama)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::Begin("customMakeup", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

	if (!m_bInited)
	{
		InitFromConfig(pNama);
	}
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(45.f / 255.f, 41.f / 255.f, 86.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(45.f / 255.f, 41.f / 255.f, 86.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(45.f / 255.f, 41.f / 255.f, 86.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 8));
	ImGui::BeginTabBar("##tabcm1", ImGuiTabBarFlags_NoAnim | ImGuiTabBarFlags_SizingPolicyEqual | ImGuiTabBarFlags_NoSelectionOnAppearing);
	ImGui::TabItem(ImVec2(416 * scaleRatioW, 36 * scaleRatioH), u8"自定义参数");
	ImGui::EndTabBar();
	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar();

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, hoverColor);
	ImGui::PushStyleColor(ImGuiCol_Border, disableColor);

	ImGui::Dummy(ImVec2(10, 20));
	int pageIndex = -1;
	bool defaultSelect = false;
	for (auto & data : m_pages)
	{
		pageIndex++;
		if (UIBridge::customMakeupIndex >= 6) {
			if (pageIndex < 4) {
				continue;
			}
		}
		ImGui::PushID(data.second.get());
		
		static ImVec4 currentColor;
		static ImVec4 currentTextColor;
		//默认选中第一个腮红
		if (m_curChoosed == nullptr) {
			m_curChoosed = data.second;
			defaultSelect = true;
		}

		if (m_curChoosed == data.second) {
			// 已选中，显示高亮
			currentColor = hoverColor;
			currentTextColor = normalColor;
		}
		else {
			// 未选中，默认
			currentColor = normalColor;
			currentTextColor = disableColor;
		}
		auto strName = data.second->GetDescName();
		auto strUtf8 = FuTool::convert2utf8(strName.data());

		ImGui::PushStyleColor(ImGuiCol_Button, currentColor);
		ImGui::PushStyleColor(ImGuiCol_Text, currentTextColor);

		if (LayoutButton(ImVec2(0, 0), ImVec2(54, 30), strUtf8.data()))
		{
			m_curChoosed = data.second;
			defaultSelect = false;
			UIBridge::customMakeupIndex = pageIndex;
		}
		ImGui::PopStyleColor(2);
		ImGui::SameLine(0, 0.f * scaleRatioW);

		ImGui::PopID();
	}

	if (m_curChoosed)
	{
		ImGui::NewLine();
		m_curChoosed->ShowUI();
	}

	if (defaultSelect && m_pages.begin()->second->m_fIntensity == 0) {
		m_curChoosed = nullptr;
	}

	if (m_curChoosed == nullptr || m_curChoosed->m_fIntensity == 0.0f) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(246.f / 255.f, 246.f / 255.f, 250.f/ 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(246.f / 255.f, 246.f / 255.f, 250.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(127.f / 255.f, 134.f / 255.f, 152.f / 255.f, 0.3f));
		LayoutButton(ImVec2(135, 38), ImVec2(126, 32), u8"一键卸妆", ImGuiButtonFlags_Disabled);
		ImGui::PopStyleColor(4);
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(246.f / 255.f, 246.f / 255.f, 250.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 1.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(149.f / 255.f, 156.f / 255.f, 180.f / 255.f, 1.f));
		if (LayoutButton(ImVec2(135, 38), ImVec2(126, 32), u8"一键卸妆"))
		{
			Reset(pNama);
		}
		ImGui::PopStyleColor(5);
	}

	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar(2);

	ImGui::End();
	ImGui::PopStyleVar(2);
}

