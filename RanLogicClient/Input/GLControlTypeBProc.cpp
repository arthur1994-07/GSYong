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
	// Ű ����

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
	// Ű ����
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

	// �޽��� �ڽ� ����
	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_HotKey_CancelMsgBox ) )
		return;

	bool bIsOpenPrivateMareket = GLWidgetScript::GetInstance().LuaCallFunc< bool >
		( NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_IsOpen );

	// ���� ���� ��� ESC�� ������ �ٷ� ������� �ʰ� �˾�â���� Ȯ���ؾ��� 
	if( bIsOpenPrivateMareket == true )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >
			( NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_Close );
		return ;
	}

	//1. ���� �����ִ� �������̽� â �ݱ�
	if( GfxInterfaceBase::GetInstance()->EscWidgetClose() )
		return;

	//2. ���� ���� ������ų�� �������̶�� ���
	const SKILLID& sSkillID = pCharacter->GetScopeSkillRun();
	if ( sSkillID != SKILLID_NULL )
	{
		pCharacter->SetScopeSkillRun(SKILLID_NULL);
		return;
	}

	//3. �ൿ���� �׼��� ����Ѵ�.(�̵� ����)
	if ( !pCharacter->IsEmptyReservationAction() || !pCharacter->isAction(GLAT_IDLE, GLAT_MOVE) )
	{
        if( !pCharacter->isAction(GLAT_FALLING, GLAT_DIE) )
        {
            pCharacter->ResetAction();
            pCharacter->ResetReservationAction();
            return;
        }
	}

	//4. ���� ������ Ÿ���� �����Ѵ�
    STARGETID& sSelectID = pCharacter->GetSelectID();
    if ( sSelectID != TARGETID_NULL )
    {
        pCharacter->ResetSelectID();
        return;
    }

	//5. ESC �޴� â�� ����
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_System );
}
