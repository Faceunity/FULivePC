# Demo运行说明文档-Windows 

级别：Public   
更新日期：2021-04-16   
SDK版本: 7.4.0
  
  ------
  
  **FaceUnity Nama SDK v7.3.2 (2021-01-25)**

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
3. 优化背景分割性能，和V7.2比，性能显著提升，Android端帧率提升64%，CPU降低25%；iOS端帧率提升41%，CPU降低47%，内存降低44%。
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

