#include "../pch.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"

#include "../../RanUI/OldUI/Interface/SkillTrayTab.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Tutorial/GLBaseTutorial.h"

#include "./GLInputManager_KeySetting.h"
#include "./GLControlTypeA.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLControlTypeA::ExternEvent_SelectAround()
{
	DoSelectAround();
}

void GLControlTypeA::DoSelectAround()
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	GASSERT(m_pKeySetting);
	GASSERT(pCharacter);

	//������ �ɷ��ִٸ� �ֺ� Ÿ���� �����ϴ� ����� �ȵȴ�.
	if ( pCharacter->IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) )
	{
		return;
	}

	if ( pCharacter->IsCheckedSkillFlagSpecial( EMSPECA_PROVOKE ) )
	{
		return;
	}

	STARGETID sTarget = m_pKeySetting->DoLogic_SelectAround(pCharacter);
	pCharacter->SetSelectID( sTarget, false );
}

void GLControlTypeA::ExternEvent_SkillTrayLeftClickUp( const WORD wQUICKSKILLSLOT )
{
	//! �տ� ����ִ� ��ų�� �����´�
	SNATIVEID sNativeID = GfxInterfaceBase::GetInstance()->GetDragSlotSkillID();

	//	��ų ���
	if ( sNativeID != NATIVEID_NULL() )
	{						
		GLControlBase::ExternEvent_SkillTrayLeftClickUp( wQUICKSKILLSLOT );
	}
	else
	{
		//	( ����� ��ų�� ���� ��� ) ���������� ��ų�� ����� ��ų�� ����.
		m_pGaeaClient->GetCharacter()->ReqSkillRunSet( wQUICKSKILLSLOT );
	}		
}

void GLControlTypeA::ExternEvent_SkillTrayLeftClickUp( const SNATIVEID& sID, const WORD wQUICKSKILLSLOT )
{
	// ��ų ���
	if( sID != NATIVEID_NULL() )
		GLControlBase::ExternEvent_SkillTrayLeftClickUp( wQUICKSKILLSLOT );
	// ����� ��ų�� ���� ��� ���� ������ ��ų�� ����� ��ų�� ����.
	else		
		m_pGaeaClient->GetCharacter()->ReqSkillRunSet( wQUICKSKILLSLOT );
}
