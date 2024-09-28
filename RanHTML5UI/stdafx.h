
#pragma once

#ifndef WINVER
#define WINVER 0x0501
#endif

#pragma warning(disable:4005)
#pragma warning(disable:4819)
#pragma warning(disable:4995)
#pragma warning(disable:4996)

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4651)

#define WIN32_LEAN_AND_MEAN		
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		
#endif

#include <afx.h>
#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.

//	dx 관련과 기타 필수 인클루드 및 설정.
#include "../InternalCommonLib/dxstdafx.h"
#include "../EngineLib/DxTools/DebugSet.h"

#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4651)
#endif

#include <algorithm>
#include <deque>
#include <fstream>
#include <hash_map>
#include <hash_set>
#include <list>
#include <map>
#include <queue>
//#include <strstream>
#include <string>
#include <vector>
#include <assert.h>

// MS
#include <strsafe.h>
#include <afxdlgs.h>

#include "afxstr.h"

//#include <GUIBase/stdafx.h>
#include "../=Html5UI_Interface/GUIBase/stdafx.h"

#define DEF_IMPROVE_DISPLAY_NAME