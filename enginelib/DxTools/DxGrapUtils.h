//	[DxGrapUtils], (2002.12.06), JDH
//
//	ScreenGrab (), ȭ���� ���Ͽ� ����.
//	CaptureScreen(), ���� �̸��� �ڵ����� �Ű��ָ鼭 ȭ���� ���Ͽ� ����.
//
#pragma once

#include <string>
#include "../../InternalCommonLib/dxstdafx.h"

namespace DxGrapUtils
{
	void SetPath( LPCTSTR szPath );

	HRESULT ScreenGrab ( LPDIRECT3DDEVICEQ pDev, LPCTSTR fileName );
	HRESULT CaptureScreen ( LPDIRECT3DDEVICEQ pDev );
	//BOOL BitmapToJPG ( LPCTSTR lpszPathName, BOOL bBitmapDelete );

    extern std::string strCapturePath;
}