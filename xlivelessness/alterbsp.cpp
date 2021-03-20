#include <Windows.h>
#include <cstdlib>
#include <iostream>
#include "thirdparty/easyloggingpp/easyloggingpp.h"
#include "modules/launch/launch.h"
#include "modules/lua/lua.h"
#include "modules/log/log.h"
#include "modules/content/content.h"
#include "global.h"
#include "alterbsp.h"

INITIALIZE_EASYLOGGINGPP

void BindStdHandlesToConsole()
{
    //TODO: Add Error checking.

    // Redirect the CRT standard input, output, and error handles to the console
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stderr);
    freopen("CONOUT$", "w", stdout);

    // Note that there is no CONERR$ file
    HANDLE hStdout = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hStdin = CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    SetStdHandle(STD_OUTPUT_HANDLE, hStdout);
    SetStdHandle(STD_ERROR_HANDLE, hStdout);
    SetStdHandle(STD_INPUT_HANDLE, hStdin);

    //Clear the error state for each of the C++ standard stream objects. 
    std::wclog.clear();
    std::clog.clear();
    std::wcout.clear();
    std::cout.clear();
    std::wcerr.clear();
    std::cerr.clear();
    std::wcin.clear();
    std::cin.clear();
}

void UnprotectMemory() {
    HMODULE hModule;
    hModule = GetModuleHandleA(NULL);

    PIMAGE_DOS_HEADER header = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD)hModule + header->e_lfanew);

    // unprotect the entire PE image
    SIZE_T size = ntHeader->OptionalHeader.SizeOfImage;
    DWORD oldProtect;
    VirtualProtect((LPVOID)hModule, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    CloseHandle(hModule);
}

void InitMod() {
    el::Logger* xliveLogger = el::Loggers::getLogger("xlive");
    el::Logger* engineLogger = el::Loggers::getLogger("engine");
    el::Logger* luaLogger = el::Loggers::getLogger("lua");
    el::Logger* discordLogger = el::Loggers::getLogger("discord");

    UnprotectMemory();

    AllocConsole();
    SetConsoleTitleA("Battlestations Pacific Console");
    BindStdHandlesToConsole();

    InitLaunch();
    //InitContent();
    InitLua();
    InitLog();

    LOG(INFO) << "alterBSP started up successfully.";
    LOG(INFO) << "Arguments: " + std::string(ParseArgs());
}