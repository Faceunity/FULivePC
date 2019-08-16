# Nama C API Reference
### Updates

2019-05-07 v6.0.2:
1. Added fuSetupLocal function to support offline authentication.

2019-03-31 v6.0.0：
1. Added fuSetFaceDetParam to set face detection parameters.
2. Added fuTrackFaceWithTongue to support tongue trackin.
3. Added fuSetTongueTracking, on/off tongue tracking.
4. Added the parameter expression_with_tongue(fuGetFaceInfo) to get the tongue information.
5. Abandoned fuLoadExtendedARData。

------
### Contents
[TOC]

------
### 1. Introduction 
This document is the infrastructure layer interface for FaceUnity face tracking and video effects development kit (Nama SDK for short). The Nama API in this document is the Native interface for direct development on PC/iOS/Android NDK/Linux. Development on the iOS and Android platforms can use the SDK's application-level interface (Objective-C/Java), which is closer to the platform-related development experience than the infrastructure layer interface in this article.

All SDK-related calls require sequential execution in the same thread, without support for multithreading. A small number of interfaces can be called asynchronously (such as props loading) and will be specifically noted in the remarks. Interfaces called by all main threads of the SDK need to keep the OpenGL context consistent, otherwise it will cause texture data exceptions. If you need to use the SDK's rendering function, all calls to the main thread need to pre-initialize the OpenGL environment, without initialization or incorrect initialization will cause a crash. Our environment requirement for OpenGL is GLES 2.0 or higher. For the specific call method, please refer to each platform demo.*

The infrastructure layer interface is classified into five categories according to the role of logic: initialization, propsloading, main running interface, destruction, functional interface, P2A related interface.

------
### 2. APIs
#### 2.1 Initialization
##### fuSetup 
Initialize the system environment, load the system data, and perform network authentication. Must be executed before calling the SDK's other interfaces, otherwise it will cause a crash.

```C
int fuSetup(float* v3data, int sz_v3data, float* ardata, void* authdata, int sz_authdata);
```

__Parameters:__

*v3data [in]*： Memory pointer to the contents of the v3.bundle file provided by the SDK

*sz_v3data [in]*: v3.bundle file bytes

*ardata [in]*： Abandoned

*authdata [in]*： Memory pointers, point to the contents of the authentication data. If you provide authentication data at compile time using the method containing authpack.h, this can be written as ```g_auth_package```

*sz_authdata [in]*：The length of the authentication data, in bytes. If the authentication data provides ```g_auth_package``` in authpack.h, here can be written ```sizeof(g_auth_package)```



__Return Value:__

Returns a non-zero value for success and 0 for failure. If initialization fails, get the error code via ```fuGetSystemError```

###### __Comment:__  

According to application requirements, authentication data can also be provided during runtime (such as network download), but it is necessary to pay attention to the risk of certificate leakage and prevent the abuse of certificates. ```int``` is used for data length to prevent cross-platform data type issues.  

Need to initialize in the place where there is GL Context.

------
##### fuSetupLocal 
Initialize the system environment, load the system data, and perform network authentication. Must be executed before calling the SDK's other interfaces, otherwise it will cause a crash.

```C
int fuSetupLocal(float* v3data, int sz_v3data,float* ardata,void* authdata,int sz_authdata,void** offline_bundle_ptr,int* offline_bundle_sz);

```

__Parameters:__

*v3data [in]*： Memory pointer to the contents of the v3.bundle file provided by the SDK

*sz_v3data [in]*: v3.bundle file bytes

*ardata [in]*： Abandoned

*authdata [in]*： Memory pointers, point to the contents of the authentication data. If you provide authentication data at compile time using the method containing authpack.h, this can be written as ```g_auth_package```

*sz_authdata [in]*：The length of the authentication data, in bytes. If the authentication data provides ```g_auth_package``` in authpack.h, here can be written ```sizeof(g_auth_package)```

*offline_bundle_ptr [in/out]*： Memory pointer to the contents of offline certificate。

*offline_bundle_sz [in/out]*：The length of the offline certificate data, in bytes



__Return Value:__

Returns a non-zero value for success and 0 for failure. If initialization fails, get the error code via ```fuGetSystemError```

If successful, Parametersoffline_bundle_ptr and offline_bundle_sz will be modified to the contents of the actual offline certificate, need to save. Such as .bundle files. Used for the next initialization authentication, no network authentication is required.



__Comment:__  

According to application requirements, authentication data can also be provided during runtime (such as network download), but it is necessary to pay attention to the risk of certificate leakage and prevent the abuse of certificates. ```int``` is used for data length to prevent cross-platform data type issues.  

Need to initialize in the place where there is GL Context.

The first time you need network authentication, after the authentication is successful, saving the new certificate, no need network next time.

------

#### 2.2 Props loading package

##### fuCreateItemFromPackage 

Load the prop package so that it can be executed in the main run interface. A prop package may be a function module or a collection of multiple function modules. The prop loading package may activate the corresponding function module and implement plug and play in the same SDK calling logic.

``` C
int fuCreateItemFromPackage(void* data, int sz);
```

__Parameters:__  

*data [in]*： Memory pointer to the contents of the loaded prop package. Props usually end with *.bundle.

*sz [in]*：The data length of the prop package content, in bytes.



__Return Value:__

An integer handle as the identifier of the item in the system.



__Comment:__  

This interface can be executed asynchronously with the main thread. In order to reduce the loader blocking the main thread, it is recommended to call this interface asynchronously.

------
##### fuItemSetParam 

Modify or set the value of the variable in the prop package. support the prop package variable name, meaning, and range of values (please refer to the prop document).

```C
// Set the type of the double variable in the item
int fuItemSetParamd(int item,char* name,double value);
// Set the double array type variable in the item
int fuItemSetParamdv(int item,char* name,double* value,int n);
// Set the string type variable in the item
int fuItemSetParams(int item,char* name,char* value);
```

__Parameters:__

*item [in]*：The item identifier, which should be the return value of the call to the ```fuCreateItemFromPackage``` function, and the item is not destroyed.

*name [in]*：A string pointer , containing the name of the item variable to be set.

*value [in]*：To set the variable value, the data type corresponds to a different function interface.

*n [in]*：The length of the array data in double when the variable is set to a double array



__Return Value:__

Returns a non-zero value for success and 0 for failure.



__Comment:__

The string to end with 0 is required when setting the variable to a string.

------
##### fuItemGetParam 
get the value of the variable in the prop item. support the prop package variable name, meaning, and range of values (please refer to the prop document)

```C
// Get float type variables in the item
double fuItemGetParamd(int item,char* name);
```
__Parameters:__

*item [in]*：The item identifier, which should be the return value of the call to the ```fuCreateItemFromPackage``` function, and the item is not destroyed.

*name [in]*：A string pointer, containing the name of the item variable to be gotten.



__Return Value:__

The value of the variable to get. If the execution fails, the return value is 0.



__Comment:__  

This interface can be executed asynchronously with the main thread.

------
##### fuItemGetParams 
Get the string type variable in the prop.

```C
// Get the string type variable in the prop
int fuItemGetParams(int item,char* name,char* buf,int sz);
```

__Parameters:__  

*item [in]*： The item identifier, which should be the return value of the call to the ```fuCreateItemFromPackage``` function, and the item is not destroyed.

*name [in]*：A string pointer, containing the name of the item variable to be modified

*buf [out]*：A memory pointer that points to the pre-allocated memory space and is used to receive the string content returned by the function.

*sz [in]*：The maximum memory length pre-allocated in buf, in bytes.



__Return Value:__  

Returns the length of the variable string. If the execution fails, the return value is -1.  



__Comment:__  

This interface can be executed asynchronously with the main thread.  




#### 2.3 Main running interface
------
##### fuRenderItems 
The input image data is sent to the SDK pipeline for processing, and output the processed image data. The interface will execute all the props required, and the certificate of the licensed function module, including face detection and tracking, beauty, stickers or avatar rendering.

This interface supports two input and output modes, input RGBA texture and output RGBA texture, or input BGRA array output BGRA array. The RGBA texture is an OpenGL texture. In this mode, the ID of the input texture is passed in and the output texture ID is returned in the function return value. The BGRA array is a memory image cache, and the data format is an 8-bit 4-channel image. In this mode, the input image is passed in as a parameter, and the output image is overwritten into the same memory space.

```C
int fuRenderItems(int texid,int* img,int w,int h,int frame_id, int* p_items,int n_items);
```

__Parameters:__  

*texid [in]*：OpenGL texture ID entered in texture mode, 0 in non-texture mode.

*img [in & out]*：The image data input in the memory array mode, the processed image data is also covered in the memory space. This parameter can be passed NULL in texture mode.

*w [in]*：The width of the input image

*h [in]*：The height of the input image

*frame_id [in]*：The current processed frame sequence number used to control the animation logic in the item.

*p_items [in]*：A memory pointer to an array of item identifiers to execute. Each of these identifiers should be the return value of the call to the ```fuCreateItemFromPackage``` function, and the prop content is not destroyed.

*n_items [in]*：The number of props in the *p_items* array




__Return Value:__  

The texture ID of the output image after processing. if the return value is less than or equal to 0, you can get the specific information via fuGetSystemError.



__Comment:__  

Even in non-texturing mode, the function still returns the texture ID of the output image. Although the input and output are all memory arrays in this mode, the rendering work is still done by the GPU, so the texture ID of the output image also exists. The output OpenGL texture is the texture created by the SDK runtime in the current OpenGL context, and its ID should be different from the input ID. Using this texture after output needs to ensure that the OpenGL context is consistent. 

The rendering interface requires an OpenGL environment, or an exception in the environment can cause a crash.

------
##### fuRenderItemsEx 
The input image data is sent to the SDK pipeline for processing, and output the processed image data. The interface will execute all the props required, and the certificate of the licensed function module, including face detection and tracking, beauty, stickers or avatar rendering.

Compared to ```fuRenderItems```, this interface supports more flexible input and output modes. For more detailed input/output format list, please refer to the subsequent chapters [Input/Output format list](#Input/Output format list).

```C
int fuRenderItemsEx(
	int out_format,void* out_ptr,
	int in_format,void* in_ptr,
	int w,int h,int frame_id, int* p_items,int n_items);
```

__Parameters:__  

*out_format [in]*：Output data format identifier

*out_ptr [out]*：Memory pointer to the output data content.

*in_format [in]*：Input data format identifier.

*in_ptr [in]*：Memory pointer to the input data content.

*w [in]*：The width of the input image.

*h [in]*：The height of the input image.

*frame_id [in]*：The current processed frame sequence number used to control the animation logic in the item.

*p_items [in]*：A memory pointer to an array of item identifiers to execute. Each of these identifiers should be the return value of the call to the ```fuCreateItemFromPackage``` function, and the prop content is not destroyed.

*n_items [in]*：The number of props in the *p_items* array



__Return Value:__  

The texture ID of the output image after processing.



__Comment:__  

Even in non-texturing mode, the function still returns the texture ID of the output image. Although the output image may be in a variety of selectable formats, rendering is always done through the GPU, so the texture ID of the output image always exists. The output OpenGL texture is the texture created by the SDK runtime in the current OpenGL context, and its ID should be different from the input ID. Using this texture after output needs to ensure that the OpenGL context is consistent.

The rendering interface requires an OpenGL environment, or an exception in the environment could cause a crash.

------
##### fuRenderItemsEx2 
The input image data is sent to the SDK pipeline for processing, and output the processed image data.

Compared to ```fuRenderItemsEx``` this interface adds user control over the pipeline. By passing in the pipeline function mask Parameters, you can control the switch of the specified function module, as well as specific rendering options. By passing in an array of prop masks, it is possible to control which faces are effective for each item in a multi-face situation.

```C
int fuRenderItemsEx2(
	int out_format,void* out_ptr,
	int in_format,void* in_ptr,
	int w,int h,int frame_id, int* p_items,int n_items,
	int func_flag, int* p_item_masks);
```

__Parameters:__  

*out_format [in]*：Output data format identifier.

*out_ptr [out]*：Memory pointer to the output data content.

*in_format [in]*：Input data format identifier.

*in_ptr [in]*：Memory pointer to the input data content.

*w [in]*：The width of the input image.

*h [in]*：The height of the input image.

*frame_id [in]*：The current processed frame sequence number used to control the animation logic in the item.

*p_items [in]*：A memory pointer to an array of item identifiers to execute. Each of these identifiers should be the return value of the call to the ```fuCreateItemFromPackage``` function, and the prop content is not destroyed.

*n_items [in]*：The number of props in the *p_items* array

*func_flag*：Pipeline functional masks, which represent pipeline-enabled functional modules, and specific rendering options. Multiple masks are connected by the operator "or". All supported masks and their meanings are as follows.

| Pipeline Functional Masks               | Meanings                                                     |
| --------------------------------------- | ------------------------------------------------------------ |
| NAMA_RENDER_FEATURE_TRACK_FACE          | Face recognition and tracking                                |
| NAMA_RENDER_FEATURE_BEAUTIFY_IMAGE      | Input image beautification function                          |
| NAMA_RENDER_FEATURE_RENDER              | Face-related rendering functions such as beauty, stickers, face deformation, filters, etc. |
| NAMA_RENDER_FEATURE_ADDITIONAL_DETECTOR | Other non-face recognition functions, including background segmentation, gesture recognition,etc |
| NAMA_RENDER_FEATURE_RENDER_ITEM         | Face-related props such as stickers                          |
| NAMA_RENDER_FEATURE_FULL                | Render pipeline functions are all started                    |
| NAMA_RENDER_OPTION_FLIP_X               | Render option, Flip Horizonta                                |
| NAMA_RENDER_OPTION_FLIP_Y               | Render option, Flip Vertical                                 |

*p_item_masks*：Prop mask means that in the multiplayer mode, each item is effective for which faces. The length of the array should be the same as *p_items*, and each item has an int type mask. In those masks, they are sorted from the low to the high, the i-th value of 1 indicates that the item is valid for the i-th face, and the value 0 is not valid.



__Return Value:__  

The texture ID of the output image after processing.



__Comment:__  

Even in non-texturing mode, the function still returns the texture ID of the output image. Although the output image may be in a variety of selectable formats, rendering is always done through the GPU, so the texture ID of the output image always exists. The output OpenGL texture is the texture created by the SDK runtime in the current OpenGL context, and its ID should be different from the input ID. Using this texture after output needs to ensure that the OpenGL context is consistent.

The rendering interface requires an OpenGL environment, or an exception in the environment could cause a crash.

------
##### fuBeautifyImage 
The input image data is sent to the SDK pipeline for full size image beautification, and output the processed image data., The interface executes only the beautification of the image plane (including filters, skin), and does not execute face tracking and all face-related operations . Due to the single function, the implementation of beautification props requires less computation and more efficient execution compared to the ```fuRenderItemsEx``` interface

```C
int fuBeautifyImage(
	int out_format,void* out_ptr,
	int in_format,void* in_ptr,
	int w,int h,int frame_id, int* p_items,int n_items);
```

__Parameters:__  

*out_format [in]*：Output image data format.

*out_ptr [out]*：Memory pointer to the output image data. 

*in_format [in]*：Input image data format. 

*in_ptr [in]*：Memory pointer to the input image data. 

*w [in]*：The width of the input image.

*h [in]*：The height of the input image.

*frame_id [in]*：The current processed frame sequence number used to control the animation logic in the item.

*p_items [in]*：A memory pointer to an array of item identifiers to execute. Each of these identifiers should be the return value of the call to the ```fuCreateItemFromPackage``` function, and the prop content is not destroyed.

*n_items [in]*：The number of props in the *p_items* array



__Return Value:__  

The texture ID of the output image after processing.



__Comment:__  

The incoming props must contain beauty props (distributed with the SDK, the file name is usually ```face_beautification.bundle```) that the interface could works properly. All non-beauty items in the incoming item will not take effect and also not occupy computing resources. 

The rendering interface requires an OpenGL environment, or an exception in the environment could cause a crash.

------
##### fuTrackFace 
Only the face tracking operation is performed on the input image data, and all other image and rendering related operations are not executed, so the function has no image output. Since this function does not perform rendering-related operations and only contains CPU calculations, it can operate without an OpenGL environment. After the function executes the face tracking operation, the resulting face information is acquired through the```fuGetFaceInfo``` 

```C
int fuTrackFace(int in_format,void* in_ptr,int w,int h);
```

__Parameters:__  

*in_format [in]*：Input image data format. 

*in_ptr [in]*：Memory pointer to the input image data. 

*w [in]*：The width of the input image.

*h [in]*：The height of the input image.



__Return Value:__  

The number of faces successfully tracked in the image.



__Comment:__  

This function only supports some input image formats, including RGBA_BUFFER, BGRA_BUFFER, NV12 BUFFER, and NV21 BUFFER.

The interface does not need to OpenGL environment, but it still needs to run in the SDK main thread.

------
##### fuTrackFaceWithTongue 
Same as ``` fuTrackFace ```，tracking the tongue's blendshape coefficient while tracking facial expressions.  
Only the face tracking operation is performed on the input image data, and all other image and rendering related operations are not executed, so the function has no image output. Since this function does not perform rendering-related operations and only contains CPU calculations, it can operate without an OpenGL environment. After the function executes the face tracking operation, the resulting face information is acquired through the```fuGetFaceInfo``` 

```C
int fuTrackFaceWithTongue(int in_format,void* in_ptr,int w,int h);
```

__Parameters:__  

*in_format [in]*：Input image data format. 

*in_ptr [in]*：Memory pointer to the input image data. 

*w [in]*：The width of the input image.

*h [in]*：The height of the input image.



__Return Value:__  

The number of faces successfully tracked in the image.



__Comment:__  

Need to load tongue.bundle to make tongue tracking available.

you can get the expression coefficient that contain the tongue via

```fuGetFaceInfo(0,"expression_with_tongue",pret,56)```



------

#### 2.4 Destroy

When you don't need to use the SDK, you can release the SDK related memory, including the prop rendering memory, and the memory of the face tracking module.。 

##### fuDestroyItem 
Destroy a specified item.

```C
void fuDestroyItem(int item);
```

__Parameters:__  

*item [in]*：The item identifier, which should be the return value of the call to the ```fuCreateItemFromPackage``` function, and the item is not destroyed.

__Comment:__  

After the function is called, the item identifier will be released immediately, and the memory occupied by the item cannot be released instantaneously, and the SDK needs to be recovered by the GC mechanism when the main processing interface is subsequently executed by the SDK.

------
##### fuDestroyAllItems 
Destroy all items loaded by the system and releases all resources used by the system while it is running.

```C
void fuDestroyAllItems();
```

__Comment:__  

This function will immediately release the system's resources. However, ```fuSetup```'s system initialization information will not be destroyed. When the application is temporarily suspended to the background, the function can be called to release resources, and it is not necessary to reinitialize the system when it is activated again.

------
##### fuOnDeviceLost 
The special function, called when the OpenGL context is released/destroyed externally, is used to reset the system's GL state.

```C
void fuOnDeviceLost();
```

__Comment:__  

This function is only called if the resource cannot be properly cleaned up in the original OpenGL context. When this function is called, it will try to clean up and recycle the resources. The memory resources occupied by all the systems will be released. However, due to the change of context, the memory related to OpenGL resources may leak.

------

#### 2.5 Functional interface - System

##### fuOnCameraChange 
Called when the camera data source is switched (for example, front/back camera switch), to reset the face tracking status.
```C
void fuOnCameraChange();
```

__Comment:__  

In the case of other facial information remaining, you can also call this function to clear the residual face information or when the camera is switched, the reset face tracking module is triggered.

------
##### fuSetFaceDetParam 
Set the face detector related parameters, **it is recommended to use the default Parameters**

```C
int fuSetFaceDetParam(char* name, float* pvalue);
```
__Parameters:__  

*name*：Parameters name 

*pvalue*: Parameters value

- `name == "use_new_cnn_detection"` ，and `pvalue == 1` the default CNN-Based face detection algorithm is used, otherwise pvalue == 0  uses the traditional face detection algorithm. This mode is turned on by default.
- name == "other_face_detection_frame_step"` ，If the current state has detected a face, you can set the parameter and perform other face detection every step` frame to help improve performance，default 10 frames.

if  `name == "use_new_cnn_detection"` ，and `pvalue == 1`  It's  on：
- `name == "use_cross_frame_speedup"`，`pvalue==1`Indicates that cross-frame execution is enabled, half of the network is executed per frame, and the lower half of the grid is executed in the next frame, which improves performance. Default pvalue==0   It's off.
- `name == "small_face_frame_step"`，`pvalue`Indicates how many frames are used to enhance small face detection. Very small face detection is very cost effective and not suitable for every frame. The default pvalue==5.
- When detecting small faces, small faces can also be defined as ranges. The lower limit of the range name == "min_facesize_small", the default pvalue==18, which means that the minimum face is 18% of the screen width. The upper limit of the range name == "min_facesize_big", the default pvalue==27, which means that the minimum face is 27% of the screen width. The Parameters must be set before fuSetup.

else，if `name == "use_new_cnn_detection"` ，and  `pvalue == 0`：
- `name == "scaling_factor"`，Set the zoom ratio of the image pyramid. The default is 1.2f.

- `name == "step_size"`，The sliding interval of the sliding window, default 2.f.

- `name == "size_min"`，The minimum face size, which pixels. The default is 50.f pixels, with reference to 640x480 resolution.

- `name == "size_max"`，Maximum face size, which pixels. The default is the largest, reference 640x480 resolution.

- `name == "min_neighbors"`，Internal Parameters, default 3.f

- `name == "min_required_variance"`， Internal Parameters, default 15.f

  


__Return Value:__  

After setting the status, 1 means successful, 0 means failed. 



__Comment:__  

`name == "min_facesize_small"`，`name == "min_facesize_small"` Parameters must be set before fuSetup.

------
##### fuSetTongueTracking 
Turn on the tracking of the tongue
```C
/**
\brief Turn on or turn off Tongue Tracking, used in trackface.
\param enable > 0 means turning on, enable <= 0 means turning off
*/
FUNAMA_API int fuSetTongueTracking(int enable);
```

__Comment:__  

When call the fuTrackFaceWithTongue interface, after loading the tongue.bundle, you need fuSetTongueTracking(1) to enable support for tongue tracking.

If the prop itself has a tongue bs, it does not need to be activated.



------
#####  fuSetASYNCTrackFace 
Set the face tracking asynchronous interface. It is off by default.
```C
int fuSetASYNCTrackFace(int enable);
```
__Parameters:__  

*enable[int]*：1 means turn on asynchronous tracking，0 means turn off.



__Return Value: __ 

Tracking status after setting, 1 means asynchronous tracking has been turned on, 0 means asynchronous tracking has been turned off.



__Comment:__  

It is off by default. After being turned on, the face tracking will be asynchronous with the rendering , and the CPU usage will increase slightly, but the overall speed will increase and the frame rate will increase.

------
#####  fuIsTracking 
Get current face tracking status and return the number of faces being tracked.
```C
int fuIsTracking();
```

__Return Value:__  

The number of faces being tracked.



__Comment:__  

The number of faces being tracked will be affected by the ```fuSetMaxFaces``` function but will not exceed the maximum value set by this function.

------
##### fuSetDefaultOrientation 
Set the default face orientation. Correctly setting the default face orientation can significantly increase the speed of first face recognition.
```C
void fuSetDefaultOrientation(int rmode);  
```
__Parameters:__  

*rmode [in]*：To set the face orientation, the value range is 0-3, corresponding to the rotation of the human face relative to the image data by 0 , 90 , 180 , and 270 degrees. 



__Comment:__  

In general, iOS's native camera data is portrait and does not require this setting. The native camera data of the Android platform is a horizontal screen and needs to be set to speed up the first recognition. According to experience, Android front camera is usually set to Parameters 1, and the rear camera is usually set to Parameters 3, except for some mobile phones. The auto-calculated code can refer to fuLiveDemo.

------
##### fuSetMaxFaces 
Sets the maximum number of faces tracked by the system. The default value is 1, which increases the performance of the face tracking module and is recommended to be set to 1 in all situations where it can be designed as a single face.
```C
int fuSetMaxFaces(int n);
```
__Parameters:__  

*n [in]*：The maximum number of faces to set.



__Return Value:__  

Set the previous system's maximum number of faces.

------
##### fuGetFaceInfo 
After the face tracking operation is performed on the main interface, the face tracking result information is obtained through the interface. Obtaining information requires a certificate to provide related permissions. Currently the face information permission includes the following levels: Default, Landmark, and Avatar.
```C
int fuGetFaceInfo(int face_id, char* name, float* pret, int num);
```
__Parameters:__  

*face_id [in]*：Face number, which represents the identified x-th face, starting from 0.

*name [in]*：The name of the information to get.

*pret [out]*：Return data container, need to allocate memory space before function call.

*num [in]*：Returns the length of the data container, in units of sizeof(float).



__Return Value:__  

Return 1 means success, and information is returned via pret, and return 0 indicates that the information obtain failed. The specific failure information is printed on the platform console. If the return value is 0 and no console prints, the required face information is currently unavailable.



__Comment:__  

The information, meaning, and permission requirements for all support obtaining are as follows：

| Name           | Length | Meaning                                                      | Permission |
| -------------- | ------ | ------------------------------------------------------------ | ---------- |
| face_rect      | 4      | Face rectangular frame, image resolution coordinates, data (x_min, y_min, x_max, y_max) | Default    |
| rotation_mode  | 1      | Identify the orientation of the face relative to the rotation of the device image. Values range from 0 to 3, representing 0, 90, 180, and 270 degrees, respectively. | Default    |
| failure_rate   | 1      | The failure rate of face tracking indicates the quality of face tracking. Value range from 0 to 2. The lower the value, the higher the quality of face tracking. | Default    |
| is_calibrating | 1      | Indicates whether the SDK is performing an active expression calibration with a value of 0 or 1. | Default    |
| focal_length   | 1      | The focus values on SDK's current 3D face tracking           | Default    |
| landmarks      | 75x2   | Face 75 feature points, image resolution coordinates         | Landmark   |
| rotation       | 4      | 3D face rotation, data rotation quaternion\*                 | Landmark   |
| translation    | 3      | Face 3D translation，data (x, y, z)                          | Landmark   |
| eye_rotation   | 4      | Eyeball rotation, data rotation quaternion\*                 | Landmark   |
| expression     | 46     | Face Expression Coefficient, Expression Coefficient Meaning Refer to《Expression Guide》 | Avatar     |

*Notes：Rotary quaternion to Euler angle can be referenced [Url](https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles)。



Get 3D Grid Code  for Reference
```C
	int vercnt = 0;
	fuGetFaceInfo(0, "armesh_vertex_num", (float*)&vercnt, 1);
	logi("armesh_vertex_num %d: %f", (int)vercnt,vercnt);
	int facecnt = 0;
	fuGetFaceInfo(0, "armesh_face_num", (float*)&facecnt, 1);
	logi("armesh_face_num %d:", facecnt);
	vector<float> v, vt;
	vector<int>f;
	v.resize(vercnt * 3);
	vt.resize(vercnt * 2);
	f.resize(facecnt* 3);
	int ret1 = fuGetFaceInfo(0, "armesh_vertices", v.data(), v.size());
	int ret2 = fuGetFaceInfo(0, "armesh_uvs", vt.data(), vt.size());
	int ret3 = fuGetFaceInfo(0, "armesh_faces", (float*)f.data(), f.size());
	logi("ret1: %d ret2: %d ret3: %d \n", ret1, ret2, ret3);
	string str = "";
	ostringstream osstr(str);
	for (int i = 0; i < vercnt; i++) {
		osstr << "v " << std::to_string(v[i * 3 + 0]) << " " << std::to_string(v[i * 3 + 1]) << " " << std::to_string(v[i * 3 + 2]) << "\n";
	}
	for (int i = 0; i < vercnt; i++) {
		osstr << "vt " << std::to_string(vt[i * 2 + 0]) << " " << std::to_string(vt[i * 2 + 1]) << "\n";
	}
	for (int i = 0; i < facecnt; i++) {
		osstr << "f " << std::to_string(f[i * 3 + 0]+1) << " " << std::to_string(f[i * 3 + 1]+1) << " " << std::to_string(f[i * 3 + 2]+1) << "\n";
	}
	string objfile = "./face.obj";
	ofstream ost;
	ost.open(objfile);
	str = osstr.str();
	ost.write(str.c_str(),str.length());
```

Get the rotated quaternion of the header and convert it to the Euler angle. Sample code:
```C
float rotation[4];
int ret = fuGetFaceInfo(0, "rotation", rotation, 4);
if (ret){
	Quaternion<float> q;
	q.v[0] = rotation[0];
	q.v[1] = rotation[1];
	q.v[2] = rotation[2];
	q.w = rotation[3];
	Float3 vec = q.toAngles();
	vec *= 180 / 3.14;
	logi("rotation angle:[%f,%f,%f]\n", vec.x(), vec.y(), vec.z());
}
```
Get the 2D feature points of the header and draw into the 2D screen space. Sample code:
```C
float data[150];
//logi("render\n");
int ret = fuGetFaceInfo(0, "landmarks", data, 150);
if (ret){
	for (int i = 0; i < 75; i++){
		//logi("%d :[%d,%d]\n", i, (int)data[i * 2], (int)data[i * 2 + 1]);
		cv::Point pointInterest;
		pointInterest.x = data[i * 2];
		pointInterest.y = data[i * 2 + 1];
		cvCircle(frameNamaProtrait, pointInterest, 2, cv::Scalar(0, 0, 255));
	}
}
```
Get the 3D feature points of the header and project to the 2D screen space. Sample code:
```C
float dde_rotation[4];
float dde_translation[3];
float dde_projection_matrix[16];
float dde_landmark_ar[75 * 3];
int ret = fuGetFaceInfo(0, "rotation", dde_rotation, 4);
ret = fuGetFaceInfo(0, "translation", dde_translation, 3);
ret = fuGetFaceInfo(0, "projection_matrix", dde_projection_matrix, 16);
ret = fuGetFaceInfo(0, "landmarks_ar", dde_landmark_ar, 75 * 3);
QuaternionF qrot; qrot.v[0] = dde_rotation[0]; qrot.v[1] = dde_rotation[1]; qrot.v[2] = dde_rotation[2]; qrot.w = dde_rotation[3];
Float3 tran(dde_translation[0], dde_translation[1], dde_translation[2]);
Mat4f trans_mat = qrot.toRotationMatrix();
for (int i = 0; i < 3; i++){
	trans_mat(i, 3) = tran[i];
}
Mat4f project_mat(dde_projection_matrix);
//logi("trans:[%f,%f,%f]\n", dde_translation[0], dde_translation[1], dde_translation[2]);
int screen_w = frameNamaProtrait->width;
int screen_h = frameNamaProtrait->height;
for (int i = 0; i < 75; i++){
	Float4 pos = Float4(dde_landmark_ar[i * 3], dde_landmark_ar[i * 3 + 1], dde_landmark_ar[i * 3 + 2],1);
	//to screen space
	pos = project_mat.trans() * trans_mat * pos;
	pos /= pos[3];
	Float3 store;
	store[0] = (pos[0] + 1) * screen_w - screen_w/2;
	store[1] = (pos[1] + 1) * screen_h - screen_h / 2;
	store[0] = (int)floor(store[0]);
	store[1] = (int)floor(store[1]);
	//draw
	cv::Point pointInterest;
	pointInterest.x = store[0];
	pointInterest.y = store[1];
	cvCircle(frameNamaProtrait, pointInterest, 2, cv::Scalar(0,0,255 ));
}
```

------
##### fuGetFaceIdentifier 
Get the identifier of the face being tracked, which is used to distinguish different faces in multiplayer situations outside the SDK.
```C
int fuGetFaceIdentifier(int face_id);
```
__Parameters:__  

*face_id [in]*：Face number, which represents the identified x-th face, starting from 0.



__Return Value:__  

Required face identifier.



__Comment:__  

Failing to track will chang identifiers, including fast re-tracking.

------
##### fuGetVersion 
Get SDK Version
```C
const char* fuGetVersion();
```

__Return Value:__  

A constant string pointer, the version number is as follows：
“Major version number\_sub version number\-Version check value”

------
##### fuGetSystemError 
System error is returned. This type of error is generally caused by a serious problem in the system mechanism, resulting in the system being shut down. Therefore, attention must be paid.
```C
const int fuGetSystemError();
```

__Return Value:__  

System error code



__Comment:__  

When complex errors exist, they are coded in a code in the form of bit operations. If the return code does not correspond to any single code in the following list, then there is a complex number error, and the most important error message can be parsed through the ```fuGetSystemErrorString``` function.

System error code and its meaning are as follows：

| Code  | Info                                            |
| ----- | ----------------------------------------------- |
| 1     | Random seed generation failed                   |
| 2     | Agency certificate parse failed                 |
| 3     | Authentication server connection failed         |
| 4     | Configure the encrypted connection failed       |
| 5     | Parse client certificate failed                 |
|       | Client key parse failed                         |
| 7     | Establish encrypted connection failed           |
| 8     | Set the authentication server address failed    |
| 9     | Encrypted connection handshake failed           |
| 10    | Verify the encrypted connection failed          |
| 11    | Request send failed                             |
| 12    | Response reception failed                       |
| 13    | Authentication response exception               |
| 14    | Certificate authority information is incomplete |
| 15    | Authentication function is not initialized      |
| 16    | Create authentication thread failed             |
| 17    | Authentication data is rejected                 |
| 18    | No authentication data                          |
| 19    | Authentication data exception                   |
| 20    | Expired certificate                             |
| 21    | Invalid certificate                             |
| 22    | System data parsing failed                      |
| 0x100 | Unofficial package loaded                       |
| 0x200 | Operation platform is prohibited by certificate |

------
##### fuGetSystemErrorString 
Parses the system error code and returns readable information.
```C
const char* fuGetSystemErrorString(int code);
```
__Parameters:__  

*code [in]*：system error code，The code is usually returned by ```fuGetSystemError```.



__Return Value:__  

A constant string explaining the meaning of the current error.



__Comment:__  

When a complex number error exists, this function returns the most important error message.

------
##### fuCheckDebugItem 
Check if a package is an informal package（debug item）.
```C
const int fuCheckDebugItem(void* data,int sz);
```
__Parameters:__  

*data [in]*： Memory pointer to the contents of the loaded prop package. Props usually end with *.bundle

*sz [in]*：The data length of the prop package content, in bytes



__Return Value:__  

Return  0 indicates that the item is a formal item, a return value of 1 indicates that the item is an informal item (debug  item), and Return -1 indicates that the item data is abnormal.



__Comment:__  

If the system loads an unofficial version of the item, it will cause the system to enter a countdown and close at the end of the countdown. If the system prompts "debug item used", or if the system stops after running for 1 minute, you need to use this function to check all loaded items. If there are unoffical items, you need to use the correct item signature.

Props signature process please contact our technical support.

------
#### 2.6 Functioncal - Effect
##### fuSetExpressionCalibration 
Set facial expression calibration function. The purpose of this function is to enable the facial expression recognition module to adapt to different people's facial features in order to achieve a more accurate and controllable expression tracking effect.

This function has two modes, active calibration and passive calibration.

- Active calibration: In this mode, the system will perform quick focused facial expression calibration, usually 2-3 seconds after the first recognition of the human face. During this period of time, the user needs to remain as expressionless as possible, and the process begins to be used again. The beginning and end of the process can be returned parameter```is_calibrating``` via the ```fuGetFaceInfo``` 
- Passive calibration: In this mode, the expression will be gradually adjusted during the entire using, and the user does not have a clear feeling about the process. The intensity of this calibration is weaker than the active calibration.

The default is passive calibration.

```C
void fuSetExpressionCalibration(int mode);
```
__Parameters:__  

*mode [in]*：Expression calibration mode, 0 is to turn off expression calibration, 1 is active calibration, and 2 is passive calibration.

__Comment:__  

When executing the main processing interface to process still pictures, since it is necessary to repeatedly call the same data, it is necessary to turn off the expression calibration function.

------
##### fuLoadAnimModel 
Load expression animation models and enable expression optimization.

The expression optimization function can make the expression obtained after real-time tracking more natural and vivid, but it will introduce a certain expression delay.

```C
int fuLoadAnimModel(void* dat, int dat_sz);
```
__Parameters:__  

*dat [in]*：A memory pointer to the contents of the animation model file. This file is distributed with the SDK and is named anim_model.bundle.

*dat_sz [in]*：The length of the animation model file, in bytes.



__Return Value:__  

Return Value 1 indicates that the loading is successful and the expression optimization function is enabled. Return Value 0 represents failure.

------
##### fuSetStrictTracking 
Enable more rigorous tracking quality inspection

After this function is enabled, when an important facial features such as occlusion and out-of-frame conditions occur, and the tracking quality is poor, it will be judged to be a tracking failure, which will prevent the system from having abnormal tracking data when the tracking quality is low.

```C
void fuSetStrictTracking(int mode);
```
__Parameters:__  

*mode [in]*：0 is disabled, 1 is enabled, and the default is disabled.

------
##### fuSetFocalLengthScale 
Modify the system focal length (effect is equivalent to focal length, or FOV), affect the perspective effect of 3D tracking, AR effects.

The parameter is a scale factor, and the focal length change brings smaller perspective distortion.

```C
/**
\brief Scale the rendering perspectivity (focal length, or FOV)
	Larger scale means less projection distortion
	This scale should better be tuned offline, and set it only once in runtime
\param scale - default is 1.f, keep perspectivity invariant
	<= 0.f would be treated as illegal input and discard	
*/
void fuSetFocalLengthScale(float scale);
```
__Parameters:__  

*scale [in]*：The scale factor of the focus adjustment, 1.0 is the default value. The recommended range is 0.1 ~ 2.0.



__Comment:__  

A coefficient less than or equal to 0 is an invalid input.

------
#### 2.6 Abandoned
##### fuSetQualityTradeoff  

```C
/**
\brief Set the quality-performance tradeoff. 
\param quality is the new quality value. 
       It's a floating point number between 0 and 1.
       Use 0 for maximum performance and 1 for maximum quality.
       The default quality is 1 (maximum quality).
*/
FUNAMA_API void fuSetQualityTradeoff(float quality);
```

------
##### fuTurnOffCamera  
```C
/**
\brief Turn off the camera
*/
FUNAMA_API void fuTurnOffCamera();
```

------
##### fuRenderItemsMasked  
```C

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
```

------
##### fuGetCameraImageSize  
```C	
/**
\brief Get the camera image size
\param pret points to two integers, which receive the size
*/
FUNAMA_API void fuGetCameraImageSize(int* pret);
```

------
##### fuLoadExtendedARData  
```C	
/**
\brief Load extended AR data, which is required for high quality AR items
\param data - the pointer to the extended AR data
\param sz - the data size, we use plain int to avoid cross-language compilation issues
\return zero for failure, non-zero for success
*/
int fuLoadExtendedARData(void* data,int sz);
```

------
### 3. Input and output format list

##### RGBA Array
The image memory array for RGBA format

#### Data format identifier

FU_FORMAT_RGBA_BUFFER

#### Data content

Contiguous memory space, length ```w*h*4```. The array element is ```int```, which represents the color information in RGBA.

#### Input and output support

 Input/Output 



#### __Comment:__

Due to memory alignment requirements on the platform, the actual width of the image memory space may not equal the semantic width of the image. When the main running interface passes in the image width, the actual width of the memory should be passed in.

------
##### BGRA Array
The image memory array for BGRA format

__Data format identifier

FU_FORMAT_BGRA_BUFFER

#### Data content

Contiguous memory space, length ```w*h*4```. The array element is ```int```, which represents the color information in BGRA.

#### Input and output support

 Input/Output 



#### __Comment:__

Due to memory alignment requirements on the platform, the actual width of the image memory space may not equal the semantic width of the image. When the main running interface passes in the image width, the actual width of the memory should be passed in.

This format is one of the native iOS camera data formats.

------
##### RGBA TEXTURE
OpenGL texture for RGBA format.

#### Data format identifier

FU_FORMAT_RGBA_TEXTURE

#### Data content

A ```GLuint``` indicates an OpenGL texture ID.

#### Input and output support

 Input/Output 

------
##### RGBA OES TEXTURE
OpenGL external OES texture for RGBA format.

#### Data format identifier

FU_FORMAT_RGBA_TEXTURE_EXTERNAL_OES

#### Data content

A ```GLuint``` indicates an OpenGL external OES texture ID.

#### Input and output support

Only input



#### __Comment:__

This format is one of the native Android camera data formats.

------
##### NV21 Array
An image memory array for NV21 format.

#### Data format identifier

FU_FORMAT_NV21_BUFFER

#### Data content

Continuous memory, the previous section is Y data, length is ```w*h```, the last section is UV data, length is ```2*((w+1)>>1)```(resolution Is half of Y but contains two UV channels). Two pieces of data are stored continuously in memory.

#### Input and output support

Input/Output



#### __Comment:__

This format requires UV data interleaved (eg: UVUVUVUV), such as separate storage of UV data (UUUUVVVV), please use the I420 array format.

This format is one of the native Android camera data formats.

------
##### NV12 Array
An image memory array for NV12 format.

#### Data format identifier

FU_FORMAT_NV12_BUFFER

#### Data content

The structure ```TNV12Buffer``` is defined as follows.

```c
typedef struct{
	void* p_Y; 
	void* p_CbCr;
	int stride_Y;
	int stride_CbCr;
}TNV12Buffer;
```
#### __Parameters:__

*p_Y*：Pointer to Y data.

*p_CbCr*：Pointer to UV data.

*stride_Y*：The length in bytes of each line of Y data.

*stride_CbCr*：The byte length of each row of UV data.



#### Input and output support

Input/Output



#### __Comment:__

This format is very similar to the NV21 array format, except that the U and V cross arrangement in the UV data is the opposite. However, this format supports separate storage of Y data and UV data, and no longer requires continuous data as a whole.

This format is one of the native iOS camera data formats.

------
##### I420 Array
The image memory array for I420 format

#### Data format identifier

FU_FORMAT_I420_BUFFER

#### Data content

Continuous memory, the first segment is Y data, the length is ```w*h```, the second segment is U data, and the length is ```((w+1)>>1)```, the third The segment is V data and is of length ```((w+1)>>1)``` (the resolution of the latter two channels is half of Y). The three pieces of data are stored continuously in memory.

#### Input and output support

Input/Output

#### Comment

This format is basically the same as the NV21 array, except that the U and V data are stored continuously.

------
##### iOS Dual Input
Dual input format for iOS native camera data. Dual input refers to GPU data input - OpenGL texture, and CPU memory data input - BGRA array or NV12 array respectively.

Compared to a single data input that only provides memory arrays or textures, this input mode can reduce data transfer between CPU-GPU once, and can slightly optimize performance (about 2ms on the iphone platform).

#### Data format identifier

FU_FORMAT_INTERNAL_IOS_DUAL_INPUT

#### Data content

The structure ```TIOSDualInput``` is defined as follows.

```c
typedef struct{
	int format;	
	void* p_BGRA;
	int stride_BGRA;	
	void* p_Y;
	void* p_CbCr;
	int stride_Y;
	int stride_CbCr;	
	int tex_handle;	
}TIOSDualInput;
```

#### Parameters

*format*：Indicates the data format of the memory array. Only BGRA and NV12 are supported. They correspond to the constants ```FU_IDM_FORMAT_BGRA``` and ```FU_IDM_FORMAT_NV12```.

*p_BGRA*：A pointer to memory data when the memory data is in BGRA format.

*stride_BGRA*：The byte width of each line of image data when the memory data is in the BGRA format.

*p_Y*：A pointer to the memory data of the Y channel when the memory data is in the NV12 format.

*p_CbCr*：A pointer to the memory data of the UV channel when the memory data is in the NV12 format.

*stride_Y*：The byte width of the Y channel memory data when the memory data is in the NV12 format.

*stride_CbCr*：The byte width of the data in the UV channel memory when the memory data is in the NV12 format.

*tex_handle*：For GPU data input, the default is OpenGL texture ID in RGBA format.

#### Input and output support

Only Input

#### Comment

Since the performance optimization is not significant on the iOS platform, it is recommended to use this interface only if the GPU data already exists in the environment and you want to reuse it, or if you want to customize the GPU data input.

------
##### Android Dual Input
Dual input format for Android native camera data. Dual input refers to the GPU data input - OpenGL texture in RGBA / NV21 / I420 format, and CPU memory data input - NV21 / RGBA / I420 format image memory array.

This input mode can reduce CPU-GPU data transfer once compared to a single data input that only provides a memory array or texture, and can significantly optimize performance on the Android platform, so it is recommended to use this interface whenever possible.

#### Data format identifier

FU_FORMAT_ANDROID_DUAL_MODE

#### Data content

The structure ```TAndroidDualMode``` is defined as follows.

```c
typedef struct{
	void* p_NV21;
	int tex;
	int flags;
}TAndroidDualMode;
```

#### Parameters

*p_NV21*：Pointer to memory image data.

*tex*：OpenGL texture ID。

*flags*：Extended function identifiers, all supported identifiers and their functions are as follows. Multiple identifiers are connected by the "or" operator.

| Extended function identifiers    | Meaning                                                      |
| -------------------------------- | ------------------------------------------------------------ |
| FU_ADM_FLAG_EXTERNAL_OES_TEXTURE | The incoming texture is OpenGL external OES texture          |
| FU_ADM_FLAG_ENABLE_READBACK      | Write processing results back to the incoming memory image data after starting |
| FU_ADM_FLAG_NV21_TEXTURE         | The incoming texture is NV21 data format                     |
| FU_ADM_FLAG_I420_TEXTURE         | The incoming texture is I420 data format                     |
| FU_ADM_FLAG_I420_BUFFER          | Incoming memory image data is in I420 data format            |
| FU_ADM_FALG_RGBA_BUFFER          | Incoming memory image data is in RGBA data format            |

#### Input and output support

Only Input

------
##### Current FBO
Refers to the currently bound OpenGL FBO when the main processing interface is invoked. The main processing interface can directly render the processing result onto this FBO.

#### Data format identifier

FU_FORMAT_GL_CURRENT_FRAMEBUFFER

#### Data content

NULL, the data pointer passes NULL directly.

#### Input and output support

Only Output

#### Comment

The creation of the FBO needs to be completed before the incoming FBO, including the binding of the color texture, which needs to pass the OpenGL integrity check.

If you have 3D rendering content, you need the FBO to have depth buffering.

------
##### Specify FBO
You can pass an externally prepared OpenGL FBO, not necessarily the currently bound FBO when calling the main processing interface. The main processing interface can directly render the processing result onto this FBO.

#### Data format identifier

FU_FORMAT_GL_SPECIFIED_FRAMEBUFFER

#### Data content

The structure ```TSPECFBO``` is defined as follows.

```c
typedef struct{
	int fbo;
	int tex;
}TSPECFBO;
```

#### Parameters

*fbo*：The specified FBO ID.

*tex*：The color texture ID bound to this FBO.

#### Input and output support

Only Output

#### Comment

The creation of the FBO needs to be completed before the incoming FBO, including the binding of the color texture, which needs to pass the OpenGL integrity check.

If you have 3D rendering content, you need to pass the FBO with depth buffer.

------
##### Avatar Driver Info
Special input data, not image data, but face-driven information, is used to drive the avatar model. The face driver information may be obtained after the main processing interface is executed, or may be externally input, such as an avatar video recording information or a user interaction generated message.

#### Data format identifier

FU_FORMAT_AVATAR_INFO

#### Data content

The structure ```TAvatarInfo``` is defined as follows.

```c
typedef struct{	
	float* p_translation;	
	float* p_rotation;
	float* p_expression;
	float* rotation_mode;
	float* pupil_pos;
	int is_valid;
}TAvatarInfo;
```

#### Parameters

*p_translation*：Pointer to memory data, the data is 3 floats, representing the translation of the human face in the camera space. Where x/y is the image resolution and z is the depth of the human face in the camera space.

*p_rotation*：Pointer to memory data, the data is 4 floats, representing the three-bit rotation of the human head. Rotation is represented by a quaternion and needs to be transformed into Euler angle rotation.

*p_expression*：Pointer to the memory data, the data is 46 floats, representing the facial expression coefficient. The meaning of the expression coefficient please refer to《Expression Guide》。

*rotation_mode*：An int, which ranges from 0 to 3, represents the rotation of the human face with respect to the image data, representing 0, 90, 180, and 270 degrees, respectively.

*pupil_pos*：Pointer to memory data, data is 2 floats, representing the Parameters coordinates of the pupil. The coordinates themselves have no semantics and are generally obtained directly from the tracking results.

*is_valid*：An int indicates whether the avatar information is valid. If the value is 0, the system will not process the corresponding avatar information.

#### Input and output support

Only Input

#### Comment

This input mode can only be used with avatar props. Loading face AR props will cause an exception.

This input mode simplifies the processing of incoming image data and the higher performance  under avatar application scenarios. In addition, the control over avatar is more flexible and allows the user to control avatar freely, such as dragging avatar turns, triggering specific expressions, and so on.

------


### 4. FAQ	 

Any question，please feel free to contact our technical support, thank you !


