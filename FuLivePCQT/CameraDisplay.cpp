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
    emit MainClass::getInstance()->m_UIBridge->updataConfig();
}

CameraDisplayRenderer::~CameraDisplayRenderer()
{

}

int CameraDisplayRenderer::updataGreenSaveArea(cv::Mat &frameData)
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
    initializeOpenGLFunctions();
    Nama *nama = MainClass::getInstance()->m_nama;
    UIBridge *uibridge = MainClass::getInstance()->m_UIBridge;
    std::unique_lock<std::mutex> lock(nama->m_frameMutex);
    QOpenGLFunctions glFuncs(QOpenGLContext::currentContext());
    glFuncs.glUseProgram(0);
    //这里长宽864/486对应qml中CameraDisplay的长宽
    int width = MainClass::getInstance()->m_UIBridge->m_cameraWidth;
    int height = MainClass::getInstance()->m_UIBridge->m_cameraHeight;
    if(uibridge->b_needCreateTex){
        nama->UpdateGreenScreenSafeArea(updataGreenSaveArea(uibridge->m_matSafeArea));
        uibridge->b_needCreateTex = false;
    }
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);//指定矩阵为投影矩阵
    glLoadIdentity();           //重置为单位矩阵
    glOrtho(0, width, 0, height, 0, 1000);//设置剪裁区域
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    m_timeRender.start();
    if(uibridge->m_bgsSelectVideo){
        //切换ar不render
    }else if(uibridge->m_flagARBody){
        nama->RenderDefNama();
    }else if(uibridge->m_bLoadBear){
        nama->RenderBear();
    }else if(uibridge->m_bodyTrackType != BodyTrackType::None){
        nama->RenderP2A();
    }else{
        nama->RenderDefNama();
    }
    uibridge->m_secondRenderTime += m_timeRender.elapsed();
    uibridge->m_FrameIDFPS++;
    m_shader->bind();
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
    m_shader->release();
    update();
}

void CameraDisplayRenderer::initializeGL()
{
    m_shader = new QOpenGLShaderProgram();
    const char str_overlay_fragment_shader[] =
            "uniform sampler2D src_tex;             \n"
            "void main (void)                       \n"
            "{                                      \n"
            "   vec4 src = texture2D(src_tex, gl_TexCoord[0].xy);"
            "   gl_FragColor = vec4(src.rgb, 1.0);"
            "}                                      \n";
    const char str_common_vertex_shader[] =
            "void main(void)                \n"
            "{                              \n"
            "   gl_FrontColor = gl_Color;   \n"
            "   gl_Position = ftransform(); \n"
            "   gl_TexCoord[0] = gl_MultiTexCoord0; \n"
            "}                              \n";
    m_shader->addShaderFromSourceCode(QOpenGLShader::Vertex,str_common_vertex_shader);
    m_shader->addShaderFromSourceCode(QOpenGLShader::Fragment,str_overlay_fragment_shader);
    if (m_shader->link()) {
        qDebug("Shaders link success.");
    } else {
        qDebug("Shaders link failed!");
    }
    //VBO，VEO数据
    //    static const GLfloat vertices[] = {
    //        //位置                 //颜色              //纹理位置
    //        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // 0右上角
    //        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // 1右下角
    //        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // 2左下角
    //        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // 3左上角
    //    };
    //    static const GLint indices[] = {  // 注意索引从0开始
    //                                      0, 1, 3,  // 第一个三角形
    //                                      1, 2, 3   // 第二个三角形
    //                                   };
    //    m_vao = new QOpenGLVertexArrayObject();
    //    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    //    m_ebo = new QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer);
    //    m_vao->create();
    //    m_vao->bind();
    //    m_vbo->create();
    //    m_vbo->bind();
    //    m_vbo->allocate(vertices, 4*8*sizeof(GLfloat));
    //    m_ebo->create();
    //    m_ebo->bind();
    //    m_ebo->allocate(indices, 2*3*sizeof(GLint));
    //    int attr = -1;
    //    //顶点属性设置
    //    attr = m_shader->attributeLocation("aPos");
    //    m_shader->setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(GLfloat) * 8);
    //    m_shader->enableAttributeArray(attr);
    //    //颜色属性设置
    //    attr = m_shader->attributeLocation("aColor");
    //    m_shader->setAttributeBuffer(attr, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 8);
    //    m_shader->enableAttributeArray(attr);
    //    //纹理属性设置
    //    attr = m_shader->attributeLocation("aTexCoord");
    //    m_shader->setAttributeBuffer(attr, GL_FLOAT, sizeof(GLfloat) * 6, 2, sizeof(GLfloat) * 8);
    //    m_shader->enableAttributeArray(attr);
    //    m_vao->release();
    //    m_vbo->release();
    //    m_ebo->release();
}
