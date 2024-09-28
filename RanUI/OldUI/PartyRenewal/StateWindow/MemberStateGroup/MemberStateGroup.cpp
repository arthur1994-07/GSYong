#include "StdAfx.h"

#include "../../../../../EngineLib/GUInterface/UIControl.h"
#include "../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../../InnerInterface.h"
#include "./MemberStateSlot/MemberStateSlot.h"
#include "MemberStateGroup.h"

MemberStateGroup::MemberStateGroup(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
{
}

void MemberStateGroup::CreateSubControl(void)
{
	CString strMemberStateSlot[MAXPARTY] = 
	{
		"MEMBER_STATE_SLOT0",
		"MEMBER_STATE_SLOT1",
		"MEMBER_STATE_SLOT2",
		"MEMBER_STATE_SLOT3",
		"MEMBER_STATE_SLOT4",
		"MEMBER_STATE_SLOT5",
		"MEMBER_STATE_SLOT6",
		"MEMBER_STATE_SLOT7",
	};

	for ( unsigned int _i = 0; _i < MAXPARTY; _i++ )
	{
		MemberStateSlot* pMemberSlot = new MemberStateSlot(m_pEngineDevice, m_pGaeaClient, _i);
		pMemberSlot->CreateSub( this, strMemberStateSlot[_i].GetString (), UI_FLAG_DEFAULT, MEMBER_STATE_SLOT0 + _i );
		pMemberSlot->CreateSubControl();
		RegisterControl( pMemberSlot );
		m_pMemberSlot[_i] = pMemberSlot;
	}	
	SetVisibleSkillEffect(FALSE);
}

void MemberStateGroup::SetVisibleSkillEffect(const BOOL bShow)
{
	for ( DWORD _i = MAXPARTY; _i; --_i )
		m_pMemberSlot[_i - 1]->SetVisibleSkillEffect(bShow);
}

float MemberStateGroup::GetVisibleSkillEffectSizeX()
{
	float fPosX = 0.f;
	float fTemp = 0.f;
	for ( DWORD _i = MAXPARTY; _i; --_i )
	{
		fTemp = m_pMemberSlot[_i - 1]->GetVisibleSkillLimitSizeX();
		if( fPosX < fTemp )
			fPosX = fTemp;
	}
	
	return fPosX;
}

void MemberStateGroup::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

	m_vMousePosition = D3DXVECTOR2(float(x), float(y));
	for ( DWORD _i = MAXPARTY; _i; --_i )
		m_pMemberSlot[_i - 1]->RESET();

 
	GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
	if ( pMyParty->isValid() == false )
		return; // 소속된 파티가 없으면 빠져 나감;		

	unsigned int dwSlotIndexUI = 0;
	GLPartySlotClient* const pMaster = pMyParty->GetMember(MASTER_PARTY);
	if ( pMaster == NULL )
		return;

	const bool bMaster = pMyParty->GetPartyID().isValidExpedition() ? (pMyParty->GetPartyIndex() == MASTER_PARTY) : true;
	m_pMemberSlot[dwSlotIndexUI++]->SetMemberStateMasterSlot(pMaster, bMaster);

	for ( DWORD _i = 1; _i < MAXPARTY; ++_i )
	{
		GLPartySlotClient* pPartySlot = pMyParty->GetMember(_i);		
		if ( pPartySlot == NULL )
			continue;		

		m_pMemberSlot[dwSlotIndexUI++]->SetMemberStateSlot(pPartySlot);
	}
}

void MemberStateGroup::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	switch ( ControlID )
	{
	case MEMBER_STATE_SLOT0:
	case MEMBER_STATE_SLOT1:
	case MEMBER_STATE_SLOT2:
	case MEMBER_STATE_SLOT3:
	case MEMBER_STATE_SLOT4:
	case MEMBER_STATE_SLOT5:
	case MEMBER_STATE_SLOT6:
	case MEMBER_STATE_SLOT7:
		{
			const DWORD dwSlotIndex = DWORD(ControlID - MEMBER_STATE_SLOT0);
			if ( CHECK_MOUSEIN_RBUPLIKE(dwMsg) )
			{
				GLPartyClient* const pParty = m_pGaeaClient->GetPartyManager()->GetMyParty();
				GLPartySlotClient* const pMember = pParty->GetMember(dwSlotIndex);
				if ( pMember == NULL )
					break;

				STARGETID sTargetID(CROW_PC, pMember->m_dwGaeaID);
				m_pGaeaClient->GetCharacter()->ReqSkillReaction(sTargetID);
			}
		}
	}	
}
