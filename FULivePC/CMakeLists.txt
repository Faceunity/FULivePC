cmake_minimum_required(VERSION 3.5)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version" FORCE)
project(FULivePC)
set(target FULivePC)
set(CMAKE_CXX_STANDARD 14)


if(WIN32)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/cmakeScripts/Modules)
    include(${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/cmakeScripts/import_custom_library.cmake)
elseif(APPLE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Mac/cmakeScripts/Modules)
    include(${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Mac/cmakeScripts/import_custom_library.cmake)
elseif(UNIX)
endif()

# add_definitions(
#     -DPERFORMANCE_OPEN
# )

if(APPLE)
add_definitions(
    -DIMGUI_IMPL_OPENGL_LOADER_GL3W
    -DGLFW_EXPOSE_NATIVE_COCOA
    -DGLFW_EXPOSE_NATIVE_NSGL
)
else() 
add_definitions(
    -DIMGUI_IMPL_OPENGL_LOADER_GL3W
    -DUNICODE
)
endif()

if(APPLE)
    find_library(AVFOUNDATION_LIB AVFoundation)
    find_library(COREMEDIA_LIB CoreMedia)
    find_library(FOUNDATION_LIB Foundation)
    find_library(COREVIDEO_LIB CoreVideo)
    find_library(COREFOUNDATION_LIB CoreFoundation)
    find_library(OPENGL_LIB OpenGL)
    find_library(APPKIT_LIB AppKit)
    find_library(COCOA_LIB Cocoa)
    find_library(SECURITY_LIB Security)
    find_library(GLKIT_LIB GLKit)
    find_library(COREAUDIO_LIB CoreAudio)
    find_library(AUDIOTOOLBOX_LIB AudioToolbox)
    find_library(VIDEOTOOLBOX_LIB VideoToolbox)
    find_library(LIBBZ2_1_0_TBD libbz2.1.0.tbd)
    find_library(LIBICONV_2_4_0_TBD libiconv.2.4.0.tbd)
    find_library(LIBLZMA_TBD liblzma.tbd)
    find_library(LIBZ_1_2_5_TBD libz.1.2.5.tbd)
    find_library(LIBCURL_TBD libcurl.4.tbd)
endif()


set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -fms-extensions")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fms-extensions")

include_directories(${PROJECT_BINARY_DIR})
include_directories(imgui)
include_directories(common)
include_directories(GUI)
include_directories(Network)
include_directories(../include)
include_directories(../include/common)
include_directories(../include/rapidjson)
include_directories(./)

if(WIN32)
include_directories(Sound)
endif()
if(APPLE)
include_directories(GLContext)
endif()


#Common

if(WIN32)
set(${target}_COMMON
    common/fu_tool.cpp
    ../include/common/fu_tool.h
    common/fu_shared_obj.cpp
    ../include/common/fu_shared_obj.h
)
else()
set(${target}_COMMON
    common/fu_tool.cpp
    ../include/common/fu_tool.h
    ../include/common/fu_tool_mac.h
    common/fu_tool_mac.mm
    common/fu_shared_obj.cpp
    ../include/common/fu_shared_obj.h
)
endif()

set(${target}_GLCONTEXT
    GLContext/NaMaGLContext.h
    GLContext/NaMaGLContext.mm
)

set(${target}_GUI
    GUI/Gui.cpp
    GUI/Gui.h
	GUI/GuiGS.cpp
    GUI/GuiGS.h
	GUI/GuiTool.cpp
    GUI/GuiTool.h
	GUI/GuiTabContent.cpp
    GUI/GuiTabContent.h
    GUI/Loader.cpp
    GUI/Loader.h
    GUI/Texture.cpp
    GUI/Texture.h
    GUI/MouseControl.cpp
    GUI/MouseControl.h
	GUI/GuiCustomMakeup.cpp
	GUI/GuiCustomMakeup.h
    GUI/GuiSticker.cpp
	GUI/GuiSticker.h
)

set(${target}_IMGUI
    imgui/gl3w.c
    imgui/gl3w.h
    imgui/imconfig.h
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imgui_draw.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_glfw.h
    imgui/imgui_impl_opengl3.cpp
    imgui/imgui_impl_opengl3.h
    imgui/imgui_internal.h
    imgui/imgui_tabs.h
    imgui/imgui_tabs.cpp
    imgui/stb_rect_pack.h
    imgui/stb_textedit.h
    imgui/stb_truetype.h
)

set(${target}_Sound
    Sound/MP3.cpp
    Sound/MP3.h
)
set(${target}_Sound_mac
    Sound/MP3_mac.mm
    Sound/MP3_mac.h
)

set(${target}_SOURCES
    Config.h
    Camera.cpp
    Camera.h
    FULivePC.cpp
    Nama.cpp
    Nama.h
    UIBridge.h
	FuController.cpp
    FuController.h
    BgSegUpdate.cpp
    BgSegUpdate.h
)

file(GLOB_RECURSE NETWORKFILES Network/*.*)
set(${target}_NETWORK
    ${NETWORKFILES}
)

file(GLOB_RECURSE TINFILES tinfiledlg/*.*)
set(${target}_TINFILEDLG
	${TINFILES}
)


if(WIN32)
list(APPEND ${target}_SOURCES
    UIBridge.cpp
)
else()
list(APPEND ${target}_SOURCES
    UIBridge.mm
)
endif()


if(WIN32)
set(CMAKE_EXE_LINKER_FLAGS "/SAFESEH:NO" )	 
add_executable(${target}
    ${${target}_SOURCES}
    ${${target}_IMGUI}
    ${${target}_COMMON}
    ${${target}_GUI}
    ${${target}_Sound}
	${${target}_TINFILEDLG}
    ${${target}_NETWORK}
)
elseif(APPLE)

set(APP_UI_RES
        ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/ResPic.bundle
        ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Resource.bundle
        ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/AppIcon.icns
)

add_executable(${target}
    ${${target}_SOURCES}
    ${${target}_IMGUI}
    ${${target}_COMMON}
    ${${target}_GUI}
    ${${target}_GLCONTEXT}
    ${${target}_Sound_mac}
    ${${target}_NETWORK}
    ${APP_UI_RES}
)

elseif(UNIX)
endif()


source_group("Source Files" FILES ${${target}_SOURCES})
source_group("Source Files\\imgui" FILES ${${target}_IMGUI})
source_group("Source Files\\common" FILES ${${target}_COMMON})
source_group("Source Files\\gui" FILES ${${target}_GUI})
source_group("Source Files\\network" FILES ${${target}_NETWORK})

if(WIN32)
source_group("Source Files\\sound" FILES ${${target}_Sound})
source_group("Source Files\\tinfiledlg" FILES ${${target}_TINFILEDLG})
elseif(APPLE)
source_group("Source Files\\glcontext" FILES ${${target}_GLCONTEXT})
source_group("Resources" FILES ${APP_UI_RES})
endif()


import_custom_library(${target} Nama)
import_custom_library(${target} Opencv)

import_custom_library(${target} FreeImage)
import_custom_library(${target} Glfw)
import_custom_library(${target} ffmpeg)
import_custom_library(${target} libcurl)


if(WIN32)

    import_custom_library(${target} VirtualCamera)

    # set_target_properties(${target} PROPERTIES COMPILE_FLAGS "/MP" )
	set_target_properties(${target} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../")
	
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
        set(SrcNamaDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/FaceUnity-SDK-PC/lib/win32)
        set(SrcFreeImageDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/FreeImage/lib/win32)
        set(SrcOpenCVDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/opencv400/lib/win32) 
		set(SrcOpenCVDLLFileFFmpeg  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/opencv400/lib/win32/opencv_ffmpeg400.dll)
		set(SrcFfmpegDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/ffmpeg/lib/win32)
		set(SrcVirtualDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/VirtualCamera/lib/win32)
        set(SrcCurlDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/libcurl/lib/win32)
	else()
        set(SrcNamaDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/FaceUnity-SDK-PC/lib/win64)
        set(SrcFreeImageDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/FreeImage/lib/win64)
        set(SrcOpenCVDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/opencv400/lib/win64)
		set(SrcOpenCVDLLFileFFmpeg  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/opencv400/lib/win64/opencv_ffmpeg400_64.dll)
		set(SrcFfmpegDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/ffmpeg/lib/win64)
		set(SrcVirtualDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Windows/VirtualCamera/lib/win64) 
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


                COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcNamaDLLFolder}/fuai.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
				COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcNamaDLLFolder}/CNamaSDK.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
                
                COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFreeImageDLLFolder}/FreeImage.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
                COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcOpenCVDLLFileFFmpeg}     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
                COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcOpenCVDLLFolder}/opencv_world400d.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}

				COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/avutil-56.dll 	${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
				COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/avformat-58.dll		${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
				COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/avcodec-58.dll		${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
				COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/swresample-3.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
				COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFfmpegDLLFolder}/libwinpthread-1.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
				COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcVirtualDLLFolder}/VirtualCamera.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}

                COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcCurlDLLFolder}/libcurl.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
                COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcCurlDLLFolder}/libcrypto.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
                COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcCurlDLLFolder}/libssl.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}

				COMMENT "copy dll"
                )

elseif(APPLE)

    target_link_libraries(${target}
        ${AVFOUNDATION_LIB}
        ${COREMEDIA_LIB}
        ${FOUNDATION_LIB}
        ${COREVIDEO_LIB}
        ${COREFOUNDATION_LIB}
        ${OPENGL_LIB}
        ${APPKIT_LIB}
        ${COCOA_LIB}
        ${SECURITY_LIB}
        ${GLKIT_LIB}
        ${COREAUDIO_LIB}
        ${AUDIOTOOLBOX_LIB}
		${VIDEOTOOLBOX_LIB}
        ${LIBBZ2_1_0_TBD}
        ${LIBICONV_2_4_0_TBD}
        ${LIBLZMA_TBD}
        ${LIBZ_1_2_5_TBD}
        ${LIBCURL_TBD}
    )
    set(MACOSX_BUNDLE_ICON_FILE AppIcon.icns)
    set_target_properties(${target} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Info.plist
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "FULivePC/FULivePC.entitlements"
        XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Developer ID Application: Faceunity Technology Co., Ltd. (35A6KD62ZJ)"
        XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "--deep"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.faceunity.FULiveMac"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "35A6KD62ZJ"
    )
    
    set( MacOS_APP_Content "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/FULivePC.app/Contents")
    
    # 如果不存在则创建，用于工程第一次运行的时候
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Resource.bundle")
    else()
        file(COPY ${CMAKE_CURRENT_LIST_DIR}/../assets DESTINATION ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Resource.bundle)
        file(COPY ${CMAKE_CURRENT_LIST_DIR}/../res/config.json DESTINATION ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Resource.bundle)
    endif()
        if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/ResPic.bundle")
    else()
        file(COPY ${CMAKE_CURRENT_LIST_DIR}/../res DESTINATION ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/ResPic.bundle)
    endif()

    add_custom_command(TARGET ${target} PRE_BUILD
        #需要先rm Resource.bundle 和 ResPic.bundle，否则删除时，无法刷新
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Resource.bundle
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/ResPic.bundle
        COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_LIST_DIR}/../assets  ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Resource.bundle
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/../res/config.json    ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Resource.bundle
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/../res    ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/ResPic.bundle
        COMMAND ${CMAKE_COMMAND} -E make_directory  ${MacOS_APP_Content}/Frameworks
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Mac/glfw/lib/*.dylib  ${MacOS_APP_Content}/Frameworks
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Mac/FaceUnity-SDK-Mac/lib/*.dylib  ${MacOS_APP_Content}/Frameworks
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Mac/libcurl/lib/*.dylib  ${MacOS_APP_Content}/Frameworks
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/Mac/Framework/*.dylib  ${MacOS_APP_Content}/Frameworks
    )


        set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/ResPic.bundle PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Resource.bundle PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/AppIcon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    elseif(UNIX)
    
endif()
