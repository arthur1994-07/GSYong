#ifndef _INTERNAL_COMMON_LIB_DX_STD_AFX_H_
#define _INTERNAL_COMMON_LIB_DX_STD_AFX_H_

#pragma once

#ifndef STRICT
	#define STRICT
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
#pragma warning(disable:4100) // disable unreference formal parameter warnings for /W4 builds
#pragma warning(disable:4819)
#pragma warning(disable:4996)

#define NOMINMAX
#include <windows.h>

// PhysX 때문에 현재 코드가 들어갔다.
// 왜 NOMINMAX 을 하는지는 모르겠다.
// 우회경로로 이렇게 만들었다.
#ifndef new_max
#define new_max(a, b)  (((a) > (b)) ? (a) : (b)) 
#endif
#ifndef new_min
#define new_min(a, b)  (((a) < (b)) ? (a) : (b)) 
#endif

#include <basetsd.h>
#include <assert.h>
#include <wchar.h>
#include <tchar.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <malloc.h> // _alloca
#include <objbase.h>

#include <mmsystem.h>
#include <commctrl.h> // for InitCommonControls() 
#include <shellapi.h> // for ExtractIcon()
#include <new.h>      // for placement new
#include <math.h>      
#include <limits.h>      
#include <multimon.h> 

// CRT's memory leak detection
#if defined(DEBUG) | defined(_DEBUG)
	#include <crtdbg.h>
#endif

// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) | defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

// Direct3D includes
#include "../../RanThirdParty/DirectX/Include/ddraw.h"
#include "../../RanThirdParty/DirectX/Include/d3d9types.h"
#include "../../RanThirdParty/DirectX/Include/d3dx9math.h"
#include "../../RanThirdParty/DirectX/Include/d3dx9mesh.h"
#include "../../RanThirdParty/DirectX/Include/d3d9.h"
#include "../../RanThirdParty/DirectX/Include/d3dx9.h"
#include "../../RanThirdParty/DirectX/Include/dxerr9.h"
#include "../../RanThirdParty/DirectX/Include/rmxfguid.h"
#include "../../RanThirdParty/DirectX/Include/dxfile.h"

#ifndef DIRECTINPUT_VERSION
	#define DIRECTINPUT_VERSION 0x0800
#endif //DIRECTINPUT_VERSION

#include "../../RanThirdParty/DirectX/Include/dinput.h"

// DirectSound includes
#include <mmsystem.h>
#include <mmreg.h>
#include "../../RanThirdParty/DirectX/Include/dsound.h"

// strsafe.h deprecates old unsecure string functions.  If you 
// really do not want to it to (not recommended), then uncomment the next line 

#define STRSAFE_NO_DEPRECATE

#ifndef STRSAFE_NO_DEPRECATE
#pragma deprecated("strncpy")
#pragma deprecated("wcsncpy")
#pragma deprecated("_tcsncpy")
#pragma deprecated("wcsncat")
#pragma deprecated("strncat")
#pragma deprecated("_tcsncat")
#endif

#include <strsafe.h>

#define LPDIRECT3DDEVICEQ			LPDIRECT3DDEVICE9
#define LPDIRECT3DTEXTUREQ			LPDIRECT3DTEXTURE9
#define LPDIRECT3DSURFACEQ			LPDIRECT3DSURFACE9

#define LPDIRECT3DVERTEXBUFFERQ		LPDIRECT3DVERTEXBUFFER9
#define LPDIRECT3DINDEXBUFFERQ		LPDIRECT3DINDEXBUFFER9
#define LPDIRECT3DCUBETEXTUREQ		LPDIRECT3DCUBETEXTURE9

#define D3DVIEWPORTQ				D3DVIEWPORT9
#define D3DMATERIALQ				D3DMATERIAL9
#define D3DLIGHTQ					D3DLIGHT9
#define LPDIRECT3DQ					LPDIRECT3D9
#define D3DCAPSQ					D3DCAPS9

// #ifndef V
// #define V(x)           { hr = x; }
// #endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return hr; } }
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

#include <sstream>
#include <memory>

#ifdef UNICODE
	#define TSTRING		std::wstring
	#define TCERR		std::wcerr
	#define TOFSTREAM	std::wofstream
	#define TFSTREAM	std::wfstream
    #define TSTRINGSTREAM	std::wstringstream
#else
	#define TSTRING		std::string
	#define TCERR		std::cerr
	#define TOFSTREAM	std::ofstream
	#define TSTREAM		std::fstream
    #define TSTRINGSTREAM	std::stringstream
#endif


#ifdef _DEBUG
	#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3CommonCHECKED_x86.lib")
	#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3CHECKED_x86.lib")
	#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3CookingCHECKED_x86.lib")
	//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3CharacterKinematicCHECKED_x86.lib")
	#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3ExtensionsCHECKED.lib")
	//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3VehicleCHECKED.lib")
	//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/RepX3CHECKED.lib")
	//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/RepXUpgrader3CHECKED.lib")
	#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysXProfileSDKCHECKED.lib")
	//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysXVisualDebuggerSDKCHECKED.lib")
	//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PxTaskCHECKED.lib")
#else
	#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3Common_x86.lib")
	#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3_x86.lib")
	#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3Cooking_x86.lib")
	//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3CharacterKinematic_x86.lib")
	#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3Extensions.lib")
	//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3Vehicle.lib")
	//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/RepX3.lib")
	//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/RepXUpgrader3.lib")
	#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysXProfileSDK.lib")
	//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysXVisualDebuggerSDK.lib")
	//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PxTask.lib")
#endif

//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3Common_x86.lib")
//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3_x86.lib")
//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysX3Extensions.lib")
//#pragma comment(lib,"../../RanThirdParty/PhysX_3_2/Lib/win32/PhysXProfileSDK.lib")


#ifdef VLD_FORCE_ENABLE
	#include "../=VLD/src/vld.h"
#endif


#endif // _INTERNAL_COMMON_LIB_DX_STD_AFX_H_