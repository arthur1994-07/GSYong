#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../RanLogicClient/ClientActor.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"

#include "../../../RanLogic/Tournament/GLTournamentClientInfo.h"

#include "../PartyRenewal/MiniWindow/MiniExpeditionInfoGroup/MiniExpeditionInfoMemberSlot/MiniExpeditionInfoMemberSlot.h"
#include "TournamentExpeditionInfoPartySlot.h"


TournamentExpeditionInfoPartySlot::TournamentExpeditionInfoPartySlot(EngineDeviceMan* pEngineDevice, const DWORD dwPartySlot,GLGaeaClient* pGaeaClient)
: MiniExpeditionInfoPartySlot(pEngineDevice,dwPartySlot),m_pGaeaClient(pGaeaClient)
{

}

void TournamentExpeditionInfoPartySlot::SetMasterInformation(DWORD pPartySlot, const bool bMaster)
{
	m_pMasterCrown[0]->SetVisibleSingle(bMaster ? TRUE : FALSE);
	m_pMasterCrown[1]->SetVisibleSingle(bMaster ? FALSE : TRUE);
	SetInformation(pPartySlot);
}

void TournamentExpeditionInfoPartySlot::SetInformation(DWORD pPartySlot)
{
	
	if( m_pGaeaClient->GetCharacter()->CharDbNum() == pPartySlot)
	{
		GLCharacter* spChar = m_pGaeaClient->GetCharacter();

		if ( spChar == NULL )
		{
			SetVisibleSingle(FALSE);
			return;
		}

		const DWORD dwCharIndex = DWORD(CharClassToClassIndex(spChar->m_emClass));
		if ( dwCharIndex < GLCL_NUM_CLASS )
		{
			if ( m_pClassBox[dwCharIndex]->IsVisible() == FALSE )
			{
				for ( DWORD _i = GLCL_NUM_CLASS; _i; --_i )
					m_pClassBox[_i - 1]->SetVisibleSingle(FALSE);
				m_pClassBox[dwCharIndex]->SetVisibleSingle(TRUE);
			}
		}

		CString  strLevel;
		strLevel.Format("Lv%d", spChar->GETLEVEL());
		m_pLevel->SetText(strLevel);	

		m_szName.Format("%s", spChar->GetName());
		m_pCharacterName->SetText(m_szName);

		for ( DWORD _i = SCHOOL_NUM; _i; --_i )
			m_pSchoolMark[_i - 1]->SetVisibleSingle(FALSE);
		if ( spChar->m_wSchool < 3 )
			m_pSchoolMark[spChar->m_wSchool]->SetVisibleSingle(TRUE);

		m_dwGaeaID = spChar->GetGaeaID();

		//m_pCheckReadyOk->SetVisibleSingle(pPartySlot->isMemberState(EMPARTY_MEMBER_STATE_READY) ? TRUE : FALSE);

		m_pLevel->SetTextColor(0, D3DCOLOR(0xffffffff));
		m_pCharacterName->SetTextColor(0, D3DCOLOR(0xffffffff));
		m_pSelectBox->SetVisibleSingle(FALSE);
		SetVisibleSingle(TRUE);

	}
	else
	{
		std::tr1::shared_ptr<GLCharClient> spChar = m_pGaeaClient->GetCharByDBnum(pPartySlot);
		// 	GLCharClient* spChar;
		// 	spChar->GetGaeaID();
		if ( spChar == NULL )
		{
			SetVisibleSingle(FALSE);
			return;
		}

		const DWORD dwCharIndex = DWORD(CharClassToClassIndex(spChar->m_emClass));
		if ( dwCharIndex < GLCL_NUM_CLASS )
		{
			if ( m_pClassBox[dwCharIndex]->IsVisible() == FALSE )
			{
				for ( DWORD _i = GLCL_NUM_CLASS; _i; --_i )
					m_pClassBox[_i - 1]->SetVisibleSingle(FALSE);
				m_pClassBox[dwCharIndex]->SetVisibleSingle(TRUE);
			}
		}

		CString  strLevel;
		strLevel.Format("Lv%d", spChar->GETLEVEL());
		m_pLevel->SetText(strLevel);	

		m_szName.Format("%s", spChar->GetName());
		m_pCharacterName->SetText(m_szName);

		for ( DWORD _i = SCHOOL_NUM; _i; --_i )
			m_pSchoolMark[_i - 1]->SetVisibleSingle(FALSE);
		if ( spChar->m_wSchool < 3 )
			m_pSchoolMark[spChar->m_wSchool]->SetVisibleSingle(TRUE);

		m_dwGaeaID = spChar->GetGaeaID();

		//m_pCheckReadyOk->SetVisibleSingle(pPartySlot->isMemberState(EMPARTY_MEMBER_STATE_READY) ? TRUE : FALSE);

		m_pLevel->SetTextColor(0, D3DCOLOR(0xffffffff));
		m_pCharacterName->SetTextColor(0, D3DCOLOR(0xffffffff));
		m_pSelectBox->SetVisibleSingle(FALSE);
		SetVisibleSingle(TRUE);
	}


}

