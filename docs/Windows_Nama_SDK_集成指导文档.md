# Windows Nama SDK 集成指导文档  
级别：Public
更新日期：2019-05-27

------
## 最新更新内容：

2019-05-27 v6.1.0：

1. 新增fuSetupLocal函数，支持离线鉴权。  
2. 新增fuDestroyLibData函数，支持tracker内存释放。  
3. 美型优化新增v脸、小脸、窄脸三款瘦脸形式。  
4. 优化“表情动图”功能，玩转图片表情包。  
5. 人脸跟踪模块优化边缘人脸抖动问题。  
6. 修复并支持i420格式。  
7. 修复asan问题。  

------
## 目录：
本文档内容目录：

[TOC]

------
## 1. 简介 
本文档旨在说明如何将Faceunity Nama SDK集成您的Windows工程中。  

------


## 2. SDK文件结构

本小节，描述Demo文件结构，各个目录，以及重要文件的功能。

```
+FULivePC
  +assets 			  	//资源目录
  +FULivePC				//示例代码目录
    +GUI				//GUI文件目录
    +ipc				//ipc文件目录
    +rapidjson			 //json库文件目录
    +Sound				 //声音库文件目录
    -Camera.cpp：相机类，负责从摄像头内读取图像帧
    -Camera.h：相机类的头文件
    -Config.h：配置文件，记录道具文件的加载路径
    -FULivePC.cpp：程序入口
	-FULivePC.vcxproj：visual studio工程配置文件
	-FULivePC.vcxproj.filters：visual studio工程目录文件
	-FULivePC.vcxproj.user：visual studio用户配置文件
	-imgui.ini：GUI的初始化配置文件
	-Nama.cpp：负责展示如何调用Nama SDK的接口
	-Nama.h：展示如何调用Nama SDK的接口类的头文件
	-UIBridge.h：负责展示虚拟摄像头的接入与使用
	-ReadMe.txt：简单的接入说明
  +docs					//文档目录
  +ThridParty			//第三方库目录
  +include				//SDK包含目录
  +Win32   				//32位SDK目录
  +Win64   				//64位SDK目录
  -readme.md			//工程总文档
  
```

------
## 3. 集成指引


### 3.1 开发环境
#### 3.1.1 支持平台
```
Windows7及以上
```
#### 3.1.2 开发环境
```
vs2015
```

### 3.2 准备工作 
- [下载demo代码](<https://github.com/Faceunity/FULivePC> )
- 获取证书:
  1. 拨打电话 **0571-88069272** 
  2. 发送邮件至 **marketing@faceunity.com** 进行咨询。

### 3.3 相关配置
- 将证书文件authpack.h放置到include文件夹内（authpack.h由3.2步骤申请到）。

- 如需要将本SDK集成到你的工程中去：
  1. 请将demo中的assets、include、Win32\Win64文件夹的所有文件及文件夹复制到你的工程中去。
  2. 同时配置OpenGL渲染环境，配置方法参考demo。
  3. 参考Nama.cpp代码流程进行集成。

### 3.4 初始化

```c
#include <funama.h>					//nama SDK 的头文件
#include <authpack.h>				//nama SDK 的key文件
#pragma comment(lib, "nama.lib")	//nama SDK 的lib文件
//然后将nama.dll拷贝到程序运行的目录
```

1.如未有初始化好的OpenGL环境，请查看demo代码中Nama.cpp中的InitOpenGL函数进行初始化，一个初始化好的OpenGL环境是正确使用FaceUnity SDK的必须条件，如果不知道自己是否成功初始化好了环境，可以参考Nama.cpp中的CheckGLContext函数进行检测。

2.fuSetup函数就是整个SDK的初始化开始，这个调用意味着人脸识别和道具渲染等其他功能开始就绪。一个应用程序只需要调用一次此函数，然后在函数初始化的同一线程里所有SDK函数均可正常运行。目前不支持跨线程的SDK初始化与调用。

### 3.5 输入图像数据

本demo使用Camera.cpp中的代码来获取摄像头的图像帧数据。**如果你想输入自己的图像数据，可直接替换图像内容而不使用Camera类。**

FaceUnity SDK中的核心渲染函数：

```c
int fuRenderItemsEx2(
	int out_format,void* out_ptr,
	int in_format,void* in_ptr,
	int w,int h,int frame_id, int* p_items,int n_items,
	int func_flag, void* p_item_masks);
```

在Nama.cpp中的RenderItems这个函数展示了如何输入图像数据。

```c
int handle[] = { mBeautyHandles,mLightMakeUpHandle, UIBridge::m_curRenderItem };
int handleSize = sizeof(handle) / sizeof(handle[0]);
//支持的格式有FU_FORMAT_BGRA_BUFFER 、 FU_FORMAT_NV21_BUFFER 、FU_FORMAT_I420_BUFFER 、FU_FORMAT_RGBA_BUFFER		
fuRenderItemsEx2(FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame), FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame),mFrameWidth, mFrameHeight, mFrameID, handle, handleSize, NAMA_RENDER_FEATURE_FULL, NULL);
```

以上我们示例将一个图像的RGBA排列格式的buffer作为本SDK输入图像数据。

`FU_FORMAT_NV21_BUFFER` NV21格式的buffer数组，通常在安卓设备上，通过原生相机插件使用

`FU_FORMAT_BGRA_BUFFER`  BGRA格式的buffer数组，一般图像库的默认格式

`FU_FORMAT_RGBA_BUFFER` RGBA格式的buffer数组，通用性最强

`FU_FORMAT_I420_BUFFER` I420格式的buffer数组

### 3.6 输出跟踪数据

fuGetFaceInfo可以来获取每帧的人脸跟踪数据。

```c
float landmarks[150];
int ret = 0;
ret = fuGetFaceInfo(0, "landmarks", landmarks, sizeof(landmarks) / sizeof(landmarks[0]));
```

**上面的代码示例了如何获取人脸跟踪点landmarks** 

在每一帧图像进行render或者trackface后，均可进行人脸跟踪数据的获取。

**关于舌头跟踪：**

1.必须加载tongue.bundle

2.在代码中加载带舌头功能的道具，比如青蛙，即可展现出舌头跟踪效果。

### 3.7 输出图像数据（道具创建、销毁、切换）

####  3.7.1道具创建

之后进行道具加载，函数返回的道具 handle 要保存在当前程序中，之后绘制道具时需要提供该 handle 作为道具的标识。其中 data 为指向道具文件内容的指针，size 为该文件的字节大小。

```c
static int mItemsArray[2] = {0, 0};
...
if (!mItemsArray[0]){
  mItemsArray[0] = fuCreateItemFromPackage(data, size);
}
```

#### 3.7.2道具销毁

##### 3.7.2.1销毁单个道具

```c
void fuDestroyItem(int item);
```

参数说明：

`item ` 要销毁的道具句柄

该接口将释放传入的句柄所对应的资源。示例如下：

```c
if (mItemsArray[0] > 0)
    fuDestroyItem(mItemsArray[0]);
```

##### 3.7.2.2销毁全部道具

```c
void fuDestroyAllItems();
```

该接口可以销毁全部道具句柄所对应的资源,同样在执行完该接口后请将所有句柄都置为0。示例如下：

```java
fuDestroyAllItems();
```

#### 3.7.3道具切换

如果需要切换句柄数组中某一位的句柄时，需要先创建一个新的道具句柄，并将该句柄替换到句柄数组中需要被替换的位置上，最后再把被替换的句柄销毁掉。下面以替换句柄数组的第ITEM_ARRAYS_EFFECT位为例进行说明：

```c
int newEffectItem = loadItem(path);
if (mItemsArray[0] > 0) {
    fuDestroyItem(mItemsArray[0]);
}
mItemsArray[0] = newEffectItem;
```

------
## 4. 功能模块
### 4.1 视频美颜

#### 美颜处理

视频美颜配置方法与视频加特效道具类似，首先创建美颜道具句柄，并保存在句柄数组中:

```c
mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

在处理视频时，美颜道具句柄会通过句柄数组传入图像处理接口，处理完成后美颜效果将会被作用到图像中。示例如下：

```c
fuRenderItemsEx2(FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame), FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame),mFrameWidth, mFrameHeight, mFrameID, mItemsArray, mItemsArraySize, NAMA_RENDER_FEATURE_FULL, NULL);
```

#### 参数设置

美颜道具主要包含七个模块的内容：滤镜、美白、红润、磨皮、亮眼、美牙、美型。

美颜道具的所有功能都通过设置参数来进行设置，以下为设置参数的代码示例：

```c
#define MAX_FACESHAPEPARAMTER 6
std::string faceBeautyParamName[] = { "blur_level","color_level", "red_level", "eye_bright", "tooth_whiten" };
std::string faceShapeParamName[] = { "cheek_thinning","eye_enlarging", "intensity_chin", "intensity_forehead", "intensity_nose","intensity_mouth" };
for (int i=0;i<MAX_BEAUTYFACEPARAMTER;i++)
{		
    if (i==0)//磨皮
    {
        fuItemSetParamd(mBeautyHandles, const_cast<char*>(faceBeautyParamName[i].c_str()), UIBridge::mFaceBeautyLevel[i] *6.0/ 100.f);
    } 
    else
    {
        fuItemSetParamd(mBeautyHandles, const_cast<char*>(faceBeautyParamName[i].c_str()), UIBridge::mFaceBeautyLevel[i] / 100.f);
    }		
}

for (int i=0;i<MAX_FACESHAPEPARAMTER;i++)
{
    fuItemSetParamd(mBeautyHandles, const_cast<char*>(faceShapeParamName[i].c_str()), UIBridge::mFaceShapeLevel[i]/100.0f);		
}
fuItemSetParamd(mBeautyHandles, "skin_detect", UIBridge::mEnableSkinDect);	
fuItemSetParamd(mBeautyHandles, "heavy_blur", UIBridge::mEnableHeayBlur);
fuItemSetParamd(mBeautyHandles, "face_shape_level", 1);
fuItemSetParamd(mBeautyHandles, "filter_level", UIBridge::mFilterLevel[UIBridge::m_curFilterIdx]/100.0f);
```

每个模块都有默认效果，它们可以调节的参数如下。

#### 一、滤镜

滤镜功能主要通过参数filter_level 和 filter_name来控制

```
filter_level 取值范围 0.0-1.0,0.0为无效果，1.0为最大效果，默认值1.0
filter_name 取值为一个字符串，默认值为 “origin” ，origin即为使用原图效果
```

filter_name参数的取值和相关介绍详见：[美颜道具功能文档](./美颜道具功能文档.md) ，在 **滤镜对应key值** 部分有详细介绍，对于老版本（6.0之前）的用户，可以参考 **新老滤镜对应关系** 部分。

#### 二、美白和红润

##### 美白

美白功能主要通过参数color_level来控制

```
color_level 取值范围 0.0-1.0,0.0为无效果，1.0为最大效果，默认值0.2
```

##### 红润

红润功能主要通过参数red_level 来控制

```
red_level 取值范围 0.0-1.0,0.0为无效果，1.0为最大效果，默认值0.5
```

#### 三、磨皮

控制磨皮的参数有四个：blur_level，skin_detect，nonshin_blur_scale，heavy_blur

```
blur_level: 磨皮程度，取值范围0.0-6.0，默认6.0
skin_detect:肤色检测开关，0为关，1为开
nonskin_blur_scale:肤色检测之后非肤色区域的融合程度，取值范围0.0-1.0，默认0.45
heavy_blur: 重度磨皮开关，0为清晰磨皮，1为重度磨皮
```

注意：重度磨皮为高级美颜功能，需要相应证书权限才能使用 

#### 四、亮眼

亮眼功能主要通过参数eye_bright 来控制

```
eye_bright   取值范围 0.0-1.0,0.0为无效果，1.0为最大效果，默认值1.0
```

注意：亮眼为高级美颜功能，需要相应证书权限才能使用 

#### 五、美牙

美牙功能主要通过参数tooth_whiten来控制

```
tooth_whiten   取值范围 0.0-1.0,0.0为无效果，1.0为最大效果，默认值1.0
```

注意：美牙为高级美颜功能，需要相应证书权限才能使用 

#### 六、美型

美型的整体程度由face_shape_level参数控制

```
face_shape_level   取值范围 0.0-1.0,0.0为无效果，1.0为最大效果，默认值1.0
```

美型的渐变由change_frames参数控制

```
change_frames       0为关闭 ，大于0开启渐变，值为渐变所需要的帧数
```

美型的种类主要由face_shape 参数控制

```
face_shape: 变形取值 0:女神变形 1:网红变形 2:自然变形 3:默认变形 4:精细变形
```

在face_shape选取不同参数时，对应可以使用的参数也不同：

##### face_shape参数详解

```
1.
face_shape 为0 1 2 3时
对应0：女神 1：网红 2：自然 3：默认
可以使用参数
eye_enlarging:  默认0.5,  //大眼程度范围0.0-1.0
cheek_thinning: 默认0.0,   //v脸程度范围0.0-1.0
2.
face_shape 为4时，为精细变形，添加了鼻子额头嘴巴下巴的调整
可以使用参数
eye_enlarging:  默认0.5,  //大眼程度范围0.0-1.0
cheek_thinning: 默认0.0,   //v脸程度范围0.0-1.0
intensity_nose: 默认0.0,       //瘦鼻程度范围0.0-1.0
intensity_forehead: 默认0.5,   //额头调整程度范围0.0-1.0
intensity_mouth:默认0.5,       //嘴巴调整程度范围0.0-1.0
intensity_chin: 默认0.5,       //下巴调整程度范围0.0-1.0
```

注意：变形为高级美颜功能，需要相应证书权限才能使用 

### 4.2 Animoji

Animoji道具是一种特殊的avatar道具，可以跟随识别到的人脸做出对应的表情，加载方式如下：

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

Animoji默认渲染时不会跟踪人脸位置，但是可以通过开启AR模式来开启跟踪和背景图像显示：

```c
fuItemSetParamd(UIBridge::m_curRenderItem, "{\"thing\":\"<global>\",\"param\":\"follow\"} ", 1);
```

镜像相关参数：

```c
//is3DFlipH 参数是用于对3D道具的顶点镜像
fuItemSetParamd(itemid, "is3DFlipH", param);
//isFlipExpr 参数是用于对道具内部的表情系数的镜像
fuItemSetParamd(itemid, "isFlipExpr", param);
//isFlipTrack 参数是用于对道具的人脸跟踪位置旋转的镜像
fuItemSetParamd(itemid, "isFlipTrack", param);
//isFlipLight 参数是用于对道具内部的灯光的镜像
fuItemSetParamd(itemid, "isFlipLight", param);
```

默认人脸识别方向，会影响Animoji默认方向：

```c
fuSetDefaultOrientation(0);
```

### 4.3 动漫滤镜

动漫是一个单独的滤镜道具，可以如下方式加载使用

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

设置参数来切换滤镜风格：

```c
fuItemSetParamd(mItemsArray[0], "style", 0);
//你可以尝试设置0以外的整数
```

### 4.4 AR面具

ar面具可以很贴合的在人脸上戴上一层面具，加载方式如下

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.5 背景分割

背景分割道具可以识别到图像中人的区域，并将其与背景区别开来，然后在背景处绘制图案进行分割显示。加载方式如下：

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.6 换脸

可以将识别到的人脸替换为制作道具时的目标图片的人脸。

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.7 表情识别

表情识别道具是在普通的道具里加入额外的识别表情然后触发特定动作的功能，加载方式如下

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.8 手势识别

目前我们的手势识别功能也是以道具的形式进行加载的。一个手势识别的道具中包含了要识别的手势、识别到该手势时触发的动效、及控制脚本。加载该道具的过程和加载普通道具、美颜道具的方法一致。

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

线上例子中 GestureRecognition文件夹下的bundle 为手势演示道具。将其作为道具加载进行绘制即可启用手势识别功能。手势识别道具可以和普通道具及美颜共存，类似美颜将手势道具句柄保存在items句柄数组即可。

自定义手势道具的流程和2D道具制作一致，具体打包的细节可以联系我司技术支持。

手势识别的旋转镜像参数为独有：

```c
//rotMode为手势识别方向
fuItemSetParamd(itemid, "rotMode", 2);
//loc_x_flip为渲染X轴镜像
fuItemSetParamd(itemid, "loc_x_flip", 1.0);
```

### 4.9 贴纸

贴纸为基础的道具称呼。加载方式如下

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.10 人脸夸张变形功能

人脸夸张变形道具可以将人脸进行比较夸张的变形。加载方式如下

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.11 音乐滤镜

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

音乐滤镜是使用播放音乐的时间戳进行驱动的，在每次处理图像前，将音乐的播放进度传入音乐滤镜道具即可，方式如下：通过随着音乐设置播放时间才能让滤镜“动”一起

```c
fuItemSetParamd(name, "music_time",  mp3Map[UIBridge::m_curRenderItem]->GetCurrentPosition() / 1e4);
```

如果没有音乐则可以模拟音乐播放进度，demo中提供的道具对应的音乐时长为28s，换算成ms为28000ms，在没有音乐的情况下，可以从加载音乐滤镜开始计时，每次处理图像前获取一下当前时间与开始加载音乐滤镜的时间差，转换成ms传入音乐滤镜即可，当时间差超过28000ms时归0重新开始计时即可。效果详见FULiveDemoPC，道具可以通过FUEditor进行制作（v4.2.1及以上）。

### 4.12 照片驱动功能

针对照片进行精确的人脸重建，然后支持实时表情驱动，预置表情播放。可以用于实时应用，也可以用于生成表情包等。

该功能的资源有两种方式生成方式：

- 使用FUEditor v4.3.0以上版本离线制作道具
- 利用相芯提供的云服务在线上传照片生成道具
  在线云服务的方式请联系技术支持获取更多细节。

__使用方法__：

- 直接加载对应的道具

- 需要带有照片驱动权限的证书

  制作好道具后如下加载即可显示。

```c#
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.13 质感美颜

加载方式如下

```c#
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

这个道具的调用方法相对比较复杂：

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
## 5. 常见问题 

### 5.1 编译相关

- visual studio 2015版本 安装时勾选c++部件。
