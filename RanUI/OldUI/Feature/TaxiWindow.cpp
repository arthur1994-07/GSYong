#include "StdAfx.h"

#include "TaxiWindow.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DebugSet.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../InnerInterface.h"
#include "../../../RanLogic/Transport/GLTaxiStation.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTaxiWindow::CTaxiWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx ( pInterface, pEngineDevice )
    , m_pGaeaClient ( pGaeaClient )
    , m_nSelectMap ( -1 )
    , m_nSelectStop( -1 )
    , m_pMapList ( NULL )
    , m_pMapListScrollBar ( NULL )
    , m_pStopList ( NULL )
    , m_pStopListScrollBar ( NULL )
    , m_pMoveButton ( NULL )
    , m_pMapTitle ( NULL )
    , m_pStopTitle ( NULL )
    , m_pMoneyTextBox( NULL )
    , m_wPosX(0)
    , m_wPosY(0)
{
}

CTaxiWindow::~CTaxiWindow ()
{
}

void CTaxiWindow::InitData()
{
	m_nSelectMap = -1;
	m_nSelectStop = -1;
	SetMoney( 0 );

	m_pMapList->ClearText ();    
	m_pStopList->ClearText ();
}

void CTaxiWindow::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CreateControl ( "TAXI_TOP_BACK_L" );
	CreateControl ( "TAXI_TOP_BACK_R" );
	CreateControl ( "TAXI_MIDDLE_BACK_L" );
	CreateControl ( "TAXI_MIDDLE_BACK_R" );
	CreateControl ( "TAXI_BOTTOM_BACK_L" );
	CreateControl ( "TAXI_BOTTOM_BACK_R" );

	m_pMapTitle = CreateStaticControl ( "TAXI_MAP_TITLE", pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pMapTitle->SetText ( ID2GAMEWORD("TAXI_TITLE",0) );
	m_pStopTitle = CreateStaticControl ( "TAXI_STOP_TITLE", pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pStopTitle->SetText ( ID2GAMEWORD("TAXI_TITLE",1) );

	//	텍스트 박스
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "TAXI_MAP_LIST_BACK" );
		RegisterControl ( pBasicLineBox );

		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "TAXI_MAP_LIST", UI_FLAG_DEFAULT, TAXI_MAP_LIST );
		pTextBox->SetFont ( pFont );		
		pTextBox->SetLineInterval ( 3.0f );
		pTextBox->SetSensitive ( true );
		RegisterControl ( pTextBox );
		m_pMapList = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, TAXI_MAP_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "TAXI_MAP_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pMapListScrollBar = pScrollBar;
	}

	//	텍스트 박스
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "TAXI_STOP_LIST_BACK" );
		RegisterControl ( pBasicLineBox );

		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "TAXI_STOP_LIST", UI_FLAG_DEFAULT, TAXI_STOP_LIST );
		pTextBox->SetFont ( pFont );		
		pTextBox->SetLineInterval ( 3.0f );
		pTextBox->SetSensitive ( true );
		RegisterControl ( pTextBox );
		m_pStopList = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, TAXI_STOP_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "TAXI_STOP_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pStopListScrollBar = pScrollBar;
	}	


	CUIControl* pControl = CreateControl ( "TAXI_MONEY_BACK" );
	pControl->SetAlignFlag ( UI_FLAG_BOTTOM );

	pControl = CreateControl ( "TAXI_MONEY_IMAGE" );
	pControl->SetAlignFlag ( UI_FLAG_BOTTOM );

	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	m_pMoneyTextBox = CreateStaticControl ( "TAXI_MONEY_TEXT", pFont8, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pMoneyTextBox->SetAlignFlag ( UI_FLAG_BOTTOM );

	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "TAXI_MOVEBUTTON", UI_FLAG_DEFAULT, TAXI_MOVEBUTTON );
	pButton->CreateFlip ( "TAXI_MOVEBUTTON_F", CBasicButton::MOUSEIN_FLIP );	
	RegisterControl ( pButton );
	m_pMoveButton = pButton;
}

void	CTaxiWindow::LoadMapList ()
{
	InitData();
	
	GLTaxiStation& sTaxiStation = GLTaxiStation::GetInstance();

	const DWORD dwNum = sTaxiStation.GetTaxiMapNum();
	for ( DWORD i = 0; i < dwNum; i++ )
	{
		STAXI_MAP* pTaxiMap = sTaxiStation.GetTaxiMap(i);
		if ( !pTaxiMap )
		{
			GASSERT ( 0 && "pTaxiMap를 찾을수 없습니다." );
			continue;
		}

		int nIndex = m_pMapList->AddText ( pTaxiMap->strMAP.c_str() );
		m_pMapList->SetTextData ( nIndex, i );
	}
}

void	CTaxiWindow::LoadStopList ()
{
	if ( m_nSelectMap < 0 ) return ;

	GLTaxiStation& sTaxiStation = GLTaxiStation::GetInstance();

	const DWORD dwIndex = m_pMapList->GetTextData ( m_nSelectMap );
	STAXI_MAP* pTaxiMap = sTaxiStation.GetTaxiMap( dwIndex );
	if ( !pTaxiMap )
	{
		GASSERT ( 0 && "pTaxiMap를 찾을수 없습니다." );
		return ;
	}

	m_pStopList->ClearText();

	const DWORD dwNum = pTaxiMap->GetStationNum();
	for ( DWORD i = 0; i < dwNum; i++ )
	{
		STAXI_STATION* pSTATION = pTaxiMap->GetStation(i);		
		if ( !pSTATION )
		{
			GASSERT ( 0 && "STATION을 찾을 수 없습니다." );
			continue;
		}
		int nIndex = m_pStopList->AddText ( pSTATION->strSTATION.c_str() );

		m_pStopList->SetTextData ( nIndex, pSTATION->dwLINKID );
	}	
}

void	CTaxiWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	{
		m_pMapList->SetUseOverColor ( FALSE );
		if ( 0 <= m_nSelectMap && m_nSelectMap < m_pMapList->GetCount () )
			m_pMapList->SetUseTextColor ( m_nSelectMap, FALSE );
	}

	{
		m_pStopList->SetUseOverColor ( FALSE );
		if ( 0 <= m_nSelectStop && m_nSelectStop < m_pStopList->GetCount () )
			m_pStopList->SetUseTextColor ( m_nSelectStop, FALSE );
	}

	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	{
		if ( 0 <= m_nSelectMap && m_nSelectMap < m_pMapList->GetCount () )
		{
			m_pMapList->SetUseTextColor ( m_nSelectMap, TRUE );
			m_pMapList->SetTextColor ( m_nSelectMap, NS_UITEXTCOLOR::ORANGE );
		}

		{
			CBasicScrollThumbFrame* const pThumbFrame = m_pMapListScrollBar->GetThumbFrame ();

			const int nTotalLine = m_pMapList->GetTotalLine ();
			const int nLinePerOneView = m_pMapList->GetVisibleLine ();
			CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
			pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
			if ( nLinePerOneView < nTotalLine )
			{
				const int nMovableLine = nTotalLine - nLinePerOneView;
				float fPercent = pThumbFrame->GetPercent ();
				int nPos = (int)floor(fPercent * nMovableLine);
				m_pMapList->SetCurLine ( nPos );			
			}
		}
	}

	{
		if ( 0 <= m_nSelectStop && m_nSelectStop < m_pStopList->GetCount () )
		{
			m_pStopList->SetUseTextColor ( m_nSelectStop, TRUE );
			m_pStopList->SetTextColor ( m_nSelectStop, NS_UITEXTCOLOR::ORANGE );
		}

		{
			CBasicScrollThumbFrame* const pThumbFrame = m_pStopListScrollBar->GetThumbFrame ();

			const int nTotalLine = m_pStopList->GetTotalLine ();
			const int nLinePerOneView = m_pStopList->GetVisibleLine ();
			CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
			pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
			if ( nLinePerOneView < nTotalLine )
			{
				const int nMovableLine = nTotalLine - nLinePerOneView;
				float fPercent = pThumbFrame->GetPercent ();
				int nPos = (int)floor(fPercent * nMovableLine);
				m_pStopList->SetCurLine ( nPos );			
			}
		}
	}
}

void	CTaxiWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
	case TAXI_MAP_LIST:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{				
				const int nIndex = m_pMapList->GetSelectPos ();
				if ( nIndex < 0 || m_pMapList->GetCount () <= nIndex ) return ;

				m_pMapList->SetUseOverColor ( TRUE );
				m_pMapList->SetOverColor ( nIndex, NS_UITEXTCOLOR::DARKORANGE );

				if ( UIMSG_LB_UP & dwMsg )
				{
					m_nSelectMap = nIndex;
					m_nSelectStop = -1;
					SetMoney( 0 );
					LoadStopList ();
				}
			}
		}
		break;

	case TAXI_STOP_LIST:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{				
				const int nIndex = m_pStopList->GetSelectPos ();
				if ( nIndex < 0 || m_pStopList->GetCount () <= nIndex ) return ;

				m_pStopList->SetUseOverColor ( TRUE );
				m_pStopList->SetOverColor ( nIndex, NS_UITEXTCOLOR::DARKORANGE );

				if ( UIMSG_LB_UP & dwMsg )
				{
					m_nSelectStop = nIndex;
					SetCalcMoney();
				}
			}
		}
		break;

	case TAXI_MOVEBUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( m_nSelectStop < 0 || m_nSelectMap < 0 ) return ;
				
				m_pGaeaClient->GetCharacter()->ReqTaxiStation ( static_cast<WORD>(m_wPosX), static_cast<WORD>(m_wPosY), m_nSelectMap, m_nSelectStop );

				m_pInterface->UiHideGroup ( GetWndID () );
			}
		}
		break;
	}
}

void CTaxiWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if ( bVisible )	LoadMapList();
}

void CTaxiWindow::SetCalcMoney()
{
	if ( m_nSelectMap < 0 || m_nSelectStop < 0  ) return ;

	LONGLONG lnCharge = m_pGaeaClient->GetCharacter()->GetCalcTaxiCharge( m_nSelectMap, m_nSelectStop );
	SetMoney ( lnCharge );

	return;
}

void CTaxiWindow::SetMoney( LONGLONG lnMoney )
{
	CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( lnMoney, 3, "," );
	m_pMoneyTextBox->SetOneLineText ( strTemp );
}