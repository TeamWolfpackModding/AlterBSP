#include <Windows.h>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include "global.h"

uint64_t baseAddress = (uint64_t)GetModuleHandleA(NULL);

char* GetPrefAdapterName() {
    std::ifstream adapterFile;
    adapterFile.open("adapter.txt");

    std::stringstream strStream;
    strStream << adapterFile.rdbuf();
    std::string adapterName = strStream.str();

    return strdup(adapterName.c_str());
}

char* ParseArgs() {
    char* s = GetCommandLineA();
    if (*s == '"') {
        ++s;
        while (*s)
            if (*s++ == '"')
                break;
    }
    else {
        while (*s && *s != ' ' && *s != '\t')
            ++s;
    }
    /* (optionally) skip spaces preceding the first argument */
    while (*s == ' ' || *s == '\t')
        s++;
    return s;
}
