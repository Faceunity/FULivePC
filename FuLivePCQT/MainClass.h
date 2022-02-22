#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QObject>
#include "UIBridge.h"
#include "Camera.h"
#include "StickerHolder.h"

class Nama;

class MainClass : public QObject
{
    Q_OBJECT
public:
    MainClass();
    ~MainClass();
    static MainClass* getInstance();
    UIBridge        *m_UIBridge;
    Nama            *m_nama;
    Camera          *m_camera;
    std::shared_ptr<StikcerHolder> m_stickerHolder = nullptr; 
private:
    //单例模式
    static MainClass *instance_;
};

#endif // MAINCLASS_H
