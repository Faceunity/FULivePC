# FUNama SDK 帮助文档

标签（空格分隔）： 帮助

---

本文档主要针对不同的接入类型提供初始化OpenGL context的方法：
- 1.Qt
- 2.MFC
- 3.DLL
- 4.控制台程序

## Qt

Qt里使用OpenGL的方法，可以继承QOpenGLWindow、QOpenGLWidget\QGLWindow、QGLWidget四个中的任意一个类，重载了initializeGL()方法后，OpenGL context就会在这个函数调用后创建出来。

window.h
```C
#ifndef WINDOW_H
#define WINDOW_H

#include <QOpenGLWindow>
#include <QOpenGLFunctions>

class Window: public QOpenGLWindow,
               protected QOpenGLFunctions
{
    Q_OBJECT

public:
    ~Window();

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

private:
    void printContextInformation();
};

#endif


...
```

window.cpp
```C
#include "window.h"

#include <QDebug>
#include <QString>

Window::~Window()
{
    makeCurrent();
    teardownGL();
}

void Window::initializeGL()
{
    // 初始化 OpenGL 后端.函数调用顺序如下
    initializeOpenGLFunctions();
    printContextInformation();
    fuSetup();
}

void Window::resizeGL(int width, int height)
{
    // 未做处理
}

void Window::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

// 打印相关信息，调试用
void Window::printContextInformation()
{
    QString glType;
    QString glVersion;
    QString glProfile;

    // 获取版本信息
    glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

    // 获取 Profile 信息
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
    switch (format().profile())
    {
        CASE(NoProfile);
        CASE(CoreProfile);
        CASE(CompatibilityProfile);
    }
#undef CASE

    qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}
```

main.cpp
```C
#include <QApplication>
#include "window.h"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  // 设置 OpenGL 信息
  // 注意：必须在show()之前设置信息
  QSurfaceFormat format;
  format.setRenderableType(QSurfaceFormat::OpenGL);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setVersion(3,3);

  Window window;
  window.setFormat(format);
  window.resize(QSize(1280, 720));
  window.show();

  return app.exec();
}
```

## MFC

MFC和其他几种情况中使用OpenGL可以参考如下


```C
#include <windows.h>
#include <stdio.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),
    1u,
    PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW,
    PFD_TYPE_RGBA,
    32u,
    0u, 0u, 0u, 0u, 0u, 0u,
    8u,
    0u,
    0u,
    0u, 0u, 0u, 0u,
    24u,
    8u,
    0u,
    PFD_MAIN_PLANE,
    0u,
    0u, 0u };

void InitOpenGL()
{
    HWND hw = CreateWindowExA(
        0, "EDIT", "", ES_READONLY,
        0, 0, 1, 1,
        NULL, NULL,
        GetModuleHandleA(NULL), NULL);
    HDC hgldc = GetDC(hw);
    int spf = ChoosePixelFormat(hgldc, &pfd);
    int ret = SetPixelFormat(hgldc, spf, &pfd);
    HGLRC hglrc = wglCreateContext(hgldc);
    wglMakeCurrent(hgldc, hglrc);

    //hglrc就是创建出的OpenGL context
    printf("hw=%08x hgldc=%08x spf=%d ret=%d hglrc=%08x\n",
        hw, hgldc, spf, ret, hglrc);
}

int main() {

    InitOpenGL();
    //打印一个OpenGL函数的地址，如果4个中任意一个不为空值，则OpenGL环境是可用的
    //此后的所有Nama SDK调用都会基于这个context
    //如果客户端有其他绘制创建了另外的OpenGL context，那么请确保调用Nama接口时一直是同一个context
    printf("gl ver test (%s:%d): %08x %08x %08x %08x\n", __FILE__, __LINE__,
        wglGetProcAddress("glGenFramebuffersARB"),
        wglGetProcAddress("glGenFramebuffersOES"),
        wglGetProcAddress("glGenFramebuffersEXT"),
        wglGetProcAddress("glGenFramebuffers"));
    system("pause");
    return 0;
}
```





