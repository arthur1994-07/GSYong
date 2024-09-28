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

	//도발이 걸려있다면 주변 타켓을 선택하는 기능이 안된다.
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
	//! 손에 들고있는 스킬을 가져온다
	SNATIVEID sNativeID = GfxInterfaceBase::GetInstance()->GetDragSlotSkillID();

	//	스킬 등록
	if ( sNativeID != NATIVEID_NULL() )
	{						
		GLControlBase::ExternEvent_SkillTrayLeftClickUp( wQUICKSKILLSLOT );
	}
	else
	{
		//	( 등록할 스킬이 없을 경우 ) 현제슬롯의 스킬을 사용할 스킬로 설정.
		m_pGaeaClient->GetCharacter()->ReqSkillRunSet( wQUICKSKILLSLOT );
	}		
}

void GLControlTypeA::ExternEvent_SkillTrayLeftClickUp( const SNATIVEID& sID, const WORD wQUICKSKILLSLOT )
{
	// 스킬 등록
	if( sID != NATIVEID_NULL() )
		GLControlBase::ExternEvent_SkillTrayLeftClickUp( wQUICKSKILLSLOT );
	// 등록할 스킬이 없을 경우 현재 슬롯의 스킬을 사용할 스킬로 설정.
	else		
		m_pGaeaClient->GetCharacter()->ReqSkillRunSet( wQUICKSKILLSLOT );
}
