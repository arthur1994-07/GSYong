#include "StdAfx.h"

#include "../../../../../../../RanLogicClient/ClientActor.h"
#include "../../../../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../../../../../RanLogicClient/Char/GLCharClient.h"
#include "ExpeditionInfoMemberSlot.h"

ExpeditionInfoPartySlot::ExpeditionInfoPartySlot(EngineDeviceMan* pEngineDevice, const DWORD dwPartySlot) :
CUIGroupHelper(pEngineDevice)
, m_dwGaeaID(DBNUM_NULL)
, m_dwMemberSlot(dwPartySlot)
{
	m_pMasterCrown[0] = NULL;
	m_pMasterCrown[1] = NULL;
}

void ExpeditionInfoPartySlot::CreateSubControl(void)
{
	CreateLineBox( "EXPEDITION_INFO_MEMBER_SLOT_NUM", "PARTY_INFO_LINEBOX_TEXTUREINFO" );	
	CreateLineBox( "EXPEDITION_INFO_MEMBER_INFORMATION", "PARTY_INFO_LINEBOX_TEXTUREINFO" );	

	if ( m_dwMemberSlot == MASTER_PARTY )
	{
		m_pMasterCrown[0] = CreateControl("EXPEDITION_INFO_MEMBER_SLOT_MASTER");
		m_pMasterCrown[1] = CreateControl("EXPEDITION_INFO_MEMBER_SLOT_SUB_MASTER");
	}
	else
	{
		CString strSlotNum;
		strSlotNum.Format("%d", m_dwMemberSlot);
		CBasicTextBox* pSlotNum;
		pSlotNum = CreateText(strSlotNum.GetString(), "EXPEDITION_INFO_MEMBER_SLOT_NUM_TEXT", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	}

	{ 
		// 원정대원 직업 색상 배경;
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

	{
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
	m_pSelectBox->SetVisibleSingle(FALSE);
	m_pCheckReadyOk = CreateControl("PARTY_WINDOW_RENEWAL_CHECK_READY_OK");
	m_pCheckReadyOk->SetVisibleSingle(FALSE);
}


void ExpeditionInfoPartySlot::RESET(void)
{	
	m_dwGaeaID = GAEAID_NULL;
	SetVisibleSingle(FALSE);
}

const bool ExpeditionInfoPartySlot::isValid(void) const
{
	return (m_dwGaeaID != GAEAID_NULL);
}

const DWORD ExpeditionInfoPartySlot::GetGaeaID(void) const
{
	return m_dwGaeaID;
}

const CString& ExpeditionInfoPartySlot::GetName(void) const
{
	return m_szName;
}

void ExpeditionInfoPartySlot::ResetSelect(void)
{
	m_pLevel->SetTextColor(0, D3DCOLOR(0xffffffff));
	m_pCharacterName->SetTextColor(0, D3DCOLOR(0xffffffff));	
	m_pSelectBox->SetVisibleSingle(FALSE);
}

void ExpeditionInfoPartySlot::SetSelect(void)
{
	if ( IsVisible() )
	{
		m_pLevel->SetTextColor(0, D3DCOLOR(0xffffff00));
		m_pCharacterName->SetTextColor(0, D3DCOLOR(0xffffff00));
		m_pSelectBox->SetVisibleSingle(TRUE);		
	}
}

void ExpeditionInfoPartySlot::SetBlank(const bool bOn)
{
	m_pSelectBox->SetVisibleSingle(bOn ? TRUE : FALSE);
}

void ExpeditionInfoPartySlot::SetMasterInformation(const GLPartySlotClient* const pPartySlot, const bool bMaster)
{
	m_pMasterCrown[0]->SetVisibleSingle(bMaster ? TRUE : FALSE);
	m_pMasterCrown[1]->SetVisibleSingle(bMaster ? FALSE : TRUE);
	SetInformation(pPartySlot);
}
void ExpeditionInfoPartySlot::SetInformation(const GLPartySlotClient* const pPartySlot)
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

	{
		for ( DWORD _i = SCHOOL_NUM; _i; --_i )
			m_pSchoolMark[_i - 1]->SetVisibleSingle(FALSE);
		if ( pPartySlot->m_wSchool < 3 )
			m_pSchoolMark[pPartySlot->m_wSchool]->SetVisibleSingle(TRUE);
	}

	m_dwGaeaID = pPartySlot->m_dwGaeaID;

	m_pCheckReadyOk->SetVisibleSingle(pPartySlot->isMemberState(EMPARTY_MEMBER_STATE_READY) ? TRUE : FALSE);

	SetVisibleSingle(TRUE);
}

void ExpeditionInfoPartySlot::SetInformation(std::tr1::shared_ptr<GLCharClient> pPartySlot)
{
	//GLCharClient* pPartySlot;
	if ( pPartySlot == NULL )
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
	strLevel.Format("Lv%d", pPartySlot->m_wLevel);
	m_pLevel->SetText(strLevel);	

	m_szName.Format("%s", pPartySlot->m_szName);
	m_pCharacterName->SetText(m_szName);

	{
		for ( DWORD _i = SCHOOL_NUM; _i; --_i )
			m_pSchoolMark[_i - 1]->SetVisibleSingle(FALSE);
		if ( pPartySlot->m_wSchool < 3 )
			m_pSchoolMark[pPartySlot->m_wSchool]->SetVisibleSingle(TRUE);
	}

	m_dwGaeaID = pPartySlot->GetGaeaID();

	//m_pCheckReadyOk->SetVisibleSingle(pPartySlot->isMemberState(EMPARTY_MEMBER_STATE_READY) ? TRUE : FALSE);

	SetVisibleSingle(TRUE);
}


void ExpeditionInfoPartySlot::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}
