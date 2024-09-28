#ifndef _ENGINE_LIB_PCH_H_
#define _ENGINE_LIB_PCH_H_

#pragma once

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
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
#pragma warning(disable:4819)
#pragma warning(disable:4995)
#pragma warning(disable:4996)

#define NOMINMAX

#include <afx.h>
#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.

#include <boost/shared_ptr.hpp>

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


// Standard
#include <algorithm>
#include <strsafe.h>
#include "../InternalCommonLib/dxstdafx.h"
#include "./Common/profile.h"
#include "./DxCommon9/DXUTmisc.h"
#include "./DxCommon9/DXUT.h"
#include "./DxTools/DebugSet.h"

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

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

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

// 문자 출력을 프리타입으로 변경
#define DEF_FREETYPE
// 메쉬 LOD 기능을 추가
#define DEF_MESH_LOD

#endif // _ENGINE_LIB_PCH_H_
