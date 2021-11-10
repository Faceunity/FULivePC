������ַ��https://github.com/Faceunity/FULivePC/tree/dev/docs
FUNama SDK �����ĵ�

��ǩ���ո�ָ����� ����

---

���ĵ���Ҫ��Բ�ͬ�Ľ��������ṩ��ʼ��OpenGL context�ķ�����

- 1.Qt
- 2.MFC
- 3.DLL
- 4.����̨����

Qt

Qt��ʹ��OpenGL�ķ��������Լ̳�QOpenGLWindow��QOpenGLWidget\QGLWindow��QGLWidget�ĸ��е�����һ���࣬������initializeGL()������OpenGL context�ͻ�������������ú󴴽�������

window.h

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

window.cpp

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
        // ��ʼ�� OpenGL ���.��������˳������
        initializeOpenGLFunctions();
        printContextInformation();
        fuSetup();
    }
    
    void Window::resizeGL(int width, int height)
    {
        // δ������
    }
    
    void Window::paintGL()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    // ��ӡ�����Ϣ��������
    void Window::printContextInformation()
    {
        QString glType;
        QString glVersion;
        QString glProfile;
    
        // ��ȡ�汾��Ϣ
        glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
        glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    
        // ��ȡ Profile ��Ϣ
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

main.cpp

    #include <QApplication>
    #include "window.h"
    
    int main(int argc, char *argv[])
    {
      QGuiApplication app(argc, argv);
    
      // ���� OpenGL ��Ϣ
      // ע�⣺������show()֮ǰ������Ϣ
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

MFC

MFC���������������ʹ��OpenGL���Բο�����

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
    
        //hglrc���Ǵ�������OpenGL context
        printf("hw=%08x hgldc=%08x spf=%d ret=%d hglrc=%08x\n",
            hw, hgldc, spf, ret, hglrc);
    }
    
    int main() {
    
        InitOpenGL();
        //��ӡһ��OpenGL�����ĵ�ַ�����4��������һ����Ϊ��ֵ����OpenGL�����ǿ��õ�
        //�˺������Nama SDK���ö���������context
        //����ͻ������������ƴ����������OpenGL context����ô��ȷ������Nama�ӿ�ʱһֱ��ͬһ��context
        printf("gl ver test (%s:%d): %08x %08x %08x %08x\n", __FILE__, __LINE__,
            wglGetProcAddress("glGenFramebuffersARB"),
            wglGetProcAddress("glGenFramebuffersOES"),
            wglGetProcAddress("glGenFramebuffersEXT"),
            wglGetProcAddress("glGenFramebuffers"));
        system("pause");
        return 0;
    }






