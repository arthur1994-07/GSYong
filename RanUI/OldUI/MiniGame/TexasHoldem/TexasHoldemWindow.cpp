#include "StdAfx.h"
#include "./TexasHoldemWindow.h"
#include "./TexasHoldemExchangeWindow.h"

#include "../../../../RanLogic/TexasHoldem/GLTexasHoldemSetting.h"

#include "../../../../RanLogicClient/TexasHoldem/GLTexasHoldemClient.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../RanLogicClient/Char/GLCharacter.h"

#include "../../Util/RnButton.h"
#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../enginelib/GUInterface/UIWindowTitle.h"
#include "../../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../enginelib/GUInterface/BasicTextBox.h"
#include "../../../../enginelib/GUInterface/BasicTextBoxEx.h"

#include "../../Util/CheckBox.h"

#include "../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../EngineLib/DxTools/d3dfont.h"
#include "../../../../EngineLib/DeviceMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace TexasHoldem
{

	TexasHoldemWindow::TexasHoldemWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIWindowEx(pInterface, pEngineDevice)
	, m_pGaeaClient(pGaeaClient)
	, m_pInterface(pInterface)
	, m_pMyChipText(NULL)
	, m_pBtnBuyChip(NULL)
	, m_pBtnRunGame(NULL)
	, m_pExchangeWindow(NULL)
	{
	}

	TexasHoldemWindow::~TexasHoldemWindow()
	{
	}

	void TexasHoldemWindow::OpenExchangeWindow()
	{
		GLTexasHoldemSetting* const pSettings = GLTexasHoldemSetting::getInstance();
		if ( NULL == pSettings )
			return;

		if( pSettings->CanChipPurchaseInGameMoney() == FALSE &&
			pSettings->CanChipPurchaseInPoint() == FALSE )
			return;

		ResizeWindow( D3DXVECTOR2( m_rcChangePos.sizeX, m_rcChangePos.sizeY ) );
		SetGlobalPos( GetGlobalPos() );

		m_pExchangeWindow->SetVisibleSingle( true );
		m_pBtnRunGame->SetNoUpdate( TRUE );
		m_pBtnBuyChip->SetNoUpdate( TRUE );
	}

	void TexasHoldemWindow::ClosedExchangeWindow()
	{
		m_pBtnRunGame->SetNoUpdate( FALSE );
		m_pBtnBuyChip->SetNoUpdate( FALSE );

		ResizeWindow( D3DXVECTOR2( m_rcBasicPos.sizeX, m_rcBasicPos.sizeY )  );
		SetGlobalPos( GetGlobalPos() );
	}

	const bool TexasHoldemWindow::SetChipMap()
	{
		GLTexasHoldemClient* pTexasHoldeMng = m_pGaeaClient->GetTexasHoldemManager();
		if( !pTexasHoldeMng )
			return false;

		std::vector< sChipPrice > chipmap = pTexasHoldeMng->GetChipMap();
		if( chipmap.empty() )
			return false;

		if( !m_pExchangeWindow->SetPurchaseChipList( chipmap ) )
			return false;

		return true;
	}

	void TexasHoldemWindow::SetFocusControl()
	{
		CUIWindowEx::SetFocusControl();
		m_pExchangeWindow->SetFocusControl();
	}

	void TexasHoldemWindow::ResetFocusControl()
	{
		CUIWindowEx::ResetFocusControl();
		m_pExchangeWindow->ResetFocusControl();
	}

	void TexasHoldemWindow::SetVisibleSingle( BOOL bVisible )
	{
		//! 텍사스 홀덤 메인 윈도우 닫으려고 할 경우 서버로 메시지 보냄.
		//! 서버에서 캐릭터 이동불가 풀고 클라이언트로 응답보내면
		//! SetVisible 함수 호출해서 윈도우를 닫는다.
		if( bVisible == false && IsVisible() )
		{
			GLTexasHoldemClient* pTexasHoldeMng = m_pGaeaClient->GetTexasHoldemManager();
			if( !pTexasHoldeMng )
				return;

			pTexasHoldeMng->CloseMainMenu();
		}
		return;
	}

	void TexasHoldemWindow::SetVisible( const bool bVisible )
	{
		//! 서버에서 조건 검사 후에만 클라이언트로 메시지 보내면 여기로 와야한다.
		//! 칩맵을 메니져에서 받아서 셋팅해야 한다.
		if( bVisible && m_pExchangeWindow && SetChipMap() )
		{
			CUIWindowEx::SetVisibleSingle( TRUE );
			CUIWindowEx::SetFocusControl();
		}
		else
		{
			CUIWindowEx::SetVisibleSingle( FALSE );
			m_pExchangeWindow->SetVisibleSingle( FALSE );
		}
	}

	void TexasHoldemWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
	{
		CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

		if( m_pExchangeWindow->IsVisible() )
			return;

		switch( ControlID )
		{
		case ID_TEXASHOLDEM_BUYCHIP_BUTTON:
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if( m_pExchangeWindow )
				{
					OpenExchangeWindow();
				}

			}
			break;
		case ID_TEXASHOLDEM_RUNGAME_BUTTON:
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				GLTexasHoldemClient* pTexasHoldeMng = m_pGaeaClient->GetTexasHoldemManager();
				if( pTexasHoldeMng )
					pTexasHoldeMng->StartGame();
			}
			break;
		}
	}

	void TexasHoldemWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
	{
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if( !pChar )
			return;

		m_pMyChipText->SetText( NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pChar->GetTexasHoldemChip(), 3, "," ) );

		CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	}


	void TexasHoldemWindow::CreateSubControl()
	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

		//! 상단 텍스트
		CBasicTextBox* pText =	new CBasicTextBox( m_pEngineDevice );
		if( pText )
		{
			pText->CreateSub( this, "MINIGAME_TEXASHOLDEM_WINDOW_TOP_TEXT", UI_FLAG_DEFAULT );
			pText->SetFont( pFont9Shadow );
			pText->SetText( ID2GAMEINTEXT("TEXASHOLDEM_WINDOW_TOP_TEXT", 0)  );
			RegisterControl( pText );
		}

		//! 보유 Chip 텍스트
		pText =	new CBasicTextBox( m_pEngineDevice );
		if( pText )
		{
			pText->CreateSub( this, "MINIGAME_TEXASHOLDEM_WINDOW_MYCHIP_TEXT", UI_FLAG_DEFAULT );
			pText->SetFont( pFont9Shadow );
			pText->SetText( ID2GAMEWORD("TEXASHOLDEM_WINDOW_TEXT", 1) );
			pText->SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
			RegisterControl( pText );
		}

		CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
		if( pLinBoxSmart )
		{
			pLinBoxSmart->CreateSub( this, "MINIGAME_TEXASHOLDEM_WINDOW_MYCHIP_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
			pLinBoxSmart->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_DARKGRAY" );
			RegisterControl( pLinBoxSmart );
		}

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		if( pTextBox )
		{
			pTextBox->CreateSub( this, "MINIGAME_TEXASHOLDEM_WINDOW_MYCHIP", UI_FLAG_DEFAULT, ID_TEXASHOLDEM_MY_CHIP );
			pTextBox->SetFont( pFont9 );
			pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
			RegisterControl( pTextBox );
			m_pMyChipText = pTextBox;
		}

		CBasicLineBoxSmart* pIcon = new CBasicLineBoxSmart(m_pEngineDevice);
		if( pIcon )
		{
			pIcon->CreateSub ( this, "MINIGAME_TEXASHOLDEM_WINDOW_MYCHIP_ICON", UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
			pIcon->CreateSubControl ( "MINIGAME_TEXASHOLDEM_EXCHANGE_ICON_CHIP" );
			pIcon->SetParent(pLinBoxSmart);
			RegisterControl ( pIcon );
		}

		//! 버튼
		{
			//! 스타일(폰트,색상,마우스오버색상,클릭색상
			RnButton::CreateArg argStyle;
			argStyle.pFont = pFont9;
			argStyle.defaultTextureKey    = "RNBUTTON_DEFAULT_TEXTURE";
			argStyle.mouseOverTextureKey  = "RNBUTTON_OVER_TEXTURE";
			argStyle.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

			//! Chip 구매
			m_pBtnBuyChip = new RnButton( m_pEngineDevice );
			if( m_pBtnBuyChip )
			{
				m_pBtnBuyChip->CreateSub( this, "MINIGAME_TEXASHOLDEM_WINDOW_BUYCHIP_BUTTON", UI_FLAG_DEFAULT, ID_TEXASHOLDEM_BUYCHIP_BUTTON );
				argStyle.text = ID2GAMEWORD("TEXASHOLDEM_WINDOW_TEXT", 2);
				m_pBtnBuyChip->CreateSubControl( argStyle );
				RegisterControl( m_pBtnBuyChip );
			}

			//! 게임 시작
			m_pBtnRunGame = new RnButton( m_pEngineDevice );
			if( m_pBtnRunGame )
			{
				m_pBtnRunGame->CreateSub( this, "MINIGAME_TEXASHOLDEM_WINDOW_RUNGAME_BUTTON", UI_FLAG_DEFAULT, ID_TEXASHOLDEM_RUNGAME_BUTTON );
				argStyle.text = ID2GAMEWORD("TEXASHOLDEM_WINDOW_TEXT", 3);
				m_pBtnRunGame->CreateSubControl( argStyle );	
				RegisterControl( m_pBtnRunGame );
			}
		}

		m_pExchangeWindow = new TexasHoldemExchangeWindow( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		if( m_pExchangeWindow )
		{
			m_pExchangeWindow->CreateSub( this, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE, MINIGAME_TEXASHOLDEM_EXCHANGE_WINDOW );
			m_pExchangeWindow->CreateBaseWindowLightGray( "MINIGAME_TEXASHOLDEM_EXCHANGE_WINDOW_BASE", ID2GAMEWORD("TEXASHOLDEM_EXCHANGE_WINDOW_TEXT", 0) );
			m_pExchangeWindow->CreateSubControl();

			D3DXVECTOR2 vMovableSize;
			vMovableSize.x = m_pExchangeWindow->GetLocalPos().right;
			vMovableSize.y = m_pExchangeWindow->GetLocalPos().sizeY;

			m_rcBasicPos = GetLocalPos();
			m_rcChangePos = m_rcBasicPos;
			m_rcChangePos.sizeX = vMovableSize.x;
			m_rcChangePos.right = m_rcChangePos.left + m_rcChangePos.sizeX;

			m_rcChangePos.sizeY = vMovableSize.y;
			m_rcChangePos.bottom = m_rcChangePos.top + m_rcChangePos.sizeY;

			m_pExchangeWindow->SetVisibleSingle(FALSE);
			RegisterControl( m_pExchangeWindow );
		}
	}

	void TexasHoldemWindow::CreateUIWindowAndRegisterOwnership( void )
	{
		Create( MINIGAME_TEXASHOLDEM_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE | UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X);
		CreateBaseWindowLightGray( "MINIGAME_TEXASHOLDEM_WINDOW_BASE", (char*)ID2GAMEWORD("TEXASHOLDEM_WINDOW_TEXT", 0) );
		CreateSubControl();
		m_pInterface->UiRegisterControl( this, true );
		m_pInterface->UiShowGroupFocus( MINIGAME_TEXASHOLDEM_WINDOW );
		CUIWindowEx::SetVisibleSingle( FALSE );
	}
}