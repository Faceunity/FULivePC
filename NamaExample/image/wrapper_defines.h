#ifndef __WRAPPER_DEFINES_H
#define __WRAPPER_DEFINES_H
#ifdef LINUX
#undef _WIN32
#endif

#ifdef _WIN32
	#include <windows.h>
	typedef INT_PTR iptr;
#else
	#include <stdint.h>
	typedef intptr_t iptr;
#endif

#ifndef _WIN32
	#define __declspec(x)
	#define __stdcall
	#define __cdecl
#endif
//#define __cplusplus
#ifdef __cplusplus
	#if defined(PM_RELEASE)||!defined(_WIN32)
		#define EXPORT extern "C"
	#else
		#define EXPORT extern "C"
	#endif
#else
	#if defined(PM_RELEASE)||!defined(_WIN32)
		#define EXPORT
	#else
		#define EXPORT
	#endif
#endif

/*
#ifdef _M_AMD64
typedef long long iptr;
#else
typedef long iptr;
#endif
*/
#endif
