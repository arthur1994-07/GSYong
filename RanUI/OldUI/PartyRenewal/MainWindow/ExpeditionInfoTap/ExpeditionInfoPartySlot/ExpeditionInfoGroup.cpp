#include "StdAfx.h"

#include "../../../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../../../RanLogicClient//Party/GLPartyClient.h"
#include "../../../../../../RanLogicClient/GLGaeaClient.h"
#include "./ExpeditionInfoMemberSlot/ExpeditionInfoMemberSlot.h"
#include "ExpeditionInfoGroup.h"

ExpeditionInfoGroup::ExpeditionInfoGroup(EngineDeviceMan* pEngineDevice) :
CUIGroupHelper(pEngineDevice)
, m_dwSelectedSlot(DEFSLOT_NULL)
{
}

void ExpeditionInfoGroup::RESET(void)
{
	for ( DWORD _i = MAXPARTY; _i; --_i )
		m_pExpeditionInfoMemberSlot[_i - 1]->RESET();
}

void ExpeditionInfoGroup::ResetSelect(void)
{
	for ( DWORD _i = MAXPARTY; _i; --_i )
		m_pExpeditionInfoMemberSlot[_i - 1]->ResetSelect();
	m_dwSelectedSlot = DEFSLOT_NULL;
}

void ExpeditionInfoGroup::CreateSubControl(void)
{
	CreateLineBox( "EXPEDITION_PARTY_GROUP", "PARTY_INFO_LINEBOX_TEXTUREINFO" );

	{
		CString strRaidInfoPartySlot[MAXPARTY] = 
		{
			"EXPEDITION_INFO_PARTY_SLOT0",
			"EXPEDITION_INFO_PARTY_SLOT1",
			"EXPEDITION_INFO_PARTY_SLOT2",
			"EXPEDITION_INFO_PARTY_SLOT3",
			"EXPEDITION_INFO_PARTY_SLOT4",
			"EXPEDITION_INFO_PARTY_SLOT5",
			"EXPEDITION_INFO_PARTY_SLOT6",
			"EXPEDITION_INFO_PARTY_SLOT7",
		};

		for ( unsigned int _i = 0; _i < MAXPARTY; _i++ )
		{
			m_pExpeditionInfoMemberSlot[_i] = new ExpeditionInfoPartySlot(m_pEngineDevice, _i);
			m_pExpeditionInfoMemberSlot[_i]->CreateSub( this, strRaidInfoPartySlot[_i].GetString (), UI_FLAG_DEFAULT, EXPEDITION_INFO_PARTY_SLOT0 + _i );
			m_pExpeditionInfoMemberSlot[_i]->CreateSubControl();
			RegisterControl(m_pExpeditionInfoMemberSlot[_i]);
			m_pExpeditionInfoMemberSlot[_i]->SetVisibleSingle(FALSE);
			m_pExpeditionInfoMemberSlot[_i]->RESET();
		}
	}	
}

const DWORD ExpeditionInfoGroup::GetSelectedGaeaID(void) const
{
	if ( m_dwSelectedSlot == DEFSLOT_NULL )
		return GAEAID_NULL;
	else
		return m_pExpeditionInfoMemberSlot[m_dwSelectedSlot]->GetGaeaID();
}
const DWORD ExpeditionInfoGroup::GetSelectedGaeaID(const DWORD dwSelectedSlot) const
{
	if ( dwSelectedSlot >= MAXPARTY )
		return GAEAID_NULL;

	return m_pExpeditionInfoMemberSlot[dwSelectedSlot]->GetGaeaID();
}

const DWORD ExpeditionInfoGroup::GetSelectedSlot(void) const
{
	return m_dwSelectedSlot;
}

const CString* ExpeditionInfoGroup::GetName(const DWORD dwSelectedSlot) const
{
	if ( dwSelectedSlot >= MAXPARTY )
		return NULL;

	return &m_pExpeditionInfoMemberSlot[dwSelectedSlot]->GetName();
}

void ExpeditionInfoGroup::SetExpeditionSlot(GLPartyClient* const pParty, const bool bSwapSlotMode /* = false */, const bool bOn /* = false */)
{
	for ( DWORD _i = MAXPARTY; _i; --_i )
		m_pExpeditionInfoMemberSlot[_i - 1]->SetVisibleSingle(FALSE);	
	
	if ( pParty == NULL )
		return;

	const bool bMaster = pParty->GetPartyID().isValidExpedition() ? (pParty->GetPartyIndex() == MASTER_PARTY) : true;
	const GLPartySlotClient* const pPartySlot = pParty->GetMember(MASTER_PARTY);
	if ( pPartySlot != NULL )
		m_pExpeditionInfoMemberSlot[0]->SetMasterInformation(pPartySlot, bMaster);

	for ( DWORD _i = 1; _i < MAXPARTY; ++_i )
	{
		const GLPartySlotClient* const pPartySlot = pParty->GetMember(_i);
		if ( pPartySlot == NULL )
		{
			m_pExpeditionInfoMemberSlot[_i]->RESET();
			continue;
		}
		m_pExpeditionInfoMemberSlot[_i]->SetInformation(pPartySlot);
	}

	if ( m_dwSelectedSlot != DEFSLOT_NULL )
		m_pExpeditionInfoMemberSlot[m_dwSelectedSlot]->SetSelect();

	if ( bSwapSlotMode )
	{
		for ( DWORD _i = MAXPARTY; _i; --_i )
		{
			if ( m_pExpeditionInfoMemberSlot[_i - 1]->isValid() == false )
				continue;

			m_pExpeditionInfoMemberSlot[_i - 1]->SetBlank(bOn);
		}
	}

	SetVisibleSingle(TRUE);
}

void ExpeditionInfoGroup::TranslateUIMessage_SelectPartySlot(const DWORD dwSlotIndex)
{
	if ( m_dwSelectedSlot != dwSlotIndex )
	{
		if ( m_dwSelectedSlot != DEFSLOT_NULL )
		{
			m_pExpeditionInfoMemberSlot[m_dwSelectedSlot]->ResetSelect();
		}
	}		

	if ( dwSlotIndex != DEFSLOT_NULL )
		m_pExpeditionInfoMemberSlot[dwSlotIndex]->SetSelect();
	m_dwSelectedSlot = dwSlotIndex;
}

void ExpeditionInfoGroup::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	switch ( ControlID )
	{
	case EXPEDITION_INFO_PARTY_SLOT0:
	case EXPEDITION_INFO_PARTY_SLOT1:
	case EXPEDITION_INFO_PARTY_SLOT2:
	case EXPEDITION_INFO_PARTY_SLOT3:
	case EXPEDITION_INFO_PARTY_SLOT4:
	case EXPEDITION_INFO_PARTY_SLOT5:
	case EXPEDITION_INFO_PARTY_SLOT6:
	case EXPEDITION_INFO_PARTY_SLOT7:
		{
			const DWORD dwSlotIndex = DWORD(ControlID - EXPEDITION_INFO_PARTY_SLOT0);
			if ( CHECK_MOUSE_IN(dwMsg) )			
				TranslateUIMessage_SelectPartySlot(dwSlotIndex);
		}
		break;	
	}
}

void ExpeditionInfoGroup::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	TranslateUIMessage_SelectPartySlot(DEFSLOT_NULL);
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}
