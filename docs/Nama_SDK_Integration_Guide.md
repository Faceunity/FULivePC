# Windows Nama SDK Integration Guide  
------
## Updates：

2019-04-28 v6.0.0：

1. The interface of the beauty props has changed, please pay attention to the synchronization code!
2. anim_model.bundle and ardata_ex.bundle are deprecated，please delete related data and code

------
## Contents：
[TOC]

------
## 1. Introduction 
This document shows you how to integrate the Faceunity Nama SDK into your Windows project.  

------


## 2. File structure

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
## 3. Integration


### 3.1 Develop environment
#### 3.1.1 Platform
```
Windows7 or above
```
#### 3.1.2 Develop environment
```
vs2015
```

### 3.2 Preparing 
- [Download demo](https://github.com/Faceunity/FULivePC )
- Get certificates:
  1. call **0571-88069272** 	
  2. send mail to **marketing@faceunity.com** 

### 3.3 Configurations
- Place the certificate file authpack.h in the include folder.

- If you need to integrate this SDK into your project：
  1. Please copy all the files and folders of the assets, include, Win32\Win64 folder in the demo to your project.
  2. Configure the OpenGL rendering environment. The configuration method refers to the demo.
  3. Refer to the Nama.cpp code flow for integration.

### 3.4 Initialization

```c
#include <funama.h>					//nama SDK header file
#include <authpack.h>				//nama SDK key file
#pragma comment(lib, "nama.lib")	//nama SDK lib file
//copy nama.dll to the directory the program is running.
```

1.If there is no initialized OpenGL environment, please check the InitOpenGL function in Nama.cpp in the demo code for initialization. An initialized OpenGL environment is a necessary condition for the correct use of the FaceUnity SDK. If you do not know whether you have successfully initialized the environment, You can refer to the CheckGLContext function in Nama.cpp for detection.

2.The fuSetup function is the initialization of the entire SDK. This call means that other functions such as face recognition and item rendering are ready. An application only needs to call this function once, and then all SDK functions can run normally in the same thread that the function initializes. Cross-threaded SDK initialization and invocation are not currently supported.

### 3.5 Input image data

This demo uses the code in Camera.cpp to get the image frame data of the camera. If you want to enter your own image data, you can directly replace the image content without using the Camera class.

Core rendering functions in the FaceUnity SDK：

```c
int fuRenderItemsEx2(
	int out_format,void* out_ptr,
	int in_format,void* in_ptr,
	int w,int h,int frame_id, int* p_items,int n_items,
	int func_flag, void* p_item_masks);
```

The RenderItems function in Nama.cpp shows how to enter image data.

```c
int handle[] = { mBeautyHandles,mLightMakeUpHandle, UIBridge::m_curRenderItem };
int handleSize = sizeof(handle) / sizeof(handle[0]);
//support FU_FORMAT_BGRA_BUFFER 、 FU_FORMAT_NV21_BUFFER 、FU_FORMAT_I420_BUFFER 、FU_FORMAT_RGBA_BUFFER		
fuRenderItemsEx2(FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame), FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame),mFrameWidth, mFrameHeight, mFrameID, handle, handleSize, NAMA_RENDER_FEATURE_FULL, NULL);
```

In the above example, we use the buffer of the RGBA arrangement format of an image as the input image data of this SDK.

`FU_FORMAT_NV21_BUFFER`: NV21 Formatted buffer array, usually on Android devices, used by native camera plugins

`FU_FORMAT_BGRA_BUFFER` : Buffer format in BGRA format, the default format of general image library

`FU_FORMAT_RGBA_BUFFER` : Buffer array in RGBA format, the most versatile

`FU_FORMAT_I420_BUFFER`:  I420 format buffer array

### 3.6 Output tracking data

fuGetFaceInfo can be used to get face tracking data for each frame.

```c
float landmarks[150];
int ret = 0;
ret = fuGetFaceInfo(0, "landmarks", landmarks, sizeof(landmarks) / sizeof(landmarks[0]));
```

The above code shows how to get face tracking points landmarks

Face tracking data can be acquired after each frame image is rendered or trackface.

**About tongue tracking：**

1.Must load the tongue.bundle

2.Loading a tongue-like item in the code, such as a frog, reveals the tongue tracking effect.

### 3.7 Output image data

####  3.7.1 Prop creation

After the item is loaded, the item handle returned by the function is saved in the current program, and then the handle is required to be used as the item identifier when drawing the item. Where data is a pointer to the contents of the item file, and size is the byte size of the file.

```c
static int mItemsArray[2] = {0, 0};
...
if (!mItemsArray[0]){
  mItemsArray[0] = fuCreateItemFromPackage(data, size);
}
```

#### 3.7.2 Prop destruction

##### 3.7.2.1 Destroy a single prop

```c
void fuDestroyItem(int item);
```

parameter：

`item `:  The handle of the item to be destroyed

This interface will release the resource corresponding to the incoming handle. An example is as follows:

```c
if (mItemsArray[0] > 0)
    fuDestroyItem(mItemsArray[0]);
```

##### 3.7.2.2 Destroy all props

```c
void fuDestroyAllItems();
```

This interface can destroy all the resources corresponding to the item handle, and also set all handles to 0 after executing the interface. Examples are as follows：

```java
fuDestroyAllItems();
```

#### 3.7.3 Prop switching

If you need to switch the handle of a bit in the handle array, you need to create a new item handle, replace the handle with the position in the handle array that needs to be replaced, and finally destroy the replaced handle. The following is an example of replacing the first ITEM_ARRAYS_EFFECT bit of the handle array:

```c
int newEffectItem = loadItem(path);
if (mItemsArray[0] > 0) {
    fuDestroyItem(mItemsArray[0]);
}
mItemsArray[0] = newEffectItem;
```

------
## 4. Function
### 4.1 Video beautification

please refer to [Beautification Filters User Specification](Beautification_Filters_User_Specification.md)。

### 4.2 Animoji

The Animoji prop is a special avatar prop that can follow the recognized face to make a corresponding expression. The loading method is as follows:

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

Animoji does not track face position when rendering by default, but can turn on tracking and background image display by turning on AR mode.：

```c
fuItemSetParamd(UIBridge::m_curRenderItem, "{\"thing\":\"<global>\",\"param\":\"follow\"} ", 1);
```

Mirror related parameters：

```c
//is3DFlipH  mirror the vertex of the 3D prop
fuItemSetParamd(itemid, "is3DFlipH", param);
//isFlipExpr mirror the expression coefficients inside the item.
fuItemSetParamd(itemid, "isFlipExpr", param);
//isFlipTrack The parameter is a mirror image used to rotate the face tracking position of the item.
fuItemSetParamd(itemid, "isFlipTrack", param);
//isFlipLight The parameter is used to mirror the light inside the item.
fuItemSetParamd(itemid, "isFlipLight", param);
```

The default face recognition direction will affect the default direction of Animoji：

```c
fuSetDefaultOrientation(0);
```

### 4.3 Comic Filter

Anime is a separate filter prop that can be loaded and used as follows:

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

Set parameters to switch filter style：

```c
fuItemSetParamd(mItemsArray[0], "style", 0);
//You can try setting an integer other than 0.
```

### 4.4 AR Mesh

Ar mask can fit a mask on the face of the person, the loading method is as follows

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.5 Background segmentation

The background segmentation item can recognize the area of the person in the image and distinguish it from the background, and then render a pattern at the background for segmentation display. The loading method is as follows:

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.6 Face Transfer

The recognized face can be replaced with the face of the target picture when the item is created.

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.7 Expression recognition

The expression recognition prop is to add an additional recognition expression to the common prop and then trigger a specific action. The loading method is as follows:

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.8 Gesture Recognition

Gesture recognition function is realized by loading prop item as well currently. Gesture recognition prop items cotain gesture to be recognized, animation effect triggered when gesture recognized and control script. Loading process is same with that of common prop items and face beautification prop item.

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

In the example, the bundle under the GestureRecognition folder is a gesture demo. Gesture recognition can be enabled by loading it as a prop. Gesture recognition props can coexist with common props and beauty, similar to beauty, the gesture props handles can be saved in the items handle array.

Process of customize gesture prop is same  with that of 2D prop item, please contact Faceunity technical support for details.

The rotation mirror parameter of gesture recognition is unique：

```c
//rotMode is the direction of the gesture
fuItemSetParamd(itemid, "rotMode", 2);
//Loc_x_flip for rendering X-axis mirroring
fuItemSetParamd(itemid, "loc_x_flip", 1.0);
```

### 4.9 Stickers

Loading method is as follows

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.10 Face Warp

Loading method is as follows

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.11 Music filter

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

Music filter is powered by time-stamp of playing music,  it can be utilized by introducing music playing course to music filter prop item every time processing video image, example code as below:

```c
fuItemSetParamd(name, "music_time",  mp3Map[UIBridge::m_curRenderItem]->GetCurrentPosition() / 1e4);
```

You can simulate music playing course if there is no music. Prop item in demo corresponds to music of 28s(28000ms) in length, time counting can be started from loading music filter prop item, get time difference between current time and time when starting loading music filter every time before processing video image and convert it into ms, set time to 0 when time difference is over 28000ms. You can check FULiveDemoPC for effect and create one utilizing FUEditor(v4.2.1 and above).

### 4.12 Dynamic Portrait Function

- Performe accurate face rebuild through photograph, then it will support real time face expression driving and preset face expression play. It can be used in real time application or be used to creat face expression package.

  This function contains two methods to creat resources:

  - Utilize FUEditor v4.3.0 and above to creat off line props

  - Utilize cloud service provided by Faceunity to upload photograph to creat props in real time.

    Please contact our technical support to get detailed information about cloud service.

  Instructions:

  - Load corresponding prop item directly
  - Certificate that possess Dynamic Portrait privilege is needed

  Once the props are created, they can be loaded as shown below.

```c#
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.13 Light Makeup

Loading like this:

```c#
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

The calling method of this prop is relatively complicated.：

```c
fuItemSetParamd((int)mLightMakeUpHandle, "is_makeup_on", 1);
fuItemSetParamd((int)mLightMakeUpHandle, "makeup_intensity", makeupitem.intensity);
                
fuItemSetParamdv((int)mLightMakeUpHandle, "makeup_lip_color", makeupitem.Lipstick_color);
fuItemSetParamd((int)mLightMakeUpHandle, "makeup_intensity_lip", makeupitem.Lipstick_intensity);
fuItemSetParamd((int)mLightMakeUpHandle, "makeup_lip_mask", 1.0);
...
Texture::SharedPtr pTexture = Texture::createTextureFromFile(mMakeupParams[index][j].textureName + ".png", false);
fuCreateTexForItem(mLightMakeUpHandle, const_cast<char*>(mMakeupParams[index][j].typeName.c_str()), pTexture->getData(), pTexture->m_width, pTexture->m_height);	
fuItemSetParamd(mLightMakeUpHandle, const_cast<char*>(mMakeupParams[index][j].valueName.c_str()), (float)mMakeupParams[index][j].value / 100.);
...
```

------
## 5. FAQ 

### 5.1 Compile related

- please check the c++ widget during the Visual studio 2015 version installation.
