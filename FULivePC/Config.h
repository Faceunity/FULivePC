//#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#define  MAX_NAMA_BUNDLE_NUM 1
extern const std::string g_fuDataDir;
extern const std::string g_v3Data;

const std::string g_faceBeautification = "face_beautification.bundle";

const std::string g_Makeup = "face_makeup.bundle";

const std::string g_NewFaceTracker = "new_face_tracker.bundle";

const std::string g_gestureRecongnition = "heart.bundle";

const std::string g_anim_model = "anim_model.bundle";

const std::string g_ardata_ex = "ardata_ex.bundle";

const std::string g_fxaa = "fxaa.bundle";

const std::string g_tongue = "tongue.bundle";

const std::string g_ai_faceprocessor = "AI_model/ai_face_processor.bundle";

const std::string g_ai_landmark239 = "AI_model/ai_facelandmarks239.bundle";

const std::string g_fuDataDir = "../../assets/";

const std::string g_v3Data = "v3.bundle";

std::string faceBeautyParamName[] = { "blur_level","color_level", "red_level", "eye_bright", "tooth_whiten" };
#endif // !CONFIG_H