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

const string g_ai_bgseg_green = "model/ai_bgseg_green.bundle";

const string g_ai_bgseg = "model/ai_bgseg.bundle";

const string g_ai_hairseg = "model/ai_hairseg.bundle";

const string g_ai_humanprocessor = "model/ai_human_processor_pc_gpu.bundle";

//const string g_ai_gesture = "model/ai_gesture.bundle";

const string g_ai_hand_processor = "model/ai_hand_processor_pc.bundle";

const string g_fuDataDir = "assets";

const string g_control = "graphics/controller_cpp.bundle";

const string g_control_cfg = "graphics/controller_config.bundle";

#ifdef __APPLE__
const string g_assetDir = "";
#else
const string g_assetDir = "../assets/";
#endif
const string g_pose_track_folder = "PoseTrack/";

const string g_greenscreen = "others/green_screen.bundle";

const string g_bgsegment = "others/bg_segment.bundle";

struct ItemFlag {
    bool mSINGLE_PEOPLE = false;
    bool mMAKEUP_FLIP = false;
    bool mIS_NEED_CLICK = false;
    bool mIS_NEED_3DFLIP = false;
};

enum class ItemCategory {
    sticker,
    avatar,
    Animoji
};

enum class BodyTrackType
{
    None = -1,
    HalfBody = 0,
    FullBody = 1,
};

struct BodyTrackParam {
    bool EnableFaceProcessor = false;
    bool UseRetargetRootScale = true;
    float UseRetargetRootScaleValue = 0;
    int AnimFilterParams_n_buffer_frames = 5;
    float AnimFilterParams_pos_w = 0;
    float AnimFilterParams_angle_w = 0;
    float GlobalOffset_x = 0;
    float GlobalOffset_y = 0;
    float GlobalOffset_z = 0;
    float Scale = 1.0f;
    float Pos_x = 0;
    float Pos_y = 50;
    float Pos_z = -900;
    float TrackMoveRange_x = 0.0f;
    float TrackMoveRange_y = 0.0f;
    float TrackMoveRange_z = 0.0f;
    float JointValidThreshold = 0.0f;
    float JointTrackingValidThresholdScale = 0.0f;
};

struct BodyTrackConfig {
    BodyTrackType DefaultType = BodyTrackType::FullBody;
    BodyTrackParam FakeManFull;
    BodyTrackParam FakeManHalf;
    BodyTrackParam BearFull;
    BodyTrackParam BearHalf;
};


#endif // !CONFIG_H
