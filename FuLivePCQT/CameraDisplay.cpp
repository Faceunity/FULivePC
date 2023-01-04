#include "CameraDisplay.h"
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <CNamaSDK.h>
#include "MainClass.h"
#include "UIBridge.h"
#include "Nama.h"

CameraDisplayRenderer* CameraDisplay::s_pRenderer(nullptr);
std::mutex CameraDisplay::s_renderer;
CameraDisplay * CameraDisplay::s_pCameraDisplay = nullptr;

CameraDisplay::CameraDisplay()
{
    s_pCameraDisplay = this;
}

CameraDisplay::~CameraDisplay()
{
    s_pCameraDisplay = nullptr;
}

QQuickFramebufferObject::Renderer *CameraDisplay::createRenderer() const
{
    std::unique_lock<std::mutex> lock(s_renderer);
    if (s_pRenderer)
        delete s_pRenderer;
    s_pRenderer = new CameraDisplayRenderer;
    return s_pRenderer;
}

CameraDisplayRenderer *CameraDisplay::getRenderer()
{
    std::unique_lock<std::mutex> lock(s_renderer);
    return s_pRenderer;
}

CameraDisplayRenderer::CameraDisplayRenderer()
{
    initializeGL();
    MainClass::getInstance()->m_nama->InitNama();
    emit MainClass::getInstance()->m_UIBridge->updateConfig();
}

CameraDisplayRenderer::~CameraDisplayRenderer()
{

}

int CameraDisplayRenderer::updateGreenSaveArea(cv::Mat &frameData)
{
    glGenTextures(1, &m_texId);
    glBindTexture(GL_TEXTURE_2D, m_texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frameData.cols, frameData.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, frameData.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return m_texId;
}

void CameraDisplayRenderer::render()
{
    Nama *nama = MainClass::getInstance()->m_nama;
    UIBridge *uibridge = MainClass::getInstance()->m_UIBridge;
    std::unique_lock<std::mutex> lock(nama->m_frameMutex);
    QOpenGLFunctions glFuncs(QOpenGLContext::currentContext());
    glFuncs.glUseProgram(0);
    //这里长宽864/486对应qml中CameraDisplay的长宽
    int width = MainClass::getInstance()->m_UIBridge->m_cameraWidth;
    int height = MainClass::getInstance()->m_UIBridge->m_cameraHeight;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);//指定矩阵为投影矩阵
    glLoadIdentity();           //重置为单位矩阵
    glOrtho(0, width, 0, height, 0, 1000);//设置剪裁区域
    glDisable(GL_DEPTH_TEST);
    m_timeRender.start();
    if(uibridge->m_newImage){
        if(uibridge->m_bgsSelectVideo){
            //切换ar不render
        }else if(uibridge->m_flagARBody && uibridge->m_arFunction){
            nama->RenderDefNama();
            uibridge->detectionBodyTip();
        }else if(uibridge->m_bLoadBear){
            nama->RenderBear();
            uibridge->detectionBodyTip();
        }else if(uibridge->m_bodyTrackType != BodyTrackType::None){
            nama->RenderP2A();
            uibridge->detectionBodyTip();
        }else{
            nama->RenderDefNama();
            if(!uibridge->m_bSelectColor){
                uibridge->detectionTip();
            }
        }
    }
    if(!uibridge->m_renderNewFrame && uibridge->m_newImage){
        uibridge->m_renderNewFrame = true;
        emit uibridge->switchARChanged(uibridge->m_arFunction);
    }
    uibridge->m_secondRenderTime += m_timeRender.elapsed();
    uibridge->m_FrameIDFPS++;
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nama->m_texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPushMatrix();
    glBegin(GL_QUADS);
    glColor4f(1.f, 1.f, 1.f, 1.f);
    //输出图纹理像需要垂直翻转
    glTexCoord2f(0.f, 1.f);
    glVertex3f(0.f, 0.f, 0.f);//左上角
    glTexCoord2f(1.f, 1.f);
    glVertex3f(width, 0.f, 0.f);//右上角
    glTexCoord2f(1.f, 0.f);
    glVertex3f(width, height, 0.f);//右下角
    glTexCoord2f(0.f, 0.f);
    glVertex3f(0.f, height, 0.f);//左下角
    glEnd();
    glPopMatrix();
    update();
}

void CameraDisplayRenderer::initializeGL()
{
    initializeOpenGLFunctions();
}
