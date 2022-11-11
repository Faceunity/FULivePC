# FUAI C API Reference


------
### Updates：


------
### Content：

[TOC]

------
### 1. Introduction 

This paper is the interface document of FaceUnity's basic layer of the algorithm libraries. FI_EXPORT related can be used directly for the underlying development on IOS / Android NDK / Windows / Linux / Mac.

------
### 2. Structure and constant definition
##### FI_Status enumeration
This enumeration is the returned state of all SDK functions

```C
typedef enum FI_Status {
  FI_STATUS_OK = 0,					// Indicate the interface returned normally
  FI_STATUS_INVALID_ARGUMENT = 1,	// Indicate the incorrect input of the interface parameters
  FI_STATUS_AUTH_FAILED = 2,		// Indicate authentication failure, please confirm FI_Setup is called normally, otherwise the validity of the authentication certificate needs to be checked
  FI_STATUS_UNKNOWN_ERROR = -1		// Undefined error
} FI_Status;
```

##### FI_ImageFormat enumeration
This enumeration is all image types supported by SDK.

```C
typedef enum FI_ImageFormat {
  FI_IMAGE_UNKNOWN = 0,		// Unknown format, it cannot be passed in as image format
  FI_IMAGE_RGB = 1,			// RGB format，3 bytes per pixel[r,g,b]
  FI_IMAGE_BGR = 2,			// BGR format，3 bytes per pixel[b,g,r]
  FI_IMAGE_RGBA = 3,		// RGBA format，4 bytes per pixel[r,g,b,a]
  FI_IMAGE_BGRA = 4,		// BGRA format，4 bytes per pixel[b,g,r,a]
  FI_IMAGE_ARGB = 5,		// ARGB format，4 bytes per pixel[a,r,g,b]
  FI_IMAGE_ABGR = 6,		// ABGR format，4 bytes per pixel[a,b,g,r]
  FI_IMAGE_GRAY = 7,		// Grayscale format，1 bytes per pixel[gray]
  FI_IMAGE_YUV_NV12 = 8,	// NV12 format. First, the Y-channel values of all the pixels are sorted in order, and then sorted in 2-fold down-sampling [U, V] channel order interval
  FI_IMAGE_YUV_NV21 = 9,	// NV12 format. First, the Y-channel values of all the pixels are sorted in order, and then sorted in 2-fold down-sampling [U, V] channel order interval
  FI_IMAGE_YUV_I420 = 10,	// I420 format. First, the Y channels of all the pixels are arranged in sequence, then the U channels are arranged by 2 times down sampling, and finally the V channels are arranged by 2 times down sampling.
} FI_ImageFormat;
```

##### FI_ImageRotation enumeration
This enumeration is all image orientation information supported by the SDK.To improve the accuracy and speed of the AI algorithm, the SDK recognizes only one oriented image.On mobile platforms, the camera can normally rotate in four directions (where hardware rotation information can be imagined through gravity sensing). So, SDK allows you to set the orientation of the current incoming image

```C
typedef enum FI_ImageRotation {
  FI_IMAGE_ROTATION_0 = 0,		// Rotate image 0 degree
  FI_IMAGE_ROTATION_90 = 1,		// Rotate image 90 degrees
  FI_IMAGE_ROTATION_180 = 2,	// Rotate image 180 degrees
  FI_IMAGE_ROTATION_270 = 3,	// Rotate image 270 degrees
} FI_ImageRotation;
```

##### FI_FaceDetectMode enumeration
This enumeration is two face detection modes supported by SDK: in video mode, in order to optimize power consumption, frame skipping and other strategies will be used to reduce the amount of computation, which can not guarantee that face is detected in each frame;In picture mode, face detection is guaranteed for each frame.

```C
typedef enum FI_FaceDetectMode {
  FI_FACE_DETECT_MODE_IMAGE = 0, // Image mode，face detection processing for each frame
  FI_FACE_DETECT_MODE_VIDEO = 1  // Video mode (default)，Face detection in frames is not guaranteed
} FI_FaceDetectMode;
```

##### FI_ImageView structure
It is used to express the structure of SDK input image information; In order to improve the performance, the structure will not copy additional data, and only read the corresponding pointer for operation; Therefore, during the effective period of the structure, the corresponding memory data cannot be changed.

```C
typedef struct FI_ImageView FI_ImageView;
```

##### FI_FaceProcessor structure

It is used to process the structure of face-related AI algorithm
```C
typedef struct FI_FaceProcessor FI_FaceProcessor;
```

------
### 3. APIs
#### 3.1 Global state function
##### FI_Setup function
Initialize the system environment, load system data, and carry out network authentication. It must be executed before calling other interfaces of SDK, otherwise other interfaces cannot be called successfully.

```C
FI_EXPORT extern FI_Status FI_Setup(const char* authpack, int size);
```

__Parameters：__

*authpack [in]*： Memory pointer, pointing to the content of authentication data. If the authpack. H method is used to provide authentication data at compile time, it can be written as ```g_auth_package``` 。

*size [in]*：The length of authentication data, in bytes. If the authentication date provides ```g_auth_package```in authpack.h，it can be written as ```sizeof(g_auth_package)```

__Return Value：__

FI_Status

__Notes：__

The interface will first detect the authentication validity. If it is invalid, it will return FI_STATUS_AUTH_FAILED directly；Then, the SDK of phase core technology will be connected for network authentication verification, which is asynchronous and will not block the function return; If asynchronous authentication fails, subsequent function calls return FI_STATUS_AUTH_FAILED.

This function needs to be called in pairs with FI_Destroy() function.

------
##### FI_Destroy
End all SDK and log off all resources

```C
FI_EXPORT extern FI_Status FI_Destroy();
```

__Return Value：:__

FI_Status

__Notes:__

This function needs to be called in pairs with FI_Setup() function.

------
##### FI_VLogSetLevel
Set the internal debug log level to investigate possible abnormal behavior

```C
FI_EXPORT extern FI_Status FI_VLogSetLevel(int level);
```

__Parameters:__

*level [in]*： The range is 0 ~ 4, the default is 0; The bigger the log, the more the information.

__Return Value:__

FI_Status

------
##### FI_Version
Get detailed version information of the current SDK

```C
FI_EXPORT extern FI_Status FI_Version(const char** version);
```

__Parameters：__

*version [out]*： Pointer to the build number string object.

__Return Value：__

FI_Status



------
#### 3.2 ImageView

##### FI_NewImageView
Create ImageView object

```C
FI_EXPORT extern FI_Status FI_NewImageView(FI_ImageView** image_view);
```

__Parameters：__

*image_view [out]*： The pointer of ImageView object.

__Return Value：__

FI_Status



------
##### FI_DeleteImageView
Destroy ImageView object

```C
FI_EXPORT extern FI_Status FI_DeleteImageView(FI_ImageView* image_view);
```

__Parameters：__

*image_view [in]*： ImageView object.

__Return Value：__

FI_Status



------
##### FI_ImageViewInit
Initialize the ImageView object.

```C
FI_EXPORT extern FI_Status FI_ImageViewInit(
    FI_ImageView* image_view, FI_ImageFormat format, int width, int height,
    FI_ImageRotation rotation, const uint8_t* data0, int stride0,
    const uint8_t* data1, int stride1, const uint8_t* data2, int stride2);
```

__Parameters：__

*image_view [in,out]*： ImageView object.

*format [in]*： Image format，belonging to FI_ImageFormat.

*width [in]*： The width of the image.

*height [in]*： The height of the image.

*rotation [in]*： Image orientation，belonging to FI_ImageRotation.

*data0 [in]*： Image memory pointer 0.

*stride0 [in]*： The number of bytes per line of the image is 0; Note that the number of bytes of the image may not be equal to its width, there may be additional padding.

*data1 [in]*： Image memory pointer 1.

*stride1 [in]*： The number of bytes per row of the image is 1.

*data2 [in]*： Image memory pointer 2.

*stride2 [in]*： The number of bytes per row of the image is 2.

__Return Value：__

FI_Status

__Notes:__
This function provides data0,stride0,data1,stride1,data2,stride2；it seeks to express all kinds of image formats as much as possible without additional copy of the image.

For RGB / RGBA / gray related image formats, it is usually a continuous memory, so you only need to provide data0 and stride0.

For NV21/NV12 image format, there are usually two continuous memory segments, one for Y channel and one for UV channel; So you need to provide the information of Y channel data0, stride0, and UV channel data1, stride1.

For I420 image format, it is usually three consecutive memory, one Y channel, one U channel and one V channel; Therefore, you need to provide Y channel information Data0, string0, U channel information data1, strid1, and V channel information data2, stringe2.



------
##### FI_ImageViewSetData
Sets the memory data for some channels of the ImageView object.

```C
FI_EXPORT extern FI_Status FI_ImageViewSetData(FI_ImageView* image_view,
                                               int index, const uint8_t* data);
```

__Parameters：__

*image_view [in,out]*： ImageView object

*index [in]*： Channel index, ranging from 0 to 2.

*data [in]*： Channel memory pointer.

__Return Value：__

FI_Status

__Notes:__
This function can be used to update the memory data of some channels without calling the entire Init function again every time


------
#### 3.3 Face AI function

##### FI_NewFaceProcessorFromBundle
Create face AI object

```C
FI_EXPORT extern FI_Status FI_NewFaceProcessorFromBundle(
    const char* data, int size, FI_FaceProcessor** model);
```

__Parameters：__

*data [in]*： Face AI model data.

*size [in]*： The number of bytes of the face AI model data.

*model [out]*： The pointer of face AI object.

__Return Value：__

FI_Status

__Notes:__

This function is used to create face AI objects, which is the precondition of all face AI algorithms. There are two kinds of AI model data of human face.

ai_face_processor.bundle  Support all face AI function

ai_face_processor_lite.bundle A simplified version of AI model (which can effectively reduce the size of the model) only supports some AI algorithms, including face basic feature points, face dense feature points and face DDE algorithm.

------
##### FI_DeleteFaceProcessor
Destroy face AI object

```C
FI_EXPORT extern FI_Status FI_DeleteFaceProcessor(FI_FaceProcessor* model);
```

__Parameters：:__

*model [in]*：Face AI object.

__Return Value：__

FI_Status

__Notes:__

This function is used to destroy face AI objects，which needs to be paired with FI_NewFaceProcessorFromBundle() function.


------
##### FI_FaceProcessorProcess
The processing function of face AI algorithm is the main calculation function of face AI related algorithm; This function can be called in every frame in video stream, and the continuous consistency between frames will be processed automatically.

If you need to deal with multiple discontinuous pictures separately, you need to call FI_FaceProcessorReset() to clear the face information of the previous frame.

```C
FI_EXPORT extern FI_Status FI_FaceProcessorProcess(
    FI_FaceProcessor* model, const FI_ImageView* image_view);
```

__Parameters：__

*model [in,out]*： Face AI object.

*image_view [in]*： Image data object.

__Return Value：__

FI_Status


------
##### FI_FaceProcessorSetDetectMode
Set face detection mode

```C
FI_EXPORT extern FI_Status FI_FaceProcessorSetDetectMode(
    FI_FaceProcessor* model, FI_FaceDetectMode detect_mode);
```

__Parameters：__

*model [in,out]*： Face AI object.

__Return Value：__

FI_Status


------
##### FI_FaceProcessorReset
Reset the face AI algorithm, lose the accumulated face information and start face detection again.

```C
FI_EXPORT extern FI_Status FI_FaceProcessorReset(FI_FaceProcessor* model);
```

__Parameters：__

*model [in,out]*： Face AI object.

__Return Value：__

FI_Status



------
##### FI_FaceProcessorSetMaxFaces
Set the maximum number of faces allowed to be recognized

```C
FI_EXPORT extern FI_Status FI_FaceProcessorSetMaxFaces(
    FI_FaceProcessor* model, int max_faces);
```

__Parameters：__

*model [in,out]*： Face AI object.

*max_faces [in]*： The maximum number of faces, the default value is 1.

__Return Value：__

FI_Status


------
##### FI_FaceProcessorResetModules
Reset each function module of face algorithm, and all face algorithms will stop after calling; then, you can call FI_FaceProcessorSetUse*() correlation function to select the desired face algorithm.


```C
FI_EXPORT extern FI_Status FI_FaceProcessorResetModules(
    FI_FaceProcessor* model);
```

__Parameters：__

*model [in,out]*： Face AI object.

__Return Value：__

FI_Status



------
##### FI_FaceProcessorSetUseFaceLandmark
Set whether to enable face basic feature point algorithm

```C
FI_EXPORT extern FI_Status FI_FaceProcessorSetUseFaceLandmark(
    FI_FaceProcessor* model, int use_face_landmark);
```

__Parameters：__

*model [in,out]*： Face AI object.

*use_face_landmark [in]*： Whether to enable face basic feature point algorithm, the default is 1.

__Return Value：__

FI_Status

__Notes:__

The definition of face basic feature points is as follows:

![landmark](./imgs/landmarks75.jpg)  


------
##### FI_FaceProcessorSetUseDenseLandmark
Set whether face intensive feature point algorithm is enabled

```C
FI_EXPORT extern FI_Status FI_FaceProcessorSetUseDenseLandmark(
    FI_FaceProcessor* model, int use_dense_landmark);
```

__Parameters：__

*model [in,out]*： Face AI object.

*use_dense_landmark [in]*： Whether to enable face dense feature point algorithm, the default is 0.

__Return Value：__

FI_Status


__Notes: __

The definition of face dense feature points is shown in the figure below, including high-precision points of eyebrow, eye, pupil and mouth.

![landmark_dense](./imgs/landmarks_dense.jpg)  


------
##### FI_FaceProcessorSetUseFaceDde
Set whether face DDE algorithm is enabled or not. The algorithm will output 3D rotation and translation of face, as well as some low precision basic expression coefficients

```C
FI_EXPORT extern FI_Status FI_FaceProcessorSetUseFaceDde(
    FI_FaceProcessor* model, int use_face_dde);
```

__Parameters：__

*model [in,out]*： Face AI object.

*use_face_dde [in]*： Whether to enable face DDE algorithm, the default is 1.

__Return Value：__

FI_Status


------
##### FI_FaceProcessorSetUseFaceCapture
Set whether to enable face 3D capture algorithm, which will output face 3D rotation, translation, and high-precision expression coefficient.

```C
FI_EXPORT extern FI_Status FI_FaceProcessorSetUseFaceCapture(
    FI_FaceProcessor* model, int use_face_capture);
```

__Parameters：__

*model [in,out]*： Face AI object.

*use_face_capture [in]*： Whether to enable face 3D capture algorithm, the default is 0.

__Return Value：__

FI_Status


------
##### FI_FaceProcessorSetFaceCaptureUseTongueExpression
Set whether the tongue coefficient algorithm in face 3D capture mode is enabled. After opening the 3D capture algorithm, the algorithm outputs tongue correlation coefficient.

```C
FI_EXPORT extern FI_Status FI_FaceProcessorSetFaceCaptureUseTongueExpression(
    FI_FaceProcessor* model, int face_capture_use_tongue);
```

__Parameters：__

*model [in,out]*： Face AI object.

*face_capture_use_tongue [in]*： Whether to enable tongue coefficient algorithm in face 3D capture mode, the default is 0.

__Return Value：__

FI_Status




------
##### FI_FaceProcessorSetMinFaceRatio
Set the minimum proportion of face screen that the face detection algorithm is allowed to detect, and the time of face detection will increase with the decrease of the proportion of face screen

```C
FI_EXPORT extern FI_Status FI_FaceProcessorSetMinFaceRatio(
    FI_FaceProcessor* model, float min_face_ratio);
```

__Parameters：__

*model [in,out]*： Face AI object.

*min_face_ratio [in]*： The minimum face ratio, the default value is 0.2 (20% screen ratio), the range is [0.01,1]

__Return Value：__

FI_Status




------
##### FI_FaceProcessorGetFov
The FOV (field-of-view) angle of face 3D correlation algorithm is obtained, which is a common parameter of perspective projection camera and is related to focal length; Is a float value.

```C
FI_EXPORT extern FI_Status FI_FaceProcessorGetFov(
    const FI_FaceProcessor* model, float* result);
```

__Parameters：__

*model [in]*： Face AI object.

*result [out]*： FOV angle pointer

__Return Value：__

FI_Status




------
##### FI_FaceProcessorSetFov
Set FOV (field-of-view) angle of face 3D correlation algorithm, which is a common parameter of perspective projection camera, and is related to focal length

```C
FI_EXPORT extern FI_Status FI_FaceProcessorSetFov(FI_FaceProcessor* model,
                                                  float fov_degree);
```

__Parameters：__

*model [in,out]*： Face AI objects.

*fov_degree [in]*： FOV angle, range [0°~90°]， Recommended range [10°~60°]


__Return Value：__

FI_Status




------
##### FI_FaceProcessorGetNumResults
Get the number of faces currently detected, which is an int value

```C
FI_EXPORT extern FI_Status FI_FaceProcessorGetNumResults(
    const FI_FaceProcessor* model, int* result);
```

__Parameters：__

*model [in]*：Face AI objects.

*result [out]*： Face number pointer

__Return Value：__

FI_Status




------
##### FI_FaceProcessorGetResultRect
Get the bounding box information of a person's face, which is four float values

```C
FI_EXPORT extern FI_Status FI_FaceProcessorGetResultRect(
    const FI_FaceProcessor* model, int index, const float** result);
```

__Parameters：__

*model [in]*： Face AI objects.

*index [in]*： Face index, the function will check whether the given index is a valid range

*result [out]*： Face bounding box information pointer; the definition is [xmin, ymin, xmax, ymax]

__Return Value：__

FI_Status




------
##### FI_FaceProcessorGetResultTrackId
Get the Track ID of the currently detected face in video tracking: a new face is detected, and the Track ID of the face remains unchanged until the face disappears. TrackID is an int value.

```C
FI_EXPORT extern FI_Status FI_FaceProcessorGetResultTrackId(
    const FI_FaceProcessor* model, int index, int* result);
```

__Parameters：__

*model [in]*： Face AI objects.

*index [in]*： Face index，the function will check whether the given index is a valid range

*result [out]*： Track ID pointer

__Return Value：__

FI_Status




------
##### FI_FaceProcessorGetResultConfidenceScore
To obtain the confidence of the face of a person currently detected, it is a float value

```C
FI_EXPORT extern FI_Status FI_FaceProcessorGetResultConfidenceScore(
    const FI_FaceProcessor* model, int index, float* result);
```

__Parameters：__

*model [in]*： Face AI objects.

*index [in]*： Face index，the function will check whether the given index is a valid range

*result [out]*： Confidence pointer

__Return Value：__

FI_Status




------
##### FI_FaceProcessorGetResultLandmarks
Get the basic feature point information of a face, which is a float array

```C
FI_EXPORT extern FI_Status FI_FaceProcessorGetResultLandmarks(
    const FI_FaceProcessor* model, int index, const float** result,
    int* result_size);
```

__Parameters：__

*model [in]*： Face AI objects.

*index [in]*： Face index，the function will check whether the given index is a valid range

*result [out]*： Pointer to face feature point array

*result_size [out]*： The pointer of the number of facial feature points is an int, which represents the number of floats of the result

__Return Value：__

FI_Status


------
##### FI_FaceProcessorGetResultAllLandmarks
Get the information of all the feature points of a person's face detected at present + extra feature points (+ dense feature points, and other feature points that may be supplemented later), which is a float array

```C
FI_EXPORT extern FI_Status FI_FaceProcessorGetResultAllLandmarks(
    const FI_FaceProcessor* model, int index, const float** result,
    int* result_size);
```

__Parameters：__

*model [in]*： Face AI objects.

*index [in]*： Face index，the function will check whether the given index is a valid range

*result [out]*： Pointer of face feature point array

*result_size [out]*： The pointer of the number of facial feature points is an int, which represents the number of floats of the result

__Return Value：__

FI_Status



------
##### FI_FaceProcessorGetResultExpression
Get the expression coefficient of a face currently detected, which is a float array;

```C
FI_EXPORT extern FI_Status FI_FaceProcessorGetResultExpression(
    const FI_FaceProcessor* model, int index, const float** result,
    int* result_size);
```

__Parameters：__

*model [in]*： Face AI objects.

*index [in]*： Face index，the function will check whether the given index is a valid range

*result [out]*： Pointer of facial expression coefficient array

*result_size [out]*： The number pointer of facial expression coefficient is an int, which represents the number of floats of the result

__Return Value：__

FI_Status

__Notes:__
The definition of blendshape refers to the figure in [File expressions](./expressions).


------
##### FI_FaceProcessorGetResultTongueExpression
Get the expression coefficient of a person's face and tongue, which is a float array

```C
FI_EXPORT extern FI_Status FI_FaceProcessorGetResultTongueExpression(
    const FI_FaceProcessor* model, int index, const float** result,
    int* result_size);
```

__Parameters：__

*model [in]*： Face AI objects.

*index [in]*： Face index，the function will check whether the given index is a valid range

*result [out]*： Pointer of tongue expression coefficient array

*result_size [out]*： The number pointer of facial expression coefficient is an int, which represents the number of floats of the result

__Return Value：__

FI_Status

__Notes:__
Get the definition of blendshape of tongue expression coefficient in [Tongue Expression Coefficient.pdf](./expressions/tongue_blendshape.pdf)


------
##### FI_FaceProcessorGetResultRotation
Get the 3D rotation information of a face currently detected, which is a float array

```C
FI_EXPORT extern FI_Status FI_FaceProcessorGetResultRotation(
    const FI_FaceProcessor* model, int index, const float** result,
    int* result_size);
```

__Parameters：__

*model [in]*：Face AI objects.

*index [in]*： Face index，the function will check whether the given index is a valid range

*result [out]*： Pointer of face 3D rotation array, a quaternion[x,y,z,w]

*result_size [out]*： The number pointer of face 3D rotation array is an int，which represents the number of floats of the result. The return value should be 4.

__Return Value：:__

FI_Status



------
##### FI_FaceProcessorGetResultTranslation
Get the 3D position information of a face currently detected, which is a float array

```C
FI_EXPORT extern FI_Status FI_FaceProcessorGetResultTranslation(
    const FI_FaceProcessor* model, int index, const float** result,
    int* result_size);
```

__Parameters：__

*model [in]*： Face AI objects.

*index [in]*： Face index，the function will check whether the given index is a valid range

*result [out]*： Face 3D position array pointer，a three dimensional coordinates[x,y,z]

*result_size [out]*： The number pointer of face 3D position is an int，which represents the number of floats of the result. The return value should be 3.

__Return Value：__

FI_Status



------
##### FI_FaceProcessorGetResultEyesRotation
Get the currently detected 3D eye rotation information of a person's face, which is a float array

```C
FI_EXPORT extern FI_Status FI_FaceProcessorGetResultEyesRotation(
    const FI_FaceProcessor* model, int index, const float** result,
    int* result_size);
```

__Parameters：__

*model [in]*： Face AI objects.

*index [in]*： Face index，the function will check whether the given index is a valid range

*result [out]*： Face eyeball 3D rotation array pointer，a quaternion[x,y,z,w]

*result_size [out]*： The number pointer of face 3D rotation is an int，which represents the number of floats of the result. The return value should be 4.

__Return Value：__

FI_Status


------

### 4. FAQ

#### Thread security of algorithm SDK
The algorithm SDK guarantees thread security through multiple instances

1. Each AI model instance needs to be called in the same thread；
2. Multiple AI model instances can be created under multithreading。

#### How to use the algorithm SDK

You can refer to the code example：[fuai_interface_sample](./sample/fuai_interface_sample.cc)