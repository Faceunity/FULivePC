#include "GlobalValue.h"

#ifdef _DEBUG
#ifdef _WIN64
const std::string g_fuDataDir = "../../../assets/";
#else
const std::string g_fuDataDir = "../../assets/";
#endif
#else
#ifdef _WIN64
const std::string g_fuDataDir = "../../../assets/";
#else
const std::string g_fuDataDir = "../../assets/";
#endif
#endif // _DEBUG


const std::string g_v3Data = "v3.bundle";