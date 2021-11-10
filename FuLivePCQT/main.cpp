#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "MainClass.h"
#include "CameraDisplay.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //解决QML File Dialog QSettings organizationName/Domain
    QCoreApplication::setOrganizationName("something");
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    MainClass *main = MainClass::getInstance();
    //创建默认图
    main->m_UIBridge->creatdefaultFrame();
    engine.rootContext()->setContextProperty("UIBridge", main->m_UIBridge);
    engine.addImageProvider(QLatin1String("ShowImage"), main->m_UIBridge->m_ImageProvider);
    qmlRegisterType<CameraDisplay>("OpenGLUnderQML", 1, 0, "CameraDisplay");
    engine.load(url);
    //开启连接相机
    main->m_camera->captureCamera();
    return app.exec();
}
