// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef _BIND_TO_CURRENT_VCLIBS_VERSION
#define _BIND_TO_CURRENT_VCLIBS_VERSION 1
#endif

#if !defined(AFX_STDAFX_H__BDBCDC2E_D595_47EC_975F_71921E87914F__INCLUDED_)
#define AFX_STDAFX_H__BDBCDC2E_D595_47EC_975F_71921E87914F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//	dx ���ð� ��Ÿ �ʼ� ��Ŭ��� �� ����.
#include "../InternalCommonLib/dxstdafx.h"
#include <afxdlgs.h>

// ���� �������� ���� �������� ����
// �ڵ� ����ȭ �Ǹ� ������ �Ȱ͵� ���� ����
//#define DEF_SKIN_SYNC

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BDBCDC2E_D595_47EC_975F_71921E87914F__INCLUDED_)
