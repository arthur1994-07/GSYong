#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../RanLogic/RANPARAM.h"

#include "../../OuterInterface.h"
#include "SelectChinaAreaPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectChinaAreaPage::CSelectChinaAreaPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIOuterWindow(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
{
}

CSelectChinaAreaPage::~CSelectChinaAreaPage ()
{
}

void CSelectChinaAreaPage::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "SELECT_AREA_UP_LINEBOX" );
	RegisterControl ( pBasicLineBox );

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "SELECT_AREA_STATIC" );
	pTextBox->SetFont ( pFont9 );
	pTextBox->AddText ( ID2GAMEEXTEXT("SELECT_AREA_STATIC") );
	RegisterControl ( pTextBox );

	pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "SELECT_AREA_DN_LINEBOX" );
	RegisterControl ( pBasicLineBox );

	CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, SELECT_AREA_BUTTON_00 );
	pTextButton->CreateBaseButton ( "SELECT_AREA_BUTTON_01", CBasicTextButton::SIZE22, CBasicButton::CLICK_FLIP, RANPARAM::China_Region[0].strName.GetString() );
	pTextButton->SetFlip ( TRUE );
	RegisterControl ( pTextButton );

	pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, SELECT_AREA_BUTTON_01 );
	pTextButton->CreateBaseButton ( "SELECT_AREA_BUTTON_02", CBasicTextButton::SIZE22, CBasicButton::CLICK_FLIP, RANPARAM::China_Region[1].strName.GetString() );
	pTextButton->SetFlip ( TRUE );
	RegisterControl ( pTextButton );

	pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, SELECT_AREA_BUTTON_02 );
	pTextButton->CreateBaseButton ( "SELECT_AREA_BUTTON_03", CBasicTextButton::SIZE22, CBasicButton::CLICK_FLIP, RANPARAM::China_Region[2].strName.GetString() );
	pTextButton->SetFlip ( TRUE );
	RegisterControl ( pTextButton );
/*
	pTextButton = new CBasicTextButton(m_pRenderDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, SELECT_AREA_BUTTON_03 );
	pTextButton->CreateBaseButton ( "SELECT_AREA_BUTTON_04", CBasicTextButton::SIZE22, CBasicButton::CLICK_FLIP, RANPARAM::China_Region[3].strName.GetString() );
	pTextButton->SetFlip ( TRUE );
	RegisterControl ( pTextButton );

	pTextButton = new CBasicTextButton(m_pRenderDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, SELECT_AREA_BUTTON_04 );
	pTextButton->CreateBaseButton ( "SELECT_AREA_BUTTON_05", CBasicTextButton::SIZE22, CBasicButton::CLICK_FLIP, RANPARAM::China_Region[4].strName.GetString() );
	pTextButton->SetFlip ( TRUE );
	RegisterControl ( pTextButton );

// 중국지역 추가 
	pTextButton = new CBasicTextButton(m_pRenderDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, SELECT_AREA_BUTTON_05 );
	pTextButton->CreateBaseButton ( "SELECT_AREA_BUTTON_06", CBasicTextButton::SIZE22, CBasicButton::CLICK_FLIP, RANPARAM::China_Region[5].strName.GetString() );
	pTextButton->SetFlip ( TRUE );
	RegisterControl ( pTextButton );
 */
	pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, SELECT_AREA_QUIT );
	pTextButton->CreateBaseButton( "SELECT_AREA_QUIT", CBasicTextButton::SIZE19, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "SELECT_SERVER_CONNECTQUIT", 1 ) );
	pTextButton->SetShortcutKey( DIK_ESCAPE );
	RegisterControl( pTextButton );

	CreateControl( "SELECT_AREA_BOTTOM" );
}

void CSelectChinaAreaPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case SELECT_AREA_QUIT:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->DoModal ( ID2GAMEINTEXT("CLOSE_GAME"), UI::MODAL_QUESTION, UI::OKCANCEL, OUTER_MODAL_CLOSEGAME );
				SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus 문제로 메세지 삭제
			}
		}
		break;
// 중국지역 추가 
	case SELECT_AREA_BUTTON_00:
	case SELECT_AREA_BUTTON_01:
	case SELECT_AREA_BUTTON_02:
//	case SELECT_AREA_BUTTON_03:
//	case SELECT_AREA_BUTTON_04:
//	case SELECT_AREA_BUTTON_05:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				SetParamChinaRegion( ControlID - SELECT_AREA_BUTTON_00 );
				ConnectLoginServer();

				m_pInterface->ToSelectServerPage( GetWndID() );
			}
		}
		break;
	}
}

void CSelectChinaAreaPage::SetParamChinaRegion( INT nIndex )
{
	if( 0 > nIndex || nIndex >= RANPARAM::MAX_CHINA_REGION )
	{
		GASSERT( 0 && "CSelectChinaAreaPage::SetParamChinaRegion()" );
		return ;
	}

	RANPARAM::nChinaRegion = nIndex;

	StringCchCopy( RANPARAM::LoginAddress, STRING_NUM_128, RANPARAM::China_Region[nIndex].LoginAddress.GetString() );

	for( int i=0 ; ; ++i )
	{
		if( RANPARAM::China_Region[nIndex].HttpAddressTable[i].IsEmpty() ) 
			break;

		StringCchCopy( RANPARAM::HttpAddressTable[i], STRING_NUM_128, RANPARAM::China_Region[nIndex].HttpAddressTable[i].GetString() );
	}

	RANPARAM::strNewsURL = RANPARAM::China_Region[nIndex].strNewsURL;
	RANPARAM::strImageURL = RANPARAM::China_Region[nIndex].strImageURL;
}

void CSelectChinaAreaPage::ConnectLoginServer()
{
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
    if (pGlobalStage->GetNetClient()->ConnectLoginServer(RANPARAM::LoginAddress, RANPARAM::LoginServerPort) <= NET_ERROR )
	{
		pGlobalStage->CloseGame ();
	}
	else
	{
		pGlobalStage->GetNetClient()->SndVersion();
		pGlobalStage->GetNetClient()->SndReqServerInfo();
	}
}