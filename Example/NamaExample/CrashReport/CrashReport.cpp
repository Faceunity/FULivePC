#include "CrashReport.h"

#ifdef _WIN64

#else
LPSymInitialize	SymInitializeI;
LPSymCleanup SymCleanupI;
LPStackWalk64 StackWalk64I;
LPSymFromAddr SymFromAddrI;
LPSymGetLineFromAddr SymGetLineFromAddrI;
LPSymFunctionTableAccess64 SymFunctionTableAccess64I;
LPSymGetModuleBase64 SymGetModuleBase64I;
LPSymSetContext SymSetContextI;
LPSymEnumSymbols SymEnumSymbolsI;
LPGetMappedFileNameA GetMappedFileNameA;
LPGetModuleFileNameExA GetModuleFileNameExA;

static const int
CV_REG_EAX = 17,
CV_REG_ECX = 18,
CV_REG_EDX = 19,
CV_REG_EBX = 20,
CV_REG_ESP = 21,
CV_REG_EBP = 22,
CV_REG_ESI = 23,
CV_REG_EDI = 24,
CV_AMD64_RBX = 329,
CV_AMD64_RCX = 330,
CV_AMD64_RDX = 331,
CV_AMD64_RSI = 332,
CV_AMD64_RDI = 333,
CV_AMD64_RBP = 334,
CV_AMD64_RSP = 335,
CV_ALLREG_ERR = 30000,
CV_ALLREG_TEB = 30001,
CV_ALLREG_TIMER = 30002,
CV_ALLREG_EFAD1 = 30003,
CV_ALLREG_EFAD2 = 30004,
CV_ALLREG_EFAD3 = 30005,
CV_ALLREG_VFRAME = 30006,
CV_ALLREG_HANDLE = 30007,
CV_ALLREG_PARAMS = 30008,
CV_ALLREG_LOCALS = 30009,
CV_ALLREG_TID = 30010,
CV_ALLREG_ENV = 30011,
CV_ALLREG_CMDLN = 30012;

//extern "C" {
//	int __cdecl NvAPI_Initialize();
//	int __cdecl NvAPI_SYS_GetDriverAndBranchVersion(int*, char*);
//}

int dbgWalkOn; //0 dump 1 normal 2 init
			   // Psapi.dll
char* gpu_erra = "";
char* gpu_errb = "";
int exp_handler_inited;
char g_local_dat[4096];
int g_sym_initialized = 0;
const char* MemSizeUnit[] = { "B","KB","MB","GB","TB","?" };

int isKnownDll(char* fname) {
	return (
		_strcmpi(fname, "RenderCore_d.dll") == 0 || _strcmpi(fname, "RenderCore.dll") == 0 ||
		_strcmpi(fname, "shadelib_d.dll") == 0 || _strcmpi(fname, "shadelib.dll") == 0 ||
		_strcmpi(fname, "rautility_d.dll") == 0 || _strcmpi(fname, "rautility.dll") == 0 ||
		_strcmpi(fname, "subd_d.dll") == 0 || _strcmpi(fname, "subd.dll") == 0 ||
		_strcmpi(fname, "texman_d.dll") == 0 || _strcmpi(fname, "texman.dll") == 0);
	//spaprt is considered as unknown
}

void getFunctionName(CONTEXT *context, char* func_name) {
	char module_file[260];
	GetModuleFileNameExA(GetCurrentProcess(), NULL, module_file, 260);
	char* pmf = strrchr(module_file, '\\');
	if (pmf != NULL)
		*(pmf + 1) = 0;
	if (g_sym_initialized || SymInitializeI(GetCurrentProcess(), module_file, TRUE)) {
		g_sym_initialized = 1;
		STACKFRAME64 psf;
		LPSTACKFRAME64 sf = &psf;
		memset(sf, 0, sizeof(STACKFRAME64));
		sf->AddrPC.Offset = context->Eip;
		sf->AddrPC.Mode = AddrModeFlat;
		sf->AddrStack.Offset = context->Esp;
		sf->AddrStack.Mode = AddrModeFlat;
		sf->AddrFrame.Offset = context->Ebp;
		sf->AddrFrame.Mode = AddrModeFlat;
		DWORD machineType = IMAGE_FILE_MACHINE_I386;  //IMAGE_FILE_MACHINE_IA64
		HANDLE hProcess = GetCurrentProcess();
		HANDLE hThread = GetCurrentThread();
		if (StackWalk64I(machineType, hProcess, hThread, sf, context, NULL, (PFUNCTION_TABLE_ACCESS_ROUTINE64)SymFunctionTableAccess64I, (PGET_MODULE_BASE_ROUTINE64)SymGetModuleBase64I, NULL)) {
			if (sf->AddrFrame.Offset != 0) {
				char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
				PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
				pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
				pSymbol->MaxNameLen = MAX_SYM_NAME;
				DWORD64 symDisplacement = 0;
				if (SymFromAddrI(hProcess, sf->AddrPC.Offset, &symDisplacement, pSymbol)) {
					strcpy(func_name, pSymbol->Name);
				}
				else
					printf("SymFromAddr failed.\n");
			}
			else
				printf("SymAddr = 0.\n");
		}
		else
			printf("StackWalk64 failed.\n");
	}
}

void SprintMemSize(long long d, char* buf) {
	float df = float(d);
	int mp = 0;
	while (df >= 100.f) {
		df /= 1024;
		mp++;
		if (mp == 4) break;
	}
	sprintf(buf, "%.2f %s\t(%16llX)", df, MemSizeUnit[mp], d);
}

void DumpCPUMemStat(FILE* fout) {
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	char msbuf[100];
	fprintf(fout, "Mem in use:           \t%ld%%\n", statex.dwMemoryLoad);
	SprintMemSize(statex.ullTotalPhys, msbuf);
	fprintf(fout, "Total Memory:         \t%s\n", msbuf);
	SprintMemSize(statex.ullAvailPhys, msbuf);
	fprintf(fout, "Free Memory:          \t%s\n", msbuf);
	SprintMemSize(statex.ullTotalPageFile, msbuf);
	fprintf(fout, "Total Paging File:    \t%s\n", msbuf);
	SprintMemSize(statex.ullAvailPageFile, msbuf);
	fprintf(fout, "Free Paging File:     \t%s\n", msbuf);
	SprintMemSize(statex.ullTotalVirtual, msbuf);
	fprintf(fout, "Total Virtual Memory: \t%s\n", msbuf);
	SprintMemSize(statex.ullAvailVirtual, msbuf);
	fprintf(fout, "Free Virtual Memory:  \t%s\n", msbuf);
	SprintMemSize(statex.ullAvailExtendedVirtual, msbuf);
	fprintf(fout, "Free Extended Memory: \t%s\n", msbuf);
}

void genProtectStr(int protect, char* protectStr) {
	// RWEC - Read Write Excecute Copy
	strcpy(protectStr, "----");
	if (protect&PAGE_EXECUTE) {
		strcpy(protectStr, "--E-");
	}
	else if (protect&PAGE_EXECUTE_READ) {
		strcpy(protectStr, "R-E-");
	}
	else if (protect&PAGE_EXECUTE_READWRITE) {
		strcpy(protectStr, "RWE-");
	}
	else if (protect&PAGE_EXECUTE_WRITECOPY) {
		strcpy(protectStr, "-WEC");
	}
	else if (protect&PAGE_NOACCESS) {
		strcpy(protectStr, "----");
	}
	else if (protect&PAGE_READONLY) {
		strcpy(protectStr, "R---");
	}
	else if (protect&PAGE_READWRITE) {
		strcpy(protectStr, "RW--");
	}
	else if (protect&PAGE_WRITECOPY) {
		strcpy(protectStr, "-W-C");
	}
	else {
		strcpy(protectStr, "----");
	}
}

void DumpVirtualMem(FILE* fout, int esp) {
	_MEMORY_BASIC_INFORMATION mi0, mi;
	//char* type;
	static char fileNameBuffer[1024 * sizeof(TCHAR)];
	static char fileNameBuffer2[1024 * sizeof(TCHAR)];
	static char protectStr[8 * sizeof(TCHAR)];
	int base_addr, region_size;//,state,protect;
							   // free pages
	char* addr = NULL;
	base_addr = 0;
	region_size = 0;
	int nfree = 0;
	unsigned int max_free = 0u;
	while (addr<(char*)0x7fff0000u) {
		VirtualQuery(addr, &mi, sizeof(mi));
		if (mi.State == MEM_FREE) {
			nfree++;
			base_addr = int(mi.BaseAddress);
			region_size = int(mi.RegionSize);
			max_free = max(max_free, (unsigned int)region_size);
		}
		addr = (char*)mi.BaseAddress + mi.RegionSize;
	}
	fprintf(fout, "Free Page Count: %d Largest Free Page: %08X\n", nfree, max_free);
	//fprintf(fout,"-------- Pages --------\n");
	addr = NULL;
	base_addr = 0;
	region_size = 0;
	strcpy(fileNameBuffer, "");
	memset(&mi0, 0, sizeof(mi0));
	memset(&mi, 0, sizeof(mi));
	for (;;) {
		mi.RegionSize = 0;
		VirtualQuery(addr, &mi, sizeof(mi));
		if (!mi.RegionSize) { break; }
		base_addr = int(mi.BaseAddress);
		region_size = int(mi.RegionSize);
		//if(mi0.State==mi.State&&mi0.Type==mi.Type&&base_addr&&base_addr==(int)addr){
		if ((int)mi.AllocationBase != (int)addr || mi.State == MEM_FREE) {
			fprintf(fout, "       - %08x ", region_size);
		}
		else {
			fprintf(fout, "%08x %08x ", base_addr, region_size);
		}
		char* sprotect = "???";
		switch (mi.Protect & 0xff) {
		default:;
			break; case PAGE_EXECUTE:
				sprotect = "--x";
				break; case PAGE_EXECUTE_READ:
					sprotect = "r-x";
					break; case PAGE_EXECUTE_READWRITE:
						sprotect = "rwx";
						break; case PAGE_EXECUTE_WRITECOPY:
							sprotect = "r=x";
							break; case PAGE_NOACCESS:
								sprotect = "---";
								break; case PAGE_READONLY:
									sprotect = "r--";
									break; case PAGE_READWRITE:
										sprotect = "rw-";
										break; case PAGE_WRITECOPY:
											sprotect = "r=-";
		}
		fprintf(fout, "%c%s ", "-R"[mi.State == MEM_RESERVE], sprotect);
		mi0 = mi;
		if (mi.State == MEM_FREE) {
			//fileNameBuffer[0]=0;
			fprintf(fout, "------\n");
		}
		else if (mi.Type == MEM_PRIVATE) {
			//fileNameBuffer[0]=0;
			fprintf(fout, "PRIV ");
		dump_data:
			if (region_size) {
				char dat[256];
				SIZE_T szread = 0;
				memset(dat, 0, sizeof(dat));
				if (mi.State != MEM_RESERVE) {
					if ((unsigned int)(esp - base_addr)<(unsigned int)region_size) {
						//STACK dump
						fprintf(fout, "STACK DUMP");
						for (int ba = 0; ba<region_size; ba += 256) {
							memset(dat, 0, sizeof(dat));
							ReadProcessMemory(GetCurrentProcess(), (void*)(base_addr + ba), dat, 256, &szread);
							fprintf(fout, "\n%08x -stack-> DUMP", base_addr + ba);
							for (int j = 0; j<256; j += 4) {
								if (base_addr + ba + j == (esp&-4)) { fprintf(fout, " --esp-->"); }
								int stkj = *(int*)&dat[j];
								fprintf(fout, " %08x", stkj);
							}
						}
					}
					else {
						fprintf(fout, "data:");
						ReadProcessMemory(GetCurrentProcess(), (void*)base_addr, dat, 256, &szread);
						for (int j = 0; j<256; j++) {
							fprintf(fout, " %02x", (int)dat[j] & 0xff);
						}
					}
				}
				fprintf(fout, "\n");
			}
		}
		else if (mi.Type == MEM_MAPPED) {
			//fileNameBuffer[0]=0;
			fileNameBuffer2[0] = 0;
			GetMappedFileNameA(GetCurrentProcess(), (LPVOID)mi.BaseAddress, fileNameBuffer2, 1024);
			if (!fileNameBuffer2[0]) {
				fprintf(fout, "MMAP ");
			}
			else {
				fprintf(fout, "file: '%s' ", fileNameBuffer2);
			}
			goto dump_data;
		}
		else {
			fileNameBuffer2[0] = 0;
			GetMappedFileNameA(GetCurrentProcess(), (LPVOID)mi.BaseAddress, fileNameBuffer2, 1024);
			if (!fileNameBuffer2[0]) {
				fprintf(fout, "WHAT? ");
				goto dump_data;
			}
			else {
				//mi.Type==MEM_IMAGE
				if (strcmp(fileNameBuffer, fileNameBuffer2) == 0) {
					fprintf(fout, "imag\n");
				}
				else {
					fprintf(fout, "imag: '%s'\n", fileNameBuffer2);
					strcpy(fileNameBuffer, fileNameBuffer2);
				}
			}
		}
		addr = (char*)mi.BaseAddress + mi.RegionSize;
	}
}

void DumpRegisters(FILE* fout, LPCONTEXT pctx) {
	fprintf(fout, "EAX: %08X EBX: %08X ECX: %08X EDX: %08X\n", pctx->Eax, pctx->Ebx, pctx->Ecx, pctx->Edx);
	fprintf(fout, "ESI: %08X EDI: %08X EBP: %08X ESP: %08X\n", pctx->Esi, pctx->Edi, pctx->Ebp, pctx->Esp);
	fprintf(fout, "EIP: %08X EFLAGS: %08X\n", pctx->Eip, pctx->EFlags);
	fprintf(fout, "CS: %04X DS: %04X ES: %04X FS: %04X GS: %04X SS: %04X\n", pctx->SegCs, pctx->SegDs, pctx->SegEs, pctx->SegFs, pctx->SegGs, pctx->SegSs);
	fprintf(fout, "DR0: %08X DR1: %08X DR2: %08X DR3: %08X\n", pctx->Dr0, pctx->Dr1, pctx->Dr2, pctx->Dr3);
	fprintf(fout, "DR6: %08X DR7: %08X\n", pctx->Dr6, pctx->Dr7);
}

void ReallyReadMemory(void* src, void* tar, int size) {
	SIZE_T szread = 0;
	for (int did = 0; did<size;) {
		ReadProcessMemory(GetCurrentProcess(), src, tar, 256, &szread);
		if (!szread) { *(char*)tar = 0; szread = 1; }
		did += szread;
		src = (void*)((int)src + szread);
		tar = (void*)((int)tar + szread);
	}
}

struct TFrameAndContext {
	LPSTACKFRAME64 sf;
	LPCONTEXT context;
	FILE* fout;
};

int GetReg(int regid0, PCONTEXT g_watch_ctx, LPSTACKFRAME64 g_watch_frame) {
	int regid = int(regid0);
	if (regid == CV_REG_EAX)return g_watch_ctx->Eax;
	if (regid == CV_REG_EBX)return g_watch_ctx->Ebx;
	if (regid == CV_REG_ECX)return g_watch_ctx->Ecx;
	if (regid == CV_REG_EDX)return g_watch_ctx->Edx;
	if (regid == CV_REG_EBP)return g_watch_ctx->Ebp;
	if (regid == CV_REG_ESP)return g_watch_ctx->Esp;
	if (regid == CV_REG_ESI)return g_watch_ctx->Esi;
	if (regid == CV_REG_EDI)return g_watch_ctx->Edi;
	if (regid == CV_ALLREG_VFRAME)return g_watch_frame->AddrFrame.Offset;
	if (regid == CV_ALLREG_PARAMS)return g_watch_frame->AddrFrame.Offset;
	if (regid == CV_ALLREG_LOCALS)return g_watch_frame->AddrFrame.Offset;
	return 0;
}

BOOL CALLBACK dump_local(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext) {
	TFrameAndContext *pfc = (TFrameAndContext*)UserContext;
	FILE* fout = pfc->fout;
	char* sname = pSymInfo->Name;
	long long val = 0;
	int addr = pSymInfo->Address;
	if (pSymInfo->Flags&SYMFLAG_REGISTER) {
		val = GetReg(pSymInfo->Register, pfc->context, pfc->sf);
		addr = (int)&val;
	}
	else if (pSymInfo->Flags&SYMFLAG_CONSTANT) {
		addr = (int)&pSymInfo->Value;
	}
	else {
		if (pSymInfo->Flags&SYMFLAG_REGREL) {
			addr += GetReg(pSymInfo->Register, pfc->context, pfc->sf);
		}
		else if (pSymInfo->Flags&SYMFLAG_FRAMEREL) {
			addr += pfc->sf->AddrFrame.Offset;
		}
	}
	fprintf(fout, "%s=", sname);
	if (pSymInfo->Size>sizeof(g_local_dat) || !pSymInfo->Size) {
		fprintf(fout, "<too-large %d>", pSymInfo->Size);
	}
	else {
		SIZE_T szread = 0;
		ReadProcessMemory(GetCurrentProcess(), (void*)addr, g_local_dat, pSymInfo->Size, &szread);
		if (pSymInfo->Size == 4 && szread == 4) {
			fprintf(fout, "%08x", *(int*)&g_local_dat[0]);
		}
		else {
			int isstr = 1, gotzero = 0;
			for (int j = 0; j<(int)szread; j++) {
				fprintf(fout, "%02x ", (int)g_local_dat[j] & 0xff);
				if (!g_local_dat[j]) { gotzero = 1; if (!j) { isstr = 0; } }
				if (!gotzero&&g_local_dat[j]<' ' || (int)g_local_dat[j] >= 127) {
					isstr = 0;
				}
			}
			if (szread != pSymInfo->Size) {
				fprintf(fout, " INCOMPLETE: %d/%d ", szread, pSymInfo->Size);
			}
			if (isstr&&gotzero) { fprintf(fout, " '%s' ", g_local_dat); }
		}
	}
	fprintf(fout, ", ");
	return 1;
}


void DumpCallStack(FILE* fout, LPCONTEXT context) {
	char dat[260];
	SIZE_T szread = 0;
	ReadProcessMemory(GetCurrentProcess(), (void*)context->Esp, dat, 256, &szread);
	fprintf(fout, "STACK: %08X", context->Esp);
	for (int j = 0; j<256; j++) {
		fprintf(fout, " %02x", (int)dat[j] & 0xff);
	}
	fprintf(fout, "\n");
	ReallyReadMemory((void*)(context->Eip - 128), dat, 256);
	fprintf(fout, "CODE: %08X", context->Eip);
	for (int j = 0; j<256; j++) {
		if (j == 128) { fprintf(fout, " --eip-->"); }
		fprintf(fout, " %02x", (int)dat[j] & 0xff);
	}
	fprintf(fout, "\n");
	int pdb_failed = 0;
	char module_file[260];
	GetModuleFileNameExA(GetCurrentProcess(), NULL, module_file, 260);
	char* pmf = strrchr(module_file, '\\');
	if (pmf != NULL)
		*(pmf + 1) = 0;
	if (g_sym_initialized || SymInitializeI(GetCurrentProcess(), module_file, TRUE)) {
		g_sym_initialized = 1;
		STACKFRAME64 psf;
		LPSTACKFRAME64 sf = &psf;
		memset(sf, 0, sizeof(STACKFRAME64));

		sf->AddrPC.Offset = context->Eip;
		sf->AddrPC.Mode = AddrModeFlat;
		sf->AddrStack.Offset = context->Esp;
		sf->AddrStack.Mode = AddrModeFlat;
		sf->AddrFrame.Offset = context->Ebp;
		sf->AddrFrame.Mode = AddrModeFlat;

		DWORD machineType = IMAGE_FILE_MACHINE_I386;

		HANDLE hProcess = GetCurrentProcess();
		HANDLE hThread = GetCurrentThread();

		for (;;) {
			if (!StackWalk64I(machineType, hProcess, hThread, sf, context, NULL, (PFUNCTION_TABLE_ACCESS_ROUTINE64)SymFunctionTableAccess64I, (PGET_MODULE_BASE_ROUTINE64)SymGetModuleBase64I, NULL))
				break;
			if (sf->AddrFrame.Offset == 0)
				break;
			char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
			PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;

			pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			pSymbol->MaxNameLen = MAX_SYM_NAME;

			DWORD64 symDisplacement = 0;
			fprintf(fout, "%08X ", sf->AddrPC.Offset);
			if (SymFromAddrI(hProcess, sf->AddrPC.Offset, &symDisplacement, pSymbol))
			{
				IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
				DWORD dwLineDisplacement;
				if (SymGetLineFromAddrI(hProcess, sf->AddrPC.Offset, &dwLineDisplacement, &lineInfo)) {
					fprintf(fout, "%s(%u): ", lineInfo.FileName, lineInfo.LineNumber);
				}
				fprintf(fout, "%s() ", pSymbol->Name);
			}
			else {
				strcpy(dat, "???");
				GetMappedFileNameA(GetCurrentProcess(), (LPVOID)sf->AddrPC.Offset, dat, 256);
				fprintf(fout, "%s ", dat);
				if (!pdb_failed) {
					MessageBoxA(NULL, "There's an error loading debug information, the log may not be useful", "ERROR", MB_OK | MB_ICONERROR);
					pdb_failed = 1;
				}
			}
			memset(dat, 0, sizeof(dat));
			szread = 0;
			ReadProcessMemory(GetCurrentProcess(), (void*)sf->AddrStack.Offset, dat, 256, &szread);
			fprintf(fout, "\n");
			fprintf(fout, "         --- STACK: %08X", sf->AddrStack.Offset);
			for (int j = 0; j<256; j++) {
				fprintf(fout, " %02x", (int)dat[j] & 0xff);
			}
			fprintf(fout, "\n");
			ReallyReadMemory((void*)(sf->AddrPC.Offset - 128), dat, 256);
			fprintf(fout, "         --- CODE: %08X", sf->AddrPC.Offset);
			for (int j = 0; j<256; j++) {
				if (j == 128) { fprintf(fout, " --eip-->"); }
				fprintf(fout, " %02x", (int)dat[j] & 0xff);
			}
			fprintf(fout, "\n");
			//dump local variables
			if (SymSetContextI(GetCurrentProcess(), sf, context)) {
				TFrameAndContext fctx;
				fctx.sf = sf;
				fctx.context = context;
				fctx.fout = fout;
				fprintf(fout, "         --- LOCALS:  ");
				SymEnumSymbolsI(GetCurrentProcess(), 0, NULL, dump_local, &fctx);
				fprintf(fout, "\n");
			}
		}
	}
	else {
		fprintf(fout, "# Unable to get call stack\n");
		if (!pdb_failed) {
			MessageBoxA(NULL, "There's an error loading debug information, the log may not be useful", "ERROR", MB_OK | MB_ICONERROR);
			pdb_failed = 1;
		}
	}
}


void init_exp_handler() {
	HMODULE psapi_dll = LoadLibraryA("Psapi.dll");
	GetMappedFileNameA = (LPGetMappedFileNameA)GetProcAddress(psapi_dll, "GetMappedFileNameA");
	GetModuleFileNameExA = (LPGetModuleFileNameExA)GetProcAddress(psapi_dll, "GetModuleFileNameExA");

	HMODULE dbghelp_dll = LoadLibraryA("dbgHelp.dll");
	SymInitializeI = (LPSymInitialize)GetProcAddress(dbghelp_dll, "SymInitialize");
	SymCleanupI = (LPSymCleanup)GetProcAddress(dbghelp_dll, "SymCleanup");
	StackWalk64I = (LPStackWalk64)GetProcAddress(dbghelp_dll, "StackWalk64");
	SymFromAddrI = (LPSymFromAddr)GetProcAddress(dbghelp_dll, "SymFromAddr");
	SymGetLineFromAddrI = (LPSymGetLineFromAddr)GetProcAddress(dbghelp_dll, "SymGetLineFromAddr");
	SymFunctionTableAccess64I = (LPSymFunctionTableAccess64)GetProcAddress(dbghelp_dll, "SymFunctionTableAccess64");
	SymGetModuleBase64I = (LPSymGetModuleBase64)GetProcAddress(dbghelp_dll, "SymGetModuleBase64");
	SymSetContextI = (LPSymSetContext)GetProcAddress(dbghelp_dll, "SymSetContext");
	SymEnumSymbolsI = (LPSymEnumSymbols)GetProcAddress(dbghelp_dll, "SymEnumSymbols");

	exp_handler_inited = 1;
}

static void disableSeh() {
	__asm {
		mov fs : dword ptr[0], 0;
	}
	GetLastError();
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	/*char buf[32];
	sprintf(buf,"%d",GetLastError());
	MessageBoxA(NULL,buf,buf,MB_OK);*/
}

//void* g_seh_handle=NULL;
void __cdecl ehook(char* a, char* b) {
#ifndef NDEBUG
	if (fflush(stdout) != 0) {
		freopen("CONOUT$", "w", stdout);
	}
#endif
	if (a) { gpu_erra = a; }
	if (b) { gpu_errb = b; }
#ifndef NDEBUG
	puts("********Crashed Here********");
	if (b) { puts(b); }
	if (a) { puts(a); }
#endif
	//disableSeh();
	__asm {
		int 3;
	}
}

/*int __stdcall dummy_wndproc(HWND a,int b,int c,int d){
return 1;
}*/

BOOL __stdcall disable_enumproc(HWND hw, LPARAM lp)
{
	DWORD pid = 0;
	GetWindowThreadProcessId(hw, &pid);
	if (pid == lp) {
		//SetWindowLongA(hw,GWL_WNDPROC,(long)dummy_wndproc);
		EnableWindow(hw, 0);
	}
	return TRUE;
}


LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	//don't assume Qt is usable here
	if (!exp_handler_inited)
		init_exp_handler();
#ifndef NDEBUG
	if (fflush(stdout) != 0) {
		freopen("CONOUT$", "w", stdout);
	}
#endif
	// walk out of nonsense crash trigger
	static char fileNameBuffer[1024 * sizeof(TCHAR)];
	static char funcNameBuffer[1024 * sizeof(TCHAR)];
	fileNameBuffer[0] = 0;
	funcNameBuffer[0] = 0;
	GetMappedFileNameA(GetCurrentProcess(), (LPVOID)ExceptionInfo->ContextRecord->Eip, fileNameBuffer, 1024);
	char* file_name = strrchr(fileNameBuffer, (int)'\\');
	if (file_name) { file_name++; }
	//printf("file fname: %s\n",file_name);
	//getchar();
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT) {
		// first break caused by ehook()
		// turn on single stepping and continue
		//printf("dbgWalkOn %d\n",dbgWalkOn);
		//getchar();
		if (file_name&&isKnownDll(file_name)) {
			getFunctionName(ExceptionInfo->ContextRecord, funcNameBuffer);
			//printf("first func fname: %s\n",funcNameBuffer);
			//getchar();
			//&&_strcmpi(file_name,"spaprt.dll")!=0
			if (strcmp(funcNameBuffer, "__breakpoint") != 0 && strcmp(funcNameBuffer, "ehook") != 0 && strcmp(funcNameBuffer, "_RTC_CheckEsp") != 0) goto DODUMP;
		}
		else {
			//printf("first unknown.\n",funcNameBuffer);
			//getchar();
		}
		ExceptionInfo->ContextRecord->Eip += 1;
		ExceptionInfo->ContextRecord->EFlags |= 256;
		dbgWalkOn = 2;
		return -1;
	}
	else if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0x80000004) {
		// break caused by single stepping
		//printf("EIP: %08x\n",ExceptionInfo->ContextRecord->Eip);
		if (file_name&&isKnownDll(file_name)) {
			getFunctionName(ExceptionInfo->ContextRecord, funcNameBuffer);
			//printf("funcNameBuffer %s\n",funcNameBuffer);
			if (strcmp(funcNameBuffer, "__breakpoint") != 0 && strcmp(funcNameBuffer, "ehook") != 0 && strcmp(funcNameBuffer, "_RTC_CheckEsp") != 0 && _strcmpi(file_name, "spaprt.dll") != 0)
				goto DODUMP;
			//printf("dbgWalkOn %d\n",dbgWalkOn);
			//getchar();
		}
		// turn on single stepping and continue
		ExceptionInfo->ContextRecord->EFlags |= 256;
		return -1;
	}
DODUMP:
	EnumWindows(disable_enumproc, GetCurrentProcessId());
	SetUnhandledExceptionFilter(NULL);//in case WE crash
									  //if(g_seh_handle){RemoveVectoredExceptionHandler(g_seh_handle);g_seh_handle=NULL;}
#ifndef NDEBUG
	if (fflush(stdout) != 0) {
		freopen("CONOUT$", "w", stdout);
	}
	printf("Crashed, dumping log...");
#endif
	time_t curtime = time(0);
	//printf("do dump!\n");
	//getchar();
	// dump virtual memory
	fileNameBuffer[0] = 0;
	GetModuleFileNameA(GetModuleHandle(NULL), fileNameBuffer, 260);
	char* pslash = strrchr(fileNameBuffer, '\\');
	if (!pslash) { pslash = strrchr(fileNameBuffer, '/'); }
	if (!pslash) { pslash = fileNameBuffer; }
	else { pslash++; }
	sprintf(pslash, "yr%08x.log", curtime);
	FILE* f = fopen(fileNameBuffer, "wb");
	if (!f) {
		ExpandEnvironmentStringsA("%USERPROFILE%\\desktop\\", fileNameBuffer, 260);
		pslash = fileNameBuffer + strlen(fileNameBuffer);
		sprintf(pslash, "yr%08x.log", curtime);
		f = fopen(fileNameBuffer, "wb");
		if (!f) {
			MessageBoxA(NULL, "We crashed... but we are unable to dump the error log to either desktop or program directory", "ERROR", MB_OK | MB_ICONERROR);
			exit(1);
		}
	}
	//fprintf(f, "\n[0] Exception info - %s\n", raGetVersion()); fflush(f);
	fprintf(f, "Code: %08x\nFlags: %08x\nAddress: %08x\n", ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo->ExceptionRecord->ExceptionFlags, ExceptionInfo->ExceptionRecord->ExceptionAddress);
	fprintf(f, "\n[1] Virtual memory mapping\n"); fflush(f);
	DumpVirtualMem(f, ExceptionInfo->ContextRecord->Esp);
	// dump CPU mem stat
	fprintf(f, "\n[2] CPU memory status\n"); fflush(f);
	DumpCPUMemStat(f);
	// dump registers
	fprintf(f, "\n[3] Registers\n");	fflush(f);
	DumpRegisters(f, ExceptionInfo->ContextRecord);
	// dump call stack
	fprintf(f, "\n[4] Call stack\n"); fflush(f);
	CONTEXT ctx2 = *ExceptionInfo->ContextRecord;
	DumpCallStack(f, &ctx2);//this modifies ctx2
	fprintf(f, "\n[5] GPU\n"); fflush(f);
	fprintf(f, "%s\n%s\n", gpu_errb, gpu_erra); fflush(f);
	//NV driver version
	int v = 0;
	static char vstr[128];
	vstr[0] = 0;
	//NvAPI_Initialize();
	//NvAPI_SYS_GetDriverAndBranchVersion(&v, vstr);
	//fprintf(f, "\n[6] NVIDIA Driver\n"); fflush(f);
	//fprintf(f, "Version: %d\nBuild: %s\n", v, vstr); fflush(f);
	// finalize
	tm tim = *localtime(&curtime);
	fprintf(f, "\n******** end@%d-%d-%d-%d:%d:%d ********\n", tim.tm_year + 1900, tim.tm_mon + 1, tim.tm_mday, tim.tm_hour, tim.tm_min, tim.tm_sec);
	fflush(f);
	fclose(f);
	static char cmdbuffer[2048];
	cmdbuffer[0] = 0;
	GetEnvironmentVariableA("HACK_IS_BATCH_RENDER_MODE", cmdbuffer, 128);
	if (cmdbuffer[0] == '1') {
		//no crash dialog in batch render mode
		exit(1);
	}
	cmdbuffer[0] = 0;
	ExpandEnvironmentStringsA("\"%SystemRoot%\\explorer.exe\" /select,", cmdbuffer, 260);
	sprintf(cmdbuffer + strlen(cmdbuffer), "%s", fileNameBuffer);
	system(cmdbuffer);
	MessageBoxA(NULL, "Crashed. Log successfully dumped, click OK to see the file.", "ERROR", MB_OK | MB_ICONERROR);
	//#ifndef NDEBUG
	//	if(fflush(stdout)!=0){
	//		freopen("CONOUT$","w",stdout);
	//	}
	//	printf("Crashed, press enter to exit...");
	//	char bufx[8];
	//	gets(bufx);
	//#endif
	//QMessageBox::warning(NULL,"RA crashed!", "Please close Maya/Max and then collect the log file(RALOG.txt) under rabin directory");
#ifndef NDEBUG
	//go on to debug it
	return EXCEPTION_CONTINUE_SEARCH;
#endif

	exit(1);
	return 1;
}

int ehook_inited = 0;

typedef void(__cdecl*CALLBACK_SPAP_ERROR_HOOK)(char*, char*);
typedef CALLBACK_SPAP_ERROR_HOOK(__cdecl*TspapSetErrorHook)(CALLBACK_SPAP_ERROR_HOOK);
#endif // _WIN64