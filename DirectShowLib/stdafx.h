// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include <afx.h>
#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.



// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

#include <string>
#include <map>
#include <algorithm>

#include "../../RanThirdParty/DirectX/Include/d3d9.h"
#include "../../RanThirdParty/DirectX/Include/d3dx9.h"

#include "../../RanThirdParty/WindowsSDK_v6.1/Include/DShow.h"
#include "../../RanThirdParty/WindowsSDK_v6.1/Include/mpconfig.h"
#include "../../RanThirdParty/WindowsSDK_v6.1/Include/vmr9.h"

#include "../../RanThirdParty/WindowsSDK_v6.1/DirectShow/BaseClasses/streams.h"
#include "../../RanThirdParty/WindowsSDK_v6.1/DirectShow/BaseClasses/wxdebug.h"
#include "../../RanThirdParty/WindowsSDK_v6.1/DirectShow/BaseClasses/wxutil.h"