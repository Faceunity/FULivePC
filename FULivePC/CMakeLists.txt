cmake_minimum_required(VERSION 3.5)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version" FORCE)
project(FULivePC)
set(target FULivePC)
set(CMAKE_CXX_STANDARD 11)


if(WIN32)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Windows/cmakeScripts/Modules)
    include(${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Windows/cmakeScripts/import_custom_library.cmake)
elseif(APPLE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Mac/cmakeScripts/Modules)
    include(${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Mac/cmakeScripts/import_custom_library.cmake)
elseif(UNIX)
endif()

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
endif()

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -fms-extensions")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fms-extensions")

include_directories(${PROJECT_BINARY_DIR})
include_directories(imgui)
include_directories(common)
include_directories(GUI)
include_directories(../include)
include_directories(../include/common)
include_directories(../include/rapidjson)
include_directories(./)

if(WIN32)
include_directories(ipc)
include_directories(Sound)
endif()
if(APPLE)
include_directories(GLContext)
endif()


#Common

if(WIN32)
set(${target}_COMMON
    common/fu_tool.cpp
)
else()
set(${target}_COMMON
    common/fu_tool.cpp
    common/fu_tool_mac.mm
)
endif()

set(${target}_GLCONTEXT
    GLContext/NaMaGLContext.h
    GLContext/NaMaGLContext.mm
)

set(${target}_GUI
    GUI/Gui.cpp
    GUI/Gui.h
    GUI/Loader.cpp
    GUI/Loader.h
    GUI/Texture.cpp
    GUI/Texture.h
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


set(${target}_IPC
    ipc/filtercommons.h
    ipc/ipcbridge.cpp
    ipc/ipcbridge.h
    ipc/mutex.cpp
    ipc/mutex.h
    ipc/waitcondition.cpp
    ipc/waitcondition.h
)

set(${target}_Sound
    Sound/MP3.cpp
    Sound/MP3.h
)

set(${target}_SOURCES
    Camera.cpp
    Camera.h
    FULivePC.cpp
    Nama.cpp
    Nama.h
    UIBridge.h
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
add_executable(${target}
    ${${target}_SOURCES}
    ${${target}_IMGUI}
    ${${target}_COMMON}
    ${${target}_GUI}
    ${${target}_IPC}
    ${${target}_Sound}
)
elseif(APPLE)

set(APP_UI_RES
        ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/ResPic.bundle
        ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Resource.bundle
)

add_executable(${target}
    ${${target}_SOURCES}
    ${${target}_IMGUI}
    ${${target}_COMMON}
    ${${target}_GUI}
    ${${target}_GLCONTEXT}
    ${APP_UI_RES}
)

elseif(UNIX)
endif()


source_group("Source Files" FILES ${${target}_SOURCES})
source_group("Source Files\\imgui" FILES ${${target}_IMGUI})
source_group("Source Files\\common" FILES ${${target}_COMMON})
source_group("Source Files\\gui" FILES ${${target}_GUI})

if(WIN32)
source_group("Source Files\\ipc" FILES ${${target}_IPC})
source_group("Source Files\\sound" FILES ${${target}_Sound})
elseif(APPLE)
source_group("Source Files\\glcontext" FILES ${${target}_GLCONTEXT})
source_group("Resources" FILES ${APP_UI_RES})
endif()


import_custom_library(${target} Nama)
import_custom_library(${target} Opencv)
import_custom_library(${target} FreeImage)
import_custom_library(${target} Glfw)

if(WIN32)
    set_target_properties(${target} PROPERTIES COMPILE_FLAGS "/MP" )
	set_target_properties(${target} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../")
	
	STRING(FIND $ENV{CMAKE_GENERATOR} "Win64" IsFinded)
    if(${IsFinded} EQUAL -1)
        set(SrcNamaDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Windows/FaceUnity-SDK-PC/lib/win32)
        set(SrcFreeImageDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Windows/FreeImage/lib/win32)
        set(SrcOpenCVDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Windows/opencv400/lib/win32)
		set(SrcOpenCVDLLFileFFmpeg  ${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Windows/opencv400/lib/win32/opencv_ffmpeg400.dll)
	else()
        set(SrcNamaDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Windows/FaceUnity-SDK-PC/lib/win64)
        set(SrcFreeImageDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Windows/FreeImage/lib/win64)
        set(SrcOpenCVDLLFolder  ${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Windows/opencv400/lib/win64)
		set(SrcOpenCVDLLFileFFmpeg  ${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Windows/opencv400/lib/win64/opencv_ffmpeg400_64.dll)
	endif()
	
	add_custom_command(TARGET ${target}
                POST_BUILD

				COMMAND ${CMAKE_COMMAND} -E make_directory $ENV{CMAKE_DEBUG_OUTPUT}
				COMMAND ${CMAKE_COMMAND} -E make_directory $ENV{CMAKE_RUNTIME_OUTPUT}

				COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcNamaDLLFolder}/fuai.dll     $ENV{CMAKE_DEBUG_OUTPUT}
				COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcNamaDLLFolder}/CNamaSDK.dll     $ENV{CMAKE_DEBUG_OUTPUT}
				COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcNamaDLLFolder}/fuai.dll 	$ENV{CMAKE_RUNTIME_OUTPUT}
				COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcNamaDLLFolder}/CNamaSDK.dll		$ENV{CMAKE_RUNTIME_OUTPUT}

                COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFreeImageDLLFolder}/FreeImage.dll     $ENV{CMAKE_DEBUG_OUTPUT}
                COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcOpenCVDLLFileFFmpeg}     $ENV{CMAKE_DEBUG_OUTPUT}
                COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcOpenCVDLLFolder}/opencv_world400d.dll     $ENV{CMAKE_DEBUG_OUTPUT}

				COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcFreeImageDLLFolder}/FreeImage.dll     $ENV{CMAKE_RUNTIME_OUTPUT}
                COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcOpenCVDLLFileFFmpeg}     $ENV{CMAKE_RUNTIME_OUTPUT}
                COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${SrcOpenCVDLLFolder}/opencv_world400.dll     $ENV{CMAKE_RUNTIME_OUTPUT}

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
    )

    set_target_properties(${target} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Info.plist
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
    )

    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/ResPic.bundle PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Resource.bundle PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    set( MacOS_APP_Content "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/FULivePC.app/Contents")
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_LIST_DIR}/../assets  ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Resource.bundle
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/../res/config.json    ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/Resource.bundle
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/../res    ${CMAKE_CURRENT_LIST_DIR}/MAC_DATA/ResPic.bundle
        COMMAND ${CMAKE_COMMAND} -E make_directory  ${MacOS_APP_Content}/Frameworks
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Mac/glfw/lib/*.dylib  ${MacOS_APP_Content}/Frameworks
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Mac/FaceUnity-SDK-Mac/lib/*.dylib  ${MacOS_APP_Content}/Frameworks
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/../ThridParty/Mac/Framework/*.dylib  ${MacOS_APP_Content}/Frameworks
    )

elseif(UNIX)
endif()
