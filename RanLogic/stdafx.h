#ifndef _RAN_LOGIC_STD_AFX_H_
#define _RAN_LOGIC_STD_AFX_H_

// stdafx.h : �� ������� �ʰ� ���� ����ϴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.
//

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

#define WIN32_LEAN_AND_MEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include <afx.h>
#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.

//	dx ���ð� ��Ÿ �ʼ� ��Ŭ��� �� ����.
//#include "DxCommon9\dxstdafx.h"

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

// MS
#include <strsafe.h>
#include <afxdlgs.h>
#include <assert.h>
#include "../../RanThirdParty/DirectX/Include/d3dx9math.h"
#include "../InternalCommonLib/dxstdafx.h"

/////////////////////////////////////
#if defined(DEBUG) | defined(_DEBUG)
// #ifndef V
// #define V(x)           { hr = x; if( FAILED(hr) ) { DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
// #endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
#endif
#else
// #ifndef V
// #define V(x)           { hr = x; }
// #endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return hr; } }
#endif
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

#ifndef __gassert_
#define __gassert_
bool _gassert ( bool bExp, const char *const szExp, const char *const szFile, const long nLine  );

#ifdef _RELEASED
#undef assert
#define assert(exp) \
	if ( _gassert ( (exp)?true:false, #exp, __FILE__, __LINE__ ) ) \
{ _asm { int 3 } }
#endif
#endif

#ifdef UNICODE
#define TSTRING		std::wstring
#define TCERR		std::wcerr
#define TOFSTREAM	std::wofstream
#define TFSTREAM	std::wfstream
#else
#define TSTRING		std::string
#define TCERR		std::cerr
#define TOFSTREAM	std::ofstream
#define TSTREAM		std::fstream
#endif

/////////////////////////////////////

#endif // _RAN_LOGIC_STD_AFX_H_