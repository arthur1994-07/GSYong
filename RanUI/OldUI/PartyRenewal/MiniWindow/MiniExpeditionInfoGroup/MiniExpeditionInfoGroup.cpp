#include "StdAfx.h"

#include "../../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../../RanLogicClient//Party/GLPartyClient.h"
#include "../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../StateWindow/MemberStateGroup/MemberStateSlot/MemberStateSlotSkillEffect.h"
#include "../../../Party/PartyBuffInfo.h"
#include "./MiniExpeditionInfoMemberSlot/MiniExpeditionInfoMemberSlot.h"
#include "MiniExpeditionInfoGroup.h"

MiniExpeditionInfoGroup::MiniExpeditionInfoGroup(EngineDeviceMan* pEngineDevice, CInnerInterface* pInterface, GLGaeaClient* pGaeaClient)
: CUIGroupHelper(pEngineDevice)
, m_pInterface(pInterface)
, m_pGaeaClient(pGaeaClient)
, m_pPartyBuffInfo(NULL)
, m_dwSelectedSlot(DEFSLOT_NULL)
, m_dwPartyIndex(DEFSLOT_NULL)
{
}

void MiniExpeditionInfoGroup::RESET(void)
{
	for ( DWORD _i = MAXPARTY; _i; --_i )
		m_pExpeditionInfoMemberSlot[_i - 1]->RESET();
	m_dwSelectedSlot = DEFSLOT_NULL;
}

void MiniExpeditionInfoGroup::ResetSelect(void)
{
	m_dwSelectedSlot = DEFSLOT_NULL;
}

void MiniExpeditionInfoGroup::CreateSubControl(void)
{
	CreateLineBox( "EXPEDITION_PARTY_GROUP", "PARTY_INFO_LINEBOX_TEXTUREINFO" );

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
		m_pExpeditionInfoMemberSlot[_i] = new MiniExpeditionInfoPartySlot(m_pEngineDevice, _i);
		m_pExpeditionInfoMemberSlot[_i]->CreateSub( this, strRaidInfoPartySlot[_i].GetString (), UI_FLAG_DEFAULT, EXPEDITION_INFO_PARTY_SLOT0 + _i );
		m_pExpeditionInfoMemberSlot[_i]->CreateSubControl();
		RegisterControl(m_pExpeditionInfoMemberSlot[_i]);
		m_pExpeditionInfoMemberSlot[_i]->SetVisibleSingle(FALSE);
		m_pExpeditionInfoMemberSlot[_i]->RESET();
	}

	CPartyBuffInfo* pPartyBuffInfo = new CPartyBuffInfo(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	pPartyBuffInfo->CreateSub ( this, "MINIPARTY_BUFF_INFO", UI_FLAG_DEFAULT, EXPEDITION_INFO_SKILLEFFECT );
	pPartyBuffInfo->CreateSubControl ();
	RegisterControl ( pPartyBuffInfo );
	m_pPartyBuffInfo = pPartyBuffInfo;
}

const DWORD MiniExpeditionInfoGroup::GetSelectedGaeaID(void) const
{
	if ( m_dwSelectedSlot == DEFSLOT_NULL )
		return DBNUM_NULL;
	else
		return m_pExpeditionInfoMemberSlot[m_dwSelectedSlot]->GetGaeaID();
}

const DWORD MiniExpeditionInfoGroup::GetSelectedSlot(void) const
{
	return m_dwSelectedSlot;
}

const CString& MiniExpeditionInfoGroup::GetName(void) const
{
	return m_pExpeditionInfoMemberSlot[m_dwSelectedSlot]->GetName();
}

void MiniExpeditionInfoGroup::SetExpeditionSlot(GLPartyClient* const pParty, const DWORD dwPartyIndex /* = DEFSLOT_NULL */)
{

	for ( DWORD _i = MAXPARTY; _i; --_i )		
		m_pExpeditionInfoMemberSlot[_i - 1]->SetVisibleSingle(FALSE);	

	if ( pParty == NULL )
		return;

	if ( pParty->GetNMember() == 0 )
		return;

	const bool bMaster = pParty->GetPartyID().isValidExpedition() ? (pParty->GetPartyIndex() == MASTER_PARTY) : true;
	const GLPartySlotClient* const pPartySlot = pParty->GetMember(MASTER_PARTY);
	if ( pPartySlot == NULL )
		return;
	else
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
	
	m_dwPartyIndex = dwPartyIndex;
	SetVisibleSingle(TRUE);
}

void MiniExpeditionInfoGroup::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
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
			GLPartyClient* const pParty = m_pGaeaClient->GetPartyManager()->GetParty(m_dwPartyIndex);
			if ( pParty == NULL )
				break;

			const GLPartySlotClient* const pMember = pParty->GetMember(dwSlotIndex);
			if ( pMember == NULL )
				break;

			if ( CHECK_MOUSE_IN(dwMsg) )
			{
				m_dwSelectedSlot = dwSlotIndex;
				m_pPartyBuffInfo->SetPartyID(dwSlotIndex, m_dwPartyIndex);
				m_pPartyBuffInfo->SetVisibleSingle(TRUE);
			}

			if ( CHECK_MOUSEIN_RBUPLIKE(dwMsg) && (m_dwPartyIndex != DEFSLOT_NULL) )
			{
				STARGETID sTargetID(CROW_PC, pMember->m_dwGaeaID);
				m_pGaeaClient->GetCharacter()->ReqSkillReaction(sTargetID);
			}
		}
		break;
	}
}

void MiniExpeditionInfoGroup::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	const UIRECT& rcGlobalPos = GetGlobalPos();
	m_pPartyBuffInfo->SetLocalPos(D3DXVECTOR2(x + 25 - rcGlobalPos.left, y + 25 - rcGlobalPos.top));
	m_pPartyBuffInfo->SetVisibleSingle(FALSE);

	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );		
}