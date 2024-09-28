#include "StdAfx.h"
#include "PetSkillWindow.h"
#include "PetSkillPage.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../Skill/SkillSlot.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPetSkillWindow::CPetSkillWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIWindowEx ( pInterface, pEngineDevice )
	, m_pGaeaClient ( pGaeaClient )
	, m_pPage ( NULL )
{
}

CPetSkillWindow::~CPetSkillWindow ()
{
}

void CPetSkillWindow::CreateSubControl ()
{
	SNATIVEID sNativeIDArray[GLSkillMan::MAX_CLASSSKILL];

	for ( int i = 0; i < GLSkillMan::MAX_CLASSSKILL; i++ )
	{
		sNativeIDArray[i] = NATIVEID_NULL();
	}

	int nSkillCount(0);

	for ( int j = 0; j < GLSkillMan::MAX_CLASSSKILL; j++ )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( EMSKILL_PET, j );
		if ( pSkill )
		{
			sNativeIDArray[nSkillCount++] = SNATIVEID(EMSKILL_PET,j);
		}
	}

	CPetSkillPage* pSkillPage = new CPetSkillPage ( m_pGaeaClient, m_pEngineDevice );
	pSkillPage->CreateSub ( this, "PET_SKILL_PAGE", UI_FLAG_DEFAULT, PET_SKILL_WINDOW_PAGE );
	pSkillPage->CreateSubControl ( sNativeIDArray, nSkillCount );
	RegisterControl ( pSkillPage );
	m_pPage = pSkillPage;
}

void CPetSkillWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case PET_SKILL_WINDOW_PAGE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nSkillIndex = m_pPage->GetSkillIndex ();
				if ( nSkillIndex < 0 ) return;

				SNATIVEID sNativeID = m_pPage->GetSkillID ( nSkillIndex );

				if ( sNativeID != NATIVEID_NULL() )
				{
					m_pInterface->SHOW_SKILL_INFO ( sNativeID, FALSE );
				}
			}
		}
		break;
	}
}

void CPetSkillWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle(bVisible);
	m_pPage->ReloadSkillInfo();
}