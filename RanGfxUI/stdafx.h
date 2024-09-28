// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include <afx.h>
#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <algorithm>

#include "../../RanThirdParty/GFx/Include/GFx_Kernel.h"
#include "../../RanThirdParty/GFx/Include/GFx.h"
#include "../../RanThirdParty/GFx/Include/GFx_Renderer_D3D9.h"
#include "../../RanThirdParty/GFx/Common/FxPlayerLog.h"
#include "../../RanThirdParty/GFx/Src/GFx/AS3/AS3_Global.h"
#include "../../RanThirdParty/GFx/Src/GFx/GFx_DrawText.h"
#include "../../RanThirdParty/GFx/Src/GFxIME/GFx_IMEManagerWin32.h"

#include "../InternalCommonLib/dxstdafx.h"

#include "../enginelib/DxTools/RENDERPARAM.h"
#include "../enginelib/Common/DXInputString.h"
#include "../enginelib/DeviceMan.h"

#include "../InternalCommonLib/dxstdafx.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

using namespace Scaleform;
using namespace Render;
using namespace GFx;

#define DEF_IMPROVE_DISPLAY_NAME