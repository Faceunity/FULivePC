#include "fu_singleton.h"
#include "Nama.h"
#include <string>

class CCameraDS;

class BgSegUpdate: public SingleTon<BgSegUpdate>
{
public:
	BgSegUpdate();
	~BgSegUpdate();

public:
	void SetSource(std::string strURL);
	bool SaveCurFrame2File(std::string strPath);
	void Update(NamaExampleNameSpace::Nama * nama);
	cv::Size GetDstResolution();

private:
	CCameraDS * m_pFrameInput = nullptr;
	bool m_bHasSetedBgForOnceChange = false;
};

