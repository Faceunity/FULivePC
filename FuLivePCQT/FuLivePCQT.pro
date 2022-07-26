QT += quick
QT += multimedia

CONFIG += c++11
CONFIG += resources_big


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    ../ThirdParty/Windows/FaceUnity-SDK-PC/include/CNamaSDK.h \
    Camera.h \
    CameraDisplay.h \
    Config.h \
    FuController.h \
    ImageProvider.h \
    MainClass.h \
    Nama.h \
    Network/zip.h \
    Network/unzip.h \
    Network/StickerHolder.h \
    Sound/MP3.h \
    UIBridge.h

SOURCES += \
        Camera.cpp \
        CameraDisplay.cpp \
        FuController.cpp \
        ImageProvider.cpp \
        MainClass.cpp \
        Nama.cpp \
        Network/zip.cpp \
        Network/unzip.cpp \
        Network/StickerHolder.cpp \
        Sound/MP3.cpp \
        main.cpp \
        UIBridge.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/../include \
               $$PWD/../include/common \
               $$PWD/../include/rapidjson \
               $$PWD/Sound \
               $$PWD/Network \
               $$PWD/../ThirdParty/Windows/GL/include \
               $$PWD/../ThirdParty/Windows/opencv400/include \
               $$PWD/../ThirdParty/Windows/VirtualCamera/include \
               $$PWD/../ThirdParty/Windows/VirtualCamera/include \
               $$PWD/../ThirdParty/Windows/ffmpeg/include \
               $$PWD/../ThirdParty/Windows/FaceUnity-SDK-PC/include \
               $$PWD/../ThirdParty/Windows/libcurl/include

defineTest(resToDestdir) {
    files = $$1
    ddir  = $$2
    for(FILE, files) {
        DDIR = $$OUT_PWD/$$ddir

        !exists($$DDIR) {
            log($$DDIR not  exists)
            mkpath($$DDIR)
        }
        win32:DDIR ~= s,/,\\,g
        win32:FILE ~= s,/,\\,g
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }
    export(QMAKE_POST_LINK)
    return(true)
}

contains(QT_ARCH, i386) {
LIBS += -L$$PWD/../ThirdParty/Windows/FaceUnity-SDK-PC/lib/win32/ -lCNamaSDK
LIBS += -L$$PWD/../ThirdParty/Windows/libcurl/lib/win32/ -llibcurl
CONFIG(release, debug|release): LIBS += -L$$PWD/../ThirdParty/Windows/opencv400/lib/win32/ -lopencv_world400
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ThirdParty/Windows/opencv400/lib/win32/ -lopencv_world400d
LIBS += -L$$PWD/../ThirdParty/Windows/VirtualCamera/lib/win32/ -lVirtualCamera

NAMA_FILES = $$PWD/../ThirdParty/Windows/FaceUnity-SDK-PC/lib/win32/*.dll
CURL_FILES = $$PWD/../ThirdParty/Windows/libcurl/lib/win32/*.dll
OPENCV_FILES = $$PWD/../ThirdParty/Windows/opencv400/lib/win32/*.dll
VIRTUAL_FILES = $$PWD/../ThirdParty/Windows/VirtualCamera/lib/win32/*.dll
} else {
LIBS += -L$$PWD/../ThirdParty/Windows/FaceUnity-SDK-PC/lib/win64/ -lCNamaSDK
LIBS += -L$$PWD/../ThirdParty/Windows/libcurl/lib/win64/ -llibcurl
CONFIG(release, debug|release): LIBS += -L$$PWD/../ThirdParty/Windows/opencv400/lib/win64/ -lopencv_world400
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ThirdParty/Windows/opencv400/lib/win64/ -lopencv_world400d
LIBS += -L$$PWD/../ThirdParty/Windows/VirtualCamera/lib/win64/ -lVirtualCamera

NAMA_FILES = $$PWD/../ThirdParty/Windows/FaceUnity-SDK-PC/lib/win64/*.dll
CURL_FILES = $$PWD/../ThirdParty/Windows/libcurl/lib/win64/*.dll
OPENCV_FILES = $$PWD/../ThirdParty/Windows/opencv400/lib/win64/*.dll
VIRTUAL_FILES = $$PWD/../ThirdParty/Windows/VirtualCamera/lib/win64/*.dll

}

LIBS += -lOpenGL32 -lole32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -loleaut32 -luuid -lcomdlg32 -ladvapi32

CONFIG(release, debug|release):
resToDestdir($$NAMA_FILES, release) \
resToDestdir($$CURL_FILES, release) \
resToDestdir($$OPENCV_FILES, release) \
resToDestdir($$VIRTUAL_FILES, release)
else:CONFIG(debug, debug|release):
resToDestdir($$NAMA_FILES, debug) \
resToDestdir($$CURL_FILES, debug) \
resToDestdir($$OPENCV_FILES, debug) \
resToDestdir($$VIRTUAL_FILES, debug)



