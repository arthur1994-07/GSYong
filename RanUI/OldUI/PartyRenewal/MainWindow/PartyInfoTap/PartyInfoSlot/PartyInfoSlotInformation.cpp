#include "StdAfx.h"

#include "../../../../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../../../EngineLib/DxTools/DxClubMan.h"
#include "../../../../../../RanLogicClient/Party/GLPartyClient.h"
#include "../../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../../../RanLogicClient/ClientActor.h"
#include "../../../../../InnerInterface.h"
#include "PartyInfoSlotInformation.h"

PartyInfoSlotInformation::PartyInfoSlotInformation(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_dwGaeaID(GAEAID_NULL)
, m_dwClubDbNum(CLUB_NULL)
{
}

void PartyInfoSlotInformation::CreateSubControl(void)
{
	CString strSchool[SCHOOL_NUM] = {
		"PARTY_INFO_SCHOOL_SM",
		"PARTY_INFO_SCHOOL_HA",
		"PARTY_INFO_SCHOOL_BH",
	};
	for ( DWORD _i = SCHOOL_NUM; _i; --_i )
		m_pSchoolMark[_i - 1] = CreateControl(strSchool[_i - 1].GetString() );

	m_pLevel = CreateText("LEVEL", "PARTY_INFO_SLOT_LEVEL", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pCharacterName = CreateText("NAME", "PARTY_INFO_SLOT_NAME", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pLoacation = CreateText("LOCATE", "PARTY_INFO_SLOT_LOCATE", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pPosition = CreateText("POSITION", "PARTY_INFO_SLOT_POSITION", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
}

void PartyInfoSlotInformation::RESET(void)
{
	SetColor(0xffffffff);
	m_dwGaeaID = GAEAID_NULL;
}

const DWORD PartyInfoSlotInformation::GetGaeaID(void) const
{
	return m_dwGaeaID;
}
const CString& PartyInfoSlotInformation::GetName(void) const
{
	return m_szName;
}

void PartyInfoSlotInformation::SetInformation(const GLPartySlotClient* const pPartyClient, const bool bAvailable)
{
	const DWORD dwMemberGaeaID = pPartyClient->m_dwGaeaID;
	if ( m_dwGaeaID != dwMemberGaeaID )
	{
		m_szName.Format("%s", pPartyClient->m_szName);
		m_pCharacterName->SetText(m_szName);	

		for ( DWORD _i = SCHOOL_NUM; _i; --_i )
			m_pSchoolMark[_i - 1]->SetVisibleSingle(FALSE);
		if ( pPartyClient->m_wSchool < 3 )
			m_pSchoolMark[pPartyClient->m_wSchool]->SetVisibleSingle(TRUE);
	}

	CString  strLocation;
	const SNATIVEID& mapID(pPartyClient->m_sMapID.getBaseMapID());
	strLocation.Format("%s", m_pGaeaClient->GetMapName(mapID));
	m_pLoacation->SetText(strLocation);	

	CString  strLevel;
	CString  strPosition;
	D3DCOLOR StringColor;
	if ( bAvailable == true )
	{
		int nPosition[2] = {0, 0};
		const D3DXVECTOR3& vPosition = pPartyClient->m_vPosition;
		m_pGaeaClient->ConvertWorldToMap(mapID, vPosition, nPosition[0], nPosition[1]);
		strPosition.Format("%d/%d", nPosition[0], nPosition[1]);
		strLevel.Format("Lv %d", pPartyClient->m_Level);
		StringColor = (pPartyClient->m_sHP.nNow == 0 ? 0xffff0000 : 0xffffffff);
	}
	else
	{
		strPosition.Format("");
		strLevel.Format("");
		StringColor = 0xffc0c0c0;
	}

	m_pPosition->SetText(strPosition);
	m_pLevel->SetText(strLevel);			

	m_dwGaeaID = pPartyClient->m_dwGaeaID;	
	
	SetColor(StringColor);
}

void PartyInfoSlotInformation::SetColor(const DWORD dwColor)
{
	m_pLevel->SetTextColor(0, D3DCOLOR(dwColor));
	m_pCharacterName->SetTextColor(0, D3DCOLOR(dwColor));
	m_pLoacation->SetTextColor(0, D3DCOLOR(dwColor));
	m_pPosition->SetTextColor(0, D3DCOLOR(dwColor));
}

