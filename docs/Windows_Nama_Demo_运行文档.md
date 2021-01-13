# Demo运行说明文档-Windows 
- 级别：Public
  更新日期：2020-09-24
  
  ------
  
  **FaceUnity Nama SDK v7.2.0 (2020-09-24)**

2020-9-24 v7.2.0:
1. 新增绿幕抠像功能，支持替换图片、视频背景等，详见绿幕抠像功能文档。
2. 美颜模块新增瘦颧骨、瘦下颌骨功能。
3. 优化美颜性能以及功耗，优化集成入第三方推流服务时易发热掉帧问题。
4. 优化手势识别功能的效果以及性能，提升识别稳定性和手势跟随性效果，优化手势识别时cpu占有率。
5. 优化PC版各个功能性能，帧率提升显著。美发、美体、背景分割帧率提升30%以上，美颜、Animoji、美妆、手势等功能也有10%以上的帧率提升。
6. 优化包增量，SDK分为lite版，和全功能版本。lite版体积更小，包含人脸相关的功能(海报换脸除外)。
7. 优化人脸跟踪稳定性，提升贴纸的稳定性。
8. 提供独立核心算法SDK，接口文档详见算法SDK文档([FUAI_C_API_参考文档.md](./FUAI_C_API_参考文档.md))。
9. fuGetFaceInfo接口新增三个参数，分别为：舌头方向(tongue_direction)，表情识别(expression_type)，头部旋转信息欧拉角参数(rotation_euler)。
10. 新增fuOnDeviceLostSafe函数，详见接口文档。
11. 新增fuSetFaceProcessorDetectMode函数，人脸识别跟踪区分图片模式和视频模式，详见接口文档。
12. 新增人体动作识别动作定义文档([人体动作识别文档.md](../resource/docs/人体动作识别文档.md))。
13. 新增ai_hand_processor.bundle，替代ai_gesture.bundle，提供手势识别跟踪能力。

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

文档：

- [美颜道具功能文档](美颜道具功能文档.md)

- [美妆道具功能文档](美妆道具功能文档.md)

- [海报换脸功能文档](海报换脸功能文档.md)

- [表情动图功能文档](表情动图功能文档.md)

  注：海报换脸功能，表情动图功能暂时没有在windows版的demo中体现，如需体验可下载本公司Android或IOS的demo进行体验。

工程案例更新：

- 美颜道具部分接口已改变，请注意同步代码！
- anim_model.bundle以及ardata_ex.bundle已弃用，请删除相关数据及代码

------
### 目录：
本文档内容目录：

[TOC]

------
### 1. 简介 
本文档旨在说明如何将Faceunity Nama SDK的Windows Demo运行起来，体验Faceunity Nama SDK的功能。FULivePC 是 Faceunity 的面部跟踪和虚拟道具功能在PC中的集成，作为一款集成示例。  

------
### 2. Demo文件结构
本小节，描述Demo文件结构，各个目录，以及重要文件的功能。

```
+FULivePC
  +assets 			  	//资源目录
  +FULivePC				//示例代码目录
    +GUI				//GUI文件目录
    +rapidjson			 //json库文件目录
    +Sound				 //声音库文件目录
    -Camera.cpp：       	相机类，负责从摄像头内读取图像帧
    -Camera.h：			相机类的头文件
    -Config.h：			配置文件，记录道具文件的加载路径
    -FULivePC.cpp：		程序入口
	-Nama.cpp：			负责展示如何调用Nama SDK的接口
	-Nama.h：			展示如何调用Nama SDK的接口类的头文件
	-UIBridge.h：		负责展示虚拟摄像头的接入与使用
	-ReadMe.txt：		简单的接入说明
  +docs					//文档目录
  +ThirdParty			//第三方库目录
  +include				//SDK包含目录
  -readme.md			//工程总文档
  run_cmake.bat   		//CMAKE 生成VS工程的脚本，可自行修改VS版本（>=2015）,以及位数
  run_cmake.sh    		//CMAKE 生成XCODE版本的脚本
```

------
### 3. 运行Demo 

#### 3.1 开发环境
##### 3.1.1 支持平台
```
Windows7及以上
```
##### 3.1.2 开发环境
```
vs2015/vs2017/vs2019
```

#### 3.2 准备工作 
- [下载demo代码 github](<https://github.com/Faceunity/FULivePC> )
- [下载demo代码 码云](<https://gitee.com/hangzhou_xiangxin_1/FULivePC> )
- 获取证书:
  1. 拨打电话 **0571-88069272** 
  2. 发送邮件至 **marketing@faceunity.com** 进行咨询。  
#### 3.3 相关配置

- 将证书文件authpack.h放置到FULivePC文件夹内（authpack.h由3.2步骤申请到）。

#### 3.4 编译运行

- 安装CMake 3.10及以上版本, 点击run_cmake.bat 生成脚本，在build目录中打开工程，编译。选择FULivePC为启动项目
- 修改VS版本 set CMAKE_GENERATOR=Visual Studio [1] [2]。 [1],[2]的组合根据VS版本可以是 16 2019、
15 2017、14 2015
- 当时x64的时候 在后面加上 Win64，例如 set CMAKE_GENERATOR=Visual Studio 16 2019 Win64

  ![](./imgs/img1.png)

------
### 4. 常见问题 
- 所使用的显卡的年代过于久远可能不支持Opengl 3.2 core profile及以上版本 ，会提示错误并退出
- **因Github不支持上传100MB以上的文件，FULivePC\bin\win\x64\Debug\opencv_world400d.rar是经过压缩的dll，run_cmake.bat会自动解压，用户也可手动自行解压！**

