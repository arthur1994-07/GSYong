// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__ACBBDC05_A691_43E4_A5BA_E41585F3CB49__INCLUDED_)
#define AFX_STDAFX_H__ACBBDC05_A691_43E4_A5BA_E41585F3CB49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef WINVER
#define WINVER 0x0501
#endif

#pragma warning(disable:4005)
#pragma warning(disable:4819)
#pragma warning(disable:4995)
#pragma warning(disable:4996)


#include <afx.h>
#include <afxwin.h>

// TODO: reference additional headers your program requires here
#include <afxext.h>         // MFC 익스텐션
#include <afxdisp.h>        // MFC 자동화 클래스

#include <afxdtctl.h>		// Internet Explorer 4 공용 컨트롤에 대한 MFC 지원
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 공용 컨트롤에 대한 MFC 지원
#endif // _AFX_NO_AFXCMN_SUPPORT


//	dx 관련과 기타 필수 인클루드 및 설정.
#include "dxstdafx.h"
#include <map>
#include <string>
#include <algorithm>
#include <strsafe.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#ifdef UNICODE
    #define TSTRING		  std::wstring
    #define TCERR		  std::wcerr
    #define TOFSTREAM	  std::wofstream
    #define TFSTREAM	  std::wfstream
    #define TSTRINGSTREAM std::wstringstream
#else
    #define TSTRING		  std::string
    #define TCERR		  std::cerr
    #define TOFSTREAM	  std::ofstream
    #define TSTREAM		  std::fstream
    #define TSTRINGSTREAM std::stringstream
#endif

#ifndef SAFE_DELETE
    #define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    

#ifndef SAFE_DELETE_ARRAY
    #define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    

#ifndef SAFE_RELEASE
    #define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#endif // !defined(AFX_STDAFX_H__ACBBDC05_A691_43E4_A5BA_E41585F3CB49__INCLUDED_)
