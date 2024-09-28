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
	//! 손에 들고있는 스킬을 가져온다
	SNATIVEID sNativeID = GfxInterfaceBase::GetInstance()->GetDragSlotSkillID();

	//	스킬 등록
	if ( sNativeID != NATIVEID_NULL() )
	{						
		m_pGaeaClient->GetCharacter()->ReqSkillQuickSet( sNativeID, wQUICKSKILLSLOT );
	
		// 튜토리얼때 들고 있는 스킬 놓기.
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
		//	( 등록할 스킬이 없을 경우 ) 현제슬롯의 스킬을 사용할 스킬로 설정.
		m_pGaeaClient->GetCharacter()->ReqSkillRunSet( wQUICKSKILLSLOT );
	}
}

void GLControlBase::ExternEvent_SkillTrayLeftClickUp( const SNATIVEID& sID, const WORD wQUICKSKILLSLOT )
{
	//	스킬 등록
	if( sID != NATIVEID_NULL() )
	{						
		m_pGaeaClient->GetCharacter()->ReqSkillQuickSet( sID, wQUICKSKILLSLOT );

		// 튜토리얼때 들고 있는 스킬 놓기.
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
		//	( 등록할 스킬이 없을 경우 ) 현제슬롯의 스킬을 사용할 스킬로 설정.
		m_pGaeaClient->GetCharacter()->ReqSkillRunSet( wQUICKSKILLSLOT );
	}		
}

void GLControlBase::ExternEvent_SelectAround()
{
	//Blank
}
