#include "StdAfx.h"

#include "../../../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../../../../RanLogicClient/ClientActor.h"
#include "MiniExpeditionInfoMemberSlot.h"

MiniExpeditionInfoPartySlot::MiniExpeditionInfoPartySlot(EngineDeviceMan* pEngineDevice, const DWORD dwPartySlot)
: CUIGroupHelper(pEngineDevice)
, m_dwGaeaID(DBNUM_NULL)
, m_pLevel(NULL)
, m_pCharacterName(NULL)
, m_pSelectBox(NULL)
, m_pCheckReadyOk(NULL)
, m_dwPartySlot(dwPartySlot)
{
	m_pMasterCrown[0] = NULL;
	m_pMasterCrown[1] = NULL;
}

void MiniExpeditionInfoPartySlot::CreateSubControl(void)
{
	CreateLineBox( "EXPEDITION_INFO_MEMBER_SLOT_NUM", "PARTY_INFO_LINEBOX_TEXTUREINFO" );	
	CreateLineBox( "EXPEDITION_INFO_MEMBER_INFORMATION", "PARTY_INFO_LINEBOX_TEXTUREINFO" );	

	if ( m_dwPartySlot == MASTER_PARTY )
	{
		m_pMasterCrown[0] = CreateControl("EXPEDITION_INFO_MEMBER_SLOT_MASTER");
		m_pMasterCrown[1] = CreateControl("EXPEDITION_INFO_MEMBER_SLOT_SUB_MASTER");
	}
	else
	{
		CString strSlotNum;
		strSlotNum.Format("%d", m_dwPartySlot);
		CBasicTextBox* pSlotNum;
		pSlotNum = CreateText(strSlotNum.GetString(), "EXPEDITION_INFO_MEMBER_SLOT_NUM_TEXT", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	}

	{ // 원정대원 직업 색상 배경;
		CString strClass[GLCL_NUM_CLASS] = 
		{
			"EXPEDITION_CLASS_FIGHTER",
			"EXPEDITION_CLASS_ARMS",
			"EXPEDITION_CLASS_ARCHER",
			"EXPEDITION_CLASS_SPIRIT",
			"EXPEDITION_CLASS_EXTREME",
			"EXPEDITION_CLASS_SCIENTIST",
			"EXPEDITION_CLASS_ASSASSIN",
			"EXPEDITION_CLASS_TRICKER",
			"EXPEDITION_CLASS_ETC",
		};

		for ( DWORD _i = GLCL_NUM_CLASS; _i; --_i )
		{
			m_pClassBox[_i - 1] = CreateControl(strClass[_i - 1].GetString());
			m_pClassBox[_i - 1]->SetVisibleSingle(FALSE);
		}
	}

	{ // 학원 마크;
		CString strSchool[SCHOOL_NUM] = {
			"EXPEDITION_INFO_SCHOOL_SM",
			"EXPEDITION_INFO_SCHOOL_HA",
			"EXPEDITION_INFO_SCHOOL_BH",
		};
		for ( DWORD _i = SCHOOL_NUM; _i; --_i )
			m_pSchoolMark[_i - 1] = CreateControl(strSchool[_i - 1].GetString() );	
	}

	m_pLevel = CreateText("LEVEL", "EXPEDITION_INFO_MEMBER_LEVEL", TEXT_ALIGN_LEFT|TEXT_ALIGN_CENTER_Y);
	m_pCharacterName = CreateText("NAME", "EXPEDITION_INFO_MEMBER_NAME", TEXT_ALIGN_LEFT|TEXT_ALIGN_CENTER_Y);	
	m_pSelectBox = CreateLineBox("EXPEDITION_INFO_SLOT_SELECT", "PARTY_INFO_SELECTBOX_TEXTUREINFO");
	m_pCheckReadyOk = CreateControl("PARTY_WINDOW_RENEWAL_CHECK_READY_OK");
}


void MiniExpeditionInfoPartySlot::RESET(void)
{
	m_pLevel->SetTextColor(0, D3DCOLOR(0xffffffff));
	m_pCharacterName->SetTextColor(0, D3DCOLOR(0xffffffff));
	m_pSelectBox->SetVisibleSingle(FALSE);
	m_dwGaeaID = GAEAID_NULL;
	SetVisibleSingle(FALSE);
}

const bool MiniExpeditionInfoPartySlot::isValid(void) const
{
	return (m_dwGaeaID != GAEAID_NULL);
}

const DWORD MiniExpeditionInfoPartySlot::GetGaeaID(void) const
{
	return m_dwGaeaID;
}

const CString& MiniExpeditionInfoPartySlot::GetName(void) const
{
	return m_szName;
}

void MiniExpeditionInfoPartySlot::SetSelect(void)
{
	if ( IsVisible() )
	{
		m_pLevel->SetTextColor(0, D3DCOLOR(0xffffff00));
		m_pCharacterName->SetTextColor(0, D3DCOLOR(0xffffff00));
		m_pSelectBox->SetVisibleSingle(TRUE);
	}
}

void MiniExpeditionInfoPartySlot::SetMasterInformation(const GLPartySlotClient* const pPartySlot, const bool bMaster)
{
	m_pMasterCrown[0]->SetVisibleSingle(bMaster ? TRUE : FALSE);
	m_pMasterCrown[1]->SetVisibleSingle(bMaster ? FALSE : TRUE);
	SetInformation(pPartySlot);
}

void MiniExpeditionInfoPartySlot::SetInformation(const GLPartySlotClient* const pPartySlot)
{
	if ( pPartySlot->isValid() == false )
	{
		SetVisibleSingle(FALSE);
		return;
	}

	const DWORD dwCharIndex = DWORD(CharClassToClassIndex(pPartySlot->m_emClass));
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
	strLevel.Format("Lv%d", pPartySlot->m_Level);
	m_pLevel->SetText(strLevel);	

	m_szName.Format("%s", pPartySlot->m_szName);
	m_pCharacterName->SetText(m_szName);

	for ( DWORD _i = SCHOOL_NUM; _i; --_i )
		m_pSchoolMark[_i - 1]->SetVisibleSingle(FALSE);
	if ( pPartySlot->m_wSchool < 3 )
		m_pSchoolMark[pPartySlot->m_wSchool]->SetVisibleSingle(TRUE);

	m_dwGaeaID = pPartySlot->m_dwGaeaID;

	m_pCheckReadyOk->SetVisibleSingle(pPartySlot->isMemberState(EMPARTY_MEMBER_STATE_READY) ? TRUE : FALSE);

	m_pLevel->SetTextColor(0, D3DCOLOR(0xffffffff));
	m_pCharacterName->SetTextColor(0, D3DCOLOR(0xffffffff));
	m_pSelectBox->SetVisibleSingle(FALSE);
	SetVisibleSingle(TRUE);
}

