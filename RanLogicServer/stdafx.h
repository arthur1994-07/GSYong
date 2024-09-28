#ifndef _RAN_LOGIC_SERVER_STD_AFX_H_
#define _RAN_LOGIC_SERVER_STD_AFX_H_

#pragma once

#ifndef WINVER				// Windows XP �̻󿡼��� ����� ����� �� �ֽ��ϴ�.
#define WINVER 0x0501		// �ٸ� ������ Windows�� �µ��� ������ ������ ������ �ֽʽÿ�.
#endif

#ifndef _WIN32_WINNT		// Windows XP �̻󿡼��� ����� ����� �� �ֽ��ϴ�.                   
#define _WIN32_WINNT 0x0501	// �ٸ� ������ Windows�� �µ��� ������ ������ ������ �ֽʽÿ�.
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
#include "../InternalCommonLib/dxstdafx.h"

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

#include <string>
#include <vector>

// MS
#include <strsafe.h>
#include <afxdlgs.h>

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#endif // _RAN_LOGIC_SERVER_STD_AFX_H_