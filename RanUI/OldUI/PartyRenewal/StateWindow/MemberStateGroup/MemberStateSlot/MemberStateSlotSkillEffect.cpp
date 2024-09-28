#include "StdAfx.h"

#include "../../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../../../../RanLogicClient/Party/GLPartyClient.h"
#include "./SkillEffectSlot/SkillEffectSlot.h"
#include "MemberStateSlotSkillEffect.h"

MemberStateSlotSkillEffect::MemberStateSlotSkillEffect(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
{
}

void MemberStateSlotSkillEffect::CreateSubControl(const bool bSmall)
{	
	CString strKeyword = bSmall ? "SKILLEFFECT_SLOT_SMALL" : "SKILLEFFECT_SLOT";
	CString strSkillEffect;
	for ( DWORD _i = 0; _i < SKILLEFFECT_SLOT_SIZE; ++_i )
	{
		strSkillEffect.Format("%s%d", strKeyword, _i);
		m_pSkillSlot[_i] = new SkillEffectSlot(m_pEngineDevice, m_pGaeaClient);
		m_pSkillSlot[_i]->CreateSub(this, strSkillEffect.GetString(), UI_FLAG_DEFAULT, SKILLEFFECT_SLOT0 + _i);
		m_pSkillSlot[_i]->CreateSubControl(bSmall);
		m_pSkillSlot[_i]->SetVisibleSingle(FALSE);
		
		RegisterControl(m_pSkillSlot[_i]);
	}
}

void MemberStateSlotSkillEffect::ResetSlotSkillEffect()
{
	for ( DWORD _i = 0; _i < SKILLEFFECT_SLOT_SIZE; ++_i )
		m_pSkillSlot[_i]->SetVisibleSingle(FALSE);
}

void MemberStateSlotSkillEffect::SetSkillEffect(const DWORD dwGaeaID)
{
	if ( IsVisible() == FALSE )
		return;

	DWORD dwIndex = 0;	

	const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();	
	if ( dwMyGaeaID == dwGaeaID )
	{
		GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
		if ( pCharacter == NULL )
		{
			SetVisibleSingle(FALSE);
			return;
		}
		
		SetVisibleSingle(TRUE);
		for ( DWORD _i = 0; _i < SKILLEFFECT_SLOT_SIZE; ++_i )
		{
			const SNATIVEID& SkillID = pCharacter->GetSkillFact(_i).sNATIVEID;
			if ( m_pSkillSlot[dwIndex]->SetSkillImage(SkillID) )
				++dwIndex;
		}
	}
	else
	{
		std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar(dwGaeaID);
		if ( pCharClient == NULL )
		{
			SetVisibleSingle(FALSE);
			return;
		}

		SetVisibleSingle(TRUE);
		for ( DWORD _i = 0; _i < SKILLEFFECT_SLOT_SIZE; ++_i )
		{
			const SNATIVEID& SkillID = pCharClient->GetSkillFact(_i).sNATIVEID;
			if ( m_pSkillSlot[dwIndex]->SetSkillImage(SkillID) )
				++dwIndex;
		}
	}
}

float MemberStateSlotSkillEffect::GetVisibleSkillEffectSizeX()
{
	if ( IsVisible() == FALSE )
		return 0.f;

	float fLimitPosX = 0.f;
	for ( DWORD _i = 0; _i < SKILLEFFECT_SLOT_SIZE; ++_i )
	{
		if ( m_pSkillSlot[_i]->IsVisible() )
		{
			if( fLimitPosX < m_pSkillSlot[_i]->GetGlobalPos().right )
				fLimitPosX = m_pSkillSlot[_i]->GetGlobalPos().right;
		}
	}

	UIRECT Rect = GetGlobalPos();
	if( fLimitPosX != 0.f )
	{
		Rect.right = fLimitPosX;
		Rect.sizeX = Rect.right - Rect.left;
	}
	else
	{
		Rect.sizeX = 0.f;
		Rect.right = Rect.left;
	}
	SetGlobalPos( Rect );
	return Rect.sizeX;
}