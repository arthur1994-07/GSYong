
// bjju.sns 추가

#include "StdAfx.h"

#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/SNS/GLSNSClient.h"

#include "../Util/UIPage.h"
#include "../../InnerInterface.h"

#include "./FaceBookWindow.h"
#include "./TwitterWindow.h"
#include "./SNSWindow.h"
#include "./SNSMenuPage.h"
#include "./SNSMenuIcon.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSNSMenuPage::CSNSMenuPage (GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIPage(pEngineDevice)
    , m_pGaeaClient         (pGaeaClient)
    , m_pInterface          (pInterface)
    , m_pIcon_FaceBookAuth	( NULL )
    , m_pIcon_TwitterAuth	( NULL )
    , m_pIcon_FaceBookUse	( NULL )
    , m_pIcon_TwitterUse	( NULL )
    , m_bFBAuth				( FALSE )
    , m_bTTAuth				( FALSE )
    , m_bEnable				( FALSE )
{
}

CSNSMenuPage::~CSNSMenuPage ()
{
}

void CSNSMenuPage::CreateSubControl()
{
	// Note : 아이콘
	{
		// Note : 페이스북 연동
		CSNSMenuIcon* pIcon;
		m_pIcon_FaceBookAuth = pIcon = new CSNSMenuIcon(m_pEngineDevice);
		pIcon->CreateSub		( this, "SNS_WINDOW_PAGE_MENU_ICON_SLOT_0", UI_FLAG_DEFAULT, PAGE_MENU_BUTTON_FACEBOOK_AUTH );
		pIcon->CreateSubControl	();
		pIcon->SetImage			( "SNS_WINDOW_IMAGE_FACEBOOK_AUTH" );
		pIcon->SetText			( ID2GAMEINTEXT ("SNS_WINDOW_PAGE_MENU_ICON_FACEBOOK_AUTH") );
		RegisterControl			( pIcon );

		// Note : 트위터 연동
		m_pIcon_TwitterAuth = pIcon = new CSNSMenuIcon(m_pEngineDevice);
		pIcon->CreateSub		( this, "SNS_WINDOW_PAGE_MENU_ICON_SLOT_1", UI_FLAG_DEFAULT, PAGE_MENU_BUTTON_TWITTER_AUTH );
		pIcon->CreateSubControl	();
		pIcon->SetImage			( "SNS_WINDOW_IMAGE_TWITTER_AUTH" );
		pIcon->SetText			( ID2GAMEINTEXT ("SNS_WINDOW_PAGE_MENU_ICON_TWITTER_AUTH") );
		RegisterControl			( pIcon );

		// Note : 페이스북 사용
		m_pIcon_FaceBookUse = pIcon = new CSNSMenuIcon(m_pEngineDevice);
		pIcon->CreateSub		( this, "SNS_WINDOW_PAGE_MENU_ICON_SLOT_2", UI_FLAG_DEFAULT, PAGE_MENU_BUTTON_FACEBOOK_USE );
		pIcon->CreateSubControl	();
		pIcon->SetImage			( "SNS_WINDOW_IMAGE_FACEBOOK_USE" );
		pIcon->SetText			( ID2GAMEINTEXT ("SNS_WINDOW_PAGE_MENU_ICON_FACEBOOK_USE") );
		RegisterControl			( pIcon );

		// Note : 트위터 사용
		m_pIcon_TwitterUse = pIcon = new CSNSMenuIcon(m_pEngineDevice);
		pIcon->CreateSub		( this, "SNS_WINDOW_PAGE_MENU_ICON_SLOT_3", UI_FLAG_DEFAULT, PAGE_MENU_BUTTON_TWITTER_USE );
		pIcon->CreateSubControl	();
		pIcon->SetImage			( "SNS_WINDOW_IMAGE_TWITTER_USE" );
		pIcon->SetText			( ID2GAMEINTEXT ("SNS_WINDOW_PAGE_MENU_ICON_TWITTER_USE") );
		RegisterControl			( pIcon );
		/*
		CBasicTextButton* pButton;

		m_pButton_Ok = pButton = new CBasicTextButton(m_pRenderDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_FRIENDFIND_BUTTON_OK );
		pButton->CreateBaseButton( "POSTBOX_PAGE_FRIENDFIND_BUTTON_OK", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_PAGE_FRIENDFIND_BUTTON", 2 ) );
		RegisterControl( pButton );

		m_pButton_Close = pButton = new CBasicTextButton(m_pRenderDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_FRIENDFIND_BUTTON_CLOSE );
		pButton->CreateBaseButton( "POSTBOX_PAGE_FRIENDFIND_BUTTON_CLOSE", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_BUTTON", 3 ) );
		RegisterControl( pButton );
		*/
	}
}

void CSNSMenuPage::OpenSubControl ()
{

}

void CSNSMenuPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	UpdateControl ();

	CUIPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSNSMenuPage::UpdateControl ()
{
	m_bEnable = TRUE;

	// Note : 페이스북 인증 토글
	if ( m_pIcon_FaceBookAuth )
	{
		BOOL bAuth = m_pGaeaClient->GetSNSClient()->FBIsAuthed();

		if ( m_bFBAuth != bAuth )
		{
			m_bFBAuth = bAuth;

			if ( bAuth )
			{
				m_pIcon_FaceBookAuth->SetImage	( "SNS_WINDOW_IMAGE_FACEBOOK_LOGOUT" );
				m_pIcon_FaceBookAuth->SetText	( ID2GAMEINTEXT("SNS_WINDOW_PAGE_MENU_ICON_FACEBOOK_UNAUTH") );
			}
			else
			{
				// bjju.sns GAMEINTEXT 필
				m_pIcon_FaceBookAuth->SetImage	( "SNS_WINDOW_IMAGE_FACEBOOK_AUTH" );
				m_pIcon_FaceBookAuth->SetText	( ID2GAMEINTEXT("SNS_WINDOW_PAGE_MENU_ICON_FACEBOOK_AUTH") );
			}
		}
	}

	// Note : 트위터 인증 토글
	if ( m_pIcon_TwitterAuth )
	{
		BOOL bAuth = m_pGaeaClient->GetSNSClient()->TTIsAuthed();

		if ( m_bTTAuth != bAuth )
		{
			m_bTTAuth = bAuth;

			if (m_pGaeaClient->GetSNSClient()->TTIsAuthed() )
			{
				m_pIcon_TwitterAuth->SetImage	( "SNS_WINDOW_IMAGE_TWITTER_LOGOUT" );
				m_pIcon_TwitterAuth->SetText	( ID2GAMEINTEXT("SNS_WINDOW_PAGE_MENU_ICON_TWITTER_UNAUTH") );
			}
			else
			{
				m_pIcon_TwitterAuth->SetImage	( "SNS_WINDOW_IMAGE_TWITTER_AUTH" );
				m_pIcon_TwitterAuth->SetText	( ID2GAMEINTEXT("SNS_WINDOW_PAGE_MENU_ICON_TWITTER_AUTH") );
			}
		}
	}

	// Note : 인터넷 창을 하나에 하나씩만 뜨게한다.
	if (   m_pInterface->UiIsVisibleGroup ( FACEBOOK_WINDOW )  
		|| m_pInterface->UiIsVisibleGroup ( TWITTER_WINDOW  ) )
	{
		m_bEnable = FALSE;
	}

	// Note : 요청중 일때는 버튼 비활성화

	if ( m_pGaeaClient->GetSNSClient()->IsReq() )
	{
		m_bEnable = FALSE;
	}

	// Note : 버튼 활성화
	if ( !m_bEnable )
	{
		m_pIcon_FaceBookAuth->SetEnable( FALSE );
		m_pIcon_TwitterAuth	->SetEnable( FALSE );
		m_pIcon_FaceBookUse	->SetEnable( FALSE );
		m_pIcon_TwitterUse	->SetEnable( FALSE );
	}
	else
	{
		m_pIcon_FaceBookAuth->SetEnable( TRUE );
		m_pIcon_TwitterAuth	->SetEnable( TRUE );

		if ( m_bFBAuth )
		{
			m_pIcon_FaceBookUse->SetEnable( TRUE );
		}
		else
		{
			m_pIcon_FaceBookUse->SetEnable( FALSE );
		}

		if ( m_bTTAuth )
		{
			m_pIcon_TwitterUse->SetEnable( TRUE );
		}
		else
		{
			m_pIcon_TwitterUse->SetEnable( FALSE );
		}
	}

}

void CSNSMenuPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{	
	if ( m_bEnable )
	{
		switch ( ControlID )
		{
		case PAGE_MENU_BUTTON_FACEBOOK_AUTH:
			{
				if ( dwMsg&UIMSG_SNS_MENU_ICON_PUSH )
				{
					// Note : 승인이 안되어있으면 로그인 페이지로
					if ( !m_bFBAuth )
					{
						m_pInterface->SNSWebFBWindowSetPageType( CFaceBookWindow::PAGETYPE_AUTH );

						if ( !m_pInterface->UiIsVisibleGroup ( FACEBOOK_WINDOW ) )
						{
							m_pInterface->UiShowGroupFocus ( FACEBOOK_WINDOW );
						}
					}
					else
					{
						m_pGaeaClient->GetSNSClient()->FBUnAuthedReq();
					}
				}
			}
			break;

		case PAGE_MENU_BUTTON_TWITTER_AUTH:
			{
				if ( dwMsg&UIMSG_SNS_MENU_ICON_PUSH )
				{
					// Note : 승인이 안되어있으면 로그인 페이지로
					if ( !m_bTTAuth )
					{
						if ( !m_pInterface->UiIsVisibleGroup ( TWITTER_WINDOW ) )
						{
							m_pInterface->UiShowGroupFocus ( TWITTER_WINDOW );
						}
					}
					else
					{
						m_pGaeaClient->GetSNSClient()->TTUnAuthedReq();
					}
				}
			}
			break;

		case PAGE_MENU_BUTTON_FACEBOOK_USE:
			{
				if ( dwMsg&UIMSG_SNS_MENU_ICON_PUSH )
				{
					RequestPush  ( CSNSWindow::SNS_PAGE_REQ_FACEBOOK_PAGE_OPEN );
				}
			}
			break;

		case PAGE_MENU_BUTTON_TWITTER_USE:
			{
				if ( dwMsg&UIMSG_SNS_MENU_ICON_PUSH )
				{
					RequestPush  ( CSNSWindow::SNS_PAGE_REQ_TWITTER_PAGE_OPEN );
				}
			}
			break;
		};
	}

	//switch ( ControlID )
	//{
	//default:
	//	break;
	//};

	CUIPage::TranslateUIMessage ( ControlID, dwMsg );
}

