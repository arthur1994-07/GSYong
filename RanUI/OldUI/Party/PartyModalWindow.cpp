#include "StdAfx.h"
#include "../Party/PartyModalWindow.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicComboBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/Character/GLCharDefine.h"
#include "../../../RanLogic/Party/GLPartyDefine.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPartyModalWindow::CPartyModalWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindow(pEngineDevice)
	, m_pGaeaClient(pGaeaClient)
    , m_pInterface( pInterface )
    , m_CallerID( NO_ID )
    , m_pOK( NULL )
    , m_pCANCEL( NULL )
    , m_pYES( NULL )
    , m_pNO( NULL )
    , m_pTextBox( NULL )
    , m_RollOverID( NO_ID )
    , m_bReqModal( FALSE )
    , m_bFIX( FALSE )
{
}

CPartyModalWindow::~CPartyModalWindow()
{
}

void CPartyModalWindow::CreateBaseModal( char* szWindowKeyword )
{
	CreateTitle( "MODAL_WINDOW_TITLE", "MODAL_WINDOW_TITLE_LEFT", "MODAL_WINDOW_TITLE_MID", "MODAL_WINDOW_TITLE_RIGHT", "MODAL_WINDOW_TEXTBOX", NULL );
	CreateBody( "MODAL_WINDOW_BODY", "MODAL_WINDOW_BODY_LEFT", "MODAL_WINDOW_BODY_UP", "MODAL_WINDOW_BODY_MAIN", "MODAL_WINDOW_BODY_DOWN", "MODAL_WINDOW_BODY_RIGHT" );

	ResizeControl( szWindowKeyword );
}

void CPartyModalWindow::CreateSubControl()
{
	//	텍스트 박스
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	WORD wAlignFlag = UI_FLAG_XSIZE | UI_FLAG_YSIZE;

	const bool bRenewal = GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal();
	{	//	외부 라인 박스
		CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
		pLineBox->CreateSub( this, "BASIC_LINE_BOX_MODAL", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		if ( bRenewal )
			pLineBox->CreateBaseBoxModal( "PARTY_RENEWAL_MODAL_LINEBOX" );		
		else
			pLineBox->CreateBaseBoxModal( "PARTY_MODAL_LINEBOX" );		
		RegisterControl( pLineBox );
	}

	CBasicTextBox* pTextBox = NULL;

	{	//	위에 서술형 문장
		pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub( this, "PARTY_MODAL_TEXT_BOX", wAlignFlag );
		pTextBox->SetFont( pFont8 );
		pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X );	
		RegisterControl( pTextBox );
		m_pTextBox = pTextBox;
	}

	if ( bRenewal == true )
	{ //	버튼들;
		m_pOK = CreateTextButton( "PARTY_RENEWAL_MODAL_OK", MODAL_PARTY_OK, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		m_pOK->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		m_pCANCEL = CreateTextButton( "PARTY_RENEWAL_MODAL_CANCEL", MODAL_PARTY_CANCEL, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		m_pCANCEL->SetShortcutKey( DIK_ESCAPE );
		m_pYES = CreateTextButton( "PARTY_RENEWAL_MODAL_YES", MODAL_PARTY_YES, (char*)ID2GAMEWORD( "MODAL_BUTTON", 2 ) );
		m_pYES->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		m_pNO = CreateTextButton( "PARTY_RENEWAL_MODAL_NO", MODAL_PARTY_NO, (char*)ID2GAMEWORD( "MODAL_BUTTON", 3 ) );
		m_pNO->SetShortcutKey( DIK_ESCAPE );		
	}
	else
	{	
		m_pOK = CreateTextButton( "PARTY_MODAL_OK", MODAL_PARTY_OK, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		m_pOK->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		m_pCANCEL = CreateTextButton( "PARTY_MODAL_CANCEL", MODAL_PARTY_CANCEL, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		m_pCANCEL->SetShortcutKey( DIK_ESCAPE );
		m_pYES = CreateTextButton( "PARTY_MODAL_YES", MODAL_PARTY_YES, (char*)ID2GAMEWORD( "MODAL_BUTTON", 2 ) );
		m_pYES->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		m_pNO = CreateTextButton( "PARTY_MODAL_NO", MODAL_PARTY_NO, (char*)ID2GAMEWORD( "MODAL_BUTTON", 3 ) );
		m_pNO->SetShortcutKey( DIK_ESCAPE );		
	}

	D3DCOLOR dwColor = NS_UITEXTCOLOR::DEFAULT;
	int nAlign = TEXT_ALIGN_LEFT;

	{	//	기본 텍스트
		if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false )
		{
			//	영역
			pTextBox = CreateStaticControl( "PARTY_MODAL_ITEM_STATIC", pFont8, dwColor, nAlign );
			pTextBox->AddText( (char*)ID2GAMEWORD( "PARTY_MODAL_CONDITION", 0 ) );

			//	회복제
			pTextBox = CreateStaticControl( "PARTY_MODAL_MONEY_STATIC", pFont8, dwColor, nAlign );
			pTextBox->AddText( (char*)ID2GAMEWORD( "PARTY_MODAL_CONDITION", 1 ) );
		}		
	}

	{	//	기능
		{
			CBasicComboBox* pComboBox = NULL;
			CBasicComboBoxRollOver*	pComboBoxRollOver = NULL;

			//	콤보박스
			{
				pComboBox = new CBasicComboBox(m_pEngineDevice);
				pComboBox->CreateSub( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, PARTY_MODAL_ITEM_COMBO_OPEN );
				pComboBox->CreateBaseComboBox( "PARTY_MODAL_ITEM_COMBO_OPEN" );			
				RegisterControl( pComboBox );
				m_pComboBoxItemOpen = pComboBox;

				pComboBox = new CBasicComboBox(m_pEngineDevice);
				pComboBox->CreateSub( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, PARTY_MODAL_MONEY_COMBO_OPEN );
				pComboBox->CreateBaseComboBox( "PARTY_MODAL_MONEY_COMBO_OPEN" );			
				RegisterControl( pComboBox );
				m_pComboBoxMoneyOpen = pComboBox;				
			}

			//	롤 오버 ( 언제나 가장 마지막에 뿌려져야합니다. )
			{
				pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
				pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, PARTY_MODAL_ITEM_COMBO_ROLLOVER );
				pComboBoxRollOver->CreateBaseComboBoxRollOver( "PARTY_MODAL_ITEM_COMBO_ROLLOVER" );			
				pComboBoxRollOver->SetVisibleSingle( FALSE );
				RegisterControl( pComboBoxRollOver );
				m_pComboBoxItemRollOver = pComboBoxRollOver;

				pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
				pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, PARTY_MODAL_MONEY_COMBO_ROLLOVER );
				pComboBoxRollOver->CreateBaseComboBoxRollOver( "PARTY_MODAL_MONEY_COMBO_ROLLOVER" );
				pComboBoxRollOver->SetVisibleSingle( FALSE );
				RegisterControl( pComboBoxRollOver );
				m_pComboBoxMoneyRollOver = pComboBoxRollOver;
			}

			if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == true )
			{
				m_pComboBoxItemOpen->SetVisibleSingle(FALSE);
				m_pComboBoxMoneyOpen->SetVisibleSingle(FALSE);
			}
			else
			{
				m_pComboBoxItemOpen->SetVisibleSingle(TRUE);
				m_pComboBoxMoneyOpen->SetVisibleSingle(TRUE);
			}
		}
	}
}

CBasicTextButton* CPartyModalWindow::CreateTextButton( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl( pButton );

	return pButton;
}

void CPartyModalWindow::DoModal( CString strText, UIGUID CallerID )
{
	if ( CallerID == UI::MODAL_PARTY_TAR )
	{
		m_bReqModal = FALSE;		
	}
	else
	{
		m_bReqModal = TRUE;
	}

	//	NOTE
	//		이미 파티가 구성되어 있거나,
	//		파티 요청이 들어온 경우에 대한 모달을 띄울때는
	//		데이타 변경이 불가하다.
	m_bFIX = FALSE;
	if ( m_pGaeaClient->GetMyPartyClient()->isValid() || !m_bReqModal )
	{
		m_bFIX = TRUE;
	}

	SetCallerID( CallerID );

	//	내용 설정
	if ( m_pTextBox )
	{
		m_pTextBox->SetText( strText );
	}

	int			nTextIndex = 0;
	if ( m_bReqModal )
	{
		m_pOK->SetVisibleSingle( TRUE );
		m_pCANCEL->SetVisibleSingle( TRUE );
		m_pYES->SetVisibleSingle( FALSE );
		m_pNO->SetVisibleSingle( FALSE );	
	}
	else
	{
		nTextIndex = 1;

		m_pOK->SetVisibleSingle( FALSE );
		m_pCANCEL->SetVisibleSingle( FALSE );
		m_pYES->SetVisibleSingle( TRUE );
		m_pNO->SetVisibleSingle( TRUE );
	}

	if( m_bExpedition == true )
		nTextIndex += 2;

	SetTitleName( (char*)ID2GAMEWORD( "PARTY_MODAL_TITLE", nTextIndex ) );

	if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == false )
		LoadDefaultCondition();
}

void CPartyModalWindow::ResizeControl( char* szWindowKeyword )
{
    CUIControl TempControl(m_pEngineDevice);
	TempControl.Create( 1, szWindowKeyword );
	const UIRECT& rcParentOldPos = GetLocalPos();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos();
	AlignSubControl( rcParentOldPos, rcParentNewPos );

	SetLocalPos( D3DXVECTOR2( rcParentNewPos.left, rcParentNewPos.top ) );
}


CUIControl*	CPartyModalWindow::CreateControl( char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub( this, szControl );
	RegisterControl( pControl );

	return pControl;
}

CBasicTextBox* CPartyModalWindow::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub( this, szControlKeyword );
	pStaticText->SetFont( pFont );
	pStaticText->SetTextAlign( nAlign );	
	RegisterControl( pStaticText );

	return pStaticText;
}

CBasicButton* CPartyModalWindow::CreateFlipButton( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->SetControlNameEx( szButton );
	RegisterControl( pButton );

	return pButton;
}

void	CPartyModalWindow::LoadComboData()
{
	m_pComboBoxItemRollOver->ClearText();
	for ( int i = 1; i < EMPARTY_ITEM_ROUND; i++ )
		m_pComboBoxItemRollOver->AddText( (char*)ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", i) );

	m_pComboBoxMoneyRollOver->ClearText();
	for ( int i = 1; i < EMPARTY_MONEY_NSIZE; i++ )		
		m_pComboBoxMoneyRollOver->AddText( (char*)ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION", i) );
}

void	CPartyModalWindow::LoadDefaultCondition()
{
	LoadComboData();
	LoadItem();
	LoadMoney();
}

void CPartyModalWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( m_RollOverID == NO_ID )
	{
		CUIWindow::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	}
	else
	{
		ResetMessageEx();

		CUIControl::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		CUIControl* pControl = m_ControlContainer.FindControl( m_RollOverID );
		if ( !pControl )
		{
			GASSERT( 0 && "심각한 오류, 노드가 널입니다." );
			return ;
		}

		pControl->Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		DWORD dwControlMsg = pControl->GetMessageEx();
		NS_UIDEBUGSET::BlockBegin();
		if ( dwControlMsg )
			TranslateUIMessage( m_RollOverID, dwControlMsg );
		NS_UIDEBUGSET::BlockEnd();

		//	스크롤바에 관계된 메시지가 아니고		
		if ( !(dwControlMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL) )
		{
			DWORD dwMsg = GetMessageEx();
			if ( dwMsg & UIMSG_LB_UP )
			{
				if ( !m_bFirstLBUP )
				{
					m_RollOverID = NO_ID;
					pControl->SetVisibleSingle( FALSE );					
				}
				m_bFirstLBUP = FALSE;
			}
		}
	}

	//CDebugSet::ToView ( 1, 18, "%d", m_RollOverID );
}

void	CPartyModalWindow::LoadItem()
{
	if ( m_bReqModal )
	{		
		if( RANPARAM::dwPARTY_GET_ITEM == 0 || RANPARAM::dwPARTY_GET_ITEM >= EMPARTY_ITEM_ROUND )
		{
			RANPARAM::dwPARTY_GET_ITEM = 3;
		}

		m_pComboBoxItemOpen->SetText( (char*)ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", RANPARAM::dwPARTY_GET_ITEM) );
	}
	else
	{		
		const SPartyOption* const pPartyOption = m_pInterface->GetPartyOption();
		m_pComboBoxItemOpen->SetText( (char*)ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", pPartyOption->GetItemOption()) );
	}
}

void	CPartyModalWindow::LoadMoney()
{
	if ( m_bReqModal )
	{	
		if( RANPARAM::dwPARTY_GET_MONEY == 0 || RANPARAM::dwPARTY_GET_MONEY >= EMPARTY_MONEY_NSIZE )
		{
			RANPARAM::dwPARTY_GET_MONEY = 3;
		}

		m_pComboBoxMoneyOpen->SetText( (char*)ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION", RANPARAM::dwPARTY_GET_MONEY) );
	}
	else
	{		
		SPartyOption* pPartyOption = m_pInterface->GetPartyOption();
		m_pComboBoxMoneyOpen->SetText( (char*)ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION", pPartyOption->GetMoneyOption()) );
	}
}

void CPartyModalWindow::SetCharDbNum( DWORD dwCharDbNum )
{
    m_dwCharDbNum = dwCharDbNum;
}

DWORD CPartyModalWindow::GetCharDbNum()
{
	return m_dwCharDbNum;
}

MyPartyModalWindow::MyPartyModalWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CPartyModalWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyPartyModalWindow::CreateUIWindowAndRegisterOwnership()
{
    CPartyModalWindow::Create( PARTY_MODAL_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() == true )
		CPartyModalWindow::CreateBaseModal( "PARTY_RENEWAL_MODAL_WINDOW");
	else
		CPartyModalWindow::CreateBaseModal( "PARTY_MODAL_WINDOW");

    CPartyModalWindow::CreateSubControl();
    CPartyModalWindow::SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
    CPartyModalWindow::LoadDefaultCondition();
    CPartyModalWindow::m_pInterface->UiRegisterControl( this );
    CPartyModalWindow::m_pInterface->UiShowGroupFocus( PARTY_MODAL_WINDOW );
}

void MyPartyModalWindow::DoModal( UI::String strText, UIGUID CallerID )
{
    CPartyModalWindow::DoModal( UI::ToString( strText ), CallerID );
}

void MyPartyModalWindow::SetPartyMode( bool bExpedition )
{
    CPartyModalWindow::SetPartyMode( bExpedition );
}
void MyPartyModalWindow::SetPartyOption(const SPartyOption& PartyOption)
{
	CPartyModalWindow::SetPartyOption(PartyOption);
}

UIGUID MyPartyModalWindow::GetCallerID()
{
    return CPartyModalWindow::GetCallerID();
}

void MyPartyModalWindow::SetCharDbNum( DWORD dwCharDbNum )
{
    CPartyModalWindow::SetCharDbNum( dwCharDbNum );
}

DWORD MyPartyModalWindow::GetCharDbNum()
{
    return CPartyModalWindow::GetCharDbNum();
}
