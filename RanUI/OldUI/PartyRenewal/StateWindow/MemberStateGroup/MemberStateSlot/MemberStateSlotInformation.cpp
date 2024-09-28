#include "StdAfx.h"

#include "../../../../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../../../EngineLib/DxTools/DxClubMan.h"
#include "../../../../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../../../../RanLogicClient/ClientActor.h"
#include "../../../../../../RanlogicClient/GLGaeaClient.h"
#include "../../../../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../../../../RanLogicClient/Char/GLCharClient.h"

#include "MemberStateSlotInformation.h"

MemberStateSlotInformation::MemberStateSlotInformation(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
{
}

void MemberStateSlotInformation::SetInformation(const GLPartySlotClient* const pPartyClient, const bool bAvailable)
{
	CString  strName;
	strName.Format("%s", pPartyClient->m_szName);
	m_pCharacterName->SetText(strName);	

	D3DCOLOR StringColor;
	const DWORD dwCharIndex = DWORD(CharClassToClassIndex(pPartyClient->m_emClass));
	if ( dwCharIndex < GLCL_NUM_CLASS )
	{
		if ( m_pHPProgressBar[dwCharIndex]->IsVisible() == FALSE )
		{
			for ( DWORD _i = GLCL_NUM_CLASS; _i; --_i )
				m_pHPProgressBar[_i - 1]->SetVisibleSingle(FALSE);
			m_pHPProgressBar[dwCharIndex]->SetVisibleSingle(TRUE);
		}
		
		CString  strHP;	
		if ( bAvailable == true )
		{
			ClientActor* const pActor = m_pGaeaClient->GetCopyActor(CROW_PC, pPartyClient->m_dwGaeaID);
			const int nNow(pPartyClient->m_sHP.nNow);
			const int nMax(pPartyClient->m_sHP.nMax);

			m_pHPProgressBar[dwCharIndex]->SetPercent(float(nNow) / float(nMax));
			strHP.Format("%d/%d", nNow, nMax);
			StringColor = (nNow == 0 ? 0xffff0000 : 0xffffffff);
		}
		else
		{
			m_pHPProgressBar[dwCharIndex]->SetPercent(1.0f);
			strHP.Format("");
			StringColor = 0xffc0c0c0;
		}			

		m_pHPText->SetText(strHP);			
	}
	m_pHPText->SetTextColor(0, StringColor);
	m_pCharacterName->SetTextColor(0, StringColor);	


	for ( DWORD _i = SCHOOL_NUM; _i; --_i )
		m_pSchoolMark[_i - 1]->SetVisibleSingle(FALSE);
	if ( pPartyClient->m_wSchool < SCHOOL_NUM )
		m_pSchoolMark[pPartyClient->m_wSchool]->SetVisibleSingle(TRUE);

	SetVisibleSingle(TRUE);
}

void MemberStateSlotInformation::SetInformation(std::tr1::shared_ptr<GLCharClient> pPartyClient, const bool bAvailable)
{
	if(pPartyClient == NULL)
		return;

	//GLCharClient* pPartyClient;
	CString  strName;
	strName.Format("%s", pPartyClient->GetName());
	m_pCharacterName->SetText(strName);	

	D3DCOLOR StringColor;
	const DWORD dwCharIndex = DWORD(CharClassToClassIndex(pPartyClient->m_emClass));
	if ( dwCharIndex < GLCL_NUM_CLASS )
	{
		if ( m_pHPProgressBar[dwCharIndex]->IsVisible() == FALSE )
		{
			for ( DWORD _i = GLCL_NUM_CLASS; _i; --_i )
				m_pHPProgressBar[_i - 1]->SetVisibleSingle(FALSE);
			m_pHPProgressBar[dwCharIndex]->SetVisibleSingle(TRUE);
		}

		CString  strHP;	
		if ( bAvailable == true )
		{
			//ClientActor* const pActor = m_pGaeaClient->GetCopyActor(CROW_PC, pPartyClient->GetGaeaID());
			const int nNow(pPartyClient->GetHp().nNow);
			const int nMax(pPartyClient->GetHp().nMax);

			m_pHPProgressBar[dwCharIndex]->SetPercent(float(nNow) / float(nMax));
			strHP.Format("%d/%d", nNow, nMax);
			StringColor = (nNow == 0 ? 0xffff0000 : 0xffffffff);
		}
		else
		{
			m_pHPProgressBar[dwCharIndex]->SetPercent(1.0f);
			strHP.Format("");
			StringColor = 0xffc0c0c0;
		}			

		m_pHPText->SetText(strHP);			
	}
	m_pHPText->SetTextColor(0, StringColor);
	m_pCharacterName->SetTextColor(0, StringColor);	


	for ( DWORD _i = SCHOOL_NUM; _i; --_i )
		m_pSchoolMark[_i - 1]->SetVisibleSingle(FALSE);
	if ( pPartyClient->GetCharSchool() < SCHOOL_NUM )
		m_pSchoolMark[pPartyClient->GetCharSchool()]->SetVisibleSingle(TRUE);

	SetVisibleSingle(TRUE);
}

void MemberStateSlotInformation::SetInformation(GLCharacter* pChar, const bool bAvailable)
{
	if( pChar == NULL )
		return;

	//GLCharClient* pPartyClient;
	CString  strName;
	strName.Format("%s", pChar->GetName());
	m_pCharacterName->SetText(strName);	

	D3DCOLOR StringColor;
	const DWORD dwCharIndex = DWORD(CharClassToClassIndex(pChar->m_emClass));
	if ( dwCharIndex < GLCL_NUM_CLASS )
	{
		if ( m_pHPProgressBar[dwCharIndex]->IsVisible() == FALSE )
		{
			for ( DWORD _i = GLCL_NUM_CLASS; _i; --_i )
				m_pHPProgressBar[_i - 1]->SetVisibleSingle(FALSE);
			m_pHPProgressBar[dwCharIndex]->SetVisibleSingle(TRUE);
		}

		CString  strHP;	
		if ( bAvailable == true )
		{
			//ClientActor* const pActor = m_pGaeaClient->GetCopyActor(CROW_PC, pPartyClient->GetGaeaID());
			const int nNow(pChar->GetHp().nNow);
			const int nMax(pChar->GetHp().nMax);

			m_pHPProgressBar[dwCharIndex]->SetPercent(float(nNow) / float(nMax));
			strHP.Format("%d/%d", nNow, nMax);
			StringColor = (nNow == 0 ? 0xffff0000 : 0xffffffff);
		}
		else
		{
			m_pHPProgressBar[dwCharIndex]->SetPercent(1.0f);
			strHP.Format("");
			StringColor = 0xffc0c0c0;
		}			

		m_pHPText->SetText(strHP);			
	}
	m_pHPText->SetTextColor(0, StringColor);
	m_pCharacterName->SetTextColor(0, StringColor);	


	for ( DWORD _i = SCHOOL_NUM; _i; --_i )
		m_pSchoolMark[_i - 1]->SetVisibleSingle(FALSE);
	if ( pChar->GetCharSchool() < SCHOOL_NUM )
		m_pSchoolMark[pChar->GetCharSchool()]->SetVisibleSingle(TRUE);

	SetVisibleSingle(TRUE);
}

void MemberStateSlotInformation::CreateSubControl(void)
{		
	CreateLineBox("MEMBER_STATE_SLOT_INFORMATION_NAME_BOX", "PARTY_INFO_LINEBOX_TEXTUREINFO");	
	CreateLineBox("MEMBER_STATE_SLOT_INFORMATION_HP_BOX", "PARTY_INFO_LINEBOX_TEXTUREINFO");	

	{
		CString strSchool[SCHOOL_NUM] = {
			"MEMBER_STATE_SCHOOL_SM",
			"MEMBER_STATE_SCHOOL_HA",
			"MEMBER_STATE_SCHOOL_BH",
		};		
		for ( DWORD _i = SCHOOL_NUM; _i; --_i )
			m_pSchoolMark[_i - 1] = CreateControl(strSchool[_i - 1].GetString() );
	}

	m_pCharacterName = CreateText("NAME", "MEMBER_STATE_SLOT_INFORMATION_NAME", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	
	{ // 체력 막대;
		CString strClass[GLCL_NUM_CLASS] = 
		{
			"PARTY_STATE_HP_OVERIMAGE_FIGHTER",
			"PARTY_STATE_HP_OVERIMAGE_ARMS",
			"PARTY_STATE_HP_OVERIMAGE_ARCHER",
			"PARTY_STATE_HP_OVERIMAGE_SPIRIT",
			"PARTY_STATE_HP_OVERIMAGE_EXTREME",
			"PARTY_STATE_HP_OVERIMAGE_SCIENTIST",
			"PARTY_STATE_HP_OVERIMAGE_ASSASSIN",
			"PARTY_STATE_HP_OVERIMAGE_TRICKER",
			"PARTY_STATE_HP_OVERIMAGE_ETC",
		};
		
		for ( DWORD _i = GLCL_NUM_CLASS; _i; --_i )
		{
			m_pHPProgressBar[_i - 1] = new CBasicProgressBar( m_pEngineDevice );
			m_pHPProgressBar[_i - 1]->CreateSub( this, "PARTY_STATE_HP_IMAGE");
			m_pHPProgressBar[_i - 1]->CreateOverImage(strClass[_i - 1].GetString());		
			m_pHPProgressBar[_i - 1]->SetDiffuseAlpha(185);
			RegisterControl(m_pHPProgressBar[_i - 1]);
			m_pHPProgressBar[_i - 1]->SetVisibleSingle(FALSE);
		}		
	}

	m_pHPText = CreateText("POSITION", "MEMBER_STATE_SLOT_INFORMATION_HP", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);	
}

