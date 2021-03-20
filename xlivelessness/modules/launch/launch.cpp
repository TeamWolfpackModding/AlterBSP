#include <Windows.h>
#include "../../global.h"
#include "launch.h"

void InitLaunch() {
	BYTE nop[5] = {0x90, 0x90, 0x90, 0x90, 0x90};
	char* args = ParseArgs();

	// NOP out stupid DRM functionality that prevents bsp.exe launching without a named pipe spawned by battlestationspacific.exe.
	memcpy((void*)(baseAddress + 0x640F5E), nop, 5);

	// Force cSkeletonAppMidway::Init to accept command line arguments.
	memcpy((void*)(baseAddress + 0x4F841F), &args, 4);
}