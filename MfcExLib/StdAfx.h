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
#include <afxext.h>         // MFC �ͽ��ټ�
#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ����

#include <afxdtctl.h>		// Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC ����
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows ���� ��Ʈ�ѿ� ���� MFC ����
#endif // _AFX_NO_AFXCMN_SUPPORT


//	dx ���ð� ��Ÿ �ʼ� ��Ŭ��� �� ����.
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
