#include "StdAfx.h"

#include "../../../../../../EngineLib/GUInterface/UIControl.h"
#include "../../../../../../RanLogic/Party/GLPartyDefine.h"
#include "../../../../../../RanLogicClient/Party/GLPartyClient.h"
#include "../../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../../../../RanLogicClient/Char/GLCharClient.h"

#include "MemberStateSlotPortrait.h"

MemberStateSlotPortrait::MemberStateSlotPortrait(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
{
}

void MemberStateSlotPortrait::RESET(void)
{
	for ( int _i = 0; _i < GLCI_NUM_ETC; ++_i ) 
	{
		m_pClassPortraitLive[_i]->SetVisibleSingle(FALSE);	
		m_pClassPortraitUnknown[_i]->SetVisibleSingle(FALSE);
	}
	m_pClassPortraitDie->SetVisibleSingle(FALSE);
}

void MemberStateSlotPortrait::SetPortrait(const GLPartySlotClient* const pPartyClient, const bool bAvailable)
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

void MemberStateSlotPortrait::SetPortrait(GLCharacter* pChar, const bool bAvailable)
{
	RESET();
	if ( pChar->m_emClass == GLCC_NONE )
		return;

	const DWORD dwCharIndex = DWORD(CharClassToIndex(pChar->m_emClass));	

	m_pClassPortraitUnknown[dwCharIndex]->SetVisibleSingle(bAvailable == false);
	m_pClassPortraitLive[dwCharIndex]->SetVisibleSingle(bAvailable);	
	if ( bAvailable == true )
		m_pClassPortraitDie->SetVisibleSingle(pChar->m_sHP.nNow == 0);
	else
		m_pClassPortraitDie->SetVisibleSingle(FALSE);
}

void MemberStateSlotPortrait::SetPortrait(std::tr1::shared_ptr<GLCharClient> pPartyClient, const bool bAvailable)
{
	//GLCharClient* pPartyClient;

	RESET();

	if(pPartyClient == NULL)
		return;

	if ( pPartyClient->GetClass() == GLCC_NONE )
		return;

	const DWORD dwCharIndex = DWORD(CharClassToIndex(pPartyClient->GetClass()));	

	m_pClassPortraitUnknown[dwCharIndex]->SetVisibleSingle(bAvailable == false);
	m_pClassPortraitLive[dwCharIndex]->SetVisibleSingle(bAvailable);	
	if ( bAvailable == true )
		m_pClassPortraitDie->SetVisibleSingle(pPartyClient->GetHp().nNow == 0);
	else
		m_pClassPortraitDie->SetVisibleSingle(FALSE);
}

void MemberStateSlotPortrait::CreatesubControl(void)
{
	CString strClassLive[GLCI_NUM_ETC] = 
	{
		"MEMBER_STATE_BRAWLER_MALE_LIVE",
		"MEMBER_STATE_SWORDSMAN_MALE_LIVE",
		"MEMBER_STATE_ARCHER_FEMALE_LIVE",
		"MEMBER_STATE_SHAMAN_FEMALE_LIVE",
		"MEMBER_STATE_EXTREME_MALE_LIVE",
		"MEMBER_STATE_EXTREME_FEMALE_LIVE",
		"MEMBER_STATE_BRAWLER_FEMALE_LIVE",
		"MEMBER_STATE_SWORDSMAN_FEMALE_LIVE",
		"MEMBER_STATE_ARCHER_MALE_LIVE",
		"MEMBER_STATE_SHAMAN_MALE_LIVE",
		"MEMBER_STATE_SCIENTIST_MALE_LIVE",
		"MEMBER_STATE_SCIENTIST_FEMALE_LIVE",
		"MEMBER_STATE_ASSASSIN_MALE_LIVE",
		"MEMBER_STATE_ASSASSIN_FEMALE_LIVE",
		"MEMBER_STATE_TRICKER_MALE_LIVE",
		"MEMBER_STATE_TRICKER_FEMALE_LIVE",
		"MEMBER_STATE_ETC_MALE_LIVE",
		"MEMBER_STATE_ETC_FEMALE_LIVE",
	};	

	CString strClassUnknown[GLCI_NUM_ETC] =
	{
		"MEMBER_STATE_BRAWLER_MALE_DIE",
		"MEMBER_STATE_SWORDSMAN_MALE_DIE",
		"MEMBER_STATE_ARCHER_FEMALE_DIE",
		"MEMBER_STATE_SHAMAN_FEMALE_DIE",
		"MEMBER_STATE_EXTREME_MALE_DIE",
		"MEMBER_STATE_EXTREME_FEMALE_DIE",
		"MEMBER_STATE_BRAWLER_FEMALE_DIE",
		"MEMBER_STATE_SWORDSMAN_FEMALE_DIE",
		"MEMBER_STATE_ARCHER_MALE_DIE",
		"MEMBER_STATE_SHAMAN_MALE_DIE",
		"MEMBER_STATE_SCIENTIST_MALE_DIE",
		"MEMBER_STATE_SCIENTIST_FEMALE_DIE",
		"MEMBER_STATE_ASSASSIN_MALE_DIE",
		"MEMBER_STATE_ASSASSIN_FEMALE_DIE",
		"MEMBER_STATE_TRICKER_MALE_DIE",
		"MEMBER_STATE_TRICKER_FEMALE_DIE",
		"MEMBER_STATE_ETC_MALE_DIE",
		"MEMBER_STATE_ETC_FEMALE_DIE",
	};

	for ( int _i = 0; _i < GLCI_NUM_ETC; ++_i ) 
	{
		m_pClassPortraitLive[_i] = CreateControl( strClassLive[_i].GetString() );
		m_pClassPortraitLive[_i]->SetVisibleSingle(FALSE);	
		m_pClassPortraitUnknown[_i] = CreateControl( strClassUnknown[_i].GetString() );		
		m_pClassPortraitUnknown[_i]->SetVisibleSingle(FALSE);
	}
	m_pClassPortraitDie = CreateControl("MEMBER_STATE_DIE");
	m_pClassPortraitDie->SetDiffuseAlpha(160);
	m_pClassPortraitDie->SetVisibleSingle(FALSE);
}