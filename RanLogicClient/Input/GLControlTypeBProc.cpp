#include "../pch.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Macro/MacroManager.h"

#include "./GLInputManager_KeySetting.h"
#include "./GLControlTypeB.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLControlTypeB::Proc_SkillSlot( const SKEYCAPS& cKeyCaps, const UINT nIndex )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	if ( !pCharacter )
	{
		return;
	}

	if( pCharacter->IsCheckedSkillFlagSpecial(EMSPECA_SCREEN_THROW) )
		return;

	WORD wTab = GLWidgetScript::GetInstance().LuaCallFunc< WORD >( NSWIDGET_SCRIPT::g_strFunc_SQS_CurrentTab );
	WORD wSlotCount = GLWidgetScript::GetInstance().LuaCallFunc< WORD >( NSWIDGET_SCRIPT::g_strFunc_SQS_TabSlotCount );

	const BOOL bSkillActive(pCharacter->SkillActive( wTab * wSlotCount + static_cast< WORD >( nIndex ) ));
	if ( (bSkillActive == TRUE) && GLMacroManager::GetInstance().IsActive() )
		GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_LOCK_USERINPUT);
}

void GLControlTypeB::Proc_ActionCancel(const SKEYCAPS& cKeyCaps)
{
	// 키 예약
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	if ( !pCharacter )
	{
		return;
	}	

	// 메시지 박스 끄기
	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_HotKey_CancelMsgBox ) )
		return;

	bool bIsOpenPrivateMareket = GLWidgetScript::GetInstance().LuaCallFunc< bool >
		( NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_IsOpen );

	// 상점 같은 경우 ESC를 누른후 바로 종료되지 않고 팝업창에서 확인해야함 
	if( bIsOpenPrivateMareket == true )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >
			( NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_Close );
		return ;
	}

	//1. 현재 열려있는 인터페이스 창 닫기
	if( GfxInterfaceBase::GetInstance()->EscWidgetClose() )
		return;

	//2. 만약 범위 지정스킬이 구동중이라면 취소
	const SKILLID& sSkillID = pCharacter->GetScopeSkillRun();
	if ( sSkillID != SKILLID_NULL )
	{
		pCharacter->SetScopeSkillRun(SKILLID_NULL);
		return;
	}

	//3. 행동중인 액션을 취소한다.(이동 제외)
	if ( !pCharacter->IsEmptyReservationAction() || !pCharacter->isAction(GLAT_IDLE, GLAT_MOVE) )
	{
        if( !pCharacter->isAction(GLAT_FALLING, GLAT_DIE) )
        {
            pCharacter->ResetAction();
            pCharacter->ResetReservationAction();
            return;
        }
	}

	//4. 현재 선택한 타겟을 해제한다
    STARGETID& sSelectID = pCharacter->GetSelectID();
    if ( sSelectID != TARGETID_NULL )
    {
        pCharacter->ResetSelectID();
        return;
    }

	//5. ESC 메뉴 창을 연다
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_System );
}
