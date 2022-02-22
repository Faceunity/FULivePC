#include "MainClass.h"

MainClass* MainClass::instance_ = nullptr;

MainClass* MainClass::getInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new MainClass();
    }
    return instance_;
}

MainClass::MainClass()
{
    m_nama = new Nama();
    m_camera = new Camera();
    m_UIBridge = new UIBridge();
    m_stickerHolder = std::make_shared<StikcerHolder>();
}

MainClass::~MainClass()
{
}
