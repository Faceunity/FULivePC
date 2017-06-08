#pragma once

//todo: provide code references for stack pointers to executable segments
//todo: make the trace less aggressive

#include <windows.h>
#include <stdio.h>

#include <time.h>
#include <dbgHelp.h>



typedef DWORD(WINAPI *LPGetMappedFileNameA) (__in HANDLE hProcess, __in LPVOID lpv, __out_ecount(nSize) LPSTR lpFilename, __in DWORD nSize);
typedef DWORD(WINAPI *LPGetModuleFileNameExA) (__in HANDLE hProcess, __in_opt HMODULE hModule, __out_ecount(nSize) LPSTR lpFilename, __in DWORD nSize);

// dbgHelp.dll
typedef BOOL(__stdcall *LPSymInitialize) (__in HANDLE hProcess, __in_opt PCSTR UserSearchPath, __in BOOL fInvadeProcess);
typedef BOOL(__stdcall *LPSymCleanup)(__in HANDLE hProcess);
typedef BOOL(__stdcall *LPStackWalk64)(__in DWORD MachineType, __in HANDLE hProcess, __in HANDLE hThread, __inout LPSTACKFRAME64 StackFrame, __inout PVOID ContextRecord, __in_opt PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine, __in_opt PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine, __in_opt PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine, __in_opt PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress);
typedef BOOL(__stdcall *LPSymFromAddr)(__in HANDLE hProcess, __in DWORD64 Address, __out_opt PDWORD64 Displacement, __inout PSYMBOL_INFO Symbol);
typedef BOOL(__stdcall *LPSymGetLineFromAddr)(__in HANDLE hProcess, __in DWORD dwAddr, __out PDWORD pdwDisplacement, __out PIMAGEHLP_LINE Line);
typedef PVOID(__stdcall *LPSymFunctionTableAccess64)(__in HANDLE hProcess, __in DWORD64 AddrBase);
typedef DWORD64(__stdcall *LPSymGetModuleBase64)(__in HANDLE hProcess, __in DWORD64 qwAddr);
typedef DWORD64(__stdcall *LPSymSetContext)(HANDLE, LPSTACKFRAME64, PVOID);
typedef BOOL(__stdcall *LPSymEnumSymbols)(HANDLE hProcess, ULONG64 BaseOfDll, PCSTR Mask, PSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback, PVOID UserContext);

LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo);
