/**
 * \date	2008/07/08
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include <string>

class DxInputDevice;
class CD3DFontPar;

enum
{
	GP_SKYBOX			= 0x00000001,
	GP_PIECE			= 0x00000002,
	GP_CHAR_MOB_ITEM	= 0x00000004,
	GP_EFFECT			= 0x00000008,
	GP_WATER			= 0x00000010,
	GP_LINE_CULLING		= 0x00000020,
	GP_DISPLAY_NAME		= 0x00000040,

	GP_INDEX_CNT		= 7,
};

namespace GLOBAL_PARAM
{
	VOID OnOff( BOOL bOn );
	BOOL IsOnOff();

	VOID Init( DWORD dwValue = 0xFFFFFFFF );
	VOID Enable( DWORD dwValue );
	VOID Disable( DWORD dwValue );
	BOOL IsValue( DWORD dwValue );

	VOID EnableIndex( INT iIndex );
	VOID DisableIndex( INT iIndex );
	BOOL IsValueIndex( INT iIndex );

	VOID StateOnOff( BOOL bOn );
	BOOL IsStateOnOff();

	VOID ChangeState( DxInputDevice* pInput );
	VOID StateDisplay( CD3DFontPar* pFont );

	extern DWORD g_dwIdentity[GP_INDEX_CNT];
	extern std::string g_strIdentity[GP_INDEX_CNT];

    extern bool bShowContentID; //! 게임내 문자열을 ID 상태로 그대로 보여줄 것인가?

}