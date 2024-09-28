#include "StdAfx.h"

#include "../../../../../../RanLogicClient/ClientActor.h"
#include "../../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../../../../RanLogicClient/Char/GLCharClient.h"
#include "MemberStateSlotPortrait.h"
#include "MemberStateSlotInformation.h"
#include "MemberStateSlotSkillEffect.h"
#include "MemberStateSlot.h"

MemberStateSlot::MemberStateSlot(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient, const DWORD dwSlotIndex)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_dwSlotIndex(dwSlotIndex)
{
	m_pMasterCrown[0] = NULL;
	m_pMasterCrown[1] = NULL;
}

void MemberStateSlot::RESET(void)
{
	SetVisibleSingle(FALSE);
	m_pMemberStateSlotSkillEffect->ResetSlotSkillEffect();
}

void MemberStateSlot::SetMemberStateMasterSlot(const GLPartySlotClient* const pPartyClient, const bool bMaster)
{
	m_pMasterCrown[0]->SetVisibleSingle(bMaster ? TRUE : FALSE);
	m_pMasterCrown[1]->SetVisibleSingle(bMaster ? FALSE : TRUE);

	SetMemberStateSlot(pPartyClient);
}
void MemberStateSlot::SetMemberStateSlot(const GLPartySlotClient* const pPartyClient)
{
	const GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter == NULL )
		return;

	GLPartyManagerClient* const pPartyManager(m_pGaeaClient->GetPartyManager());
	if ( pPartyManager == NULL )
		return;

	GLPartyClient* const pMyParty(pPartyManager->GetMyParty());
	if ( pMyParty == NULL )
		return;

	GLPartySlotClient* const pMyPartySlot(pMyParty->GetMemberByGaeaID(pCharacter->GetGaeaID()));
	if ( pMyPartySlot == NULL )
		return;

	const bool bAvailable(pMyPartySlot->m_sMapID == pPartyClient->m_sMapID);

	m_pMemberStateSlotPortrait->SetPortrait(pPartyClient, bAvailable);
	m_pMemberStateSlotInformation->SetInformation(pPartyClient, bAvailable);
	m_pMemberStateSlotSkillEffect->SetSkillEffect(pPartyClient->m_dwGaeaID);

	SetVisibleSingle(TRUE);
}
void MemberStateSlot::SetMemberStateSlot(std::tr1::shared_ptr<GLCharClient> pPartySlot)
{
	if(pPartySlot == NULL)
		return;
	

	//const bool bAvailable(pMyPartySlot->m_sMapID == pPartySlot->m_sMapID);

	m_pMemberStateSlotPortrait->SetPortrait(pPartySlot, true);
	m_pMemberStateSlotInformation->SetInformation(pPartySlot, true);
	m_pMemberStateSlotSkillEffect->SetSkillEffect(pPartySlot->GetGaeaID());

	SetVisibleSingle(TRUE);
}

void MemberStateSlot::SetMemberStateSlot(GLCharacter* pChar)
{
	if( pChar == NULL )
		return;


	//const bool bAvailable(pMyPartySlot->m_sMapID == pPartySlot->m_sMapID);

	m_pMemberStateSlotPortrait->SetPortrait(pChar, true);
	m_pMemberStateSlotInformation->SetInformation(pChar, true);
	m_pMemberStateSlotSkillEffect->SetSkillEffect(pChar->GetGaeaID());

	SetVisibleSingle(TRUE);

}

void MemberStateSlot::SetVisibleSkillEffect(const BOOL bShow)
{
	m_pMemberStateSlotSkillEffect->SetVisibleSingle(bShow);
}

float MemberStateSlot::GetVisibleSkillLimitSizeX()
{
	return m_pMemberStateSlotSkillEffect->GetVisibleSkillEffectSizeX();
}

const UIRECT& MemberStateSlot::GetSkillEffectPos()
{
	return m_pMemberStateSlotSkillEffect->GetGlobalPos();
}

const UIRECT MemberStateSlot::GetSlotPos()
{
	UIRECT rect = m_pMemberStateSlotPortrait->GetGlobalPos();
	rect.sizeX = m_pMemberStateSlotInformation->GetGlobalPos().right - rect.left;
	rect.right = rect.left + rect.sizeX;
	return rect;
}

const UIRECT& MemberStateSlot::GetSlotPortraitPos()
{
	return m_pMemberStateSlotPortrait->GetGlobalPos();
}

const UIRECT& MemberStateSlot::GetSlotInfoPos()
{
	return m_pMemberStateSlotInformation->GetGlobalPos();
}

void MemberStateSlot::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

	UIRECT rect = m_pMemberStateSlotSkillEffect->GetGlobalPos();
	rect.left = m_pMemberStateSlotInformation->GetGlobalPos().right;
	rect.sizeX = m_pMemberStateSlotSkillEffect->GetVisibleSkillEffectSizeX();
	rect.right = rect.left + rect.sizeX;
	m_pMemberStateSlotSkillEffect->SetGlobalPos( rect );
}

void MemberStateSlot::CreateSubControl(void)
{
	{ // 직업 초상화;
		MemberStateSlotPortrait* pMemberStateSlotPortrait = new MemberStateSlotPortrait(m_pEngineDevice, m_pGaeaClient);
		pMemberStateSlotPortrait->CreateSub(this, "MEMBER_STATE_SLOT_PORTRAIT", UI_FLAG_DEFAULT, MEMBER_STATE_SLOT_PORTRAIT);
		pMemberStateSlotPortrait->CreatesubControl();
		RegisterControl(pMemberStateSlotPortrait);
		m_pMemberStateSlotPortrait = pMemberStateSlotPortrait;
	}

	if ( m_dwSlotIndex == MASTER_PARTY )
	{
		m_pMasterCrown[0] = CreateControl("MEMBER_STATE_SLOT_MASTER");
		m_pMasterCrown[1] = CreateControl("MEMBER_STATE_SLOT_SUB_MASTER");
	}

	{ // 케릭터 정보 ( 학원, 클럽, 이름, 체력 );
		MemberStateSlotInformation* pMemberStateSlotInformation = new MemberStateSlotInformation(m_pEngineDevice, m_pGaeaClient);
		pMemberStateSlotInformation->CreateSub(this, "MEMBER_STATE_SLOT_INFORMATION", UI_FLAG_DEFAULT, MEMBER_STATE_SLOT_INFORMATION);
		pMemberStateSlotInformation->CreateSubControl();
		RegisterControl(pMemberStateSlotInformation);
		m_pMemberStateSlotInformation = pMemberStateSlotInformation;
	}

	{ // 버프(스킬 효과) 정보;
		MemberStateSlotSkillEffect* pMemberStateSlotSkillEffect = new MemberStateSlotSkillEffect(m_pEngineDevice, m_pGaeaClient);
		pMemberStateSlotSkillEffect->CreateSub(this, "MEMBER_STATE_SLOT_SKILLEFFECT", UI_FLAG_DEFAULT, MEMBER_STATE_SLOT_SKILLEFFECT);
		pMemberStateSlotSkillEffect->CreateSubControl();
		RegisterControl(pMemberStateSlotSkillEffect);
		m_pMemberStateSlotSkillEffect = pMemberStateSlotSkillEffect;
	}
}