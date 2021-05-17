#ifndef CNAMASDKSCENE_H
#define CNAMASDKSCENE_H

#include "CNamaSDK.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
 * global api
 */

/**
 \brief use binary shader program or not.
 \param enable > 0 means use binary shader program, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableBinaryShaderProgram(int enable);

/**
 \brief Specify directory for saving and loading binary shader program.
 \param path is directory path
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetBinaryShaderProgramDirectory(const char* path);

/*
 * create, destroy, bind, and unbind api
 */

/**
 \brief Create a new scene.
 \return an unsigned integer handle representing the scene.
*/
FUNAMA_API unsigned int fuCreateScene();

/**
 \brief Destroy the target scene.
 \param handle_id is the target scene handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuDestroyScene(unsigned int handle_id);

/**
 \brief Set the target scene for render.
 \param handle_id is the target scene handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetCurrentScene(unsigned int handle_id);

/**
 \brief Bind items to the target scene
 \param handle_id is the target scene handle
 \param p_items points to a list of item handles to be bind to the target
 scene
 \param n_items is the number of item handles in p_items
 \return the number of items successfully bound to the target scene
*/
FUNAMA_API int fuBindItemsToScene(unsigned int handle_id, int* p_items,
                                  int n_items);

/**
 \brief Unbind items from the target scene
 \param handle_id is the target scene handle
 \param p_items points to a list of item handles to be unbind from the target
 scene
 \param n_items is the number of item handles in p_items
 \return the number of items successfully unbound from the target scene
*/
FUNAMA_API int fuUnbindItemsFromScene(unsigned int handle_id, int* p_items,
                                      int n_items);

/**
 \brief Create an instance for a target scene.
 \param handle_id is the target scene handle
 \return an unsigned integer handle representing the instance.
*/
FUNAMA_API unsigned int fuCreateInstance(unsigned int handle_id);

/**
 \brief Destroy the target instance.
 \param handle_id is the target instance handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuDestroyInstance(unsigned int handle_id);

/**
 \brief Bind items to the target instance
 \param handle_id is the target instance handle
 \param p_items points to a list of item handles to be bind to the target
 instance
 \param n_items is the number of item handles in p_items
 \return the number of items successfully bound to the instance
*/
FUNAMA_API int fuBindItemsToInstance(unsigned int handle_id, int* p_items,
                                     int n_items);

/**
 \brief Unbind items from the target instance
 \param handle_id is the target instance handle
 \param p_items points to a list of item handles to be unbind from the target
 instance
 \param n_items is the number of item handles in p_items
 \return the number of items successfully unbound from the instance
*/
FUNAMA_API int fuUnbindItemsFromInstance(unsigned int handle_id, int* p_items,
                                         int n_items);
/*
 * render related api
 */

/**
 \brief Enable or not render camera for the target scene.
 \param handle_id is the target scene handle
 \param enable > 0 means render, enable <= 0 means not render
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableRenderCamera(unsigned int handle_id, int enable);

/**
 \brief Enable or not use pure background color for the target scene.
 \param handle_id is the target scene handle
 \param enable > 0 means use, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableBackgroundColor(unsigned int handle_id, int enable);

/**
 \brief Set background rgba color for the target scene.
 \param handle_id is the target scene handle
 \param r is color of R channel, from 0 to 255
 \param g is color of G channel, from 0 to 255
 \param b is color of B channel, from 0 to 255
 \param a is color of A channel, from 0 to 255
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetBackgroundColor(unsigned int handle_id, int r, int g, int b,
                                    int a);

/**
 \brief Enable or not main light shadow for the target scene.
 \param handle_id is the target scene handle
 \param enable > 0 means use, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableShadow(unsigned int handle_id, int enable);

/**
 \brief Set instance main light shadow PCF level
 \param handle_id is the target instance handle
 \param level = 0, 1, 2
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceShadowPCFLevel(unsigned int handle_id, int level);

/**
 \brief Set instance main light shadow map sample offset scale.
 \param handle_id is the target instance handle
 \param offset_scale is sample offset scale
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceShadowSampleOffset(unsigned int handle_id,
                                               int offset_scale);

/**
 \brief Enable or not bloom for the target scene.
 \param handle_id is the target scene handle
 \param enable > 0 means use, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableBloom(unsigned int handle_id, int enable);

/**
 \brief Enable use or not low quality lighting for the target scene.
 \param handle_id is the target scene handle
 \param enable > 0 means use, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableLowQualityLighting(unsigned int handle_id, int enable);

/*
 * ai related api
 */

/**
 \brief Open or close face processor for the target scene .
 \param handle_id is the target scene handle
 \param enable > 0 means open, enable <= 0 means close
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableFaceProcessor(unsigned int handle_id, int enable);

/**
 \brief Turn on or off ar mode for the target scene.
 \param handle_id is the target scene handle
 \param enable > 0 means turn on , enable <= 0 means turn off
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableARMode(unsigned int handle_id, int enable);

/**
 \brief Set instance input camera buffer matrix, for transforming fuai result
 \param handle_id is the target instance handle
 \param mat  the transform matrix use to transform the input
 cpu buffer
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceInputCameraBufferMatrix(unsigned int handle_id,
                                                    TRANSFORM_MATRIX mat);

/**
 \brief Set instance face id of face processor.
 \param handle_id is the target instance handle
 \param value is face id
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceFaceProcessorFaceId(unsigned int handle_id,
                                                int value);

/**
 \brief Set instance filter size of face processor.
 \param handle_id is the target instance handle
 \param filter_size_rotaion is filter size of head rotation
 \param filter_size_translation is filter size of head translation
 \param filter_size_eye_rotaion is filter size of eye rotation
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceFaceProcessorFilterSize(
    unsigned int handle_id, int filter_size_rotaion,
    int filter_size_translation, int filter_size_eye_rotation);

/**
 \brief Reset instance filter of face processor.
 \param handle_id is the target instance handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuResetInstanceFaceProcessorFilter(unsigned int handle_id);

/**
 \brief Set instance head rotation deltaX of face processor.
 \param handle_id is the target instance handle
 \param value is x delta rotation
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceHeadRotationDeltaX(unsigned int handle_id,
                                               float value);

/**
 \brief Set instance eye rotation deltaX of face processor.
 \param handle_id is the target instance handle
 \param value is x delta rotation
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceEyeRotationDeltaX(unsigned int handle_id,
                                              float value);

/**
 \brief Enable instance rotate head or not.
 \param handle_id is the target instance handle
 \param enable > 0 means enalbe blend , enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceFaceProcessorRotateHead(unsigned int handle_id,
                                                       int enable);

/**
 \brief Open or close human processor for the target scene.
 \param handle_id is the target scene handle
 \param enable > 0 means open, enable <= 0 means close
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableHumanProcessor(unsigned int handle_id, int enable);

/**
 \brief set human processor 3D scene.
 \param handle_id is the target scene handle
 \param scene = 0 means dance, enable > 0 means selfie
 \return zero for failed, one for success
*/
FUNAMA_API int fuHumanProcessorSet3DScene(unsigned int handle_id, int scene);

/**
 \brief Enable use or not follow mode for the target scene.
 \param handle_id is the target scene handle
 \param enable > 0 means use, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableHumanFollowMode(unsigned int handle_id, int enable);

/**
 \brief Open or close hand detetor for the target scene.
 \param handle_id is the target scene handle
 \param enable > 0 means open, enable <= 0 means close
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableHandDetetor(unsigned int handle_id, int enable);

/*
 * camera animation
 */

/**
 \brief Enable use camera aniamtion for the target scene or not.
 \param handle_id is the target scene handle
 \param enable > 0 means use, enable <= 0 means not use
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableCameraAnimation(unsigned int handle_id, int enable);

/**
 \brief Play target camera animation for the target scene.
 \param handle_id is the target scene handle
 \param anim_handle is target animation item handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuPlayCameraAnimation(unsigned int handle_id, int anim_handle);

/**
 \brief Play target camera animation once for the target scene.
 \param handle_id is the target scene handle
 \param anim_handle is target animation item handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuPlayCameraAnimationOnce(unsigned int handle_id,
                                         int anim_handle);

/**
 \brief Start or continue current camera animation for the target scene.
 \param handle_id is the target scene handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuStartCameraAnimation(unsigned int handle_id);

/**
 \brief Pause current camera animation for the target scene.
 \param handle_id is the target scene handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuPauseCameraAnimation(unsigned int handle_id);

/**
 \brief Restart current camera animation for the target scene.
 \param handle_id is the target scene handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuResetCameraAnimation(unsigned int handle_id);

/**
 \brief Set camera animation transition time for the target scene.
 \param handle_id is the target scene handle
 \param time is transition time, second
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetCameraAnimationTransitionTime(unsigned int handle_id,
                                                  float time);

/**
 \brief Enable camera aniamtion internal lerp for the target scene or not.
 \param handle_id is the target scene handle
 \param enable > 0 means enable, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableCameraAnimationInternalLerp(unsigned int handle_id,
                                                   int enable);

/*
 * time updating releated api
 */

/**
 \brief Switch to system background, pause time update for the target scene
 \param handle_id is the target scene handle
 \param enable > 0 means switch to system background, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuPauseTimeUpdate(unsigned int handle_id, int enable);

/**
 \brief Enable control system time updating or not for the target scene
 \param handle_id is the target scene handle
 \param enable > 0 means control time byself, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableControlTimeUpdate(unsigned int handle_id, int enable);

/**
 \brief Set current time to control system time updating for the target scene
 \param handle_id is the target scene handle
 \param value is current in seconds
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetCurrentTime(unsigned int handle_id, float value);

/*
 * instance related api
 */

/**
 \brief Enable instance visible or not.
 \param handle_id is the target instance handle
 \param visible > 0 means visible, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceVisible(unsigned int handle_id, int visible);

/**
 \brief Enable instance single mesh visible or not.
 \param handle_id is the target instance handle
 \param mesh_handle is target mesh item handle
 \param visible > 0 means visible, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceSingleMeshVisible(unsigned int handle_id,
                                                 int mesh_handle, int visible);

/**
 \brief Enable instance use face beauty custom order or not.
 \param handle_id is the target instance handle
 \param enable > 0 means use custom order, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceUseFaceBeautyOrder(unsigned int handle_id,
                                                  int enable);

/**
 \brief Set instance face beauty custom order.
 \param handle_id is the target instance handle
 \param value points to an array of ints
 \param n specifies the number of elements in value
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceFaceBeautyOrder(unsigned int handle_id, int* value,
                                            int n);

/**
 \brief Set instance face body visible list.
 \param handle_id is the target instance handle
 \param value points to an array of ints
 \param n specifies the number of elements in value
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceBodyVisibleList(unsigned int handle_id, int* value,
                                            int n);

/**
 \brief Enable instance hide neck or not.
 \param handle_id is the target instance handle
 \param enable > 0 means hide neck, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceHideNeck(unsigned int handle_id, int enable);

/**
 \brief Reset instance head to default.
 \param handle_id is the target instance handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuResetInstanceHead(unsigned int handle_id);

/**
 \brief Turn on or off instance facepup mode.
 \param handle_id is the target instance handle
 \param enable > 0 means turn on , enable <= 0 means turn off
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceFacepupMode(unsigned int handle_id, int enable);

/**
 \brief Set instance facepup value.
 \param handle_id is the target instance handle
 \param name is the parameter name
 \param value is the parameter value to be set
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceFacepup(unsigned int handle_id, const char* name,
                                    float value);

/**
 \brief Set instance skeleton deformation.
 \param handle_id is the target instance handle
 \param name is the parameter name
 \param value is the parameter value to be set
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceDeformation(unsigned int handle_id,
                                        const char* name, float value);

/**
 \brief Enable instance expression blend or not.
 \param handle_id is the target instance handle
 \param enable > 0 means enalbe blend , enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceExpressionBlend(unsigned int handle_id,
                                               int enable);

/**
 \brief Set instance blend expression array.
 \param handle_id is the target instance handle
 \param value points to an array of floats
 \param n specifies the number of elements in value
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceBlendExpression(unsigned int handle_id,
                                            float* value, int n);

/**
 \brief Set instance blend expression weight.
 \param handle_id is the target instance handle
 \param value points to an array of floats
 \param n specifies the number of elements in value
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceExpressionWeight0(unsigned int handle_id,
                                              float* value, int n);

/**
 \brief Set instance default expression(face processor or animation) weight.
 \param handle_id is the target instance handle
 \param value points to an array of floats
 \param n specifies the number of elements in value
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceExpressionWeight1(unsigned int handle_id,
                                              float* value, int n);

/**
 \brief Enable instance focus eye to camera or not.
 \param handle_id is the target scene handle
 \param enable > 0 means focus eye to camera, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceFocusEyeToCamera(unsigned int handle_id,
                                                int enable);

/**
 \brief Set instance focus eye to camera params
 \param handle_id is the target scene handle
 \param height_adjust is height adjust delta
 \param distance_adjust is distance adjust delta
 \param weight is weight
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceFocusEyeToCameraParams(unsigned int handle_id,
                                                   float height_adjust,
                                                   float distance_adjust,
                                                   float weight);

/**
 \brief set instance rotation delta.
 \param handle_id is the target instance handle
 \param value is rotation delta
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceRotDelta(unsigned int handle_id, float value);

/**
 \brief Set instance scale delta.
 \param handle_id is the target instance handle
 \param value is scale delta
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceScaleDelta(unsigned int handle_id, float value);

/**
 \brief Set instance translate delta.
 \param handle_id is the target instance handle
 \param value is translate delta
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceTranslateDelta(unsigned int handle_id, float value);

/**
 \brief Set instance target rotation angle.
 \param handle_id is the target instance handle
 \param value is target rotation angle
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceTargetAngle(unsigned int handle_id, float value);

/**
 \brief Set instance target xyz position.
 \param handle_id is the target instance handle
 \param x is position of x world coordinate
 \param y is position of y world coordinate
 \param z is position of z world coordinate
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceTargetPosition(unsigned int handle_id, float x,
                                           float y, float z);

/**
 \brief Set instance skin color.
 \param handle_id is the target instance handle
 \param r is color of R channel, from 0 to 255
 \param g is color of G channel, from 0 to 255
 \param b is color of B channel, from 0 to 255
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceSkinColor(unsigned int handle_id, int r, int g,
                                      int b);

/**
 \brief Set instance iris color.
 \param handle_id is the target instance handle
 \param r is color of R channel, from 0 to 255
 \param g is color of G channel, from 0 to 255
 \param b is color of B channel, from 0 to 255
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceIrisColor(unsigned int handle_id, int r, int g,
                                      int b);

/**
 \brief Set instance iris color intensity, for old color change type.
 \param handle_id is the target instance handle
 \param intensity, intensity
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceIrisColorIntensity(unsigned int handle_id,
                                               float intensity);

/**
 \brief Set instance eyebrow color.
 \param handle_id is the target instance handle
 \param r, is color of R channel, from 0 to 255
 \param g, is color of G channel, from 0 to 255
 \param b, is color of B channel, from 0 to 255
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceEyebrowColor(unsigned int handle_id, int r, int g,
                                         int b);

/**
 \brief Set instance eyebrow color intensity, for old color change type.
 \param handle_id is the target instance handle
 \param intensity, intensity
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceEyebrowColorIntensity(unsigned int handle_id,
                                                  float intensity);

/**
 \brief Set instance hair color.
 \param handle_id is the target instance handle
 \param r is color of R channel, from 0 to 255
 \param g is color of G channel, from 0 to 255
 \param b is color of B channel, from 0 to 255
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceHairColor(unsigned int handle_id, int r, int g,
                                      int b);

/**
 \brief Set instance hair color intensity, for old color change type.
 \param handle_id is the target instance handle
 \param intensity, intensity
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceHairColorIntensity(unsigned int handle_id,
                                               float intensity);

/**
 \brief Set instance beard color.
 \param handle_id is the target instance handle
 \param r is color of R channel, from 0 to 255
 \param g is color of G channel, from 0 to 255
 \param b is color of B channel, from 0 to 255
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceBeardColor(unsigned int handle_id, int r, int g,
                                       int b);

/**
 \brief Set instance beard color intensity, for old color change type.
 \param handle_id is the target instance handle
 \param intensity, intensity
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceBeardColorIntensity(unsigned int handle_id,
                                                float intensity);

/**
 \brief Set instance hat color.
 \param handle_id is the target instance handle
 \param r is color of R channel, from 0 to 255
 \param g is color of G channel, from 0 to 255
 \param b is color of B channel, from 0 to 255
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceHatColor(unsigned int handle_id, int r, int g,
                                     int b);

/**
 \brief Set instance glass color.
 \param handle_id is the target instance handle
 \param r is color of R channel, from 0 to 255
 \param g is color of G channel, from 0 to 255
 \param b is color of B channel, from 0 to 255
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceGlassColor(unsigned int handle_id, int r, int g,
                                       int b);

/**
 \brief Set instance glass frame color.
 \param handle_id is the target instance handle
 \param r is color of R channel, from 0 to 255
 \param g is color of G channel, from 0 to 255
 \param b is color of B channel, from 0 to 255
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceGlassFrameColor(unsigned int handle_id, int r,
                                            int g, int b);

/**
 \brief Set instance single face beauty color.
 \param handle_id is the target instance handle
 \param face_beauty_handle is the target face beauty item handle
 \param r is color of R channel, from 0 to 255
 \param g is color of G channel, from 0 to 255
 \param b is color of B channel, from 0 to 255
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceFacebeautyColor(unsigned int handle_id,
                                            int face_beauty_handle, int r,
                                            int g, int b);
/*
 * animation releated api
 */
/**
 \brief Play instance target animation.
 \param handle_id is the target instance handle
 \param anim_handle is target animation item handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuPlayInstanceAnimation(unsigned int handle_id, int anim_handle);

/**
 \brief Play instance target animation once.
 \param handle_id is the target instance handle
 \param anim_handle is target animation item handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuPlayInstanceAnimationOnce(unsigned int handle_id,
                                           int anim_handle);

/**
 \brief Start or continue to play instance current animation again.
 \param handle_id is the target instance handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuStartInstanceAnimation(unsigned int handle_id);

/**
 \brief Pause instance current animation.
 \param handle_id is the target instance handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuPauseInstanceAnimation(unsigned int handle_id);

/**
 \brief Stop instance current animation.
 \param handle_id is the target instance handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuStopInstanceAnimation(unsigned int handle_id);

/**
 \brief Restart instance current animation.
 \param handle_id is the target instance handle
 \return zero for failed, one for success
*/
FUNAMA_API int fuResetInstanceAnimation(unsigned int handle_id);

/**
 \brief Set instance animation transition time.
 \param handle_id is the target instance handle
 \param time is transition time in seconds
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetInstanceAnimationTransitionTime(unsigned int handle_id,
                                                    float time);

/**
 \brief Turn on or off instance animation internal lerp.
 \param handle_id is the target instance handle
 \param enable > 0 means turn on , enable <= 0 means turn off
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceAnimationInternalLerp(unsigned int handle_id,
                                                     int enable);

/*
 * DynamicBone releated api
 */

/**
 \brief Enable instance DynamicBone or not.
 \param handle_id is the target instance handle
 \param enable > 0 means turn on , enable <= 0 means turn off
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceDynamicBone(unsigned int handle_id, int enable);

/**
 \brief Reset instance DynamicBone rigid body position.
 \param handle_id is the target instance handle
 \param immediate > 0 means reset DynamicBone immediate, <= 0 means delayed
 \return zero for failed, one for success
*/
FUNAMA_API int fuResetInstanceDynamicBone(unsigned int handle_id,
                                          int immediate);

/**
 \brief Refresh/rebuild instance DynamicBone, time consuming
 \param handle_id is the target instance handle
 \param immediate > 0 means reset DynamicBone immediate, <= 0 means delayed
 \return zero for failed, one for success
*/
FUNAMA_API int fuRefreshInstanceDynamicBone(unsigned int handle_id,
                                            int immediate);
/**
 \brief Enable instance model mat to bone or not
 \param handle_id is the target instance handle
 \param enable > 0 means enable , enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceModelMatToBone(unsigned int handle_id,
                                              int enable);

/**
 \brief Open instance single mesh DynamicBone or close
 \param handle_id is the target instance handle
 \param mesh_handle is target mesh item handle
 \param enable > 0 means open, enable <= 0 means close
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceSingleDynamicBone(unsigned int handle_id,
                                                 int mesh_handle, int enable);

/**
 \brief Enable instance DynamicBone teleport mode or not.
 \param handle_id is the target instance handle
 \param enable > 0 means turn on , enable <= 0 means turn off
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceDynamicBoneTeleportMode(unsigned int handle_id,
                                                       int enable);

/**
 \brief Enable instance DynamicBone root translation speed limit mode or not.
 \param handle_id is the target instance handle
 \param enable > 0 means turn on , enable <= 0 means turn off
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceDynamicBoneRootTranslationSpeedLimitMode(
    unsigned int handle_id, int enable);

/**
 \brief Enable instance DynamicBone root rotation speed limit mode or not.
 \param handle_id is the target instance handle
 \param enable > 0 means turn on , enable <= 0 means turn off
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableInstanceDynamicBoneRootRotationSpeedLimitMode(
    unsigned int handle_id, int enable);

/*
 * MVP related api
 */

/**
 \brief Use orthogonal projection or perspective projection for the target
 scene.
 \param handle_id is the target scene handle
 \param enable = 1 means use orthogonal projection, enable = 0 means use
 perspective projection
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableOrthogonalProjection(unsigned int handle_id, int enable);

/**
 \brief Set perspective projection matrix's fov for the target scene.
 \param handle_id is the target scene handle
 \param fov is the degree of angle, from 0 to 180
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetProjectionMatrixFov(unsigned int handle_id, float fov);

/**
 \brief Set orthogonal projection matrix's frustum size for the target scene.
 \param handle_id is the target scene handle
 \param size is frustum size in X-coordinate
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetProjectionMatrixOrthoSize(unsigned int handle_id,
                                              float size);

/**
 \brief Set projection matrix's z_near and z_farr for the target scene.
 \param handle_id is the target scene handle
 \param z_near
 \param z_far
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetProjectionMatrixZnearZfar(unsigned int handle_id,
                                              float z_near, float z_far);

/**
 \brief Enable use outer MVP matrix for the target scene.
 \param handle_id is the target scene handle
 \param enable > 0 means use outer MVP matrix, enable <= 0 means not
 \return zero for failed, one for success
*/
FUNAMA_API int fuEnableOuterMVPMatrix(unsigned int handle_id, int enable);

/**
 \brief Set outer projection matrix for the target scene.
 \param handle_id is the target scene handle
 \param mat, 4x4 matrix
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetOuterProjectionMatrix(unsigned int handle_id,
                                          const float* mat);

/**
 \brief Set outer view matrix for the target scene.
 \param handle_id is the target scene handle
 \param mat, 4x4 column major matrix
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetOuterViewMatrix(unsigned int handle_id, const float* mat);

/**
 \brief Set background params for the target scene.
 \param handle_id is the target scene handle
 \param bg_handle is the target background item handle
 \param x_size is normalized width of background, from 0.0 to 1.0
 \param y_size is normalized height of background, from 0.0 to 1.0
 \param x_offset is normalized offset , form -1.0 to 1.0
 \param x_offset is normalized offset , form -1.0 to 1.0
 \param is_foreground, is foreground or background
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetBackgroundParams(unsigned int handle_id, int bg_handle,
                                     float x_size, float y_size, float x_offset,
                                     float y_offset, int is_foreground);

/**
 \brief Set outer model matrix for the target scene.
 \param handle_id is the target scene handle
 \param mat, 4x4 column major matrix
 \return zero for failed, one for success
*/
FUNAMA_API int fuSetOuterModelMatrix(unsigned int handle_id, const float* mat);

/*
 * get api
 */

/**
 \brief Get camera animation frame number for the target scene.
 \param handle_id is the target scene handle
 \param anim_handle is the target camera animation item handle
 \return frame number
*/
FUNAMA_API int fuGetCameraAnimationFrameNumber(unsigned int handle_id,
                                               int anim_handle);

/**
 \brief Get camera animation progress for the target scene.
 \param handle_id is the target scene handle
 \param anim_handle is the target camera animation item handle
 \return progress
*/
FUNAMA_API float fuGetCameraAnimationProgress(unsigned int handle_id,
                                              int anim_handle);

/**
 \brief Get camera animation transition progress for the target scene.
 \param handle_id is the target scene handle
 \return transition progress
*/
FUNAMA_API float fuGetCameraAnimationTransitionProgress(unsigned int handle_id);

/**
 \brief Get instance facepup original value.
 \param handle_id is the target instance handle
 \param name is the parameter name
 \return facepup original value
*/
FUNAMA_API float fuGetInstanceFacepupOriginalValue(unsigned int handle_id,
                                                   const char* name);

/**
 \brief Get instance facepup array.
 \param handle_id is the target instance handle
 \param ret allocated memory space as container
 \param size is number of float allocated in ret
 \return 1 means successful fetch, container filled with info 0 means failure
*/
FUNAMA_API int fuGetInstanceFacepupArray(unsigned int handle_id, float* ret,
                                         int size);

/**
 \brief Get instance skin color index.
 \param handle_id is the target instance handle
 \return skin color index
 */
FUNAMA_API int fuGetInstanceSkinColorIndex(unsigned int handle_id);

/**
 \brief Get instance position, three dimensional
 \param handle_id is the target instance handle
 \param ret allocated memory space as container
 \param size is number of float allocated in ret
 \return 1 means successful fetch, container filled with info 0 means failure
 */
FUNAMA_API int fuGetInstancePosition(unsigned int handle_id, float* ret,
                                     int size);

/**
 \brief Get instance head center screen coordinate, two dimensional
 \param handle_id is the target instance handle
 \param ret allocated memory space as container
 \param size is number of float allocated in ret
 \return 1 means successful fetch, container filled with info 0 means failure
 */
FUNAMA_API int fuGetInstanceHeadCenterScreenCoordinate(unsigned int handle_id,
                                                       float* ret, int size);

/**
 \brief Get instance bone screen coordinate, two dimensional
 \param handle_id is the target instance handle
 \param name is the target bone name
 \param ret allocated memory space as container
 \param size is number of float allocated in ret
 \return 1 means successful fetch, container filled with info 0 means failure
 */
FUNAMA_API int fuGetInstanceBoneScreenCoordinate(unsigned int handle_id,
                                                 const char* name, float* ret,
                                                 int size);

/**
 \brief Get instance Face vertex screen coordinate, three dimensional
 \param handle_id is the target instance handle
 \param index is the target instance handle
 \param ret allocated memory space as container
 \param size is number of float allocated in ret
 \return 1 means successful fetch, container filled with info 0 means failure
 */
FUNAMA_API int fuGetInstanceFaceVertexScreenCoordinate(unsigned int handle_id,
                                                       int index, float* ret,
                                                       int size);

/**
 \brief Get instance target animation frame number.
 \param handle_id is the target instance handle
 \param anim_handle is target animation item handle
 \return animation frame number
 */
FUNAMA_API int fuGetInstanceAnimationFrameNumber(unsigned int handle_id,
                                                 int anim_handle);

/**
 \brief Get instance target animation progress
 \param handle_id is the target instance handle
 \param anim_handle is target animation item handle
 \return animation progress
 */
FUNAMA_API float fuGetInstanceAnimationProgress(unsigned int handle_id,
                                                int anim_handle);

/**
 \brief Get instance target animation transition progress
 \param handle_id is the target instance handle
 \param anim_handle is target animation item handle
 \return animation transition progress
 */
FUNAMA_API float fuGetInstanceAnimationTransitionProgress(
    unsigned int handle_id, int anim_handle);

#ifdef __cplusplus
}
#endif

#endif  // !CNAMASDK_H
