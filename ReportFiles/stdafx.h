// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

#ifndef _BIND_TO_CURRENT_VCLIBS_VERSION
#define _BIND_TO_CURRENT_VCLIBS_VERSION 1
#endif


#include <iostream>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include <afx.h>
#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.
#include <afxdtctl.h>		// Internet Explorer 4의 공용 컨트롤에 대한 MFC 지원입니다.
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>
