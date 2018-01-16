#pragma once

#include <string>

extern const std::string g_fuDataDir;
extern const std::string g_v3Data;

const std::string g_propName[] = {	
	"EatRabbi.bundle",
	"bg_seg.bundle",
	"fu_zh_duzui.bundle",
	"yazui.bundle",	
	"mask_matianyu.bundle",
	"houzi5.bundle",
	"Mood.bundle",
	"gradient.bundle",
	"yuguan.bundle"
};
const int g_propCount = sizeof(g_propName) / sizeof(g_propName[0]);

const std::string g_faceBeautification = "face_beautification.bundle";

const std::string g_gestureRecongnition = "heart.bundle";

const std::string g_anim_model = "anim_model.bundle";