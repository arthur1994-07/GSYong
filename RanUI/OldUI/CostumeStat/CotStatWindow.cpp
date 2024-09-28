#include "StdAfx.h"

#include "../../InnerInterface.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxEx.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBox.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBoxSmart.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/Item/GLItemCostumeAttach.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/CostumeStat/CotStatClient.h"

#include "../Item/InventoryInterface.h"

#include "CotStatProperty.h"
#include "CotStatWindow.h"


CCostumeStatWindow::CCostumeStatWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx( pInterface, pEngineDevice ), m_pSrcItem( NULL ), m_pDstItem( NULL ), m_pGaeaClient( pGaeaClient ), m_pFont10( NULL ), m_nItemPosX( -1 ), m_nItemPosY( -1 )
{
	INIT_ARRAY_NULL( m_pSubCtrl, CS_SUBCTRL_MAX );
}

CCostumeStatWindow::~CCostumeStatWindow( )
{

}

void CCostumeStatWindow::CretaeWindow()
{
	Create( COSTUME_STATS_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	CCostumeStatWindow::CreateBaseWindowLightGray( "COSTUME_STAT_WINDOW", (char*)ID2GAMEWORD("COSTUME_STAT_ASSIGN", 0) );
	CreateSubControl();
	SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
	m_pInterface->UiRegisterControl( this, true );
}

void CCostumeStatWindow::CreateSubControl()
{
	CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
	pLinBoxSmart->CreateSub( this, "COSTUME_WINDOW_BODY_MAIN_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLinBoxSmart->CreateSubControl( "COSTUME_WINDOW_BODY_MAIN_BACK_LINE" );
	RegisterControl( pLinBoxSmart );

	pLinBoxSmart->SetVisibleSingle(FALSE);

	pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
	pLinBoxSmart->CreateSub( this, "COSTUME_WINDOW_BODY_WHITE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLinBoxSmart->CreateSubControl( "COSTUME_WINDOW_BODY_WHITE_IMAGE" );
	RegisterControl( pLinBoxSmart );

	pLinBoxSmart->SetDiffuseAlpha(128);


	m_pSubCtrl[CS_SUBCTRL_PARTS] = new CCostumeParts( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	m_pSubCtrl[CS_SUBCTRL_PARTS]->CreateSub(this, "COSTUME_PARTS", UI_FLAG_XSIZE | UI_FLAG_YSIZE, IDCTRL( CS_SUBCTRL_PARTS ) );
	DCAST( CCostumeParts, m_pSubCtrl[CS_SUBCTRL_PARTS] )->CreateSubControl();		
	RegisterControl ( m_pSubCtrl[CS_SUBCTRL_PARTS] );

	m_pSubCtrl[CS_SUBCTRL_POINT] = new CCostumeHavePoint( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	m_pSubCtrl[CS_SUBCTRL_POINT]->CreateSub(this, "COSTUME_POINT", UI_FLAG_XSIZE | UI_FLAG_YSIZE, IDCTRL( CS_SUBCTRL_POINT ) );
	DCAST( CCostumeHavePoint, m_pSubCtrl[CS_SUBCTRL_POINT] )->CreateSubControl();		
	RegisterControl ( m_pSubCtrl[CS_SUBCTRL_POINT] );

	//////// 일반효과효과 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pSubCtrl[CS_SUBCTRL_STAT_GENERAL] = new CCostumeStats( m_pGaeaClient, m_pInterface, m_pEngineDevice, CCostumeStats::E_STAT_GENERAL_TYPE );
	m_pSubCtrl[CS_SUBCTRL_STAT_GENERAL]->CreateSub(this, "COSTUME_STAT", UI_FLAG_XSIZE | UI_FLAG_YSIZE, IDCTRL( CS_SUBCTRL_STAT_GENERAL ) );
	DCAST( CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_GENERAL] )->CreateSubControl("COSTUME_STAT_BODY_BACK");		

	CCostumeStatTitle* pTemp = DCAST( CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_GENERAL] )->GetTitleText();//GET_TITLE ( DCAST( CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_GENERAL] ) );
	
	if( pTemp ) {
		pTemp->SetTextTitle(ID2GAMEWORD("COSTUME_STAT_ASSIGN", 1), COSS_GN_MAX, 0 );
		pTemp->SetInvestPoint(0);
	}
	
	RegisterControl ( m_pSubCtrl[CS_SUBCTRL_STAT_GENERAL] );

	//////// 변화율효과 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pSubCtrl[CS_SUBCTRL_STAT_VARRATE] = new CCostumeStats( m_pGaeaClient, m_pInterface, m_pEngineDevice, CCostumeStats::E_STAT_VARRATE_TYPE );
	m_pSubCtrl[CS_SUBCTRL_STAT_VARRATE]->CreateSub(this, "COSTUME_STAT_VAR_RATE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, IDCTRL( CS_SUBCTRL_STAT_VARRATE ) );
	DCAST( CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_VARRATE] )->CreateSubControl("COSTUME_STAT_BODY_VARRATE_BACK");	

	pTemp = DCAST( CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_VARRATE] )->GetTitleText();//GET_TITLE ( DCAST( CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_VARRATE] ) );

	if( pTemp ) {
		pTemp->SetTextTitle(ID2GAMEWORD("COSTUME_STAT_ASSIGN", 2), COSS_VAR_MAX, 0 );
		pTemp->SetInvestPoint(0);
	}

	RegisterControl ( m_pSubCtrl[CS_SUBCTRL_STAT_VARRATE] );

	//////// 가산효과 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pSubCtrl[CS_SUBCTRL_STAT_ADD] = new CCostumeStats( m_pGaeaClient, m_pInterface, m_pEngineDevice, CCostumeStats::E_STAT_ADDED_TYPE );
	m_pSubCtrl[CS_SUBCTRL_STAT_ADD]->CreateSub(this, "COSTUME_STAT_ADD", UI_FLAG_XSIZE | UI_FLAG_YSIZE, IDCTRL( CS_SUBCTRL_STAT_ADD ) );
	DCAST( CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_ADD] )->CreateSubControl("COSTUME_STAT_BODY_ADD_BACK");		

	pTemp = DCAST( CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_ADD] )->GetTitleText();//GET_TITLE ( DCAST( CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_ADD] ) );

	if( pTemp ) {
		pTemp->SetTextTitle(ID2GAMEWORD("COSTUME_STAT_ASSIGN", 3), COSS_ADDED_MAX, 0 );
		pTemp->SetInvestPoint(0);
	}

	RegisterControl ( m_pSubCtrl[CS_SUBCTRL_STAT_ADD] );

	//////// 설명 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_pSubCtrl[CS_SUBCTRL_STAT_EXPLAIN] = new CCostumeStatExplain( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	m_pSubCtrl[CS_SUBCTRL_STAT_EXPLAIN]->CreateSub(this, "COSTUME_STAT_EXPLAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE, IDCTRL( CS_SUBCTRL_STAT_EXPLAIN ) );
	DCAST( CCostumeStatExplain, m_pSubCtrl[CS_SUBCTRL_STAT_EXPLAIN] )->CreateSubControl();		
	RegisterControl ( m_pSubCtrl[CS_SUBCTRL_STAT_EXPLAIN] );

	//////// 버튼들 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	RnButton* pStatBtn = NULL;
	RnButton::CreateArg arg;

	arg.pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_BOLD_FLAG | D3DFONT_SHADOW );;
	arg.defaultTextureKey    = "CHARACTER_WINDOW_STAT_RED_BUTTON_NORMAL_TEXTUREINFO";
	arg.mouseOverTextureKey  = "CHARACTER_WINDOW_STAT_RED_BUTTON_MOUSEOVER_TEXTUREINFO";
	arg.mouseClickTextureKey = "CHARACTER_WINDOW_STAT_RED_BUTTON_MOUSECLICK_TEXTUREINFO";
	arg.inactiveTextureKey   = "CHARACTER_WINDOW_STAT_RED_BUTTON_NORMAL_TEXTUREINFO";
	arg.text				 = ID2GAMEWORD("CHARACTER_STAT_INIT");

	m_pSubCtrl[CS_SUBCTRL_BTN_INIT ] = new RnButton( m_pEngineDevice );
	m_pSubCtrl[CS_SUBCTRL_BTN_INIT ]->CreateSub(this,"COSTUME_STAT_INIT_BUTTON",UI_FLAG_DEFAULT, IDCTRL( CS_SUBCTRL_BTN_INIT ) );
	DCAST( RnButton, m_pSubCtrl[CS_SUBCTRL_BTN_INIT ] )->CreateSubControl(arg);
	RegisterControl( m_pSubCtrl[CS_SUBCTRL_BTN_INIT ] );

	arg.text = ID2GAMEWORD("CHARACTER_STAT_APPLY");

	m_pSubCtrl[CS_SUBCTRL_BTN_APPLY ] = new RnButton( m_pEngineDevice );
	m_pSubCtrl[CS_SUBCTRL_BTN_APPLY ]->CreateSub(this,"COSTUME_STAT_APPLY_BUTTON",UI_FLAG_DEFAULT, IDCTRL( CS_SUBCTRL_BTN_APPLY ) );
	DCAST( RnButton, m_pSubCtrl[CS_SUBCTRL_BTN_APPLY ] )->CreateSubControl(arg);
	RegisterControl( m_pSubCtrl[CS_SUBCTRL_BTN_APPLY ] );

	arg.defaultTextureKey    = "RAN_RNBUTTON_DEFAULT_TEXTURE";
	arg.mouseOverTextureKey  = "RAN_RNBUTTON_OVER_TEXTURE";
	arg.mouseClickTextureKey = "RAN_RNBUTTON_CLICK_TEXTURE";
	arg.inactiveTextureKey   = "RAN_RNBUTTON_DEFAULT_TEXTURE";

	arg.text = ID2GAMEWORD("CHAT_OK_BUTTON", 1);

	m_pSubCtrl[CS_SUBCTRL_BTN_CANCEL ] = new RnButton( m_pEngineDevice );
	m_pSubCtrl[CS_SUBCTRL_BTN_CANCEL ]->CreateSub(this,"COSTUME_STAT_CANCEL_BUTTON",UI_FLAG_DEFAULT, IDCTRL( CS_SUBCTRL_BTN_CANCEL) );
	DCAST( RnButton, m_pSubCtrl[CS_SUBCTRL_BTN_CANCEL ] )->CreateSubControl(arg);
	RegisterControl( m_pSubCtrl[CS_SUBCTRL_BTN_CANCEL ] );
	

}

void CCostumeStatWindow::ResetPoint()
{

}

bool CCostumeStatWindow::SetItemInfo( const SINVENITEM* pSrc, const SINVENITEM* pDest )
{
	if( pSrc == NULL || pDest == NULL ) return false;

	m_pSrcItem = pSrc;
	m_pDstItem = pDest;

	const SITEM* pSrcItem = GLogicData::GetInstance().GetItem( m_pSrcItem->sItemCustom.GetNativeID() );
	const SITEM* pDstItem = GLogicData::GetInstance().GetItem( m_pDstItem->sItemCustom.GetNativeID() );

	if( pSrcItem == NULL || pDstItem == NULL ) return false;

	int iWordIndex = 0;
	
	if( m_pSubCtrl[CS_SUBCTRL_PARTS] )
	{
		DCAST( CCostumeParts, m_pSubCtrl[CS_SUBCTRL_PARTS] )->SetPartsName( ID2GAMEWORD("ITEM_INFO_SET_PARTS", static_cast<int>(pDstItem->sSuitOp.emSuit) ) );
	}

	WORD  wLvPoint		 = COSSDATA.GetPartsEnableMaxPt( pDstItem->sSuitOp.emSuit, (WORD)pSrcItem->sDrugOp.dwCureVolume);

	if(wLvPoint == 0 )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ERROR_COSTUME_MSG5") );
		return false;
	}

	if( m_pSubCtrl[CS_SUBCTRL_POINT] )
	{
		DCAST( CCostumeHavePoint, m_pSubCtrl[CS_SUBCTRL_POINT] )->SetHavePoint(static_cast<__int64>(wLvPoint));
	}

	CCostumeStatClient* csClient = m_pGaeaClient->GetCostumeClient();

	if( csClient ) csClient->RestMember();

	const COSTUMESTATSDATA::CPartsDt::mPsData* const pData = COSSDATA.GetStatInfo( pDstItem->sSuitOp.emSuit );
	
	if( !pData ) { 
		DCAST(CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_GENERAL])->ResetAll();
		DCAST(CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_ADD])->ResetAll();
		DCAST(CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_VARRATE])->ResetAll();
		
		return false;
	}

	DCAST(CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_GENERAL])->SetDefaultInfo(pDstItem->sSuitOp.emSuit);
	DCAST(CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_ADD])->SetDefaultInfo(pDstItem->sSuitOp.emSuit);
	DCAST(CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_VARRATE])->SetDefaultInfo(pDstItem->sSuitOp.emSuit);
			
	if( csClient )
	{
		csClient->SetSuit(pDstItem->sSuitOp.emSuit);
		csClient->SetHavePoint( wLvPoint );
		csClient->SetItem(pSrc, pDest);
	}

	if( m_pDstItem )
	{
		//m_pInterface->UiShowGroupFocus( INVENTORY_WINDOW_RENEW );
		IInventory* pInventory = m_pInterface->GetInventory();

		if ( pInventory )
		{
			m_nItemPosX = (int)m_pDstItem->wPosX;
			m_nItemPosY = (int)m_pDstItem->wPosY;

			pInventory->ChangeInvenPage( m_nItemPosX, m_nItemPosY );
			pInventory->SetWindowOpenBox( m_nItemPosX, m_nItemPosY, TRUE );
		}
	}

	return true;
}

void CCostumeStatWindow::UpdateHavePoint()
{
	if( m_pSubCtrl[CS_SUBCTRL_STAT_GENERAL] &&
		m_pSubCtrl[CS_SUBCTRL_STAT_ADD]     &&
		m_pSubCtrl[CS_SUBCTRL_STAT_VARRATE] )
	{
		int iTotal = DCAST(CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_GENERAL])->GetInvestPt() +
			DCAST(CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_ADD])->GetInvestPt() +
			DCAST(CCostumeStats, m_pSubCtrl[CS_SUBCTRL_STAT_VARRATE])->GetInvestPt();


		CCostumeStatClient* csClient = m_pGaeaClient->GetCostumeClient();

		if( csClient ) csClient->SetSubPoint(iTotal);

		DCAST( CCostumeHavePoint, m_pSubCtrl[CS_SUBCTRL_POINT] )->SetSubPoint(iTotal );
	}
}

void CCostumeStatWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
}

void CCostumeStatWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch( dwMsg )
	{
	case CCostumeStats::MSG_UPDATE_POINT :
		{
			if( IDCTRL(CS_SUBCTRL_STAT_GENERAL) || 
				IDCTRL(CS_SUBCTRL_STAT_VARRATE) ||
				IDCTRL(CS_SUBCTRL_STAT_ADD) )
			{
				UpdateHavePoint();
			}
		}
		break;
	}

	/////////// 버튼들 /////////////////////////////////////////////////////////////////////////////////
	switch( ControlID )
	{
	case IDCTRL( CS_SUBCTRL_BTN_INIT  ) :
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				SetItemInfo( m_pSrcItem, m_pDstItem );
			}
		}
		break;
	case IDCTRL( CS_SUBCTRL_BTN_APPLY ) :
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				CCostumeStatClient* csClient = m_pGaeaClient->GetCostumeClient();

				int iMaxPt    = csClient->GetHavePoint();
				int iRemain   = csClient->GetRemainPoint();
				int iInvestPt = csClient->GetSubPoint();

				if( iRemain == 0 ) {
					// 포인트 전부 분배
					m_pInterface->DoModal( ID2GAMEINTEXT("MODAL_REQ_COSTUME_STAT_APPLY_MSG1"), UI::MODAL_QUESTION, UI::OKCANCEL, UI::MODAL_COSTUME_STAT_APPLY );
				}
				else 
				if( iRemain == iMaxPt ) {				
					// 포인트 분배 한개도 안함
					m_pInterface->DoModal( ID2GAMEINTEXT("MODAL_REQ_COSTUME_STAT_APPLY_MSG3"), UI::MODAL_WARNING, UI::OK );
				}
				else
				if( iRemain  > 0 ) {
					// 포인트 분배 남아있음.
					m_pInterface->DoModal( ID2GAMEINTEXT("MODAL_REQ_COSTUME_STAT_APPLY_MSG2"), UI::MODAL_QUESTION, UI::OKCANCEL, UI::MODAL_COSTUME_STAT_APPLY );
				}
				
			}
		}
		break;
	case ET_CONTROL_BUTTON :
	case IDCTRL( CS_SUBCTRL_BTN_CANCEL) :
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if( m_pInterface ) m_pInterface->OpenCostumeStatWindow(NULL, NULL);
			}
		}
		break;
	}

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void CCostumeStatWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle(bVisible);
}

void CCostumeStatWindow::CloseWindow( )
{
	if( m_nItemPosX >= 0 && m_nItemPosY >= 0 )
	{
		//m_pInterface->UiShowGroupFocus( INVENTORY_WINDOW_RENEW );
		IInventory* pInventory = m_pInterface->GetInventory();

		if ( pInventory )
		{
			pInventory->SetWindowOpenBox( m_nItemPosX, m_nItemPosY, FALSE );
		}
	}

	m_pSrcItem = NULL;
	m_pDstItem = NULL;

	m_nItemPosX = -1;
	m_nItemPosY = -1;
}
