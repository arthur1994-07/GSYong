#include "StdAfx.h"

#include "../../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../../../EngineLib/DeviceMan.h"
#include "../../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../../EngineLib/DxTools/d3dfont.h"

#include "../../../../../RanLogic/GLogicData.h"
#include "../../../../../RanLogic/Item/GLItem.h"
#include "../../../../../RanLogic/Character/GLCharDefine.h"

#include "../../../../../RanlogicClient/Party/GLPartyClient.h"
#include "../../../../../RanLogicClient/Party/GLPartyTenderManagerClient.h"

#include "DistributionMemberSlot.h"

DistributionMemberSlot::DistributionMemberSlot(EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
, m_pMember(NULL)
{
}

void DistributionMemberSlot::RESET(void)
{
	m_pMember = NULL;
	SetVisibleSingle(FALSE);
}

void DistributionMemberSlot::CreateSubControl(void)
{
	CD3DFontPar* pFont18 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 18, D3DFONT_BOLD | D3DFONT_SHADOW_EX | D3DFONT_KSC5601 );
	m_pClass = CreateText("CLASS", "PARTY_DISTRIBUTION_LIST_SLOT_CLASS", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pClass->SetFont(pFont18);
	m_pLevel = CreateText("LEVEL", "PARTY_DISTRIBUTION_LIST_SLOT_LEVEL", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pLevel->SetFont(pFont18);	
	m_pName = CreateText("NAME", "PARTY_DISTRIBUTION_LIST_SLOT_NAME", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pName->SetFont(pFont18);

	m_pSelectBox = CreateLineBox( "PARTY_DISTRIBUTION_MEMBER_SLOT", "PARTY_INFO_SELECTBOX_TEXTUREINFO" );	
}

const bool DistributionMemberSlot::SetSlot(GLPartySlotClient* pMemberSlot, const SITEM* const pItemData, const SNATIVEID& ItemMapID)
{
	if ( pMemberSlot->m_sMapID.getBaseMapID() != ItemMapID )
		return false;

	const bool bTakeItem = pMemberSlot->isMemberState(EMPARTY_MEMBER_STATE_TAKEITEM);
	const DWORD dwDefaultColor = bTakeItem ? ITEM_INFOR_COLOR::dwAddAbility : ITEM_INFOR_COLOR::dwDefault;
	{		
		const DWORD dwColor = 
			pItemData == NULL ? dwDefaultColor : (pItemData->sBasicOp.dwReqCharClass & pMemberSlot->m_emClass ? dwDefaultColor : ITEM_INFOR_COLOR::dwReqWrong);		

		const DWORD dwClassIndex = DWORD(CharClassToClassIndex(pMemberSlot->m_emClass));
		CString strClass;
		strClass.Format("%s", ID2GAMEWORD("CLASS_NAME_NO_SEX", dwClassIndex));
		m_pClass->ClearText();
		m_pClass->SetText(strClass, dwColor);
	}

	{
		const DWORD dwColor = 
			pItemData == NULL ? dwDefaultColor : (pItemData->sBasicOp.wReqLevelDW > pMemberSlot->m_Level ? ITEM_INFOR_COLOR::dwReqWrong : dwDefaultColor);		

		CString strLevel;
		strLevel.Format("Lv%d", pMemberSlot->m_Level);
		m_pLevel->ClearText();
		m_pLevel->SetText(strLevel, dwColor);
	}
	
	CString strName;
	strName.Format("%s", pMemberSlot->m_szName);
	m_pName->ClearText();
	m_pName->SetText(strName, dwDefaultColor);
	
	m_pSelectBox->SetVisibleSingle(pMemberSlot->isMemberState(EMPARTY_MEMBER_STATE_SELECTED));

	SetVisibleSingle(TRUE);

	m_pMember = pMemberSlot;
	return true;
}

void DistributionMemberSlot::SetSelect(void)
{
	if ( m_pMember == NULL )
		return;

	m_pMember->InverseMemberState(EMPARTY_MEMBER_STATE_SELECTED);	
}
