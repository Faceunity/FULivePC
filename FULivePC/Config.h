#ifndef _CONFIG_H
#define _CONFIG_H

#include <string>
using namespace std;
#define  MAX_NAMA_BUNDLE_NUM 1
#define  MAX_NAMA_CM_BUNDLE_NUM 35

const string g_faceBeautification = "graphics/face_beautification.bundle";

const string g_Makeup = "graphics/face_makeup.bundle";

const string g_fxaa = "graphics/fxaa.bundle";

const string g_bodySlim = "graphics/body_slim.bundle";

const string g_tongue = "graphics/tongue.bundle";

const string g_ai_faceprocessor = "model/ai_face_processor_pc.bundle";

const string g_ai_landmark239 = "AI_model/ai_facelandmarks239.bundle";

const string g_ai_bgseg_green = "model/ai_bgseg_green.bundle";

const string g_ai_bgseg = "model/ai_bgseg.bundle";

const string g_ai_hairseg = "model/ai_hairseg.bundle";

const string g_ai_humanprocessor = "model/ai_human_processor_pc.bundle";

//const string g_ai_gesture = "model/ai_gesture.bundle";

const string g_ai_hand_processor = "model/ai_hand_processor_pc.bundle";

const string g_fuDataDir = "assets";

const string g_control = "graphics/controller.bundle";

const string g_control_cfg = "graphics/controller_config.bundle";

const string g_fakeman = "others/fakeman.bundle";

const string g_human3d = "model/human3d.bundle";

const string g_fxaa_flipx = "graphics/fxaa_flipX.bundle";
#ifdef __APPLE__
const string g_assetDir = "";
#else
const string g_assetDir = "assets/";
#endif
const string g_pose_track_folder = g_assetDir+"PoseTrack/";

const string g_avatar_def_bg = "others/default_bg.bundle";

const string g_greenscreen = "others/green_screen.bundle";

#endif // !CONFIG_H
