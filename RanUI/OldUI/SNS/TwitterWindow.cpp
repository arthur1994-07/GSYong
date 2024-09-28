
// bjju.sns 추가

#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../EngineLib/GUInterface/UIWindowBody.h"
#include "../EngineLib/GUInterface/UIKeyCheck.h"
#include "../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../EngineLib/GUInterface/BasicTextButton.h"
#include "../EngineLib/GUInterface/BasicTextBox.h"
#include "../EngineLib/GUInterface/UIEditBox.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/Common/CommonWeb.h"

#include "../../InnerInterface.h"
#include "../Interface/BasicChat.h"
#include "./TwitterWindow.h"
#include "./SNSWindow.h"

#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/SNS/GLSNSClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTwitterWindow::CTwitterWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CWebWindowBase( pInterface, pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
	, m_pButton_Pin( NULL )
	, m_pLineBox_Pin( NULL )
	, m_pEditBox_Pin( NULL )
{
    // 아래, 핀번호 입력 공간
    m_iAlignBottom = 34;
}

CTwitterWindow::~CTwitterWindow()
{
}

void CTwitterWindow::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	const int	nAlignLeft	= TEXT_ALIGN_LEFT;

	// Note : 정적 텍스트
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, "TWITTER_WINDOW_STATIC_TEXT_PIN", UI_FLAG_DEFAULT );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlignLeft );	
	RegisterControl ( pStaticText );
	pStaticText->SetText( ID2GAMEINTEXT ( "TWITTER_WINDOW_TEXT_PIN" ) );

	// Note : 핀번호 입력 상자
	{
		m_pLineBox_Pin = new CBasicLineBoxSmart(m_pEngineDevice);
		m_pLineBox_Pin->CreateSub ( this,  "TWITTER_WINDOW_LINEBOX_PIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE  );
		m_pLineBox_Pin->CreateSubControl( "TWITTER_WINDOW_LINEBOX_TEXINFO" );
		RegisterControl ( m_pLineBox_Pin );	

		m_pEditBox_Pin = new CUIEditBox(m_pEngineDevice);
		m_pEditBox_Pin->CreateSub ( this, "TWITTER_WINDOW_EDITBOX_PIN", UI_FLAG_DEFAULT, TWITTER_WINDOW_EDITBOX_PIN );
		m_pEditBox_Pin->CreateCarrat ( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
		m_pEditBox_Pin->SetFont ( pFont );
		m_pEditBox_Pin->SetLimitInput ( 64 );
		RegisterControl ( m_pEditBox_Pin );
	}

	// Note : 핀번호 확인 버튼
	{
		CBasicTextButton* pButton;
		m_pButton_Pin = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, TWITTER_WINDOW_BUTTON_PIN );
		pButton->CreateBaseButton( "TWITTER_WINDOW_BUTTON_PIN", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, ID2GAMEINTEXT( "TWITTER_WINDOW_BUTTON_PIN_OK" ) );
		RegisterControl( pButton );
	}
}

void CTwitterWindow::SetVisibleSingle( BOOL bVisible )
{	
	if( !GLUseFeatures::GetInstance().IsUsingSNS() )
	{
        CWebWindowBase::SetVisibleSingle( FALSE );
		return;
	}

	if( bVisible )
	{
		if ( m_pEditBox_Pin )
			m_pEditBox_Pin->ClearEdit();

		std::string strURL = "";
		if( !m_pGaeaClient->GetSNSClient()->IsWait() &&
            m_pGaeaClient->GetSNSClient()->TTGetAuthURL( strURL ) )
		{
            SetAddress( strURL.c_str() );
		}
		else
		{
			bVisible = FALSE;// Some message handling here.
		}
	}
	else
	{
		if ( m_pEditBox_Pin )
		{
			CString strPIN = m_pEditBox_Pin->GetEditString();

			if ( m_pGaeaClient->GetSNSClient()->TTGetAccessToken( std::string( strPIN.GetString() ) ) )
			{
				m_pGaeaClient->GetSNSClient()->TTAuthedReq ();
			}
			else
			{
				m_pInterface->SNSWindowDoModalError( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR_TW_AUTH") );
			}
		}
	}

	m_pInterface->GetBasicChatWindow()->SetNoUpdate( bVisible );

    CWebWindowBase::SetVisibleSingle( bVisible );
}

void CTwitterWindow::Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl )
{
	UIKeyCheck::GetInstance()->Check ( DIK_RETURN, DXKEY_DOWN );

	CWebWindowBase::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CTwitterWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CWebWindowBase::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case TWITTER_WINDOW_EDITBOX_PIN:
		{
			if( !m_pEditBox_Pin ) return;

			if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
			{
				m_pEditBox_Pin->EndEdit();
				m_pEditBox_Pin->BeginEdit();
			}
			else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
			{
				m_pEditBox_Pin->EndEdit();
			}
		}
		break;

	case TWITTER_WINDOW_BUTTON_PIN:
		{
			if ( !m_pEditBox_Pin )
			{
				break;
			}

			if ( m_pEditBox_Pin->GetEditLength() == 0 )
			{
				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;

	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( TWITTER_WINDOW );
			}
		}
		break;
	}
}