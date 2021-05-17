#ifndef _GUI_STICKER_H_
#define _GUI_STICKER_H_

#include "Nama.h"

enum eStickerLevel
{
	STICKER_LEVEL_MIDDLE,
	STICKER_LEVEL_HIGH
};

class StikcerHolder;

class GUISticker
{
public:
	GUISticker();
	~GUISticker();

	static void LoadResource();

	static void ShowStickerPannel(NamaExampleNameSpace::Nama * nama);

	static void ShowStickerList(NamaExampleNameSpace::Nama * nama, int tagIndex);

private:

	static std::shared_ptr<StikcerHolder> mNetHolder;
};

#endif