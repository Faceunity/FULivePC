# Windows Nama SDK 集成指导文档  

级别：Public   
更新日期：2024-07-03   
SDK版本: 8.11.0

### 最新更新内容：

**2024-07-03 v8.11.0:

1. 【SDK升级】新增极速版策略，更好的兼容中低端机型，实现功能与性能的极致平衡，使用体验更丝滑
2. 【人脸关键点】算法升级，广泛覆盖人种特征，提升算法精度和适用性
3. 【AR mesh】算法升级，全面提升新mesh模型稳定性
4.  修复部分已知问题

**2024-04-19 v8.10.0:
1. 新增fuSetFaceModelConfig / fuSetFaceAlgorithmConfig / fuSetHumanModelConfig / fuSetHumanAlgorithmConfig / fuSetModelToCPU接口。
用于配置算法bundle的加载，需要再loadaibundle前才有效。

**2024-01-05 v8.9.0:
1. 【美颜】Android端部分高端机型新增祛斑祛痘AI美颜功能；新增仅皮肤区域美白模式
2. 【人脸关键点】算法升级，全脸精度优化，重点提升嘴唇点位准确性
3. 【AR mesh】算法升级，准确性、飘动感、贴合度优化
4. 【绿幕抠像】发丝级抠像升级，保留发丝细节，无明显绿边且过渡自然
5. 【机型适配】移动端机型适配策略调优
6. 【精品贴纸】新增精品贴纸24款，包含氛围类、头饰类、搞笑类、节日类等互动贴纸
7. 修复部分已知问题

**2023-10-19 v8.8.0:
1. 【美颜】新增清晰、祛斑祛痘高级美颜功能（暂未上线）
2. 【精品贴纸】新增精品贴纸22款，包含氛围类、头饰类、搞笑类、节日类等互动贴纸
3. 修复部分已知问题

**2023-07-07 v8.7.0:
1、 修复部分已知问题
2、 人脸关键点算法优化升级

**2023-03-30 v8.6.0:
1、 支持分版本。
2、 8个美型功能效果升级。
3、 美妆新增全脸遮挡分割算法，实现全脸遮挡不穿帮
4、 修复部分已知问题

**2023-01-04 v8.5.0:
1、 新增风格推荐模块（StyleRecommendation）。
2、 美妆，风格推荐模块开启全脸分割。 
3、 demo ui界面调整，规范化。
4、 新增模块权限验证，无权限灰色图标不可点击。

**2022-11-11 v8.4.0:
1、 人像加会议模式
2、 美颜新增五官立体、眼睑下至、嘴唇厚度，眉毛粗细、眼睛位置

**2022-09-09 v8.3.1:
1、 适配Mac M系列平台,重新编译opencv,ffmpeg,glfw等demo三方库适配mac arm64架构。
2、 Mac OC代码都包上autorelease,避免出现泄露

**2022-07-25 v8.3.0:
1.绿幕，人像，人像功能道具更新。美妆眉毛新增古典眉、野生眉，美妆道具更新。绿幕新增白幕，参数更新。
2. fuPreprocessAIModelFromPackage预加载处理humanprocessor，人体使用gpu加速（ai_human_processor_pc_gpu.bundle）
3.优化摄像头采集模块。

**2022-04-19 v8.2.0:
1.替换美白，亮眼，黑眼圈，法令纹,小脸,窄脸，瘦鼻mode2，大眼，嘴型model3

**2022-02-18 v8.1.0:
1. 新增小脸检测支持接口fuFaceProcessorSetDetectSmallFace，默认在高性能设备上开启。  
2. 修复一系列问题，美妆切换偶现crash，背景道具黑屏问题。  
3. 修复快速上下动，人脸跟踪丢失问题。

**2021-11-10 v8.0.0:**
1. 优化高分辨美颜磨皮效果，相同尺度上对齐低分辨率  
2. 修复人像分割贴纸人体和贴纸没有同时出现的问题  
3. 修复道具切换过程中，偶现人脸检测重置问题。  

**2021-07-08 v7.4.1:**
1. 优化高分辨美颜磨皮效果，相同尺度上对齐低分辨率  
2. 修复人像分割贴纸人体和贴纸没有同时出现的问题  
3. 修复道具切换过程中，偶现人脸检测重置问题。  

2021-01-25 v7.4.0:

1. 优化2D人体点位和美体性能。  
2. 优化人像分割效果和性能。优化手臂和手识别不稳定问题，优化背景误识别问题。修复人像分割偏移问题。  
3. 优化美妆效果。优化美瞳贴合效果和溢色问题；优化唇妆遮挡效果，遮挡时口红不再显现。  
4. 优化Animoji面部驱动效果。优化小幅度动作，如小幅度张嘴和眨眼时，驱动效果更加灵敏。  
5. 优化情绪识别，支持8种基本情绪识。  
6. 新增接口fuSetUseAsyncAIInference，支持异步模式，开启异步模式，帧率提升，可改善客户在一些低端设备上帧率不足问题。  
7. 新增fuRender接口，为所有业务统一渲染接口，详见接口定义。  
8. 新增接口 fuSetInputCameraBufferMatrix，fuSetInputCameraBufferMatrixState，fuSetInputCameraTextureMatrix，fuSetInputCameraTextureMatrixState，fuSetOutputMatrix，fuSetOutputMatrixState，用于设置图像转换矩阵，用于调整输出图像方向，详见接口定义。  

2021-01-25 v7.3.2:

1. 优化人脸表情跟踪驱动性能。

2. fuSetup 函数改为线程安全。

3. fuSetUp 、fuCreateItemFromPackage、fuLoadAIModel函数增加异常处理，增强鲁棒性。

4. 修复自定义哈哈镜功能效果问题。

5. 修复SDK在Mac 10.11上crash问题。

6. 修复SDK在贴纸和Animoji混用时crash问题。  

2020-12-29 v7.3.0:

1. 优化美妆性能，和V7.2比，标准美妆Android端帧率提升29%，iOS端帧率提升17%；标准美颜+标准美妆，集成入第三方推流1小时后，在低端机上帧率高于15fps，可流畅运行。
2. 优化美体性能，和V7.2比，性能显著提升，Android端帧率提升26%，CPU降低32%；iOS端帧率提升11%，CPU降低46%，内存降低45%。
3. 优化人像分割性能，和V7.2比，性能显著提升，Android端帧率提升64%，CPU降低25%；iOS端帧率提升41%，CPU降低47%，内存降低44%。
4. 优化美体功能效果，优化大幅度运动时，头部和肩部位置附近物体变形幅度大的问题；人体在画面中出现消失时过渡更自然；遮挡情况美体效果更加稳定，不会有高频持续抖动情况。
5. 优化表情识别功能，提高识别准确性，共能识别17种表情动作。
6. 优化绿幕抠像效果，提高边缘准确度。
7. 优化人脸表情跟踪驱动效果，优化首帧检测模型显示较慢问题，加强细微表情跟踪，优化人脸转动时模型明显变小问题。
8. 优化全身Avatar跟踪驱动效果，针对做连续高频大幅度运动的情况，如跳舞等场景，整体模型稳定性，尤其手臂稳定性提升，抖动情况显著改善。
9. 优化美颜亮眼下眼睑溢色问题。
10. 新增人脸拖拽变形功能，可使用FUCreator 2.1.0进行变形效果编辑。
11. 新增美颜美型模块瘦圆眼功能，效果为使眼睛整体放大，尤其是纵向放大明显。
12. 新增支持手势回调接口fuSetHandGestureCallBack，详见接口文档。
13. 新增AI能力，表情识别，AITYPE为FUAITYPE_FACEPROCESSOR_EXPRESSION_RECOGNIZER
14. windows sdk支持虚拟摄像头，用户可在直播平台上通过虚拟摄像头方式接入
15. 控花、控雨、控雪道具重新制作，优化跟踪效果不连贯的问题

2020-9-27 v7.2.0:

1. 新增绿幕抠像功能，支持替换图片、视频背景等。
2. 美颜模块新增瘦颧骨、瘦下颌骨功能。
3. 优化美颜性能以及功耗，优化集成入第三方推流服务时易发热掉帧问题。
4. 优化手势识别功能的效果以及性能，提升识别稳定性和手势跟随性效果，优化手势识别时cpu占有率。
5. 优化PC版各个功能性能，帧率提升显著。美发、美体、人像分割帧率提升30%以上，美颜、Animoji、美妆、手势等功能也有10%以上的帧率提升。
6. 优化包增量，SDK分为lite版，和全功能版本。lite版体积更小，包含人脸相关的功能(海报换脸除外)。
7. 优化人脸跟踪稳定性，提升贴纸的稳定性。
8. 提供独立核心算法SDK，接口文档详见算法SDK文档(FUAI_C_API_参考文档.md)。
9. fuGetFaceInfo接口新增三个参数，分别为：舌头方向(tongue_direction)，表情识别(expression_type)，头部旋转信息欧拉角参数(rotation_euler)。
10. 新增人体动作识别动作定义文档(人体动作识别文档.md)。


2020-7-29 v7.1.0:

1. 新增美颜锐化功能，见美颜参数文档。
2. 优化美颜磨皮效果，保留更多的高频细节。
3. 添加fuHumanProcessorGetFov接口。
4. 添加fuHumanProcessorSetFov接口。

2020-07-24 v7.0.1：
1. 新增接口fuHumanProcessorSetBonemap
2. 新增接口fuHumanProcessorGetResultTransformArray
3. 新增接口fuHumanProcessorGetResultModelMatrix
4. 修复fuGetSestemError问题。
5. 修复fuSetMaxFaces，在释放AI模型后，设置失效问题。
6. 修复Android非高通机型，OES输入问题。
7. 修复美妆远距离嘴部黑框问题。
8. 修复美体美颜共存不支持问题。


2020-6-30 v7.0.0:
1. 新增人体算法能力接口，包括人体检测、2D人体关键点（全身、半身）、人体3D骨骼（全身、半身）、手势识别、人像mask、头发mask、头部mask、动作识别等能力。
2. 新增接口，详见接口说明
  - fuGetLogLevel,获取当前日志级别。
  - fuSetLogLevel,设置当前日志级别。
  - fuOpenFileLog,打开文件日志，默认使用console日志。
  - fuFaceProcessorSetMinFaceRatio，设置人脸检测距离的接口。
  - fuSetTrackFaceAIType，设置fuTrackFace算法运行类型接口。
  - fuSetCropState，设置裁剪状态。
  - fuSetCropFreePixel，设置自由裁剪参数。
  - fuSetFaceProcessorFov，设置FaceProcessor人脸算法模块跟踪fov。
  - fuGetFaceProcessorFov，获取FaceProcessor人脸算法模块跟踪fov。
  - fuHumanProcessorReset，重置HumanProcessor人体算法模块状态。
  - fuHumanProcessorSetMaxHumans，设置HumanProcessor人体算法模块跟踪人体数。
  - fuHumanProcessorGetNumResults，获取HumanProcessor人体算法模块跟踪人体数。
  - fuHumanProcessorGetResultTrackId，获取HumanProcessor人体算法模块跟踪Id。
  - fuHumanProcessorGetResultRect，获取HumanProcessor人体算法模块跟踪人体框。
  - fuHumanProcessorGetResultJoint2ds，获取HumanProcessor人体算法模块跟踪人体2D关键点。
  - fuHumanProcessorGetResultJoint3ds，获取HumanProcessor人体算法模块跟踪人体3D关键点。
  - fuHumanProcessorSetBonemap，设置HumanProcessor人体算法模块，3D骨骼拓扑结构信息。
  - fuHumanProcessorGetResultTransformArray， 获取HumanProcessor人体算法模块跟踪人体3D骨骼信息。
  - fuHumanProcessorGetResultModelMatrix， 获取HumanProcessor人体算法模块跟踪人体3D骨骼，根节点模型变化矩阵。

  - fuHumanProcessorGetResultHumanMask，获取HumanProcessor人体算法模块全身mask。
  - fuHumanProcessorGetResultActionType，获取HumanProcessor人体算法模块跟踪人体动作类型。
  - fuHumanProcessorGetResultActionScore，获取HumanProcessor人体算法模块跟踪人体动作置信度。
  - fuFaceProcessorGetResultHairMask，获取HumanProcessor人体算法模块头发mask。
  - fuFaceProcessorGetResultHeadMask，获取HumanProcessor人体算法模块头部mask。
  - fuHandDetectorGetResultNumHands，获取HandGesture手势算法模块跟踪手势数量。
  - fuHandDetectorGetResultHandRect，获取HandGesture手势算法模块跟踪手势框。
  - fuHandDetectorGetResultGestureType，获取HandGesture手势算法模块跟踪手势类别。
  - fuHandDetectorGetResultHandScore，获取HandGesture手势算法模块跟踪手势置信度。
3. 废弃接口
  - fuSetStrictTracking
  - fuSetASYNCTrackFace
  - fuSetFaceTrackParam  
  - fuSetDeviceOrientation  
  - fuSetDefaultOrientation

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
  +assets 			  	 //资源目录
  +FULivePC				 //示例代码目录
    +GUI				 //GUI文件目录
    +rapidjson			 //json库文件目录
    +Sound				 //声音库文件目录
    -Camera.cpp：		//辅助相机类，负责从摄像头内读取图像帧
    -Camera.h：			//辅助相机类的头文件
    -Config.h：			// 配置文件，记录道具文件的加载路径
    -FULivePC.cpp：程序入口
	-Nama.cpp：负责展示如何调用Nama SDK的接口
	-Nama.h：展示如何调用Nama SDK的接口类的头文件
	-UIBridge.h：负责展示虚拟摄像头的接入与使用
	-ReadMe.txt：简单的接入说明
  +docs					//文档目录
  +ThridParty			//第三方库目录
  +include				//SDK包含目录
  -readme.md			//工程总文档
  run_cmake.bat   //CMAKE 生成VS工程的脚本，可自行修改VS版本（>=2015）,以及位数
  run_cmake.sh    //CMAKE 生成XCODE版本的脚本
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
vs2015/vs2017/vs2019
```

### 3.2 准备工作 
- [下载demo代码](<https://github.com/Faceunity/FULivePC> )
- 获取证书:
  1. 拨打电话 **0571-88069272** 
  2. 发送邮件至 **marketing@faceunity.com** 进行咨询。

### 3.3 相关配置
- 将证书文件authpack.h放置到FULivePC文件夹内（authpack.h由3.2步骤申请到）。

- 如需要将本SDK集成到你的工程中去：
  1. 请将demo中的assets文件、ThirdParty\Windows\FaceUnity-SDK-PC\下所有库文件复制到你的工程中去。
  2. 同时配置OpenGL渲染环境，配置方法参考demo。
  3. 参考Nama.cpp代码流程进行集成。

### 3.4 初始化

#### 3.4.1 初始化SDK

```c
#include <CNamaSDK.h>				//nama SDK 的头文件
#include <authpack.h>				//nama SDK 的key文件
```

首先初始化好窗口以及OpenGL环境，初始化好的OpenGL环境是正确使用FaceUnity SDK的必须条件。

OpenGL环境可以有多种初始化方式

1. 使用QT开发环境，可以直接调用QT OpenGL Widget创建窗口。QT会自动初始化好OpenGL环境。

2. 参考demo代码中Nama.cpp中的InitOpenGL函数手工进行初始化。Demo提供了检查OpenGL环境是否成功的样例代码Nama.cpp中的CheckGLContext函数

3. 直接使用glfw窗口库，https://github.com/glfw/glfw ，参考glfw example实现

   ```c++
   glfwSetErrorCallback(error_callback);
   
   if (!glfwInit())
       exit(EXIT_FAILURE);
   
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   
   GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
   if (!window)
   {
       glfwTerminate();
       exit(EXIT_FAILURE);
   }
   
   glfwSetKeyCallback(window, key_callback);
   glfwMakeContextCurrent(window);
   gladLoadGL(glfwGetProcAddress);
   glfwSwapInterval(1);
   ```

   调用fuSetup初始化SDK，SDK的初始化整个应用周期只需调用一次。fuSetup不依赖OpenGL环境，在函数初始化的同一线程里，其他函数可以正常调用。所有道具创建，销毁，图片渲染的API调用都需要保证有OpenGL context初始化好的环境。目前不支持跨线程的SDK初始化与调用。 初始化示例代码如下，g_auth_package为步骤3.2获取的证书权限码。

```
  fuSetup(nullptr, 0, nullptr, g_auth_package, sizeof(g_auth_package)); 
```

​	fuSetup具体API参数详见 [Nama_C_API_参考文档.md](Nama_C_API_参考文档.md)

#### 3.4.2 加载AI能力

根据需要的AI能力加载对应的AI bundle，如加载人脸驱动能力ai_face_processor_pc.bundle.

1. 首先加载二进制bundle到内存
2. 通过fuLoadAIModelFromPackage初始化具体AI能力并设置AI能力type为FUAITYPE::FUAITYPE_FACEPROCESSOR 人脸模块

```c++
vector<char> ai_model_data;
if (false == FuTool::LoadBundle(g_ai_faceprocessor, ai_model_data))
{
    cout << "Error:fail load faceprocessor model" << g_ai_faceprocessor << endl;
    return false;
}
fuLoadAIModelFromPackage(reinterpret_cast<float*>(&ai_model_data[0]), ai_model_data.size(), FUAITYPE::FUAITYPE_FACEPROCESSOR);
```

fuLoadAIModelFromPackage具体API参数详见 [Nama_C_API_参考文档.md](Nama_C_API_参考文档.md)

### 3.5 渲染道具创建、销毁、切换

####  3.5.1道具创建

每一个效果道具都是一个独立的Bundle文件，可以通过fuCreateItemFromPackage函数创建的道具并返回道具 handle 。其中 data 为指向道具文件内容的指针，size 为该文件的字节大小。

之后每帧的绘制时需要输入 handle 作为道具的标识。

```c
static int mItemsArray[2] = {0, 0};
...
if (!mItemsArray[0]){
  mItemsArray[0] = fuCreateItemFromPackage(data, size);
}
```

#### 3.5.2道具销毁

##### 销毁单个道具

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

##### 销毁全部道具

```c
void fuDestroyAllItems();
```

该接口可以销毁全部道具句柄所对应的资源,同样在执行完该接口后请将所有句柄都置为0。示例如下：

```java
fuDestroyAllItems();
```

#### 3.5.3道具切换

如果需要切换句柄数组中某一位的句柄时，需要先创建一个新的道具句柄，销毁原道具句柄，并将该新句柄替换到句柄数组中需要被替换的位置上。下面以替换句柄数组的第ITEM_ARRAYS_EFFECT位为例进行说明：

```c
int newEffectItem = loadItem(path);
if (mItemsArray[0] > 0) {
    fuDestroyItem(mItemsArray[0]);
}
mItemsArray[0] = newEffectItem;
```

### 3.6 输入图像数据

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
fuRenderItemsEx2(FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame), FU_FORMAT_RGBA_BUFFER, reinterpret_cast<int*>(frame),mFrameWidth, mFrameHeight, mFrameID, handle_list, handleSize, NAMA_RENDER_FEATURE_FULL, NULL);
```

以上我们示例将一个图像的RGBA排列格式的buffer作为本SDK输入图像数据。

`FU_FORMAT_NV21_BUFFER` NV21格式的buffer数组，通常在安卓设备上，通过原生相机插件使用

`FU_FORMAT_BGRA_BUFFER`  BGRA格式的buffer数组，一般图像库的默认格式

`FU_FORMAT_RGBA_BUFFER` RGBA格式的buffer数组，通用性最强

`FU_FORMAT_I420_BUFFER` I420格式的buffer数组

主要参数说明：

out_format ： 输出的格式，支持RGBA的buffer， YUV I420输出，或者输出OpenGL纹理

out_ptr:           输出数据地址

in_format:		输入图形数据格式，支持RGBA, NV21, I420等不同格式

p_items:		   需要处理效果的道具bundle 列表

n_items：         输入道具bundle个数

frame_id：        绘制的帧数

具体fuRenderItemsEx2 API详见 [Nama_C_API_参考文档.md](Nama_C_API_参考文档.md)



几种常见的调用方式：

RGBA相机输入，RGBA CPU Buffer输出，适合本地图片处理：

```

fuRenderItemsEx2(FU_FORMAT_RGBA_BUFFER, output_data, FU_FORMAT_RGBA_BUFFER, camera_data, camera_img_width, camera_img_height, mFrameID, handle_list, handleSize, NAMA_RENDER_FEATURE_FULL, NULL);
```

RGBA相机输入，OpenGL 贴图输出，本地窗口绘制：

```
fuRenderItemsEx2(FU_FORMAT_RGBA_Texture, output_data, FU_FORMAT_RGBA_BUFFER, camera_data, camera_img_width, camera_img_height, mFrameID, handle_list, handleSize, NAMA_RENDER_FEATURE_FULL, NULL);
```

RGBA相机输入，I420格式Buffer输出，适合推拉流或者视频处理：

```
fuRenderItemsEx2(FU_FORMAT_I420_BUFFER , output_data, FU_FORMAT_RGBA_BUFFER, camera_data, camera_img_width, camera_img_height, mFrameID, handle_list, handleSize, NAMA_RENDER_FEATURE_FULL, NULL);
```

### 3.7 输出跟踪数据

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

美颜道具主要包含：滤镜、美白、红润、磨皮、亮眼、美牙、美型，锐化等模块。

美颜道具的所有功能都通过设置参数来进行设置，以下为设置参数的代码示例：

```c
#define MAX_BEAUTYFACEPARAMTER 8
#define MAX_FACESHAPEPARAMTER 17
const string g_faceBeautyParamName[MAX_BEAUTYFACEPARAMTER] = { "blur_level","color_level", "red_level","sharpen","eye_bright", "tooth_whiten" ,"remove_pouch_strength", "remove_nasolabial_folds_strength" };

const string g_faceShapeParamName[MAX_FACESHAPEPARAMTER] = { "cheek_thinning","eye_enlarging", "intensity_chin", "intensity_forehead", "intensity_nose","intensity_mouth","cheek_v","cheek_narrow","cheek_small","intensity_cheekbones","intensity_lower_jaw","intensity_canthus", "intensity_eye_space", "intensity_eye_rotate", "intensity_long_nose","intensity_philtrum", "intensity_smile" };


	for (int i = 0; i < MAX_BEAUTYFACEPARAMTER; i++)
	{
		if (i == 0)
		{
			fuItemSetParamd(mBeautyHandles, const_cast<char*>(g_faceBeautyParamName[i].c_str()), UIBridge::mFaceBeautyLevel[i] * 6.0 / 100.f);
		}
		else
		{
			fuItemSetParamd(mBeautyHandles, const_cast<char*>(g_faceBeautyParamName[i].c_str()), UIBridge::mFaceBeautyLevel[i] / 100.f);
		}
	}

	for (int i = 0; i < MAX_FACESHAPEPARAMTER; i++)
	{
		if (g_faceShapeParamShowFlag[i] == FACE_SHAPE_SHOW_FLAG_MIDDLE)
		{
			UIBridge::mFaceShapeLevel[i] += 50;
		}
		fuItemSetParamd(mBeautyHandles, const_cast<char*>(g_faceShapeParamName[i].c_str()), UIBridge::mFaceShapeLevel[i] / 100.0f);

		if (g_faceShapeParamShowFlag[i] == FACE_SHAPE_SHOW_FLAG_MIDDLE)
		{
			UIBridge::mFaceShapeLevel[i] -= 50;
		}
	}
	fuItemSetParamd(mBeautyHandles, "skin_detect", UIBridge::mEnableSkinDect);
	map<int, int> blurType = { {0,2},{1,0},{2,1} };
	fuItemSetParamd(mBeautyHandles, "blur_type", blurType[UIBridge::mEnableHeayBlur]);
	fuItemSetParamd(mBeautyHandles, "face_shape_level", 1);
	fuItemSetParamd(mBeautyHandles, "filter_level", UIBridge::mFilterLevel[UIBridge::m_curFilterIdx] / 100.0f);
```

每个模块都有默认效果，它们可以调节的参数如下。

#### 一、滤镜

滤镜功能主要通过参数 `filter_level` 和 `filter_name` 来控制。

`filter_name` 参数的取值和相关介绍详见：[美颜道具功能文档](美颜道具功能文档.md) ，在**滤镜对应key值**部分有详细介绍，对于老版本（6.0之前）的用户，可以参考**新老滤镜对应关系**部分。

`filter_level` 取值范围 0.0-1.0，0.0为无效果，1.0为最大效果，默认值1.0。

#### 二、美白和红润

##### 美白

美白功能主要通过参数color_level来控制

```
color_level 取值范围 0.0-1.0,0.0为无效果，1.0为最大效果，默认值0.3
```

##### 红润

红润功能主要通过参数red_level 来控制

```
red_level 取值范围 0.0-1.0,0.0为无效果，1.0为最大效果，默认值0.3```

#### 三、磨皮

控制磨皮的参数有以下几个：

```
blur_level: 磨皮程度，取值范围0.0-6.0，默认4.2
heavy_blur: 磨皮开关，0为清晰磨皮，1为朦胧磨皮
blur_type：此参数优先级比heavy_blur低，在使用时要将heavy_blur设为0，0 清晰磨皮  1 朦胧磨皮  2精细磨皮 3均匀磨皮
blur_use_mask: 默认为0，1为开启基于人脸的磨皮mask，0为不使用mask正常磨皮。只在blur_type为2时生效。开启此功能需要高级美颜权限。
```

#### 四、亮眼

亮眼功能主要通过参数eye_bright 来控制

```
eye_bright   取值范围 0.0-1.0,0.0为无效果，1.0为最大效果，默认值0.2
```

注意：亮眼为高级美颜功能，需要相应证书权限才能使用 

#### 五、美牙

美牙功能主要通过参数tooth_whiten来控制

```
tooth_whiten   取值范围 0.0-1.0,0.0为无效果，1.0为最大效果，默认值0.0
```

注意：美牙为高级美颜功能，需要相应证书权限才能使用 

#### 六、美型

美型的整体程度由face_shape_level参数控制

```
face_shape_level   取值范围 0.0-1.0,0.0为无效果，1.0为最大效果，默认值1.0
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
cheek_thinning: 默认0.0,   //瘦脸程度范围0.0-1.0
2.
face_shape 为4时，为精细变形，添加了鼻子额头嘴巴下巴的调整
可以使用参数
eye_enlarging:  默认0.5,  //大眼程度范围0.0-1.0
cheek_thinning: 默认0.0,   //瘦脸程度范围0.0-1.0
intensity_nose: 默认0.0,       //瘦鼻程度范围0.0-1.0
intensity_forehead: 默认0.5,   //额头调整程度范围0.0-1.0
intensity_mouth:默认0.5,       //嘴巴调整程度范围0.0-1.0
intensity_chin: 默认0.5,       //下巴调整程度范围0.0-1.0
```

具体可参考[美颜道具功能文档.md](美颜道具功能文档.md)

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

### 4.5 人像分割

人像分割道具可以识别到图像中人的区域，并将其与背景区别开来，然后在背景处绘制图案进行分割显示。加载方式如下：

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.6 表情识别

表情识别道具是在普通的道具里加入额外的识别表情然后触发特定动作的功能，加载方式如下

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.7 手势识别

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

### 4.8 贴纸

贴纸为基础的道具称呼。加载方式如下

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.9 人脸夸张变形功能

人脸夸张变形道具可以将人脸进行比较夸张的变形。加载方式如下

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

### 4.10 音乐滤镜

```c
 mItemsArray[0] = fuCreateItemFromPackage(data, size);
```

音乐滤镜是使用播放音乐的时间戳进行驱动的，在每次处理图像前，将音乐的播放进度传入音乐滤镜道具即可，方式如下：通过随着音乐设置播放时间才能让滤镜按音频每一帧的数据
驱动

```c
fuItemSetParamd(name, "music_time",  mp3Map[UIBridge::m_curRenderItem]->GetCurrentPosition() / 1e4);
```

如果没有音乐则可以模拟音乐播放进度，demo中提供的道具对应的音乐时长为28s，换算成ms为28000ms，在没有音乐的情况下，可以从加载音乐滤镜开始计时，每次处理图像前获取一下当前时间与开始加载音乐滤镜的时间差，转换成ms传入音乐滤镜即可，当时间差超过28000ms时归0重新开始计时即可。效果详见FULiveDemoPC，道具可以通过FUEditor v4.3.0及以上进行制作（FUCreator暂不支持）。

### 4.11 人脸美妆

[美妆bundle参数说明](美妆道具功能文档.md)

### 4.12 美发功能

[美发参数说明](美发道具功能文档.md)

加载美发道具 `itemName`，并保存到句柄数组items

```objective-c
NSString *path = [[NSBundle mainBundle] pathForResource:[itemName stringByAppendingString:@".bundle"] ofType:nil];
int itemHandle = [FURenderer itemWithContentsOfFile:path];
```

发色种类设置，`colorIndex` (0~n）

```objective-c
[FURenderer itemSetParam:items[FUNamaHandleTypeItem] withName:@"Index" value:@(colorIndex)]; 
```

发色程度值,`Strength` 值为(0 ~ 1.0)

```objective-c
 [FURenderer itemSetParam:items[FUNamaHandleTypeItem] withName:@"Strength" value: @(strength)]; 
```

### 4.13 轻美妆功能

Nama SDK 从 6.0.0 开始支持质感美颜功能。

轻美妆方案是一套更为精致高效的美颜解决方案，包含磨皮、美型、滤镜、美妆4大模块，提供60+套丰富素材库，支持客户任意切换风格与效果变化。

首先加载 light_makeup.bundle，然后设置腮红、眼影、眼线、口红等参数，使用方法请参考[轻美妆道具说明](轻美妆功能文档.md)，同时参考 FULiveDemo 中的示例代码。

### 4.14 美体功能

Nama SDK 从 6.4.0 开始支持美体功能，仅支持单人。

使用方法请参考[美体功能文档](美体道具功能文档.md)

### 4.15 全身Avatar功能

Nama SDK 从 7.0.0 开始支持全身Avatar功能。

使用方法请参考[全身Avatar功能文档](controller功能文档.md)

## 5. 常见问题 

### 5.1 编译相关
- 所使用的显卡的年代过于久远可能不支持Opengl 3.2 core profile ，会提示错误并退出
- visual studio 2015/2017/2019版本 安装时需勾选c++组件以及windows sdk。
