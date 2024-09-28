#include "StdAfx.h"

#include "./PartyTenderNotifyButton.h"
#include "../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../../RanLogicClient/Party/GLPartyTenderManagerClient.h"

#include "../../../../EngineLib/DeviceMan.h"
#include "../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../EngineLib/DxTools/d3dfont.h"
#include "../../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../../RanLogicClient/GLGaeaClient.h"

CPartyTenderNotifyButton::CPartyTenderNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup( pEngineDevice )
, m_pGaeaClient( pGaeaClient )
, m_pInterface( pInterface )
, m_pButtonActiveImg( NULL )
, m_pNotify_Head ( NULL )
, m_pNotify_Body ( NULL )
, m_pNotify_Tail ( NULL )
, m_pNotify_Arrow( NULL )
, m_pNotify_Text ( NULL )
, m_emNotify(TENDER_NOTIFY_NONE)
{
}

CPartyTenderNotifyButton::~CPartyTenderNotifyButton ()
{
}

void CPartyTenderNotifyButton::CreateSubControl()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "PARTY_TENDER_NOTIFY_NORMAL", UI_FLAG_DEFAULT, TENDER_NOTIFY_BUTTON );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	m_pButtonActiveImg = new CBasicButton(m_pEngineDevice);
	m_pButtonActiveImg->CreateSub ( this, "PARTY_TENDER_NOTIFY_ACTIVE", UI_FLAG_DEFAULT );
	m_pButtonActiveImg->SetUseGlobalAction ( TRUE );
	RegisterControl ( m_pButtonActiveImg );

	m_pNotify_Head = new CUIControl(m_pEngineDevice);
	m_pNotify_Head->CreateSub( this, "PARTY_TENDER_NOTIFY_ALARM_HEAD" );
	m_pNotify_Head->SetVisibleSingle( FALSE );
	RegisterControl ( m_pNotify_Head );

	m_pNotify_Body = new CUIControl(m_pEngineDevice);
	m_pNotify_Body->CreateSub( this, "PARTY_TENDER_NOTIFY_ALARM_BODY" );
	m_pNotify_Body->SetVisibleSingle( FALSE );
	RegisterControl ( m_pNotify_Body );

	m_pNotify_Tail = new CUIControl(m_pEngineDevice);
	m_pNotify_Tail->CreateSub( this, "PARTY_TENDER_NOTIFY_ALARM_TAIL" );
	m_pNotify_Tail->SetVisibleSingle( FALSE );
	RegisterControl ( m_pNotify_Tail );

	m_pNotify_Arrow = new CUIControl(m_pEngineDevice);
	m_pNotify_Arrow->CreateSub( this, "PARTY_TENDER_NOTIFY_ALARM_ARROW" );
	m_pNotify_Arrow->SetVisibleSingle( FALSE );
	RegisterControl ( m_pNotify_Arrow );

	m_pNotify_Text = new CBasicTextBox(m_pEngineDevice);
	m_pNotify_Text->CreateSub ( this, "PARTY_TENDER_NOTIFY_ALARM_TEXT", UI_FLAG_DEFAULT );
	m_pNotify_Text->SetFont ( pFont );
	m_pNotify_Text->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
	RegisterControl ( m_pNotify_Text );

	m_strNotify = ID2GAMEWORD("PARTY_TENDER_NOTIFY_TEXT", 0);
	SetVisibleSingle( FALSE );
}

void CPartyTenderNotifyButton::ResizeNotify()
{
	const UIRECT& rcBase  = m_pButtonActiveImg->GetGlobalPos();
	const UIRECT& rcTail  = m_pNotify_Tail->GetGlobalPos();
	const UIRECT& rcArrow = m_pNotify_Arrow->GetGlobalPos();

	m_pNotify_Tail->SetGlobalPos( D3DXVECTOR2(rcBase.right, rcBase.top - rcArrow.sizeY - rcTail.sizeY + 3) );

	SIZE strSize;
	strSize.cx = 1;
	strSize.cy = 1;

	if ( !m_strNotify.empty() )
	{
		CD3DFontPar* pFont = m_pNotify_Text->GetFont();
		pFont->GetTextExtent ( m_strNotify.c_str(), strSize );
	}

	m_pNotify_Body->SetGlobalPos( UIRECT(rcTail.left - (float)strSize.cx + 2.0f, rcTail.top, (float)strSize.cx + 2.0f, rcTail.sizeY) );
	m_pNotify_Text->SetLocalPos( m_pNotify_Body->GetGlobalPos() );
	m_pNotify_Text->SetLocalPos( D3DXVECTOR2(0.0f, 0.0f) );
	m_pNotify_Text->SetGlobalPos( m_pNotify_Body->GetGlobalPos() );
	m_pNotify_Text->SetOneLineText( m_strNotify.c_str(), NS_UITEXTCOLOR::WHITE );

	const UIRECT& rcBody = m_pNotify_Body->GetGlobalPos();
	const UIRECT& rcHead = m_pNotify_Head->GetGlobalPos();	
	m_pNotify_Head->SetGlobalPos( D3DXVECTOR2(rcBody.left - rcHead.sizeX, rcBody.top) );
	m_pNotify_Arrow->SetGlobalPos( D3DXVECTOR2((rcBase.left + rcBase.right)/2.0f, rcBase.top - rcArrow.sizeY + 2) );
}

void CPartyTenderNotifyButton::Notify( bool bIsNew )
{
	if( bIsNew && !m_pInterface->IsVisible( PARTY_TENDER_WINDOW ) )
	{
		m_pButtonActiveImg->SetVisibleSingle( bIsNew );
		m_pNotify_Text->SetVisibleSingle( bIsNew );
		m_pNotify_Head->SetVisibleSingle( bIsNew );
		m_pNotify_Body->SetVisibleSingle( bIsNew );
		m_pNotify_Tail->SetVisibleSingle( bIsNew );
		m_pNotify_Arrow->SetVisibleSingle( bIsNew );
		ResizeNotify();
	}
	else if( !bIsNew )
	{
		m_pButtonActiveImg->SetVisibleSingle( bIsNew );
		m_pNotify_Text->SetVisibleSingle( bIsNew );
		m_pNotify_Head->SetVisibleSingle( bIsNew );
		m_pNotify_Body->SetVisibleSingle( bIsNew );
		m_pNotify_Tail->SetVisibleSingle( bIsNew );
		m_pNotify_Arrow->SetVisibleSingle( bIsNew );
	}
}

void CPartyTenderNotifyButton::ClickNotifyButton()
{
	switch( m_emNotify )
	{
	case TENDER_NOTIFY_DICE:
		if( m_pInterface->IsVisible( PARTY_TENDER_WINDOW ) )
			m_pInterface->ToggleWindowTenderItem( false );
		else
		{
			m_pInterface->ToggleWindowTenderItem(true);
			Notify(false);
		}
		break;
	case TENDER_NOTIFY_MASTERTRANSFER:
		if( m_pInterface->IsVisible( PARTY_DISTRIBUTION_WINDOW ) )
			m_pInterface->ToggleWindowMasterTransfer(false);
		else
		{
			m_pInterface->ToggleWindowMasterTransfer(true);
			Notify(false);
		}
		break;
	case TENDER_NOTIFY_NONE:
		break;
	}	
}

void CPartyTenderNotifyButton::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
	switch( ControlID )
	{
	case TENDER_NOTIFY_BUTTON:
		{   
			if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
			{
				ClickNotifyButton();
			}
		}
		break;
	}
}

void CPartyTenderNotifyButton::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( !m_pGaeaClient->GetMyPartyClient()->isValid() )
	{
		SetVisibleSingle( FALSE );
		return;
	}
}

MyPartyTenderNotifyButton::MyPartyTenderNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CPartyTenderNotifyButton( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyPartyTenderNotifyButton::CreateUIWindowAndRegisterOwnership()
{
	CPartyTenderNotifyButton::Create( PARTY_TENDER_NOTIFY_BUTTON, "PARTY_TENDER_NOTIFY_BUTTON", UI_FLAG_RIGHT | UI_FLAG_BOTTOM );
	CPartyTenderNotifyButton::CreateSubControl();
	CPartyTenderNotifyButton::m_pInterface->UiRegisterControl( this );
	CPartyTenderNotifyButton::m_pInterface->UiShowGroupBottom( PARTY_TENDER_NOTIFY_BUTTON );
}

void MyPartyTenderNotifyButton::Notify( bool bNotify )
{
	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	const SPartyOption& PartyOption = pPartyManager->GetPartyOption();

	switch ( PartyOption.GetItemOption() )
	{
	case EMPARTY_ITEM_DICE:
		CPartyTenderNotifyButton::SetNotifyType( TENDER_NOTIFY_DICE );
		break;
	case EMPARTY_ITEM_LEADER_SHARE:
		CPartyTenderNotifyButton::SetNotifyType( TENDER_NOTIFY_MASTERTRANSFER );
		break;
	default:
		CPartyTenderNotifyButton::SetNotifyType( TENDER_NOTIFY_NONE );
		Reset();
		break;
	}

	CPartyTenderNotifyButton::Notify( bNotify );
}