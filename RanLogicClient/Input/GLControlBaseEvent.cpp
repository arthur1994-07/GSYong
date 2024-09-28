#include "../pch.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Tutorial/GLBaseTutorial.h"

#include "./GLControlBase.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLControlBase::ExternEvent_SkillTrayLeftClickUp( const WORD wQUICKSKILLSLOT )
{
	//! �տ� ����ִ� ��ų�� �����´�
	SNATIVEID sNativeID = GfxInterfaceBase::GetInstance()->GetDragSlotSkillID();

	//	��ų ���
	if ( sNativeID != NATIVEID_NULL() )
	{						
		m_pGaeaClient->GetCharacter()->ReqSkillQuickSet( sNativeID, wQUICKSKILLSLOT );
	
		// Ʃ�丮�� ��� �ִ� ��ų ����.
		if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
		{
			if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep() &&
				m_pGaeaClient->GetTutorial()->IsSubSixStep()
				|| m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep() &&
				m_pGaeaClient->GetTutorial()->IsSubSevenStep() )
			{
				m_pGaeaClient->GetTutorial()->m_sHoldSkill = NATIVEID_NULL();
			}
		}
	}
	else
	{
		//	( ����� ��ų�� ���� ��� ) ���������� ��ų�� ����� ��ų�� ����.
		m_pGaeaClient->GetCharacter()->ReqSkillRunSet( wQUICKSKILLSLOT );
	}
}

void GLControlBase::ExternEvent_SkillTrayLeftClickUp( const SNATIVEID& sID, const WORD wQUICKSKILLSLOT )
{
	//	��ų ���
	if( sID != NATIVEID_NULL() )
	{						
		m_pGaeaClient->GetCharacter()->ReqSkillQuickSet( sID, wQUICKSKILLSLOT );

		// Ʃ�丮�� ��� �ִ� ��ų ����.
		if( m_pGaeaClient->GetTutorial()->IsTutorial() )
		{
			if( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep() &&
				m_pGaeaClient->GetTutorial()->IsSubSixStep() ||
				m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep() &&
				m_pGaeaClient->GetTutorial()->IsSubSevenStep() )
			{
				m_pGaeaClient->GetTutorial()->m_sHoldSkill = NATIVEID_NULL();
			}
		}
	}
	else
	{
		//	( ����� ��ų�� ���� ��� ) ���������� ��ų�� ����� ��ų�� ����.
		m_pGaeaClient->GetCharacter()->ReqSkillRunSet( wQUICKSKILLSLOT );
	}		
}

void GLControlBase::ExternEvent_SelectAround()
{
	//Blank
}
