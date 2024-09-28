#include "stdafx.h"
#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogicClient/Char/GLCharacter.h"

#include "./OldUI/Quest/QuestWindow.h"
#include "./OldUI/Quest/QuestHelper.h"
#include "./OldUI/Quest/WindowRenewal/QuestWindowRenewal.h"

#include "./InnerInterface.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

void CInnerInterface::SetQuestWindowOpen( DWORD dwQuestID )
{
    if ( NATIVEID_NULL().dwID != m_dwEventQuestID )
    {
        m_pGaeaClient->GetCharacter()->ReqQuestREADINGReset( m_dwEventQuestID );
        m_dwEventQuestID = NATIVEID_NULL().dwID;
    }

    UiShowGroupFocus ( QUEST_WINDOW );

	if ( m_pQuestWindowRenewal )
		m_pQuestWindowRenewal->AutoSelectPage( dwQuestID );
}

const UIRECT& CInnerInterface::GetQuestWindowGlobalPos() const
{
	static UIRECT sTempRect;

	if ( m_pQuestWindowRenewal )
		return m_pQuestWindowRenewal->GetUIWindowGlobalPos();

	return sTempRect;
}

const UIRECT& CInnerInterface::GetQuestWindowRenewalGlobalPos() const
{
	static UIRECT sTempRect;

	if ( m_pQuestWindowRenewal )
		return m_pQuestWindowRenewal->GetUIWindowGlobalPos();

	return sTempRect;
}

void CInnerInterface::SetQuestWindowGlobalPos(const D3DXVECTOR2& vPos)
{
	if ( m_pQuestWindowRenewal )
		m_pQuestWindowRenewal->SetUIWindowGlobalPos( vPos );
}

void CInnerInterface::SetQuestWindowRenewalGlobalPos(const D3DXVECTOR2& vPos)
{
	if ( m_pQuestWindowRenewal )
		m_pQuestWindowRenewal->SetUIWindowGlobalPos(vPos);
}

void CInnerInterface::SetQuestWindowShowPage(int nPage, bool bMakeMsg)
{
	if ( m_pQuestWindowRenewal )
		m_pQuestWindowRenewal->SetShowPage();
}

void CInnerInterface::SetQuestWindowAutoSelectPage(DWORD dwQuestID)
{
	if ( m_pQuestWindowRenewal )
		m_pQuestWindowRenewal->AutoSelectPage( dwQuestID );
}
