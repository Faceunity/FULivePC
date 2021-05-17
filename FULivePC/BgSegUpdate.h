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
	void Update(NamaExampleNameSpace::Nama * nama);

private:
	CCameraDS * m_pFrameInput = nullptr;
	bool m_bHasSetedBgForOnceChange = false;
};

