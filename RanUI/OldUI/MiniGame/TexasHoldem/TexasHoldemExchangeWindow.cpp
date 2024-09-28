#include "StdAfx.h"
#include "TexasHoldemExchangeWindow.h"
#include "TexasHoldemExchangeList.h"

#include "../../../../RanLogic/TexasHoldem/GLTexasHoldemSetting.h"

#include "../../../../RanLogicClient/TexasHoldem/GLTexasHoldemClient.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../../engineLib/GUInterface/GameTextControl.h"
#include "../../../../enginelib/GUInterface/UIWindowTitle.h"
#include "../../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../enginelib/GUInterface/BasicTextBox.h"
#include "../../../../engineLib/GUInterface/BasicTextBoxEx.h"

#include "../../Util/CheckBox.h"
#include "../../Util/MessageBox.h"

#include "../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../EngineLib/DxTools/d3dfont.h"
#include "../../../../EngineLib/DeviceMan.h"

// ----------------------------------------------------------------------------
#include "../../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
namespace TexasHoldem
{

	TexasHoldemExchangeWindow::TexasHoldemExchangeWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIWindowEx(pInterface, pEngineDevice)
	, m_pGaeaClient(pGaeaClient)
	, m_pInterface(pInterface)
	, m_pMyMoneyText(NULL)
	, m_pBtnBuyChip(NULL)
	, m_pBtnCancle(NULL)
	, m_pBuyChipMsgBox(NULL)
	, m_pRadioMoney(NULL)
	, m_pRadioPoint(NULL)
	, m_pExchangeList(NULL)
	, m_pCurrentTap(NULL)
	{
	}

	TexasHoldemExchangeWindow::~TexasHoldemExchangeWindow()
	{
	}

	void TexasHoldemExchangeWindow::OpenBuyChipMessageBox()
	{
		if( m_pBuyChipMsgBox && CheckInvenMoney() )
		{
			m_pBuyChipMsgBox->OpenMessageBox( ID2GAMEWORD("MODAL_TITLE", 1)
			, ID2GAMEINTEXT("TEXASHOLDEM_EXCHANGE_WINDOW_BUYCHIP_MSG_TEXT"), CMessageBox::TYPE_YESNO, ID_TEXASHOLDEM_EXCHANGE_BUYCHIP_BUTTON );

			m_pExchangeList->SetNoUpdate(TRUE);
		}
	}

	bool TexasHoldemExchangeWindow::CheckInvenMoney()
	{
		GLTexasHoldemClient* pTexasMng = m_pGaeaClient->GetTexasHoldemManager();
		if( !pTexasMng )
			return false;

		const eExchange_Type type = m_pExchangeList->GetSelectType();
		const int nSelected = m_pExchangeList->GetSelectPos();
		if( nSelected < 0 )
			return false;

		std::vector<sChipPrice> _list = m_pExchangeList->GetList();

		if( _list.size() < (unsigned)nSelected )
			return false;

		const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( !pCharacter )
			return false;

		switch( type )
		{
		case eExchange_Gamemoney:
			if( _list.at( nSelected ).GameMoney > pCharacter->GetInvenMoney() )
			{
				pTexasMng->DisplayMsg( eLackOfGameMoney );
				return false;
			}
			break;
		case eExchange_Ranpoint:
			if( _list.at( nSelected ).Point > pCharacter->PointShopPoint() )
			{
				pTexasMng->DisplayMsg( eLackOfPoint );
				return false;
			}
			break;
		}

		return true;
	}

	void TexasHoldemExchangeWindow::EventSelectedTap( UIGUID ControlID )
	{
		switch ( ControlID )
		{	
		case ID_TEXASHOLDEM_EXCHANGE_MONEY_RADIO:
			if( m_pExchangeList )
				m_pExchangeList->SetSeletedRadioType( eExchange_Gamemoney );
			break;
		case ID_TEXASHOLDEM_EXCHANGE_POINT_RADIO:
			if( m_pExchangeList )
				m_pExchangeList->SetSeletedRadioType( eExchange_Ranpoint );
			break;
		default:
			{
			}
		}
	}

	const bool TexasHoldemExchangeWindow::SetPurchaseChipList( const std::vector< sChipPrice >& chipMap )
	{
		if( m_pExchangeList->SetList( chipMap ) )
			return true;

		return false;
	}

	void TexasHoldemExchangeWindow::RequestExchangeChip()
	{
		GLTexasHoldemClient* pTexasMng = m_pGaeaClient->GetTexasHoldemManager();
		if( !pTexasMng )
			return;

		const eExchange_Type type = m_pExchangeList->GetSelectType();
		const int nSelected = m_pExchangeList->GetSelectPos();
		if( nSelected < 0 )
			return;

		std::vector<sChipPrice> _list = m_pExchangeList->GetList();
		std::vector<sChipPrice> _Mnglist = pTexasMng->GetChipMap();

		if( _list.size() < (unsigned)nSelected || _Mnglist.size() < (unsigned)nSelected )
			return;

		//! 칩 구매
		switch( type )
		{
			case eExchange_Gamemoney:
				if( _list.at( nSelected ).GameMoney == _Mnglist.at( nSelected ).GameMoney )
					pTexasMng->RequestChipPurchaseInGameMoney( _list.at( nSelected ).Chip );
				break;
			case eExchange_Ranpoint:
				if( _list.at( nSelected ).Point == _Mnglist.at( nSelected ).Point )
					pTexasMng->RequestChipPurchaseInPoint( _list.at( nSelected ).Chip );
				break;
		}
	}

	void TexasHoldemExchangeWindow::SetVisibleSingle( BOOL bVisible )
	{
		CUIWindowEx::SetVisibleSingle( bVisible );

		//! 게임머니 또는 포인트로 환전이 불가능 할 경우 버튼을 막는다;
		if( IsVisible() )
		{
			ResetList();
		}
		//! 환전 윈도우가 닫히면
		//! 메인 윈도우에 알려줘야 한다.
		//! 마우스 이벤트를 받을수 있도록 한다.
		else
		{
			m_pInterface->GetTexasHoldemWindow()->ClosedExchangeWindow();
		}
	}

	void TexasHoldemExchangeWindow::ResetList()
	{
		GLTexasHoldemSetting* pSettings = GLTexasHoldemSetting::getInstance();
		if( !pSettings )
			return;

		if( pSettings->CanChipPurchaseInGameMoney() == FALSE &&
			pSettings->CanChipPurchaseInPoint() == FALSE )
		{
			m_pRadioPoint->SetVisibleSingle( FALSE );
			m_pRadioMoney->SetVisibleSingle( FALSE );
			m_pExchangeList->ClearText();
		}
		else if( pSettings->CanChipPurchaseInGameMoney() == FALSE )
		{
			m_pRadioMoney->SetVisibleSingle( FALSE );
			TapSelect( ID_TEXASHOLDEM_EXCHANGE_POINT_RADIO );
		}
		else if( pSettings->CanChipPurchaseInPoint() == FALSE )
		{
			m_pRadioPoint->SetVisibleSingle( FALSE );
			TapSelect( ID_TEXASHOLDEM_EXCHANGE_MONEY_RADIO );
		}
	}

	void TexasHoldemExchangeWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
	{
		if( m_pBuyChipMsgBox->IsVisible() )
		{
			return;
		}

		EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
		TapSelectorUIMessage( ControlID, dwMsg, emResult );
		CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

		switch( ControlID )
		{
		case ID_TEXASHOLDEM_EXCHANGE_CANCLE_BUTTON:
		case ET_CONTROL_BUTTON:
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				SetVisibleSingle( FALSE );
			}
			break;
		case ID_TEXASHOLDEM_EXCHANGE_BUYCHIP_BUTTON:
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				OpenBuyChipMessageBox();
			}
			break;
		case ID_TEXASHOLDEM_EXCHANGE_MSG_BOX:
			if ( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
			{
				RequestExchangeChip();
				m_pExchangeList->SetNoUpdate(FALSE);
			}
			else if ( dwMsg & UIMSG_MESSAGEBOX_NEGATIVE )
			{
				m_pExchangeList->SetNoUpdate(FALSE);
			}
			break;
		}

	}

	void TexasHoldemExchangeWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
	{
		CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

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

		m_pMyMoneyText->ClearText();
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if( pChar )
		{
			__int64 nPoint = 0;
			std::string strPoint = "";
			int nIndex = 0;
			switch( m_pExchangeList->GetSelectType() )
			{
			case eExchange_Gamemoney:
				nPoint = pChar->GetInvenMoney();
				strPoint = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(	nPoint, 3, "," );
				nIndex = m_pMyMoneyText->AddText( strPoint.c_str(), NS_UITEXTCOLOR::WHITE );
				m_pMyMoneyText->SetTextImage(nIndex, "INVENTORY_MONEY_BUTTON_F");
				break;
			case eExchange_Ranpoint:
				nPoint = pChar->PointShopPoint();
				strPoint = sc::string::format( "%1%", nPoint );
				nIndex = m_pMyMoneyText->AddText( strPoint.c_str(), NS_UITEXTCOLOR::WHITE );
				m_pMyMoneyText->SetTextImage(nIndex, "INVENTORY_POINT_BUTTON_RENEW_F");
				break;
			default:
				strPoint = "";
				m_pMyMoneyText->SetText( strPoint.c_str(), NS_UITEXTCOLOR::WHITE );
			}
		}
	}

	void TexasHoldemExchangeWindow::CreateSubControl()
	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

		//! 상단 텍스트
		CBasicTextBox* pText =	new CBasicTextBox( m_pEngineDevice );
		if( pText )
		{
			pText->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_WINDOW_TOP_TEXT", UI_FLAG_DEFAULT );
			pText->SetFont( pFont9Shadow );
			pText->SetText( ID2GAMEINTEXT("TEXASHOLDEM_EXCHANGE_WINDOW_TOP_TEXT")  );
			RegisterControl( pText );
		}

		//! 버튼
		//! 스타일(폰트,색상,마우스오버색상,클릭색상
		RnButton::CreateArg argStyle;
		argStyle.pFont = pFont9;
		argStyle.defaultTextureKey    = "RNBUTTON_DEFAULT_TEXTURE";
		argStyle.mouseOverTextureKey  = "RNBUTTON_OVER_TEXTURE";
		argStyle.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

		//! 구매하기
		m_pBtnBuyChip = new RnButton( m_pEngineDevice );
		if( m_pBtnBuyChip )
		{
			m_pBtnBuyChip->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_WINDOW_BUYCHIP_BUTTON", UI_FLAG_DEFAULT, ID_TEXASHOLDEM_EXCHANGE_BUYCHIP_BUTTON );
			argStyle.text = ID2GAMEWORD("TEXASHOLDEM_EXCHANGE_WINDOW_TEXT", 1);
			m_pBtnBuyChip->CreateSubControl( argStyle );	
			RegisterControl( m_pBtnBuyChip );
		}

		//! 취소
		m_pBtnCancle = new RnButton( m_pEngineDevice );
		if( m_pBtnCancle )
		{
			m_pBtnCancle->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_WINDOW_CANCLE_BUTTON", UI_FLAG_DEFAULT, ID_TEXASHOLDEM_EXCHANGE_CANCLE_BUTTON );
			argStyle.text = ID2GAMEWORD("TEXASHOLDEM_EXCHANGE_WINDOW_TEXT", 2);
			m_pBtnCancle->CreateSubControl( argStyle );	
			RegisterControl( m_pBtnCancle );
		}

		// 라디오버튼( 게임머니, 란포인트 )
		DEFAULT_RADIO_BUTTON* pRadioMoney = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
		if( pRadioMoney )
		{
			pRadioMoney->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_WINDOW_MONEY_RADIO", UI_FLAG_DEFAULT, ID_TEXASHOLDEM_EXCHANGE_MONEY_RADIO );
			pRadioMoney->CreateSubControl( ID2GAMEWORD("TEXASHOLDEM_EXCHANGE_WINDOW_TEXT", 3), TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP, 
				"MINIGAME_TEXASHOLDEM_EXCHANGE_WINDOW_MONEY_RADIO_TEXT", NS_UITEXTUTIL::WHITE, 
				"MAP_WINDOW_MAIN_RADIO_BUTTON_CHECK_BOX", "MAP_WINDOW_RADIO_ON_TEXTUREINFO","MAP_WINDOW_RADIO_OFF_TEXTUREINFO",
				pFont9Shadow );

			RegisterControl(pRadioMoney);
			RegisterTapButton(pRadioMoney);
			m_pRadioMoney = pRadioMoney;
		}

		DEFAULT_RADIO_BUTTON* pRadioPoint = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
		if( pRadioPoint )
		{
			pRadioPoint->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_WINDOW_POINT_RADIO", UI_FLAG_DEFAULT, ID_TEXASHOLDEM_EXCHANGE_POINT_RADIO );
			pRadioPoint->CreateSubControl( ID2GAMEWORD("TEXASHOLDEM_EXCHANGE_WINDOW_TEXT", 4), TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP, 
				"MINIGAME_TEXASHOLDEM_EXCHANGE_WINDOW_MONEY_RADIO_TEXT", NS_UITEXTUTIL::WHITE, 
				"MAP_WINDOW_MAIN_RADIO_BUTTON_CHECK_BOX", "MAP_WINDOW_RADIO_ON_TEXTUREINFO","MAP_WINDOW_RADIO_OFF_TEXTUREINFO",
				pFont9Shadow );

			RegisterControl(pRadioPoint);
			RegisterTapButton(pRadioPoint);
			m_pRadioPoint = pRadioPoint;
		}

		//! chip 판매 리스트
		CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
		if( pLinBoxSmart )
		{
			pLinBoxSmart->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_SELL_LIST_TEXT_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
			pLinBoxSmart->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
			RegisterControl( pLinBoxSmart );
		}
		CBasicTextBox* pChipListText =	new CBasicTextBox( m_pEngineDevice );
		if( pChipListText )
		{
			pChipListText->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_SELL_LIST_TEXT", UI_FLAG_YSIZE );
			pChipListText->SetFont( pFont9 );
			pChipListText->SetText( ID2GAMEWORD("TEXASHOLDEM_EXCHANGE_WINDOW_TEXT", 5)  );
			pChipListText->SetTextAlign( TEXT_ALIGN_CENTER_Y );
			RegisterControl( pChipListText );
		}

		TexasHoldemExchangeList* pList = new TexasHoldemExchangeList(m_pGaeaClient,  m_pEngineDevice, m_pInterface );
		if( pList )
		{
			pList->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_LIST",  UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_TEXASHOLDEM_EXCHANGE_LIST );
			pList->CreateSubControl();
			RegisterControl( pList );
			m_pExchangeList = pList;
			m_pExchangeList->SetSeletedRadioType( eExchange_Gamemoney );
		}

		//! 보유 금액
		CBasicTextBox* pHaveMoneyText =	new CBasicTextBox( m_pEngineDevice );
		if( pHaveMoneyText )
		{
			pHaveMoneyText->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_WINDOW_MY_MONEY_TEXT", UI_FLAG_RIGHT );
			pHaveMoneyText->SetFont( pFont9Shadow );
			pHaveMoneyText->SetText( ID2GAMEWORD("TEXASHOLDEM_EXCHANGE_WINDOW_TEXT", 6) );
			RegisterControl( pHaveMoneyText );
		}

		CBasicLineBoxSmart* pMyMoneyBox = new CBasicLineBoxSmart(m_pEngineDevice);
		if( pMyMoneyBox )
		{
			pMyMoneyBox->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_WINDOW_MY_MONEY_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE | UI_FLAG_CENTER_Y );
			pMyMoneyBox->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_DARKGRAY" );
			RegisterControl( pMyMoneyBox );
		}

		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		if( pTextBox )
		{
			pTextBox->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_WINDOW_MY_MONEY", UI_FLAG_DEFAULT, ID_TEXASHOLDEM_EXCHANGE_MY_MONEY );
			pTextBox->SetFont( pFont9 );
			pTextBox->SetSensitive( true );
			pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y );
			RegisterControl( pTextBox );
			m_pMyMoneyText = pTextBox;
		}

		//! 메시지박스
		CMessageBox* pMessageBox = new CMessageBox( m_pEngineDevice );
		if( pMessageBox )
		{
			pMessageBox->CreateSub( this, "UTILE_MESSAGEBOX_SIZE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_TEXASHOLDEM_EXCHANGE_MSG_BOX );
			pMessageBox->CreateSubControl();
			RegisterControl( pMessageBox );
			m_pBuyChipMsgBox = pMessageBox;
		}

		m_vMoveGap.x = GetLocalPos().left;
		m_vMoveGap.y = GetLocalPos().top;
	}

}