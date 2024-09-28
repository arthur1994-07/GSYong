#include "StdAfx.h"

#include "../../../../../../RanLogic/Party/GLPartyDefine.h"
#include "../../../../../../RanLogicClient/Party/GLPartyClient.h"
#include "../../../../../../EngineLib/GUInterface/UIControl.h"
#include "PartyInfoSlotPortrait.h"


PartyInfoSlotPortrait::PartyInfoSlotPortrait(EngineDeviceMan* pEngineDevice) :
CUIGroupHelper(pEngineDevice)
{
}

void PartyInfoSlotPortrait::RESET(void)
{
	for ( int _i = 0; _i < GLCI_NUM_ETC; ++_i ) 
	{
		m_pClassPortraitLive[_i]->SetVisibleSingle(FALSE);	
		m_pClassPortraitUnknown[_i]->SetVisibleSingle(FALSE);
	}
	m_pClassPortraitDie->SetVisibleSingle(FALSE);
}

void PartyInfoSlotPortrait::SetPortrait(const GLPartySlotClient* const pPartyClient, const bool bAvailable)
{
	RESET();
	if ( pPartyClient->m_emClass == GLCC_NONE )
		return;

	const DWORD dwCharIndex = DWORD(CharClassToIndex(pPartyClient->m_emClass));	

	m_pClassPortraitUnknown[dwCharIndex]->SetVisibleSingle(bAvailable == false);
	m_pClassPortraitLive[dwCharIndex]->SetVisibleSingle(bAvailable);	
	if ( bAvailable == true )
		m_pClassPortraitDie->SetVisibleSingle(pPartyClient->m_sHP.nNow == 0);
	else
		m_pClassPortraitDie->SetVisibleSingle(FALSE);
}

void PartyInfoSlotPortrait::CreateSubControl(void)
{
	CString strClassLive[GLCI_NUM_ETC] = 
	{
		"PARTY_INFO_BRAWLER_MALE_LIVE",
		"PARTY_INFO_SWORDSMAN_MALE_LIVE",
		"PARTY_INFO_ARCHER_FEMALE_LIVE",
		"PARTY_INFO_SHAMAN_FEMALE_LIVE",
		"PARTY_INFO_EXTREME_MALE_LIVE",
		"PARTY_INFO_EXTREME_FEMALE_LIVE",
		"PARTY_INFO_BRAWLER_FEMALE_LIVE",
		"PARTY_INFO_SWORDSMAN_FEMALE_LIVE",
		"PARTY_INFO_ARCHER_MALE_LIVE",
		"PARTY_INFO_SHAMAN_MALE_LIVE",
		"PARTY_INFO_SCIENTIST_MALE_LIVE",
		"PARTY_INFO_SCIENTIST_FEMALE_LIVE",
		"PARTY_INFO_ASSASSIN_MALE_LIVE",
		"PARTY_INFO_ASSASSIN_FEMALE_LIVE",
		"PARTY_INFO_TRICKER_MALE_LIVE",
		"PARTY_INFO_TRICKER_FEMALE_LIVE",
		"PARTY_INFO_ETC_MALE_LIVE",
		"PARTY_INFO_ETC_FEMALE_LIVE",
	};

	CString strClassUnknown[GLCI_NUM_ETC] =
	{
		"PARTY_INFO_BRAWLER_MALE_DIE",
		"PARTY_INFO_SWORDSMAN_MALE_DIE",
		"PARTY_INFO_ARCHER_FEMALE_DIE",
		"PARTY_INFO_SHAMAN_FEMALE_DIE",
		"PARTY_INFO_EXTREME_MALE_DIE",
		"PARTY_INFO_EXTREME_FEMALE_DIE",
		"PARTY_INFO_BRAWLER_FEMALE_DIE",
		"PARTY_INFO_SWORDSMAN_FEMALE_DIE",
		"PARTY_INFO_ARCHER_MALE_DIE",
		"PARTY_INFO_SHAMAN_MALE_DIE",
		"PARTY_INFO_SCIENTIST_MALE_DIE",
		"PARTY_INFO_SCIENTIST_FEMALE_DIE",
		"PARTY_INFO_ASSASSIN_MALE_DIE",
		"PARTY_INFO_ASSASSIN_FEMALE_DIE",
		"PARTY_INFO_TRICKER_MALE_DIE",
		"PARTY_INFO_TRICKER_FEMALE_DIE",
		"PARTY_INFO_ETC_MALE_DIE",
		"PARTY_INFO_ETC_FEMALE_DIE",
	};

	for ( int _i = 0; _i < GLCI_NUM_ETC; ++_i ) 
	{
		m_pClassPortraitLive[_i] = CreateControl( strClassLive[_i].GetString() );
		m_pClassPortraitLive[_i]->SetVisibleSingle(FALSE);		
		m_pClassPortraitUnknown[_i] = CreateControl( strClassUnknown[_i].GetString() );		
		m_pClassPortraitUnknown[_i]->SetVisibleSingle(FALSE);
	}	

	m_pClassPortraitDie = CreateControl("PARTY_INFO_DIE");
	m_pClassPortraitDie->SetDiffuseAlpha(160);
	m_pClassPortraitDie->SetVisibleSingle(FALSE);
}