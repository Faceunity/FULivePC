# Demo running documentation-Mac  



### Updates：

2019-04-29 v1.2.0： Nama 6.0.0

- Optimize face detection, improve detection rate and improve performance
- Added the Light-Makeup function 
- Optimize the Facefusion 
- Optimize thesegmentation precision
- Tongue tracking trackface logic support, Getfaceinfo support
- Added Avatar pinch function, need FUEditor 6.0.0 or above
- Optimize Beauty filter 
- Fix mebedtls symbol conflicts
- Hairdressing and Animoji props support FUEditor v5.6.0 and above. The rest of the props are compatible with any SDK.

Demo：
- added the light make up
-  update Nama 6.0.0

------
### Contents：
[TOC]

------
### 1. Introduction 
This document shows you how to run the Mac Demo of the Faceunity Nama SDK and experience the functions of the Faceunity Nama SDK. 

FULiveDemoMac is an example of the integrated facial tracking, beauty, Animoji, props stickers, AR meh, face transfer, expression recognition, music filters, background segmentation, gesture recognition, distorting mirrors, portrait lighting, and portrait drivers.

------
### 2. File structure
This section describes the structure of the Demo file, the various directories, and the functions of important files.

```
+FULiveDemo
  +FULiveDemo 			  	//source code directory
    +Controller             //controller file
      -FUCameraSetController.h.m   //camera controller
      -FUBeautyViewController.h.m  //beauty controller
      ...
    +Model                  //date model
      -FUBeautyModel        //beauty date model
      -FUMakeupModle        //make up date model
      ...
    +Views                  //view  
      -FUBeautyTableView              //beauty table
      +OpenGLView           //0penGLView 
      ...  
    +Managers				//business file 
      -FUManager             //nama class 
      -FUAppDataCenter       	  //data manage class
      -FUConstManager       //const class
      ...
    +Lib                    //nama SDK  
      +Faceunity
        -authpack.h             //permission file
        +FaceUnity-SDK-Mac
          +Headers
            -funama.h                //C interface
            -FURenderer.h            //OC interface
          +Resources               //SDK core resource 
            -ardata_ex.bundle    //High precision data model
            -tongue.bundle       //Tongue drive data model
            -fxaa.bundle         //3d Anti-aliasing props
          -libnama.a               //Nama lib
        +items                   //prop resource 
  +Other						//others
    +Category               //category
    +PrefixHeader           //Hearder
```

------
### 3.Demo running  

#### 3.1 Develop environment
##### 3.1.1 platform
```
macOS 10.6 or above
```
##### 3.1.2 Develop environment
```
Xcode 8 or above
```

#### 3.2 Preparing 
- [download FULiveDemoMac](https://github.com/Faceunity/FULiveDemoMac)
- replace **authpack.h**，refer to **3.3.1**

#### 3.3 Configurations
##### 3.3.1 import
Get certificates:

1. call **0571-88069272** 
2. send mail to **marketing@faceunity.com** 

The certificate issued by iOS is the g_auth_package array contained in authpack.h. If you have obtained the authentication certificate, import authpack.h into the project. Depending on the application requirements, authentication data can also be provided at runtime (such as network downloads), but pay attention to the risk of certificate leakage and prevent the certificate from being abused.

#### 3.4 Compile and running
![](./imgs/runDemo.jpg)

------
### 4. FAQ 

#### 4.1 running error

The first running demo will report the error of the missing certificate. If you already have the certificate issued by our company, replace the certificate with the project and re-run it.

