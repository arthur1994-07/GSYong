#include "stdafx.h"
#include "./OldUI/Skill/SkillWindow.h"
#include "./OldUI/Skill/RnSkillWindow.h"
#include "./InnerInterface.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

ISkillWindow* CInnerInterface::GetSkillWindow()
{ 
    return m_pSkillWindow;
}


void CInnerInterface::SkillWindowSetVisiblePageForTutorial()
{
    m_pSkillWindow->ShowPageForTutorial();
}

void CInnerInterface::REFRESH_SKILLWINDOW()
{

    if( m_pSkillWindow != NULL )
        m_pSkillWindow->Refresh();
}

SNATIVEID CInnerInterface::SkillWindowToTrayGetSkill()
{
	if ( !m_pSkillWindowToTray )
	{
		return SNATIVEID(false);
	}

	return m_pSkillWindowToTray->GetSkill();

}

void CInnerInterface::SkillWindowToTrayResetSkill()
{
	if ( !m_pSkillWindowToTray )
	{
		return;
	}

	m_pSkillWindowToTray->ResetSkill();
}
