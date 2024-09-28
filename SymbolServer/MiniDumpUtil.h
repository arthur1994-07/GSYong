#ifndef MiniDumpUtil_H
#define MiniDumpUtil_H

#include "windows.h"

extern "C" DWORD SnapCurrentProcess(LPCONTEXT lpContext);

#endif