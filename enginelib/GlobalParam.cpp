#include "pch.h"
#include "./GlobalParam.h"

#include "./DxTools/DxInputDevice.h"
#include "./DxTools/DxFontMan.h"

#include "../SigmaCore/DebugInclude.h"

namespace GLOBAL_PARAM
{
	const DWORD g_dwFontColor = D3DCOLOR_ARGB( 255, 255, 0, 255 );

	BOOL g_bOn = TRUE;
	BOOL g_bState = FALSE;
	DWORD g_dwParam = 0xFFFFFFFF;
    bool bShowContentID = FALSE;

	VOID OnOff( BOOL bOn )
	{
		g_bOn = bOn;
		g_dwParam = 0xFFFFFFFF;
	}

	BOOL IsOnOff()
	{
		return g_bOn;
	}

	VOID Init( DWORD dwValue )
	{
		g_dwParam = dwValue;
	}

	VOID Enable( DWORD dwValue )
	{
		g_dwParam |= dwValue;
	}

	VOID Disable( DWORD dwValue )
	{
		g_dwParam &= ~dwValue;
	}

	BOOL IsValue( DWORD dwValue )
	{
		return BOOL( g_dwParam & dwValue );
	}

	VOID EnableIndex( INT iIndex )
	{
		if( iIndex <= 0 || iIndex > GP_INDEX_CNT )
			return;

		Enable( g_dwIdentity[ iIndex - 1 ] );
	}

	VOID DisableIndex( INT iIndex )
	{
		if( iIndex <= 0 || iIndex > GP_INDEX_CNT )
			return;

		Disable( g_dwIdentity[ iIndex - 1 ] );
	}

	BOOL IsValueIndex( INT iIndex )
	{
		if( iIndex <= 0 || iIndex > GP_INDEX_CNT )
			return TRUE;

		return IsValue( g_dwIdentity[ iIndex - 1 ] );
	}

	VOID StateOnOff( BOOL bOn )
	{
		if( !g_bOn )
			return;

		g_bState = bOn;
	}

	BOOL IsStateOnOff()
	{
		return g_bState;
	}

	VOID ChangeState( DxInputDevice* pInput )
	{
		if( !pInput )
			return;

		// 일반유저가 사용할 수 없도록 한다.
		//if( ( pInput->GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED ) &&
		//	( pInput->GetKeyState( DIK_LMENU ) & DXKEY_PRESSED ) &&
		//	( pInput->GetKeyState( DIK_BACK ) & DXKEY_UP ) )
		//{
		//	StateOnOff( !IsStateOnOff() );
		//}

		if( !g_bState )
			return;

		for( INT i=0; i<10; ++i )
		{
			if( pInput->GetKeyState( DIK_1 + i ) & DXKEY_UP )
			{
				INT iIndex = i + 1;

				if( pInput->GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED )
				{
					Init( 0 );
					EnableIndex( iIndex );
				}
				else if( pInput->GetKeyState( DIK_LMENU ) & DXKEY_PRESSED )
				{
					Init();
					DisableIndex( iIndex );
				}
				else
				{
					if( IsValueIndex( iIndex ) )
						DisableIndex( iIndex );
					else
						EnableIndex( iIndex );
				}
			}
		}
	}

	VOID StateDisplay( CD3DFontPar* pFont )
	{
		if( !g_bState || !pFont )
			return;

		FLOAT fPosX = 2.0f;
		FLOAT fPosY = 62.0f;
		CString strText;

		for( INT i=0; i<GP_INDEX_CNT; ++i )
		{
			strText.Format(
				_T( "%02d, %s, %s" ),
				i + 1,
				IsValueIndex( i + 1 ) ? _T( "O" ) : _T( "X" ),
				g_strIdentity[ i ].c_str() );

			pFont->DrawText( fPosX, fPosY, g_dwFontColor, strText.GetString(), 0, FALSE );
			fPosY += 20.0f;
		}
	}

	DWORD g_dwIdentity[ GP_INDEX_CNT ] =
	{
		GP_SKYBOX,
		GP_PIECE,
		GP_CHAR_MOB_ITEM,
		GP_EFFECT,
		GP_WATER,
		GP_LINE_CULLING,
		GP_DISPLAY_NAME,
	};

	std::string g_strIdentity[ GP_INDEX_CNT ] =
	{
		_T( "SkyBox" ),
		_T( "Piece" ),
		_T( "CharMobItem" ),
		_T( "Effect" ),
		_T( "Water" ),
		_T( "LineCulling" ),
		_T( "DisplayName" ),
	};
}