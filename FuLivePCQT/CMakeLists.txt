cmake_minimum_required(VERSION 3.5)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version" FORCE)
project(FULivePCQT)
set(target FULivePCQT)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(WIN32)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/cmakeScripts/Modules)
    include(${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/cmakeScripts/import_custom_library.cmake)
elseif(APPLE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Mac/cmakeScripts/Modules)
    include(${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Mac/cmakeScripts/import_custom_library.cmake)
elseif(UNIX)
endif()

if(NOT QT_DIR)
   if(WIN32)
      set(QT_DIR ${CMAKE_CURRENT_LIST_DIR}/../Thirdparty/Qt5.9.8/windows/5.9.8/msvc2017_64)
   elseif(APPLE)
      set(QT_DIR ${CMAKE_CURRENT_LIST_DIR}/../Thirdparty/Qt5.9.8/macosx/5.9.8/clang_64)
   endif()
endif()
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_DIR})


set(Qt5_DIR "${QT_DIR}/lib/cmake/Qt5")
set(Qt5Gui_DIR "${QT_DIR}/lib/cmake/Qt5Gui")
set(Qt5Qml_DIR "${QT_DIR}/lib/cmake/Qt5Qml")
set(Qt5Quick_DIR "${QT_DIR}/lib/cmake/Qt5Quick")
set(Qt5Multimedia_DIR "${QT_DIR}/lib/cmake/Qt5Multimedia")
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

include_directories(${PROJECT_BINARY_DIR})
include_directories(common)
include_directories(../include)
include_directories(../include/common)
include_directories(../include/rapidjson)
include_directories(Network)
include_directories(./)

if(WIN32)
    include_directories(Sound)
endif()

if(WIN32)
    set(${target}_COMMON
        common/fu_tool.cpp
        ../include/common/fu_tool.h
        common/fu_shared_obj.cpp
        ../include/common/fu_shared_obj.h
        )
endif()

set(${target}_Sound
    Sound/MP3.cpp
    Sound/MP3.h
    )
set(${target}_qml
      qml/main.qml
      qml/content/CustomSlider.qml
      qml/content/ListViewProp.qml
      qml/content/ListViewPropOption.qml
      qml/content/ListViewSide.qml
      qml/content/RectangleButtonGreenScreen.qml
      qml/content/RectangleTabContent.qml
      qml/content/TextBlack.qml
    )

set(${target}_SOURCES
    authpack.h
    Camera.h
    CameraDisplay.h
    Config.h
    FuController.h
    ImageProvider.h
    MainClass.h
    Nama.h
    UIBridge.h
    Camera.cpp
    CameraDisplay.cpp
    FuController.cpp
    ImageProvider.cpp
    MainClass.cpp
    Nama.cpp
    main.cpp
    UIBridge.cpp
    )

file(GLOB_RECURSE NETWORKFILES Network/*.*)
set(${target}_NETWORK
    ${NETWORKFILES}
)

file(GLOB_RECURSE TINFILES tinfiledlg/*.*)
set(${target}_TINFILEDLG
        ${TINFILES}
)

qt5_add_resources(qrc_FILES qml.qrc)

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "/SAFESEH:NO" )
    add_executable(${target}
        ${${target}_NETWORK}
        ${${target}_SOURCES}
        ${qrc_FILES}
        ${${target}_COMMON}
        ${${target}_Sound}
        ${${target}_qml}
        ${${target}_TINFILEDLG}
        )
endif()
source_group("Source Files" FILES ${${target}_SOURCES})
source_group("Source Files\\common" FILES ${${target}_COMMON})
source_group("Source Files\\sound" FILES ${${target}_Sound})
source_group("Source Files\\network" FILES ${${target}_NETWORK})
source_group("Source Files\\qml" FILES ${${target}_qml})

qt5_use_modules(${PROJECT_NAME} Core)
qt5_use_modules(${PROJECT_NAME} Gui)
qt5_use_modules(${PROJECT_NAME} Qml)
qt5_use_modules(${PROJECT_NAME} Quick)
qt5_use_modules(${PROJECT_NAME} Multimedia)

import_custom_library(${target} Nama)
import_custom_library(${target} Opencv)
import_custom_library(${target} FreeImage)
import_custom_library(${target} ffmpeg)
import_custom_library(${target} libcurl)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})

find_package(Qt5 COMPONENTS Core Quick REQUIRED)
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(DEPLOYQT_EXECUTABLE NAMES windeployqt macdeployqt HINTS "${_qt_bin_dir}")
if(APPLE)
    configure_file(${CMAKE_SOURCE_DIR}/Info.plist.in ${CMAKE_BINARY_DIR}/Info.plist)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_BINARY_DIR}/Info.plist)
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${DEPLOYQT_EXECUTABLE} "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>" -qmldir=${CMAKE_SOURCE_DIR}
    )
elseif(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${DEPLOYQT_EXECUTABLE} "$<TARGET_FILE:${PROJECT_NAME}>"
            --$<LOWER_CASE:$<CONFIG>> --qmldir ${CMAKE_SOURCE_DIR}
    )
endif()

if(WIN32)
    import_custom_library(${target} VirtualCamera)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
        set(SrcNamaDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/FaceUnity-SDK-PC/lib/win32)
        set(SrcFreeImageDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/FreeImage/lib/win32)
        set(SrcOpenCVDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/opencv400/lib/win32)
        set(SrcOpenCVDLLFileFFmpeg  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/opencv400/lib/win32/opencv_ffmpeg400.dll)
        set(SrcFfmpegDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/ffmpeg/lib/win32)
        set(SrcVirtualDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/VirtualCamera/lib/win32)
        set(SrcQTDLLFolderr  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Qt5.9.8/windows/5.9.8/msvc2017_64/bin)
        set(SrcQTpluginsDLLFolderr  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Qt5.9.8/windows/5.9.8/msvc2017_64/plugins)
        set(SrcCurlDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/libcurl/lib/win32)
    else()
        set(SrcNamaDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/FaceUnity-SDK-PC/lib/win64)
        set(SrcFreeImageDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/FreeImage/lib/win64)
        set(SrcOpenCVDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/opencv400/lib/win64)
        set(SrcOpenCVDLLFileFFmpeg  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/opencv400/lib/win64/opencv_ffmpeg400_64.dll)
        set(SrcFfmpegDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/ffmpeg/lib/win64)
        set(SrcVirtualDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/VirtualCamera/lib/win64)
        set(SrcQTDLLFolderr  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Qt5.9.8/windows/5.9.8/msvc2017_64/bin)
        set(SrcQTpluginsDLLFolderr  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Qt5.9.8/windows/5.9.8/msvc2017_64/plugins)
        set(SrcCurlDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/libcurl/lib/win64)
    endif()

    add_custom_command(TARGET ${target}
        POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}

        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcNamaDLLFolder}/fuai.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcNamaDLLFolder}/CNamaSDK.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFreeImageDLLFolder}/FreeImage.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcOpenCVDLLFileFFmpeg}     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcOpenCVDLLFolder}/opencv_world400.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/avutil-56.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/avformat-58.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/avcodec-58.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/swresample-3.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/libwinpthread-1.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcVirtualDLLFolder}/VirtualCamera.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcCurlDLLFolder}/libcurl.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcCurlDLLFolder}/libcrypto.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcCurlDLLFolder}/libssl.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Core.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Gui.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Multimedia.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Network.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Qml.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5QuickControls2.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Quick.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5QuickTemplates2.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5QuickTest.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Svg.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Test.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Widgets.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5WinExtras.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}

        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcNamaDLLFolder}/fuai.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcNamaDLLFolder}/CNamaSDK.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFreeImageDLLFolder}/FreeImage.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcOpenCVDLLFileFFmpeg}     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcOpenCVDLLFolder}/opencv_world400d.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/avutil-56.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/avformat-58.dll	${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/avcodec-58.dll	${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/swresample-3.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/libwinpthread-1.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcVirtualDLLFolder}/VirtualCamera.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcCurlDLLFolder}/libcurl.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcCurlDLLFolder}/libcrypto.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcCurlDLLFolder}/libssl.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Cored.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Guid.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Multimediad.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Networkd.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Qmld.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5QuickControls2d.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Quickd.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5QuickTemplates2d.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5QuickTestd.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Svgd.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Testd.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5Widgetsd.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#        COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcQTDLLFolderr}/Qt5WinExtrasd.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMENT "copy dll"

        )
endif()

