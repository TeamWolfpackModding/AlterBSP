#include "windows.h"
#include "xlln/xlln.h"
#include "alterbsp.h"
#include <iostream>


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		InitMod();
		if (InitXLLN(hModule))
			return FALSE;
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
		if (UninitXLLN())
			return FALSE;
	}
	return TRUE;
}
