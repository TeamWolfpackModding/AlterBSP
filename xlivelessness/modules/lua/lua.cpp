#include <Windows.h>
#include <iostream>
#include "../../thirdparty/easyloggingpp/easyloggingpp.h"
#include "../../thirdparty/subhook/subhook.h"
#include "../../global.h"
#include "lua.h"

subhook_t fputs_hook;
subhook_flags_t fputs_hook_flags;

DECLSPEC_NOINLINE int fputs_detour(const char* bruh, FILE*) {
	CLOG(INFO, "lua") << bruh;
	return 0;
}

void InitLua() {
	BYTE nop[5] = {0x90, 0x90, 0x90, 0x90, 0x90};

	// Patch luaB_print to remove unneccesary fputs calls that cause weird output in log
	memcpy((void*)(baseAddress + 0x6660A6), nop, 5);
	memcpy((void*)(baseAddress + 0x6660E2), nop, 5);

	// Detour fputs function to fputs_hook
	fputs_hook = subhook_new((void*)(baseAddress + 0x7FE2B6), (void*)fputs_hook, fputs_hook_flags);
	subhook_install(fputs_hook);
}
