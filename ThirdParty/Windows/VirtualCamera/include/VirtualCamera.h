#ifndef VIRTURALCAMERA_H
#define VIRTURALCAMERA_H
#define VIRTURALCAMERA_API __declspec(dllexport)

//param type: Ĭ��0 RGBA��ʽ  1 I420��ʽ
//param width height: �������ֱ���  Ĭ��3940*2160 ����changeResolutionOfVirturalCamera�л��ֱ���ʱ���ô������õ�
VIRTURALCAMERA_API void createVirturalCamera(int type = 0, int width = 3940, int height = 2160);

VIRTURALCAMERA_API void pushDataToVirturalCamera(void* pData, int width, int height);

// audio format:16bits per sample,44100hz,two channels pcm
VIRTURALCAMERA_API void pushAudioToVirturalCamera(void* pData, int nLength, double nDuration);

VIRTURALCAMERA_API void changeResolutionOfVirturalCamera(int width, int height);

#endif //VIRTURALCAMERA_H
