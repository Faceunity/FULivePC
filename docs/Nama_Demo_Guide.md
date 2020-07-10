# Demo running documentation-Windows 
### Updates：

2019-04-28 v6.0.0：

   - Optimize face detection, improve detection rate and improve performance
- Added the Light-Makeup function 
- Optimize the Facefusion 
- Optimize the segmentation precision
- Tongue tracking trackface logic support, Getfaceinfo support
- Added Avatar pinch function, need FUEditor 6.0.0 or above
- Optimize Beauty filter 
- Fix mebedtls symbol conflicts
- Hairdressing and Animoji props support FUEditor v5.6.0 and above. The rest of the props are compatible with any SDK.



project updates：

- The interface of the beauty props has changed, please pay attention to the synchronization code!
- anim_model.bundle and ardata_ex.bundle are deprecated，please delete related data and code

------
### Contents：
[TOC]

------
### 1. Introduction 
This document shows you how to integrate the Faceunity Nama SDK into your Windows project.  

------
### 2. File structure
This section describes the structure of the Demo file, the various directories, and the functions of important files.

```
+FULivePC
  +assets 			  	//resource
  +FULivePC				//sample code 
    +GUI				//GUI files
    +ipc				//ipc files
    +rapidjson			 //json lib file
    +Sound				 //sound lib file
    -Camera.cpp：Camera class, responsible for reading image frames from the camera
    -Camera.h：header file
    -Config.h：config file，Record the loading path of the prop file
    -FULivePC.cpp：
	-FULivePC.vcxproj：visual studio config file
	-FULivePC.vcxproj.filters：visual studio project file
	-FULivePC.vcxproj.user：visual studio user config file
	-imgui.ini：GUI initialization config file
	-Nama.cpp：shows how to call the Nama SDK interface
	-Nama.h：Shows how to call the header file of the interface class of the Nama SDK
	-UIBridge.h：Responsible for displaying the access and use of virtual cameras
	-ReadMe.txt：
  +docs					//doc
  +ThridParty			//Third-party library directory
  +include				//SDK directory
  +Win32   				//32-bit SDK directory
  +Win64   				//64-bit SDK directory
  -readme.md			//Project total document
```

------
### 3.Demo running  

#### 3.1 Develop environment
##### 3.1.1 Platform
```
Windows7 or above 
```
##### 3.1.2 Develop environment
```
vs2015
```

#### 3.2 Preparing  
- [Download demo](<https://github.com/Faceunity/FULivePC> )
- Get certificates:
  1. call **0571-88069272** 	
  2. send mail to **marketing@faceunity.com** 
#### 3.3 Configurations

- Place the certificate file authpack.h in the include folder

#### 3.4 Compile and run

- Double-click FULivePC.sln to open the project and press F5 to compile the program.

  ![](./imgs/img1.png)

------
### 4. FAQ 
- When the visual studio version is not the 2015 version, it will prompt an error at compile time.
- **Since Github does not support uploading files larger than 100MB, FULivePC\ThridParty\opencv400\opencv\build\x64\vc14\bin\opencv_world400d.rar is a compressed dll, please unpack it yourself!**

