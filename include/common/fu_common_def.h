#ifndef _FL_COMMON_DEF_H_
#define _FL_COMMON_DEF_H_

class Bitmap
{
public:
    ~Bitmap()
    {
        if (mpData)
        {
            delete[] mpData;
            mpData = nullptr;
        }
    }
public:
    Bitmap() = default;
    uint8_t* mpData = nullptr;
    uint32_t mWidth = 0;
    uint32_t mHeight = 0;
};

#endif

