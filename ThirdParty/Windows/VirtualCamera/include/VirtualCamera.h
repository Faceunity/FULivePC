#ifndef VIRTURALCAMERA_H
#define VIRTURALCAMERA_H
#define VIRTURALCAMERA_API __declspec(dllexport)

//param type: 默认0 RGBA格式  1 I420格式
//param width height: 可用最大分辨率  默认3940*2160 在用changeResolutionOfVirturalCamera切换分辨率时不得大于设置的
VIRTURALCAMERA_API void createVirturalCamera(int type = 0, int width = 3940, int height = 2160);

VIRTURALCAMERA_API void pushDataToVirturalCamera(void* pData, int width, int height);

VIRTURALCAMERA_API void changeResolutionOfVirturalCamera(int width, int height);

#endif //VIRTURALCAMERA_H