QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BgSegUpdate.cpp \
    Camera.cpp \
    FULivePC.cpp \
    FuController.cpp \
    GUI/Gui.cpp \
    GUI/GuiBgSeg.cpp \
    GUI/GuiCustomMakeup.cpp \
    GUI/GuiGS.cpp \
    GUI/GuiSticker.cpp \
    GUI/GuiTabContent.cpp \
    GUI/GuiTool.cpp \
    GUI/Loader.cpp \
    GUI/MouseControl.cpp \
    GUI/Texture.cpp \
    Nama.cpp \
    Network/StickerHolder.cpp \
    Sound/MP3.cpp \
    UIBridge.cpp \
    common/fu_shared_obj.cpp \
    common/fu_tool.cpp \
    imgui/gl3w.c \
    imgui/imgui.cpp \
    imgui/imgui_demo.cpp \
    imgui/imgui_draw.cpp \
    imgui/imgui_impl_glfw.cpp \
    imgui/imgui_impl_opengl3.cpp \
    imgui/imgui_tabs.cpp \
    tinfiledlg/tinyfiledialogs.c


HEADERS += \
    ../ThirdParty/Windows/FaceUnity-SDK-PC/include/CNamaSDK.h \
    BgSegUpdate.h \
    Camera.h \
    Config.h \
    FuController.h \
    GUI/Gui.h \
    GUI/GuiBgSeg.h \
    GUI/GuiCustomMakeup.h \
    GUI/GuiGS.h \
    GUI/GuiSticker.h \
    GUI/GuiTabContent.h \
    GUI/GuiTool.h \
    GUI/Loader.h \
    GUI/MouseControl.h \
    GUI/Texture.h \
    Nama.h \
    Network/StickerHolder.h \
    Sound/MP3.h \
    UIBridge.h \
    authpack.h \
    imgui/gl3w.h \
    imgui/glcorearb.h \
    imgui/imconfig.h \
    imgui/imgui.h \
    imgui/imgui_impl_glfw.h \
    imgui/imgui_impl_opengl3.h \
    imgui/imgui_internal.h \
    imgui/imgui_tabs.h \
    imgui/stb_rect_pack.h \
    imgui/stb_textedit.h \
    imgui/stb_truetype.h \
    tinfiledlg/tinyfiledialogs.h \
    include/common/fu_shared_obj.h \
    include/common/fu_tool.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/../include \
               $$PWD/../include/common \
               $$PWD/../include/rapidjson \
               $$PWD/GUI \
               $$PWD/imgui \
               $$PWD/Network \
               $$PWD/../ThirdParty/Windows/GL/include \
               $$PWD/../ThirdParty/Windows/opencv400/include \
               $$PWD/../ThirdParty/Windows/VirtualCamera/include \
               $$PWD/../ThirdParty/Windows/FreeImage/include \
               $$PWD/../ThirdParty/Windows/VirtualCamera/include \
               $$PWD/../ThirdParty/Windows/ffmpeg/include \
               $$PWD/../ThirdParty/Windows/FaceUnity-SDK-PC/include \
               $$PWD/../ThirdParty/Windows/libcurl/include

win32: LIBS += -L$$PWD/../ThirdParty/Windows/FaceUnity-SDK-PC/lib/win64/ -lCNamaSDK
win32: LIBS += -L$$PWD/../ThirdParty/Windows/ffmpeg/lib/win64/ -lavcodec -lavformat -lavutil -lswresample
win32: LIBS += -L$$PWD/../ThirdParty/Windows/FreeImage/lib/win64/ -lFreeImage
win32: LIBS += -L$$PWD/../ThirdParty/Windows/libcurl/lib/win64/ -llibcurl

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ThirdParty/Windows/GL/lib/win64/ -lglfw3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ThirdParty/Windows/GL/lib/win64/ -lglfw3d
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ThirdParty/Windows/opencv400/lib/win64/ -lopencv_world400
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ThirdParty/Windows/opencv400/lib/win64/ -lopencv_world400d
win32: LIBS += -L$$PWD/../ThirdParty/Windows/VirtualCamera/lib/win64/ -lVirtualCamera

#qt缺省的库,vs自动添加,解决DirectShow开发时，遇上“无法解析的外部符号”
win32: LIBS += -lOpenGL32 -lGlU32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32
