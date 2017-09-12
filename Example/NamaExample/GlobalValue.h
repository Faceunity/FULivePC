#pragma once

#include <string>

extern const std::string g_fuDataDir;
extern const std::string g_v3Data;

const std::string g_propName[] = {	
	"EatRabbi.bundle",
	"gradient.bundle",	
	"bg_seg.bundle",
	"fu_zh_duzui.bundle",
	"yuguan.bundle",		
	"Mood.bundle",
	"yazui.bundle",	
	"lixiaolong.bundle",
	"mask_matianyu.bundle"
};
const int g_propCount = sizeof(g_propName) / sizeof(g_propName[0]);

const std::string g_faceBeautification = "face_beautification.bundle";

const std::string g_gestureRecongnition = "heart.bundle";