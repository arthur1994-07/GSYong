#pragma once

#define WIN32_LEAN_AND_MEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
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
#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.

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