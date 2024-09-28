#include "StdAfx.h"

#include "../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../../RanLogicClient/Party/GLPartyTenderManagerClient.h"
#include "../../../Display/ItemInfoSimpleToolTip.h"
#include "../../../Util/RnProgressBar.h"

#include "TenderSlot.h"

TenderSlot::TenderSlot(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
{	
}
TenderSlot::~TenderSlot(void)
{
}

void TenderSlot::SetVisibleSingle(BOOL bVisible)
{
	CUIGroupHelper::SetVisibleSingle(bVisible);
	m_pSelectBox->SetVisibleSingle(FALSE);
}

void TenderSlot::CreateSubControl(void)
{
	RnProgressBar::CreateArg arg;
	arg.controlName = "ITEM_INFOR_SIMPLE_TOOLTIP";
	arg.backgroundTextureName = "PARTY_TENDER_ITEM_PROGRESS_BG_TEXTURE";
	arg.overTextureName = "PARTY_TENDER_ITEM_PROGRESS_OVER_TEXTURE";

	m_pProgressBar = new RnProgressBar( m_pEngineDevice );
	m_pProgressBar->CreateSub( this, "ITEM_INFOR_SIMPLE_TOOLTIP", UI_FLAG_DEFAULT);
	m_pProgressBar->CreateSubControl(arg);
	RegisterControl(m_pProgressBar);

	m_pTenderItemInfo = new ItemInfoSimpleToolTip(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	m_pTenderItemInfo->CreateSub(this, "ITEM_INFOR_SIMPLE_TOOLTIP", UI_FLAG_DEFAULT);
	m_pTenderItemInfo->CreateSubControl();
	RegisterControl(m_pTenderItemInfo);

	m_pTenderStateIcon[EMPARTY_TENDER_STATE_TENDER] = CreateControl("TENDER_STATE_ICON_DICE"); // 주사위 박스아이콘;
	m_pTenderStateIcon[EMPARTY_TENDER_STATE_NONEAVAILABLE] = CreateControl("TENDER_STATE_ICON_X"); // 엑스 박스아이콘;
	m_pTenderStateIcon[EMPARTY_TENDER_STATE_FREE] = CreateControl("TENDER_STATE_ICON_EMPTY"); // 빈 박스 아이콘;	

	m_pSelectBox = CreateLineBox("ITEM_INFOR_SIMPLE_TOOLTIP", "PARTY_INFO_SELECTBOX_TEXTUREINFO");
}

void TenderSlot::SetSelect(const bool bSelect)
{
	const BOOL bShow = (IsVisible() && (bSelect == true)) ? TRUE : FALSE;
	m_pSelectBox->SetVisibleSingle(bShow);
}

void TenderSlot::SetProgress(const float fPercent)
{
	m_pProgressBar->SetMax(100);
	m_pProgressBar->SetNow(DWORD(fPercent * 100.0f));
}

void TenderSlot::SetState(const DWORD emTenderState)
{
	if ( emTenderState > (EMPARTY_TENDER_STATE_NSIZE - 1) )
		return;

	for ( DWORD _i = EMPARTY_TENDER_STATE_NSIZE; _i; --_i )
		m_pTenderStateIcon[_i - 1]->SetVisibleSingle(FALSE);

	m_pTenderStateIcon[emTenderState]->SetVisibleSingle(TRUE);
}

void TenderSlot::SetSlotInfo(GLTenderItemCustomClient* const pItemCustom)
{	
	const bool bNoneEquipable = m_pTenderItemInfo->SetItemInformation(pItemCustom->m_ItemCustom);
	if ( (pItemCustom->m_emTenderState != EMPARTY_TENDER_STATE_TENDER) && (bNoneEquipable == true) )
		pItemCustom->m_emTenderState = EMPARTY_TENDER_STATE_NONEAVAILABLE;

	SetState(DWORD(pItemCustom->m_emTenderState));
	
	SetVisibleSingle(TRUE);
}

void TenderSlot::ResetSlotInfo(void)
{
	SetVisibleSingle(FALSE);
}

const bool TenderSlot::isSelected(void) const
{
	return m_pSelectBox->IsVisible();
}