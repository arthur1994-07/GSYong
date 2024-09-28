/**
 * \date	2006/03/06
 * \author  Jun-Hyuk Choi
 * \todo	개별 윈도우의 Active/DeActive 발생 시 정렬 필요함
 *          윈도우별로 텍스쳐를 만들지 않으면 다중 출력 보장 못함
 */
#pragma once

#include <string>

class DxInputDevice;

namespace COMMON_WEB
{
    VOID Create( CWnd* pParent, TSTRING strAppPath );
	VOID Destroy();

	BOOL IsCreate();
	INT IssueNewID();

    VOID SetFocusParent();
    VOID TranslateAccelerator( MSG& msg );
    VOID CheckIntegrity( CString& strUrl );
    VOID SetVisible( INT iID, BOOL bVisible );
	VOID Navigate( INT iID, const TCHAR* pszPath, BOOL bShow, const TCHAR* pszPOST = NULL );
	VOID MoveWindow( INT iID, INT iX, INT iY, INT iWidth, INT iHeight, BOOL bRepaint );
	VOID Refresh( INT iID );

    INT GetTopWindowID();
    BOOL GetVisible( INT iID );
    BOOL IsEditFocus( INT iID );
    BOOL IsCompleteLoad( INT iID );
	VOID SetCompleteLoad( INT iID, BOOL bCompleteLoad );
	TSTRING GetDocument( INT iID );
	VOID* GetWebBrowser( INT iID );
    TSTRING GetLocationUrl( INT iID );

	VOID InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID Render();
	VOID DeleteDeviceObjects();

	VOID TestVisible( DxInputDevice* pInput, WORD nWidth, WORD nHeight );
}