#include "StdAfx.h"
#include "RnCharacterAdditionalWindow.h"

#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../../RanLogicClient/ReferChar/ReferChar.h"
#include "../../../../RanLogic/GLogicDataMan.h"
#include "../../Item/RnInventoryWindow.h"

#include "../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../EngineLib/DxTools/d3dfont.h"
#include "../../../../EngineLib/DeviceMan.h"

#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../enginelib/GUInterface/BasicTextBox.h"
#include "../../../../enginelib/GUInterface/BasicButton.h"

RnCharacterAdditionalWindow::RnCharacterAdditionalWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_bIsRefer(false)
{
}

RnCharacterAdditionalWindow::~RnCharacterAdditionalWindow()
{
}

void RnCharacterAdditionalWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	if ( ControlID >= STATE_OVER_RECT_CRITICAL && ControlID <= STATE_OVER_RECT_INC_INT_SP)
	{
		OverRectMap::iterator it = m_OverRectMap.find(ControlID);
		if( m_OverRectMap.end() != it )
		{
			CUIControl* pControl = it->second;
			pControl->SetUseRender(CHECK_MOUSE_IN( dwMsg ));
		}
	}

	PrintTooltip( ControlID, dwMsg );
}

void RnCharacterAdditionalWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( !m_bIsRefer )
		UpdateMyChar();

	CUIControl* pParent = GetParent();
	if ( !pParent )
		return;

	if( m_PosX != x || m_PosY != y )
	{
		m_PosX = x;
		m_PosY = y;
		if ( IsExclusiveSelfControl() )
			pParent->SetGlobalPos ( D3DXVECTOR2 ( x - m_vMoveGap.x - m_vGap.x, y - m_vMoveGap.y - m_vGap.y ) );
	}
}

void RnCharacterAdditionalWindow::UpdateMyChar()
{
	if( !m_apCharData.get() )
		m_apCharData.reset(new GLCHARLOGIC);

	GLCHARLOGIC* pCharLogic = &m_pGaeaClient->GetCharacterLogic();
	SSUM_ITEM& sSumItem = pCharLogic->m_sSUMITEM;

	*m_apCharData = *pCharLogic;

	RefreshBasicInfo( *pCharLogic, sSumItem );
	ApplyInvenSlotItem( m_apCharData.get() );
	RefreshUpInfo( *pCharLogic, *m_apCharData );
}

void RnCharacterAdditionalWindow::RefreshReferChar()
{
	StateClearText();

	if( !m_bIsRefer )		
		return;

	ReferChar::CClient* pReferChar = m_pGaeaClient->GetReferChar();
	if( !pReferChar )
		return;

	ReferChar::SCharInfoAddition* pTarget = pReferChar->GetCompleteDataAddition();
	if( !pTarget )
		return;

	GLCHARLOGIC* pCharLogic = &m_pGaeaClient->GetCharacterLogic();
	if( !pCharLogic )
		return;

	RefreshReferInfo( *pTarget );
	RefreshReferExInfo( *pTarget, *pCharLogic );
}

void RnCharacterAdditionalWindow::ApplyInvenSlotItem( GLCHARLOGIC* pChar )
{
	if( !pChar )
		return;

	IInventory* pInventory = m_pInterface->GetInventory();
	if( !pInventory )
		return;

	if( pInventory->GetTapSelected() != CRnInventoryWindow::ETapInven )
		return;

	// RM# 989: 포장된 아이템은 캐릭터 정보창에서 아이템의 능력치 비교가 되지않도록 수정요청이 옴;
	const SINVENITEM* pInvenItem = pInventory->GetFocusItem();
	if( !pInvenItem || pInvenItem->GetNativeID() == NATIVEID_NULL() || pInvenItem->CustomItem().IsBoxWrapped())
		return;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
	if( !pItem )
		return;

	if( pItem->IsSuitable() )
	{
		if( pItem->sBasicOp.dwReqCharClass & pChar->m_emClass )
		{
			for( int i=0; i<SLOT_TSIZE; ++i )
			{
				if( EMSLOTCHECK_OK == pChar->CHECKSLOT_ITEM( pInvenItem->GetNativeID(), EMSLOT( i ) ) )
				{
					pChar->m_PutOnItems[ i ] = pInvenItem->sItemCustom;
					pChar->INIT_DATA( FALSE, FALSE );
					pChar->UPDATE_DATA( 1, 1 );
					return;
				}
			}
		}
	}

}

void RnCharacterAdditionalWindow::StateClearText()
{
	for( int i=0; i<STAT_MAX; ++i )
	{
		m_pStatUpValue[i]->ClearText();
		m_pStatValue[i]->ClearText();
	}
}

void RnCharacterAdditionalWindow::RefreshBasicInfo( GLCHARLOGIC& pCharLogic, SSUM_ITEM& sSumItem)
{
	if(!(&pCharLogic) || !(&sSumItem) )
		return;

	SetStatePerText( m_pStatValue[STATE_CRITICAL], pCharLogic.GETCRITICAL_TOTAL() );
	SetStatePerText( m_pStatValue[STATE_CRUSHING_BLOW], sSumItem.fIncR_CrushingBlow );
	
	SetStatePerText( m_pStatValue[STATE_MOVE_SPEED], pCharLogic.GETMOVE_TOTAL() - 1 );
	SetStatePerText( m_pStatValue[STATE_ATK_SPEED], pCharLogic.GETATT_TOTAL() - 1 );
	SetStatePerText( m_pStatValue[STATE_EXP_MULTIPLE], pCharLogic.GETEXPVELO() - 1 );

	SetStatePerText( m_pStatValue[STATE_INC_PER_HP], pCharLogic.m_fVariationRateHP );
	SetStatePerText( m_pStatValue[STATE_INC_PER_MP], pCharLogic.m_fVariationRateMP );
	SetStatePerText( m_pStatValue[STATE_INC_PER_SP], pCharLogic.m_fVariationRateSP );
	SetStateText( m_pStatValue[STATE_INC_PER_CP], sSumItem.nReceive_CP_Inc + pCharLogic.m_nGET_CP_VALUE );
	SetStateText( m_pStatValue[STATE_INC_INT_HP], sSumItem.nPotion_HP_Inc + pCharLogic.m_nPotion_HP_Inc_SKILL );
	SetStateText( m_pStatValue[STATE_INC_INT_MP], sSumItem.nPotion_MP_Inc + pCharLogic.m_nPotion_MP_Inc_SKILL );
	SetStateText( m_pStatValue[STATE_INC_INT_SP], sSumItem.nPotion_SP_Inc + pCharLogic.m_nPotion_SP_Inc_SKILL );
}

void RnCharacterAdditionalWindow::RefreshUpInfo( GLCHARLOGIC& pCharLogic, GLCHARLOGIC& pUpCharLogic )
{
	if(!(&pCharLogic) || !(&pUpCharLogic) )
		return;

	const SSUM_ITEM& sSumItem = pCharLogic.m_sSUMITEM;
	const SSUM_ITEM& upSSumItem = pUpCharLogic.m_sSUMITEM;

	SetStatePerText( m_pStatUpValue[STATE_CRITICAL], pUpCharLogic.GETCRITICAL_TOTAL() - pCharLogic.GETCRITICAL_TOTAL(), true );
	SetStatePerText( m_pStatUpValue[STATE_CRUSHING_BLOW], upSSumItem.fIncR_CrushingBlow - sSumItem.fIncR_CrushingBlow, true );

	SetStatePerText( m_pStatUpValue[STATE_MOVE_SPEED], pUpCharLogic.GETMOVE_TOTAL() - pCharLogic.GETMOVE_TOTAL(), true );
	SetStatePerText( m_pStatUpValue[STATE_ATK_SPEED], pUpCharLogic.GETATT_TOTAL() - pCharLogic.GETATT_TOTAL(),  true );
	SetStatePerText( m_pStatUpValue[STATE_EXP_MULTIPLE], pUpCharLogic.GETEXPVELO() - pCharLogic.GETEXPVELO(), true );

	SetStatePerText( m_pStatUpValue[STATE_INC_PER_HP], pUpCharLogic.m_fVariationRateHP - pCharLogic.m_fVariationRateHP, true );
	SetStatePerText( m_pStatUpValue[STATE_INC_PER_MP], pUpCharLogic.m_fVariationRateMP - pCharLogic.m_fVariationRateMP, true );
	SetStatePerText( m_pStatUpValue[STATE_INC_PER_SP], pUpCharLogic.m_fVariationRateSP - pCharLogic.m_fVariationRateSP, true );
	SetStateText( m_pStatUpValue[STATE_INC_PER_CP], ((upSSumItem.nReceive_CP_Inc - sSumItem.nReceive_CP_Inc)) + (pUpCharLogic.m_nGET_CP_VALUE - pCharLogic.m_nGET_CP_VALUE), true );
	SetStateText( m_pStatUpValue[STATE_INC_INT_HP], ((upSSumItem.nPotion_HP_Inc - sSumItem.nPotion_HP_Inc) + (pUpCharLogic.m_nPotion_HP_Inc_SKILL - pCharLogic.m_nPotion_HP_Inc_SKILL)), true );
	SetStateText( m_pStatUpValue[STATE_INC_INT_MP], ((upSSumItem.nPotion_MP_Inc - sSumItem.nPotion_MP_Inc) + (pUpCharLogic.m_nPotion_MP_Inc_SKILL - pCharLogic.m_nPotion_MP_Inc_SKILL)), true );
	SetStateText( m_pStatUpValue[STATE_INC_INT_SP], ((upSSumItem.nPotion_SP_Inc - sSumItem.nPotion_SP_Inc) + (pUpCharLogic.m_nPotion_SP_Inc_SKILL - pCharLogic.m_nPotion_SP_Inc_SKILL)), true );
}

void RnCharacterAdditionalWindow::RefreshReferInfo( ReferChar::SCharInfoAddition& cReferInfo )
{
	if( !(&cReferInfo) )
		return;

	SetStatePerText( m_pStatValue[STATE_CRITICAL], cReferInfo.m_fCritical );
	SetStatePerText( m_pStatValue[STATE_CRUSHING_BLOW], cReferInfo.m_fCrushingBlow );
	
	SetStatePerText( m_pStatValue[STATE_MOVE_SPEED], cReferInfo.m_fMoveSpeed - 1 );
	SetStatePerText( m_pStatValue[STATE_ATK_SPEED], cReferInfo.m_fAtkSpeed - 1 );
	SetStatePerText( m_pStatValue[STATE_EXP_MULTIPLE], cReferInfo.m_fExpRate - 1 );

	SetStatePerText( m_pStatValue[STATE_INC_PER_HP], cReferInfo.m_fINCR_HP_TOTAL );
	SetStatePerText( m_pStatValue[STATE_INC_PER_MP], cReferInfo.m_fINCR_MP_TOTAL );
	SetStatePerText( m_pStatValue[STATE_INC_PER_SP], cReferInfo.m_fINCR_SP_TOTAL );
	SetStateText( m_pStatValue[STATE_INC_PER_CP], cReferInfo.m_nReceive_CP_Inc );
	SetStateText( m_pStatValue[STATE_INC_INT_HP], cReferInfo.m_nPotion_HP_Inc );
	SetStateText( m_pStatValue[STATE_INC_INT_MP], cReferInfo.m_nPotion_MP_Inc );
	SetStateText( m_pStatValue[STATE_INC_INT_SP], cReferInfo.m_nPotion_SP_Inc );
}

void RnCharacterAdditionalWindow::RefreshReferExInfo( ReferChar::SCharInfoAddition& cReferInfo, GLCHARLOGIC& pUpCharLogic )
{
	if(!(&cReferInfo) || !(&pUpCharLogic) )
		return;

	const SSUM_ITEM& upSSumItem = pUpCharLogic.m_sSUMITEM;

	SetStatePerText( m_pStatUpValue[STATE_CRITICAL], pUpCharLogic.GETCRITICAL_TOTAL() - cReferInfo.m_fCritical, true );
	SetStatePerText( m_pStatUpValue[STATE_CRUSHING_BLOW], upSSumItem.fIncR_CrushingBlow - cReferInfo.m_fCrushingBlow, true );

	SetStatePerText( m_pStatUpValue[STATE_MOVE_SPEED], pUpCharLogic.GETMOVE_TOTAL() - cReferInfo.m_fMoveSpeed, true );
	SetStatePerText( m_pStatUpValue[STATE_ATK_SPEED], pUpCharLogic.GETATT_TOTAL() - cReferInfo.m_fAtkSpeed, true );
	SetStatePerText( m_pStatUpValue[STATE_EXP_MULTIPLE], pUpCharLogic.GETEXPVELO() - cReferInfo.m_fExpRate, true );

	SetStatePerText( m_pStatUpValue[STATE_INC_PER_HP], pUpCharLogic.m_fVariationRateHP - cReferInfo.m_fINCR_HP_TOTAL, true );
	SetStatePerText( m_pStatUpValue[STATE_INC_PER_MP], pUpCharLogic.m_fVariationRateMP - cReferInfo.m_fINCR_MP_TOTAL, true );
	SetStatePerText( m_pStatUpValue[STATE_INC_PER_SP], pUpCharLogic.m_fVariationRateSP - cReferInfo.m_fINCR_SP_TOTAL, true );
	SetStateText( m_pStatUpValue[STATE_INC_PER_CP], upSSumItem.nReceive_CP_Inc - cReferInfo.m_nReceive_CP_Inc, true );
	SetStateText( m_pStatUpValue[STATE_INC_INT_HP], upSSumItem.nPotion_HP_Inc - cReferInfo.m_nPotion_HP_Inc, true );
	SetStateText( m_pStatUpValue[STATE_INC_INT_MP], upSSumItem.nPotion_MP_Inc - cReferInfo.m_nPotion_MP_Inc, true );
	SetStateText( m_pStatUpValue[STATE_INC_INT_SP], upSSumItem.nPotion_SP_Inc - cReferInfo.m_nPotion_SP_Inc, true );
}

void RnCharacterAdditionalWindow::SetStateText( CBasicTextBox* const pTextBox, const int nValue, const bool bColor )
{
	CString strCombine;
	strCombine.Format( "%d", nValue );

	if( pTextBox )
	{
		if( bColor && nValue )
		{
			if( nValue < 0 )
				pTextBox->SetText( strCombine, NS_UITEXTCOLOR::RED );
			else if( nValue > 0)
			{
				strCombine.Insert( 0, "+" );
				pTextBox->SetText( strCombine, NS_UITEXTCOLOR::GREEN );
			}
		}
		else
			pTextBox->SetText( strCombine );
	}
}

void RnCharacterAdditionalWindow::SetStatePerText( CBasicTextBox* const pTextBox, const float fValue, const bool bColor )
{
	CString strCombine;
	strCombine.Format( "%.2f", fValue * 100.0f );
	strCombine += "%";

	if( pTextBox )
	{
		if( bColor && fValue )
		{
			if( fValue < 0 )
				pTextBox->SetText( strCombine, NS_UITEXTCOLOR::RED );
			else if( fValue > 0)
			{
				strCombine.Insert( 0, "+" );
				pTextBox->SetText( strCombine, NS_UITEXTCOLOR::GREEN );
			}
		}
		else
			pTextBox->SetText( strCombine );
	}
}

void RnCharacterAdditionalWindow::CreateSubControl()
{
	CreateLineBox( "CHARACTOR_ADDITIONAL_ABILITY_WINDOW_REGION", "CHARACTER_WINDOW_REGION_TEXTUREINFO" );

	CreateLineBox( "CHARACTOR_ADDITIONAL_ABILITY_POINT1_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTOR_ADDITIONAL_ABILITY_POINT2_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );

	CreateFlipButton( "CHARACTER_ADDITION_WINDOW_STAT_QUESTION_BUTTON", "CHARACTER_ADDITION_WINDOW_STAT_QUESTION_BUTTON_OVER", STATE_QUESTION_BUTTON, CBasicButton::MOUSEIN_FLIP );

	CreateOverRext();
	CreateStateText();

	m_vMoveGap.x = GetLocalPos().left;
	m_vMoveGap.y = GetLocalPos().top;
}

void RnCharacterAdditionalWindow::CreateOverRext()
{
	m_OverRectMap[STATE_OVER_RECT_CRITICAL]		= CreateOverRect( "STATE_CRITICAL_MOUSEOVER_RECT", STATE_OVER_RECT_CRITICAL );
	m_OverRectMap[STATE_OVER_RECT_CRUSHING_BLOW]= CreateOverRect( "STATE_CRUSHING_BLOW_MOUSEOVER_RECT", STATE_OVER_RECT_CRUSHING_BLOW );
	m_OverRectMap[STATE_OVER_RECT_MOVE_SPEED]	= CreateOverRect( "STATE_MOVE_SPEED_MOUSEOVER_RECT", STATE_OVER_RECT_MOVE_SPEED );
	m_OverRectMap[STATE_OVER_RECT_ATK_SPEED]	= CreateOverRect( "STATE_ATK_SPEED_MOUSEOVER_RECT", STATE_OVER_RECT_ATK_SPEED );
	m_OverRectMap[STATE_OVER_RECT_EXP_MULTIPLE] = CreateOverRect( "STATE_EXP_MULTIPLE_MOUSEOVER_RECT", STATE_OVER_RECT_EXP_MULTIPLE );

	m_OverRectMap[STATE_OVER_RECT_INC_PER_HP] = CreateOverRect( "STATE_INC_PER_HP_MOUSEOVER_RECT", STATE_OVER_RECT_INC_PER_HP );
	m_OverRectMap[STATE_OVER_RECT_INC_PER_MP] = CreateOverRect( "STATE_INC_PER_MP_MOUSEOVER_RECT", STATE_OVER_RECT_INC_PER_MP );
	m_OverRectMap[STATE_OVER_RECT_INC_PER_SP] = CreateOverRect( "STATE_INC_PER_SP_MOUSEOVER_RECT", STATE_OVER_RECT_INC_PER_SP );
	m_OverRectMap[STATE_OVER_RECT_INC_PER_CP] = CreateOverRect( "STATE_INC_PER_CP_MOUSEOVER_RECT", STATE_OVER_RECT_INC_PER_CP );

	m_OverRectMap[STATE_OVER_RECT_INC_INT_HP] = CreateOverRect( "STATE_INC_INT_HP_MOUSEOVER_RECT", STATE_OVER_RECT_INC_INT_HP );
	m_OverRectMap[STATE_OVER_RECT_INC_INT_MP] = CreateOverRect( "STATE_INC_INT_MP_MOUSEOVER_RECT", STATE_OVER_RECT_INC_INT_MP );
	m_OverRectMap[STATE_OVER_RECT_INC_INT_SP] = CreateOverRect( "STATE_INC_INT_SP_MOUSEOVER_RECT", STATE_OVER_RECT_INC_INT_SP );
}

void RnCharacterAdditionalWindow::CreateStateText()
{
	CreateText( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 16), "STATE_CRITICAL_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 17), "STATE_CRUSHING_BLOW_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 18), "STATE_MOVE_SPEED_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 19), "STATE_ATK_SPEED_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 20), "STATE_EXP_MULTIPLE_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 21), "STATE_INC_PER_HP_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 22), "STATE_INC_PER_MP_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 23), "STATE_INC_PER_SP_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 24), "STATE_INC_PER_CP_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 25), "STATE_INC_INT_HP_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 26), "STATE_INC_INT_MP_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 27), "STATE_INC_INT_SP_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );

	m_pStatValue[STATE_CRITICAL]		= CreateText( "", "STATE_CRITICAL_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatValue[STATE_CRUSHING_BLOW]	= CreateText( "", "STATE_CRUSHING_BLOW_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatValue[STATE_MOVE_SPEED]		= CreateText( "", "STATE_MOVE_SPEED_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatValue[STATE_ATK_SPEED]		= CreateText( "", "STATE_ATK_SPEED_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatValue[STATE_EXP_MULTIPLE]	= CreateText( "", "STATE_EXP_MULTIPLE_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatValue[STATE_INC_PER_HP]		= CreateText( "", "STATE_INC_PER_HP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatValue[STATE_INC_PER_MP]		= CreateText( "", "STATE_INC_PER_MP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatValue[STATE_INC_PER_SP]		= CreateText( "", "STATE_INC_PER_SP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatValue[STATE_INC_PER_CP]		= CreateText( "", "STATE_INC_PER_CP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatValue[STATE_INC_INT_HP]		= CreateText( "", "STATE_INC_INT_HP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatValue[STATE_INC_INT_MP]		= CreateText( "", "STATE_INC_INT_MP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatValue[STATE_INC_INT_SP]		= CreateText( "", "STATE_INC_INT_SP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );

	m_pStatUpValue[STATE_CRITICAL]		= CreateText( "", "STATE_CRITICAL_UP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatUpValue[STATE_CRUSHING_BLOW]	= CreateText( "", "STATE_CRUSHING_BLOW_UP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatUpValue[STATE_MOVE_SPEED]	= CreateText( "", "STATE_MOVE_SPEED_UP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatUpValue[STATE_ATK_SPEED]		= CreateText( "", "STATE_ATK_SPEED_UP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatUpValue[STATE_EXP_MULTIPLE]	= CreateText( "", "EXP_MULTIPLE_UP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatUpValue[STATE_INC_PER_HP]	= CreateText( "", "STATE_INC_PER_HP_UP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatUpValue[STATE_INC_PER_MP]	= CreateText( "", "STATE_INC_PER_MP_UP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatUpValue[STATE_INC_PER_SP]	= CreateText( "", "STATE_INC_PER_SP_UP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatUpValue[STATE_INC_PER_CP]	= CreateText( "", "STATE_INC_PER_CP_UP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatUpValue[STATE_INC_INT_HP]	= CreateText( "", "STATE_INC_INT_HP_UP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatUpValue[STATE_INC_INT_MP]	= CreateText( "", "STATE_INC_INT_MP_UP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatUpValue[STATE_INC_INT_SP]	= CreateText( "", "STATE_INC_INT_SP_UP_VALUE", TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
}

void RnCharacterAdditionalWindow::PrintTooltip( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case STATE_OVER_RECT_CRITICAL:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT("CHARACTER_WINDOW_ADDITION_CRITICAL_TOOLTIP"), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			break;
		}
	case STATE_OVER_RECT_CRUSHING_BLOW:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT("CHARACTER_WINDOW_ADDITION_CRUSHING_BLOW_TOOLTIP"), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			break;
		}
	case STATE_OVER_RECT_MOVE_SPEED:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT("CHARACTER_WINDOW_ADDITION_MOVE_SPEED_TOOLTIP"), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			break;
		}
	case STATE_OVER_RECT_ATK_SPEED:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT("CHARACTER_WINDOW_ADDITION_ATK_SPEED_TOOLTIP"), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			break;
		}
	case STATE_OVER_RECT_EXP_MULTIPLE:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT("CHARACTER_WINDOW_ADDITION_EXP_MULTIPLE_TOOLTIP"), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			break;
		}
	case STATE_OVER_RECT_INC_PER_HP:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT("CHARACTER_WINDOW_ADDITION_INC_PER_HP_TOOLTIP"), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			break;
		}
	case STATE_OVER_RECT_INC_PER_MP:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT("CHARACTER_WINDOW_ADDITION_INC_PER_MP_TOOLTIP"), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			break;
		}
	case STATE_OVER_RECT_INC_PER_SP:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT("CHARACTER_WINDOW_ADDITION_INC_PER_SP_TOOLTIP"), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			break;
		}
	case STATE_OVER_RECT_INC_PER_CP:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT("CHARACTER_WINDOW_ADDITION_INC_PER_CP_TOOLTIP"), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			break;
		}
	case STATE_OVER_RECT_INC_INT_HP:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT("CHARACTER_WINDOW_ADDITION_INC_INT_HP_TOOLTIP"), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			break;
		}
	case STATE_OVER_RECT_INC_INT_MP:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT("CHARACTER_WINDOW_ADDITION_INC_INT_MP_TOOLTIP"), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			break;
		}
	case STATE_OVER_RECT_INC_INT_SP:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT("CHARACTER_WINDOW_ADDITION_INC_INT_SP_TOOLTIP"), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			break;
		}
	case STATE_QUESTION_BUTTON:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
				m_pInterface->BEGIN_COMMON_LINEINFO_MULTI(TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);

				NS_UITEXTUTIL::MULTILINETEXT multiLine 
					= NS_UITEXTUTIL::GetMLTextWithoutCRLF( ID2GAMEINTEXT("CHARACTER_ADDITION_WINDOW_POINT_QUESTION_BUTTON"), 360, pFont );

				for (size_t i=0; i < multiLine.size(); ++i)
					m_pInterface->ADD_COMMON_LINEINFO_MULTI(multiLine[i].strLine, NS_UITEXTCOLOR::LIGHTSKYBLUE);

				m_pInterface->END_COMMON_LINEINFO_MULTI();
			}
			break;
		}
	case ET_CONTROL_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if( m_bIsRefer )
					m_pInterface->SetVisibleReferCharacterAdditionalWindow(false);
				else
					m_pInterface->SetVisibleCharacterAdditionalWindow(false);					
			}
			break;
		}
	}
}