#pragma once

#include <vector>
#include <memory>
#include <string>
#include <sstream>
class CCameraDS;
namespace NamaExampleNameSpace
{
	class Nama
	{
		enum MODE
		{
			PROP,
			LANDMARK
		};
	public:
		int m_frameID;
		int m_curCameraIdx;
		Nama();
		~Nama();
		int CameraCount();
		std::tr1::shared_ptr<unsigned char> QueryFrame();
		int CameraName(int nCamID, char * sName, int nBufferSize);
		void ReOpenCamera();
		void Init(const int width = 1280, const int height = 720);
		void SwitchRenderMode();
		void SwitchBeauty();
		void PreBundle();
		void NextBundle();
		void SetCurrentBundle(int index);
		void SetCurrentShape(int index);
		void NextShape();
		void UpdateFilter();
		void UpdateBeauty();
		void ScissorFrameBuffer(std::tr1::shared_ptr<unsigned char> frame);
		std::tr1::shared_ptr<unsigned char> ConvertBetweenBGRAandRGBA(std::tr1::shared_ptr<unsigned char> frame);
		void RenderItems(std::tr1::shared_ptr<unsigned char> frame);
		std::tr1::shared_ptr<unsigned char> RenderEx();	
		void DrawLandmarks(std::tr1::shared_ptr<unsigned char> frame);
	private:
		void CreateBundle();
		void CreateBundle(const int idx);		
		void DrawPoint(std::tr1::shared_ptr<unsigned char> frame, int x, int y, unsigned char r = 255, unsigned char g = 240, unsigned char b = 33);

	private:
		static std::tr1::shared_ptr<Nama> m_pInstance;
		std::tr1::shared_ptr<CCameraDS> m_cap;
		int m_curBundleIdx;
		
		int m_face_shape;
		MODE m_mode;
		std::vector<int> m_propHandles;
		int m_beautyHandles;
		int m_gestureHandles;
		int m_frameWidth, m_frameHeight;
		static bool m_hasSetup;
	public:
		int m_isBeautyOn;
		int m_isDrawProp;
		int m_isDrawPoints;
		int m_isDrawWireFram;
		int m_isDrawSplines;

		int m_curFilterIdx;
		float m_curColorLevel;
		float m_curBlurLevel;
		float m_curCheekThinning;
		float m_curEyeEnlarging;
		float m_faceShapeLevel;
		float m_redLevel;
		std::string m_curTranslation;
		std::string m_curRotation;
		static std::string _filters[6];
	};
}

template < class T>
std::string ConvertToString(T value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}