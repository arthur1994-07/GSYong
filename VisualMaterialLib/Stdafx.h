#pragma once

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

// If app hasn't choosen, set to work with Windows 98, Windows Me, Windows 2000, Windows XP and beyond
#ifndef WINVER
#define WINVER         0x0410
#endif
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410 
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0500 
#endif

#pragma warning(disable:4005)
#pragma warning(disable:4819)
#pragma warning(disable:4996)

#include <afx.h>
#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.

// Standard
#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <hash_map>
#include <set>
#include <hash_set>
#include <list>

// MS
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <wincrypt.h>
#include <rtcapi.h>
#include <imm.h>
#include <io.h>
#include <malloc.h>

#include <boost/shared_ptr.hpp>

#include "../InternalCommonLib/dxstdafx.h"