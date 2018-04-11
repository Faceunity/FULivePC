# FULivePC
FULivePC 是 Faceunity 的面部跟踪和虚拟道具功能在PC中的集成，作为一款集成示例。
## 目录
[新特性](#sdk-v50-更新)
[SDK内容](#运行环境)
[集成说明](#集成方法)
[道具失效等问题](#faq)

## SDK v5.0 更新

本次更新：

- 新增高级美颜功能
- 新增精细脸型调整功能
- 新增3D绘制抗锯齿功能
- 新增照片驱动功能
- 新增人脸夸张变形功能
- 新增音乐节奏滤镜
- 新增被动表情校准模式
- 优化手势识别
- 人脸跟踪底层性能进一步优化
- 其他累积问题修复、接口调整

由于深度学习框架的升级，SDK的库文件从之前的 ~3M 增加到了 ~5M，如果不需要AI相关功能，可以下载[SDK lite版](https://github.com/Faceunity/FULivePC/releases)，库文件大小和老版本保持一致。

与新版SDK一起，我们也推出更方便和好用的2D/3D贴纸道具制作工具——FUEditor，助力视频应用快速应对市场，推出具有个性化和吸引力的道具和玩法。相关文档和下载在[这里](https://github.com/Faceunity/FUEditor)，制作过程中遇到问题可以联系我司技术支持。

此外，我们优化了SDK的系统稳定性，在网络条件波动的情况下保持SDK正常运行，并提供了获取SDK系统错误信息的接口，方便应用灵活处理。
具体更新内容可以到docs/目录下查看详细文档。

## 运行环境

本SDK目前我们提供了32位和64位双平台的库文件

- [SDK]内部的API执行前要保证初始化好OpenGL环境，【确保】fuSetup等API调用时OpenGL context是可用的。

- [Demo]为了演示SDK功能我们制作了Demo程序，界面绘制使用Qt5.31 vs2013 opengl版，下载:[x64](http://download.qt.io/archive/qt/5.3/5.3.1/qt-opensource-windows-x86-msvc2013_64_opengl-5.3.1.exe),[x86](http://download.qt.io/archive/qt/5.3/5.3.1/qt-opensource-windows-x86-msvc2013_opengl-5.3.1.exe)。该界面库有多个vs版本而且区分32位和64位，下载对应开发环境对应的版本才可正常编译。QT插件[下载](http://download.qt.io/archive/vsaddin/)
注：SDK仅包含下述文件列表内容里的\*.dll,\*.lib和\*.h文件，它是不依赖任何界面库的，如需更换其他界面库如MFC或者不具备编译环境等，可选择不编译此Demo，直接参考代码将SDK的dll等[集成](#jump2)到您的新工程。
## 文件列表
  - funama.h 函数调用接口头文件
  - Win32/Win64 库文件
  - assets 数据文件

## 数据文件
目录 assets 下的 \*.bundle 为程序的数据文件。数据文件中都是二进制数据，与扩展名无关。实际使用时，打包在程序内或者从网络接口下载这些数据都是可行的，只要在相应的函数接口传入正确的二进制数据即可。

其中 v3.bundle 是所有道具共用的数据文件，缺少该文件会导致初始化失败。其他每一个文件对应一个道具。自定义道具制作的文档和工具请联系我司获取。

## 集成方法
首先把nama.lib链接到工程中，并确保nama.dll在运行时可以正确加载。包含funama.h之后就可以开始调用我们提供的接口函数。

首先初始化OpenGL环境。

接着对 faceunity 环境进行初始化。其中 v3data 是人脸跟踪数据的指针，指向 v3.bundle 文件中的内容； g_auth_package 为密钥数组，该密钥定义在我司提供的证书文件authpack.h中，没有该密钥会导致初始化失败。

```C
fuSetup(v3data, nullptr, g_auth_package, sizeof(g_auth_package));
```

之后进行道具加载，函数返回的道具 handle 要保存在当前程序中，之后绘制道具时需要提供该 handle 作为道具的标识。其中 data 为指向道具文件内容的指针，size 为该文件的字节大小。

```C
static int g_items[2] = {0, 0};
...
if (!g_items[0]){
  g_items[0] = fuCreateItemFromPackage(data, size);
}
```

之后就可以调用绘制函数将已经加载的道具绘制到图像数据中，如下例所示，提供图像数据的指针、宽度stride、高度、当前运行帧号、道具 handle 列表、以及绘制道具数量等信息后，完成绘制操作。其中，图像数据必须是布局为BGRA、类型为unsigned char的数据。

```  
fuRenderItems(0, img, stride, h, g_frame_id++, g_items, 1);
```

## 视频美颜
美颜功能实现步骤与道具类似，首先加载美颜道具，并将fuCreateItemFromPackage返回的美颜道具handle保存下来:

```C
g_items[1] = fuCreateItemFromPackage(g_res_zip, (int)g_res_size);
```

之后，将该handle和其他需要绘制的道具一起传入绘制接口即可。注意 fuRenderItems() 最后一个参数为所绘制的道具数量，这里以一个普通道具和一个美颜道具一起绘制为例。加载美颜道具后不需设置任何参数，即可启用默认设置的美颜的效果。

```C
fuRenderItems(0, img, stride/4, h, g_frame_id, g_items, 2);
```

美颜道具主要包含五个模块的内容，滤镜，美白和红润，磨皮，美型。

#### 滤镜

在目前版本中提供以下滤镜：
```C
"origin", "delta", "electric", "slowlived", "tokyo", "warm"
```

其中 "origin" 作为默认的美白滤镜，其他滤镜属于风格化滤镜。切换滤镜时，通过 fuItemSetParams 设置美颜道具的参数，如：
```C
//  Set item parameters - filter
fuItemSetParams(g_items[1], "filter_name", "origin");
```

#### 美白和红润

当滤镜设置为美白滤镜 "origin" 时，通过参数 color_level 来控制美白程度。当滤镜为其他风格化滤镜时，该参数用于控制风格化程度。该参数取值为大于等于0的浮点数，0为无效果，1为默认效果，大于1为继续增强效果。

设置参数的例子代码如下：

```C
//  Set item parameters - whiten
fuItemSetParamd(g_items[1], "color_level", 1.0);
```

新版美颜新增红润调整功能。参数名为 red_level 来控制红润程度。使用方法基本与美白效果一样。该参数的推荐取值范围为[0, 1]，0为无效果，0.5为默认效果，大于1为继续增强效果。

#### 磨皮

新版美颜中，控制磨皮的参数有两个：blur_level、use_old_blur。

参数 blur_level 指定磨皮程度。该参数的推荐取值范围为[0, 6]，0为无效果，对应7个不同的磨皮程度。

参数 use_old_blur 指定是否使用旧磨皮。该参数设置为0即使用新磨皮，设置为大于0即使用旧磨皮

设置参数的例子代码如下：

```C
//  Set item parameters - blur
fuItemSetParamd(g_items[1], "blur_level", 5.0);
//  Set item parameters - use old blur
fuItemSetParamd(g_items[1], "use_old_blur", 1.0);
```

如果对默认的6个磨皮等级不满意，想进一步自定义磨皮效果，可以联系我司获取内部参数调节的方式。

#### 美型

目前我们支持四种基本脸型：女神、网红、自然、默认。由参数 face_shape 指定：默认（3）、女神（0）、网红（1）、自然（2）。
```C
//  Set item parameters - shaping
fuItemSetParamd(g_items[1], "face_shape", 3);
```
在上述四种基本脸型的基础上，我们提供了以下三个参数：face_shape_level、eye_enlarging、cheek_thinning。

参数 face_shape_level 用以控制变化到指定基础脸型的程度。该参数的取值范围为[0, 1]。0为无效果，即关闭美型，1为指定脸型。

若要关闭美型，可将 face_shape_level 设置为0。
```C
//  Set item parameters - shaping level
fuItemSetParamd(g_items[1], "face_shape_level", 1.0);
```
参数 eye_enlarging 用以控制眼睛大小。此参数受参数 face_shape_level 影响。该参数的推荐取值范围为[0, 1]。大于1为继续增强效果。
```C
//  Set item parameters - shaping
fuItemSetParamd(g_items[1], "eye_enlarging", 1.0);
```
参数 cheek_thinning 用以控制脸大小。此参数受参数 face_shape_level 影响。该参数的推荐取值范围为[0, 1]。大于1为继续增强效果。
```C
//  Set item parameters - shaping
fuItemSetParamd(g_items[1], "cheek_thinning", 1.0);
```

## 手势识别
目前我们的手势识别功能也是以道具的形式进行加载的。一个手势识别的道具中包含了要识别的手势、识别到该手势时触发的动效、及控制脚本。加载该道具的过程和加载普通道具、美颜道具的方法一致。

线上例子中 heart.bundle 为爱心手势演示道具。将其作为道具加载进行绘制即可启用手势识别功能。手势识别道具可以和普通道具及美颜共存，类似美颜将 g_items 扩展为三个并在最后加载手势道具即可。

自定义手势道具的流程和2D道具制作一致，具体打包的细节可以联系我司技术支持。

## 鉴权

我们的系统通过标准TLS证书进行鉴权。客户在使用时先从发证机构申请证书，之后将证书数据写在客户端代码中，客户端运行时发回我司服务器进行验证。在证书有效期内，可以正常使用库函数所提供的各种功能。没有证书或者证书失效等鉴权失败的情况会限制库函数的功能，在开始运行一段时间后自动终止。

证书类型分为**两种**，分别为**发证机构证书**和**终端用户证书**。

#### - 发证机构证书
**适用对象**：此类证书适合需批量生成终端证书的机构或公司，比如软件代理商，大客户等。

发证机构的二级CA证书必须由我司颁发，具体流程如下。

1. 机构生成私钥
机构调用以下命令在本地生成私钥 CERT_NAME.key ，其中 CERT_NAME 为机构名称。
```
openssl ecparam -name prime256v1 -genkey -out CERT_NAME.key
```

2. 机构根据私钥生成证书签发请求
机构根据本地生成的私钥，调用以下命令生成证书签发请求 CERT_NAME.csr 。在生成证书签发请求的过程中注意在 Common Name 字段中填写机构的正式名称。
```
openssl req -new -sha256 -key CERT_NAME.key -out CERT_NAME.csr
```

3. 将证书签发请求发回我司颁发机构证书

之后发证机构就可以独立进行终端用户的证书发行工作，不再需要我司的配合。

如果需要在终端用户证书有效期内终止证书，可以由机构自行用OpenSSL吊销，然后生成pem格式的吊销列表文件发给我们。例如如果要吊销先前误发的 "bad_client.crt"，可以如下操作：
```
openssl ca -config ca.conf -revoke bad_client.crt -keyfile CERT_NAME.key -cert CERT_NAME.crt
openssl ca -config ca.conf -gencrl -keyfile CERT_NAME.key -cert CERT_NAME.crt -out CERT_NAME.crl.pem
```
然后将生成的 CERT_NAME.crl.pem 发回给我司。

#### - 终端用户证书
**适用对象**：直接的终端证书使用者。比如，直接客户或个人等。

终端用户由我司或者其他发证机构颁发证书，并通过我司的证书工具生成一个代码头文件交给用户。该文件中是一个常量数组，内容是加密之后的证书数据，形式如下。
```
static char g_auth_package[]={ ... }
```
## FAQ

## 为什么过了一段时间人脸识别失效了？

检查证书。如证书是否正确使用，是否过期。您需要拥有我司颁发的证书才能使用我们的SDK的功能，获取证书方法：1、拨打电话 **0571-88069272** 2、发送邮件至 **marketing@faceunity.com** 进行咨询。

用户在库环境初始化时，需要提供该数组进行鉴权，具体参考 fuSetup 接口。没有证书、证书失效、网络连接失败等情况下，会造成鉴权失败，在控制台或者Android平台的log里面打出 "not authenticated" 信息，并在运行一段时间后停止渲染道具。

任何其他关于授权问题，请email：support@faceunity.com
