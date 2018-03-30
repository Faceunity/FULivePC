# FULivePC
FULivePC 是 Faceunity 的面部跟踪和虚拟道具功能在PC中的集成，作为一款集成示例。

## FuNama SDK v4.6

本次更新主要包含以下改动：
- 增强表情优化功能，在人脸快速转动时提高表情稳定性

由于深度学习框架的升级，SDK的库文件从之前的 ~3M 增加到了 ~5M，如果不需要AI相关功能，可以下载[SDK lite版](https://github.com/Faceunity/FULivePC/releases)，库文件大小和老版本保持一致。

与新版SDK一起，我们也推出更方便和好用的2D/3D贴纸道具制作工具——FUEditor，助力视频应用快速应对市场，推出具有个性化和吸引力的道具和玩法。相关文档和下载在[这里](https://github.com/Faceunity/FUEditor)，制作过程中遇到问题可以联系我司技术支持。

此外，我们优化了SDK的系统稳定性，在网络条件波动的情况下保持SDK正常运行，并提供了获取SDK系统错误信息的接口，方便应用灵活处理。
具体更新内容可以到docs/目录下查看详细文档。
## 运行环境

目前我们提供了32位和64位双平台的库文件，运行前要保证初始化好OpenGL环境，并【确保】当前有可用的OpenGL context。
程序界面现在使用Qt5.31 vs2013 opengl版，下载:[x64](http://download.qt.io/archive/qt/5.3/5.3.1/qt-opensource-windows-x86-msvc2013_64_opengl-5.3.1.exe),[x86](http://download.qt.io/archive/qt/5.3/5.3.1/qt-opensource-windows-x86-msvc2013_opengl-5.3.1.exe)。该界面库有多个vs版本而且区分32位和64位，下载对应开发环境对应的版本才可正常编译。QT插件[下载](http://download.qt.io/archive/vsaddin/)
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

用户在库环境初始化时，需要提供该数组进行鉴权，具体参考 fuSetup 接口。没有证书、证书失效、网络连接失败等情况下，会造成鉴权失败，在控制台或者Android平台的log里面打出 "not authenticated" 信息，并在运行一段时间后停止渲染道具。

任何其他关于授权问题，请email：support@faceunity.com

## FAQ

## 为什么过了一段时间人脸识别失效了？

检查证书。如证书是否正确使用，是否过期。您需要拥有我司颁发的证书才能使用我们的SDK的功能，获取证书方法：1、拨打电话 **0571-88069272** 2、发送邮件至 **marketing@faceunity.com** 进行咨询。

## 函数接口及参数说明

```C
/**
\brief Initialize and authenticate your SDK instance to the FaceUnity server, must be called exactly once before all other functions.
  The buffers should NEVER be freed while the other functions are still being called.
  You can call this function multiple times to "switch pointers".
\param v3data should point to contents of the "v3.bin" we provide
\param ardata should be NULL
\param authdata is the pointer to the authentication data pack we provide. You must avoid storing the data in a file.
  Normally you can just `#include "authpack.h"` and put `g_auth_package` here.
\param sz_authdata is the authentication data size, we use plain int to avoid cross-language compilation issues.
  Normally you can just `#include "authpack.h"` and put `sizeof(g_auth_package)` here.
\return non-zero for success, zero for failure
*/
FUNAMA_API int fuSetup(float* v3data,float* ardata,void* authdata,int sz_authdata);
/**
\brief Call this function when the GLES context has been lost and recreated.
  That isn't a normal thing, so this function could leak resources on each call.
*/
FUNAMA_API void fuOnDeviceLost();
/**
\brief Call this function to reset the face tracker on camera switches
*/
FUNAMA_API void fuOnCameraChange();
/**
\brief Create an accessory item from a binary package, you can discard the data after the call.
  This function MUST be called in the same GLES context / thread as fuRenderItems.
\param data is the pointer to the data
\param sz is the data size, we use plain int to avoid cross-language compilation issues
\return an integer handle representing the item
*/
FUNAMA_API int fuCreateItemFromPackage(void* data,int sz);
/**
\brief Destroy an accessory item.
  This function MUST be called in the same GLES context / thread as the original fuCreateItemFromPackage.
\param item is the handle to be destroyed
*/
FUNAMA_API void fuDestroyItem(int item);
/**
\brief Destroy all accessory items ever created.
  This function MUST be called in the same GLES context / thread as the original fuCreateItemFromPackage.
*/
FUNAMA_API void fuDestroyAllItems();

FUNAMA_API void fuClearRenderData();
/**
\brief Render a list of items on top of a GLES texture or a memory buffer.
  This function needs a GLES 2.0+ context.
\param texid specifies a GLES texture. Set it to 0u if you want to render to a memory buffer.
\param img specifies a memory buffer. Set it to NULL if you want to render to a texture.
  If img is non-NULL, it will be overwritten by the rendered image when fuRenderItems returns
\param w specifies the image width
\param h specifies the image height
\param frameid specifies the current frame id.
  To get animated effects, please increase frame_id by 1 whenever you call this.
\param p_items points to the list of items
\param n_items is the number of items
\return a new GLES texture containing the rendered image in the texture mode
*/
FUNAMA_API int fuRenderItems(int texid,int* img,int w,int h,int frame_id, int* p_items,int n_items);

/**
\brief Generalized interface for rendering a list of items.
  This function needs a GLES 2.0+ context.
\param out_format is the output format
\param out_ptr receives the rendering result, which is either a GLuint texture handle or a memory buffer
  Note that in the texture cases, we will overwrite *out_ptr with a texture we generate.
\param in_format is the input format
\param in_ptr points to the input image, which is either a GLuint texture handle or a memory buffer
\param w specifies the image width
\param h specifies the image height
\param frameid specifies the current frame id.
  To get animated effects, please increase frame_id by 1 whenever you call this.
\param p_items points to the list of items
\param n_items is the number of items
\return a GLuint texture handle containing the rendering result if out_format isn't FU_FORMAT_GL_CURRENT_FRAMEBUFFER
*/
FUNAMA_API int fuRenderItemsEx(
  int out_format,void* out_ptr,
  int in_format,void* in_ptr,
  int w,int h,int frame_id, int* p_items,int n_items);

/**
\brief Generalized interface for rendering a list of items.
  This function needs a GLES 2.0+ context.
\param out_format is the output format
\param out_ptr receives the rendering result, which is either a GLuint texture handle or a memory buffer
  Note that in the texture cases, we will overwrite *out_ptr with a texture we generate.
\param in_format is the input format
\param in_ptr points to the input image, which is either a GLuint texture handle or a memory buffer
\param w specifies the image width
\param h specifies the image height
\param frameid specifies the current frame id.
  To get animated effects, please increase frame_id by 1 whenever you call this.
\param p_items points to the list of items
\param n_items is the number of items
\param p_masks indicates a list of masks for each item, bitwisely work on certain face
\return a GLuint texture handle containing the rendering result if out_format isn't FU_FORMAT_GL_CURRENT_FRAMEBUFFER
*/
FUNAMA_API int fuRenderItemsMasked(
  int out_format,void* out_ptr,
  int in_format,void* in_ptr,
  int w,int h,int frame_id, int* p_items,int n_items, int* p_masks);

/**
\brief Generalized interface for beautifying image.
  Disable face tracker and item rendering.
  This function needs a GLES 2.0+ context.
\param out_format is the output format
\param out_ptr receives the rendering result, which is either a GLuint texture handle or a memory buffer
  Note that in the texture cases, we will overwrite *out_ptr with a texture we generate.
\param in_format is the input format
\param in_ptr points to the input image, which is either a GLuint texture handle or a memory buffer
\param w specifies the image width
\param h specifies the image height
\param frameid specifies the current frame id.
  To get animated effects, please increase frame_id by 1 whenever you call this.
\param p_items points to the list of items
\param n_items is the number of items
\return a GLuint texture handle containing the rendering result if out_format isn't FU_FORMAT_GL_CURRENT_FRAMEBUFFER
*/
FUNAMA_API int fuBeautifyImage(
  int out_format,void* out_ptr,
  int in_format,void* in_ptr,
  int w,int h,int frame_id, int* p_items,int n_items);

/**
\brief Generalized interface for tracking face.
  Disable item rendering and image beautifying.
  This function needs a GLES 2.0+ context.
\param out_format is the output format
\param out_ptr receives the rendering result, which is either a GLuint texture handle or a memory buffer
  Note that in the texture cases, we will overwrite *out_ptr with a texture we generate.
\param in_format is the input format
\param in_ptr points to the input image, which is either a GLuint texture handle or a memory buffer
\param w specifies the image width
\param h specifies the image height
\param frameid specifies the current frame id.
  To get animated effects, please increase frame_id by 1 whenever you call this.
\param p_items points to the list of items
\param n_items is the number of items
\return a GLuint texture handle containing the rendering result if out_format isn't FU_FORMAT_GL_CURRENT_FRAMEBUFFER
*/
FUNAMA_API int fuTrackFace(int in_format,void* in_ptr,int w,int h);

/**
\brief Generalized interface for rendering a list of items with extension.
  This function needs a GLES 2.0+ context.
\param out_format is the output format
\param out_ptr receives the rendering result, which is either a GLuint texture handle or a memory buffer
  Note that in the texture cases, we will overwrite *out_ptr with a texture we generate.
\param in_format is the input format
\param in_ptr points to the input image, which is either a GLuint texture handle or a memory buffer
\param w specifies the image width
\param h specifies the image height
\param frameid specifies the current frame id.
  To get animated effects, please increase frame_id by 1 whenever you call this.
\param p_items points to the list of items
\param n_items is the number of items
\param func_flag flags indicate all changable functionalities of render interface
\param p_masks indicates a list of masks for each item, bitwisely work on certain face
\return a GLuint texture handle containing the rendering result if out_format isn't FU_FORMAT_GL_CURRENT_FRAMEBUFFER
*/
FUNAMA_API int fuRenderItemsEx2(
  int out_format,void* out_ptr,
  int in_format,void* in_ptr,
  int w,int h,int frame_id, int* p_items,int n_items,
  int func_flag, void* p_item_masks);

/**************************************************************
The set / get functions do not make sense on their own. Refer to
the documentation of specific items for their get/set-able
parameters. Most items do not have any.
**************************************************************/

/**
\brief Set an item parameter to a double value
\param item specifies the item
\param name is the parameter name
\param value is the parameter value to be set
\return zero for failure, non-zero for success
*/
FUNAMA_API int fuItemSetParamd(int item,char* name,double value);
/**
\brief Set an item parameter to a double array
\param item specifies the item
\param name is the parameter name
\param value points to an array of doubles
\param n specifies the number of elements in value
\return zero for failure, non-zero for success
*/
FUNAMA_API int fuItemSetParamdv(int item,char* name,double* value,int n);
/**
\brief Set an item parameter to a string value
\param item specifies the item
\param name is the parameter name
\param value is the parameter value to be set
\return zero for failure, non-zero for success
*/
FUNAMA_API int fuItemSetParams(int item,char* name,char* value);
/**
\brief Get an item parameter as a double value
\param item specifies the item
\param name is the parameter name
\return double value of the parameter
*/
FUNAMA_API double fuItemGetParamd(int item,char* name);
/**
\brief Get an item parameter as a string
\param item specifies the item
\param name is the parameter name
\param buf receives the string value
\param sz is the number of bytes available at buf
\return the length of the string value, or -1 if the parameter is not a string.
*/
FUNAMA_API int fuItemGetParams(int item,char* name,char* buf,int sz);

/**
\brief Turn off the camera
*/
FUNAMA_API void fuTurnOffCamera();
/**
\brief Get the camera image size
\param pret points to two integers, which receive the size
*/
FUNAMA_API void fuGetCameraImageSize(int* pret);
/**
\brief Get the face tracking status
\return The number of valid faces currently being tracked
*/
FUNAMA_API int fuIsTracking();
/**
\brief Set the default orientation for face detection. The correct orientation would make the initial detection much faster.
\param rmode is the default orientation to be set to, one of 0..3 should work.
*/
FUNAMA_API void fuSetDefaultOrientation(int rmode);
/**
\brief Set the maximum number of faces we track. The default value is 1.
\param n is the new maximum number of faces to track
\return The previous maximum number of faces tracked
*/
FUNAMA_API int fuSetMaxFaces(int n);
/**
\brief Set the quality-performance tradeoff.
\param quality is the new quality value.
       It's a floating point number between 0 and 1.
       Use 0 for maximum performance and 1 for maximum quality.
       The default quality is 1 (maximum quality).
*/
FUNAMA_API void fuSetQualityTradeoff(float quality);

/**
\brief Get face info. Certificate aware interface.
\param face_id is the id of face, index is smaller than which is set in fuSetMaxFaces
\param name is among "landmarks", "eye_rotation", "translation", "rotation"
\param pret allocated memory space as container
\param num is number of float allocated in pret
  eg:     "landmarks" - 75*2 float
        "landmarks_ar" - 75*3 float
        "eye_rotation" - 4
        "translation" - 3
        "rotation" - 4
        "projection_matrix" - 16
\return 1 means successful fetch, container filled with info
  0 means failure, general failure is due to invalid face info
  other specific failure will print on the console
*/
FUNAMA_API int fuGetFaceInfo(int face_id, char* name, float* pret, int num);

/**
\brief Bind items to an avatar, already bound items won't be unbound
\param avatar_item is the avatar item handle
\param p_items points to a list of item handles to be bound to the avatar
\param n_items is the number of item handles in p_items
\param p_contracts points to a list of contract handles for authorizing items
\param n_contracts is the number of contract handles in p_contracts
\return the number of items newly bound to the avatar
*/
FUNAMA_API int fuAvatarBindItems(int avatar_item, int* p_items,int n_items, int* p_contracts,int n_contracts);
/**
\brief Unbind items from an avatar
\param avatar_item is the avatar item handle
\param p_items points to a list of item handles to be unbound from the avatar
\param n_items is the number of item handles in p_items
\return the number of items unbound from the avatar
*/
FUNAMA_API int fuAvatarUnbindItems(int avatar_item, int* p_items,int n_items);

//
FUNAMA_API int fuBindItems(int item_src, int* p_items,int n_items);
FUNAMA_API int fuUnbindAllItems(int item_src);

/**
\brief Get SDK version string
\return SDK version string in const char*
*/
FUNAMA_API const char* fuGetVersion();
```
用户在库环境初始化时，需要提供该数组进行鉴权，具体参考 fuSetup 接口。没有证书、证书失效、网络连接失败等情况下，会造成鉴权失败，在控制台或者Android平台的log里面打出 "not authenticated" 信息，并在运行一段时间后停止渲染道具。

任何其他关于授权问题，请email：support@faceunity.com
