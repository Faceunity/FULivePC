#ifndef VIRTURALCAMERA_H
#define VIRTURALCAMERA_H
#define VIRTURALCAMERA_API __declspec(dllexport)

//param type: Ĭ��0 RGBA��ʽ  1 I420��ʽ
//param width height: �������ֱ���  Ĭ��3940*2160 ����changeResolutionOfVirturalCamera�л��ֱ���ʱ���ô������õ�
VIRTURALCAMERA_API void createVirturalCamera(int type = 0, int width = 3940, int height = 2160);

VIRTURALCAMERA_API void pushDataToVirturalCamera(void* pData, int width, int height);

VIRTURALCAMERA_API void changeResolutionOfVirturalCamera(int width, int height);

#endif //VIRTURALCAMERA_H