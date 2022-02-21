#ifndef CAMERADISPLAY_H
#define CAMERADISPLAY_H

#include <Windows.h>
#include <mutex>
#include <QtQuick/QQuickFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QTime>
#include <opencv2/opencv.hpp>

//这是一个qml嵌入类,负责展示opengl渲染,刷新在render函数中

class CameraDisplayRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions
{
public:
    CameraDisplayRenderer();
    ~CameraDisplayRenderer();
    GLuint m_texId = 0;
    //绿幕安全区域
    int updataGreenSaveArea(cv::Mat &frameData);
public:
    void initializeGL();
    void render(void) override;
private:
    QTime m_timeRender;
};


class CameraDisplay : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    CameraDisplay();
    ~CameraDisplay();
    Renderer* createRenderer() const override;
    static CameraDisplayRenderer* getRenderer();
public:
    static CameraDisplay* s_pCameraDisplay;
private:
    static CameraDisplayRenderer* s_pRenderer;
    static std::mutex s_renderer;
};


#endif // CAMERADISPLAY_H
