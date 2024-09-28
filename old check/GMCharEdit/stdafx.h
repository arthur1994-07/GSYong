// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#pragma warning(disable:4995)
#pragma warning(disable:4996)

#ifndef _BIND_TO_CURRENT_VCLIBS_VERSION
#define _BIND_TO_CURRENT_VCLIBS_VERSION 1
#endif


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0501		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0501		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0501 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0500	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// Lets help compiler to skip some headers
#define _XTP_EXCLUDE_CALENDAR
#define _XTP_EXCLUDE_SYNTAXEDIT
#define _XTP_EXCLUDE_REPORTCONTROL

#define _XTP_STATICLINK
#define _XTPLIB_NOAUTOLINK
#include "../../RanThirdParty/XtremeToolKit/Include/XTToolkitPro.h"

#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h> // Safe string function


// Auto Link Library Files
#pragma comment (lib,"ws2_32.lib")
#pragma comment (lib,"Winmm.lib")
#pragma comment (lib,"strsafe.lib")


//	dx 관련과 기타 필수 인클루드 및 설정.
#include "../InternalCommonLib/dxstdafx.h"


//
//ADO
//
#import "C:\Program Files (x86)\Common Files\System\ado\msado27.tlb" rename ("EOF", "adoEOF") no_namespace
#include <afxdlgs.h>


#define DAUM
// #define TEST
// #define TAIWAN
// #define JAPAN