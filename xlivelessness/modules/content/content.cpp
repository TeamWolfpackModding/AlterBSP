#include <Windows.h>
#include "../../thirdparty/easyloggingpp/easyloggingpp.h"
#include "../../thirdparty/subhook/subhook.h"
#include "../../global.h"
#include "content.h"

void InitContent() {
	const char* path = "data\\";

	memcpy((void*)(baseAddress + 0x33D7F2), &path, 4);
}