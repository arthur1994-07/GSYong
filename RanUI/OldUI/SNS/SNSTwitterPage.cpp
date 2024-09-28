
// bjju.sns 추가

#include "StdAfx.h"

#include "../../../RanLogic/RANPARAM.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxGrapUtils.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBoxSmart.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/SNS/GLSNSClient.h"

#include "../../InnerInterface.h"
#include "../../ModalCallerID.h"
#include "../Util/ModalWindow.h"
#include "../Util/CheckBox.h"
#include "../Util/UIPage.h"

#include "./SNSWindow.h"
#include "./SNSTwitterPage.h"
#include "./SNSMenuIcon.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSNSTwitterPage::CSNSTwitterPage (GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIPage(pEngineDevice)
    , m_pGaeaClient             (pGaeaClient)
    , m_pInterface              (pInterface)
    , m_pStaticTextBox_Tweet	( NULL  )
    , m_pLineBox_Tweet			( NULL  )
    , m_pButton_Tweet			( NULL  )
    , m_pButton_Back			( NULL  )
    , m_pButton_Follow			( NULL  )
    , m_pButton_Unfollow		( NULL  )
    , m_pEditBox_Tweet			( NULL  )
    , m_pEditBox_Follower		( NULL  )
{
}

CSNSTwitterPage::~CSNSTwitterPage ()
{
	for ( int i=0; i<PAGE_CHECKBOX_MAX; ++i )
	{
		m_pCheckBox[i] = NULL;
	}

	for ( int i=0; i<PAGE_CHECKBOX_MAX; ++i )
	{
		m_pCheckBoxText[i] = NULL;
	}
}

void CSNSTwitterPage::CreateSubControl ()
{
	CD3DFontPar*	pFont		= m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	const int		nAlignLeft	= TEXT_ALIGN_LEFT;

	// Note : 정적 텍스트
	{
		m_pStaticTextBox_Tweet = CreateStaticControl( "SNS_WINDOW_PAGE_TWITTER_STATIC_TEXT_TWEET", pFont, nAlignLeft );
		m_pStaticTextBox_Tweet->SetText( ID2GAMEWORD("SNS_TWITTER_PAGE_TEXT", 0) );

		m_pStaticTextBox_Follower = CreateStaticControl( "SNS_WINDOW_PAGE_TWITTER_STATIC_TEXT_FOLLOWER", pFont, nAlignLeft );
		m_pStaticTextBox_Follower->SetText( ID2GAMEWORD("SNS_TWITTER_PAGE_TEXT", 1) );
	}

	// Note : 에디트박스
	{
		// Note : 배경
		{
			m_pLineBox_Tweet	= CreateLineBox ( "SNS_WINDOW_PAGE_TWITTER_LINEBOX_TWEET" );	
			m_pLineBox_Follower = CreateLineBox ( "SNS_WINDOW_PAGE_TWITTER_LINEBOX_FOLLOWER" );	
		}

		m_pEditBox_Tweet = new CUIMultiEditBoxSmart(m_pEngineDevice);
		m_pEditBox_Tweet->CreateSub				( this, "SNS_WINDOW_PAGE_TWITTER_EDITBOX_TWEET", UI_FLAG_DEFAULT, PAGE_EDITBOX_TWEET );
		m_pEditBox_Tweet->CreateCarrat			( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
		m_pEditBox_Tweet->CreateScrollVertical	();
		m_pEditBox_Tweet->SetFont				( pFont );
		m_pEditBox_Tweet->SetLimitInput			( 1024 );
		m_pEditBox_Tweet->SetAutoTurn			( true );
		RegisterControl ( m_pEditBox_Tweet );

		m_pEditBox_Follower = new CUIEditBox(m_pEngineDevice);
		m_pEditBox_Follower->CreateSub ( this, "SNS_WINDOW_PAGE_TWITTER_EDITBOX_FOLLOWER", UI_FLAG_DEFAULT, PAGE_EDITBOX_FOLLOWER );
		m_pEditBox_Follower->CreateCarrat ( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
		m_pEditBox_Follower->SetFont ( pFont );
		m_pEditBox_Follower->SetLimitInput ( CHR_ID_LENGTH+1 );
		RegisterControl ( m_pEditBox_Follower );
	}

	// Note : 버튼
	{
		CBasicTextButton* pButton;
		m_pButton_Tweet = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, PAGE_BUTTON_TWEET );
		pButton->CreateBaseButton( "SNS_WINDOW_PAGE_TWITTER_BUTTON_TWEET", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD("SNS_TWITTER_PAGE_BUTTON", 0) );
		RegisterControl( pButton );


		m_pButton_Follow = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, PAGE_BUTTON_FOLLOW );
		pButton->CreateBaseButton( "SNS_WINDOW_PAGE_TWITTER_BUTTON_FOLLOW", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD("SNS_TWITTER_PAGE_BUTTON", 1) );
		RegisterControl( pButton );

		m_pButton_Unfollow = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, PAGE_BUTTON_UNFOLLOW );
		pButton->CreateBaseButton( "SNS_WINDOW_PAGE_TWITTER_BUTTON_UNFOLLOW", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD("SNS_TWITTER_PAGE_BUTTON", 2) );
		RegisterControl( pButton );

		m_pButton_Back = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, PAGE_BUTTON_BACK );
		pButton->CreateBaseButton( "SNS_WINDOW_PAGE_TWITTER_BUTTON_BACK", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD( "SNS_PAGE_BUTTON", 0 ) );
		RegisterControl( pButton );
	}

	// Note : 체크박스
	{
		// Note : 상자
		for ( int i=0; i<PAGE_CHECKBOX_MAX; ++i )
		{
			CString strControl;

			strControl.Format( "SNS_WINDOW_PAGE_TWITTER_CHECK_%d", i );

			m_pCheckBox[i] = new CCheckBox(m_pEngineDevice);
			m_pCheckBox[i]->CreateSub ( this, strControl, UI_FLAG_XSIZE | UI_FLAG_YSIZE, PAGE_CHECKBOX_BEGIN+i );
			m_pCheckBox[i]->CreateSubControl ( "SNS_WINDOW_IMAGE_CHECKBOX_ON", "SNS_WINDOW_IMAGE_CHECKBOX_OFF" );
			RegisterControl ( m_pCheckBox[i] );
		}

		// Note : 텍스트
		for ( int i=0; i<PAGE_CHECKBOX_MAX; ++i )
		{
			CString strControl;

			strControl.Format( "SNS_WINDOW_PAGE_TWITTER_CHECK_TEXT_%d", i );

			m_pCheckBoxText[i] = CreateStaticControl( strControl.GetString(), pFont, nAlignLeft );
			m_pCheckBoxText[i]->SetOneLineText( "Empty" );
		}

		CString strFormat;
		m_pCheckBoxText[0]->SetOneLineText( ID2GAMEINTEXT( "SNS_WINDOW_PAGE_TWITTER_CHECK_LEVELUP" ) );

		strFormat.Format ( ID2GAMEINTEXT("SNS_WINDOW_PAGE_FACEBOOK_CHECK_GRINDING"), GLCONST_SNS::dw_REGISTER_GRINDING_LEVEL );
		m_pCheckBoxText[1]->SetOneLineText( strFormat );

		// Note : 사용하지 않은 체크박스들을 임시로 꺼준다.
		m_pCheckBox		[2]->SetVisibleSingle( FALSE );
		m_pCheckBoxText	[2]->SetVisibleSingle( FALSE );

		m_pCheckBox		[3]->SetVisibleSingle( FALSE );
		m_pCheckBoxText	[3]->SetVisibleSingle( FALSE );
	}
}

void CSNSTwitterPage::OpenSubControl ()
{
    //  Note : 체크박스 세팅
    RANPARAM::SNSTWITTER_SET&  sOptTwitter  = RANPARAM::sSNSTwitter;
	m_pCheckBox[INDEX_CHECK_BOX_LEVELUP		]->SetCheck  ( sOptTwitter.bLEVELUP		);
	m_pCheckBox[INDEX_CHECK_BOX_GRINDING	]->SetCheck  ( sOptTwitter.bGRINDING	);
}

void CSNSTwitterPage::CloseSubControl ()
{
    //  Note : 체크박스 저장
    RANPARAM::SNSTWITTER_SET&  sOptTwitter  = RANPARAM::sSNSTwitter;
	sOptTwitter.bLEVELUP	= m_pCheckBox[INDEX_CHECK_BOX_LEVELUP	]->IsChecked();
	sOptTwitter.bGRINDING	= m_pCheckBox[INDEX_CHECK_BOX_GRINDING	]->IsChecked();
}

void CSNSTwitterPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSNSTwitterPage::ClearEditTweet ()
{
	if ( m_pEditBox_Tweet )
	{
		m_pEditBox_Tweet->ClearEdit();
	}
}

void CSNSTwitterPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case PAGE_EDITBOX_TWEET:
		{
			if( !m_pEditBox_Tweet ) return;

			if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
			{
				m_pEditBox_Tweet->EndEdit();
				m_pEditBox_Tweet->BeginEdit();
			}
			else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
			{
				m_pEditBox_Tweet->EndEdit();
			}

			m_pEditBox_Tweet->TranslateUIMessage( PAGE_BUTTON_TWEET, dwMsg );
		}
		break;

	case PAGE_EDITBOX_FOLLOWER:
		{
			if( !m_pEditBox_Follower ) return;

			if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
			{
				m_pEditBox_Follower->EndEdit();
				m_pEditBox_Follower->BeginEdit();
			}
			else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
			{
				m_pEditBox_Follower->EndEdit();
			}
		}
		break;

	// Note : 팔로우
	case PAGE_BUTTON_FOLLOW:
		{
			if ( !m_pGaeaClient->GetSNSClient()->TTIsAuthed() )
			{
				break;
			}

			if ( !m_pEditBox_Follower )
			{
				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CString strFollow = m_pEditBox_Follower->GetEditString ();

				if ( strFollow.GetLength() == 0 )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR_EMPTY_EDITBOX"), MODAL_ERROR, OK );
					break;
				}

				if ( !m_pGaeaClient->GetSNSClient()->TTFollowID( std::string( strFollow.GetString() ) ) )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR"), MODAL_ERROR, OK );
					break;
				}

				SetEnableFollower( FALSE );
			}
		}
		break;

	// Note : 언팔로우
	case PAGE_BUTTON_UNFOLLOW:
		{
			if ( !m_pGaeaClient->GetSNSClient()->TTIsAuthed() )
			{
				break;
			}

			if ( !m_pEditBox_Follower )
			{
				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CString strUnfollow = m_pEditBox_Follower->GetEditString ();

				if ( strUnfollow.GetLength() == 0 )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR_EMPTY_EDITBOX"), MODAL_ERROR, OK );
					break;
				}

				if ( !m_pGaeaClient->GetSNSClient()->TTUnFollowID( std::string( strUnfollow.GetString() ) ) )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR"), MODAL_ERROR, OK );
					break;
				}

				SetEnableFollower( FALSE );
			}
		}
		break;

	// Note : 트위터에 쓰기
	case PAGE_BUTTON_TWEET:
		{
			if ( !m_pGaeaClient->GetSNSClient()->TTIsAuthed() )
			{
				break;
			}

			if ( !m_pEditBox_Tweet )
			{
				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CString strTweet = m_pEditBox_Tweet->GetEditString ();

				if ( strTweet.GetLength() == 0 )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR_EMPTY_EDITBOX"), MODAL_ERROR, OK );
					break;
				}

				if ( !m_pGaeaClient->GetSNSClient()->TTTweetMsg ( std::string(strTweet.GetString()) ) )
				{
					break;
				}

				SetEnableTweet( FALSE );
			}
			break;
		}
		break;

		// Note : 메뉴로 돌아가기
	case PAGE_BUTTON_BACK:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{				
				RequestPush( CSNSWindow::SNS_PAGE_REQ_MENU_PAGE_OPEN );
			}
		}
		break;
	}

	CUIPage::TranslateUIMessage ( ControlID, dwMsg );
}


CBasicLineBoxSmart* CSNSTwitterPage::CreateLineBox ( const char* szControl, UIGUID ControlID )
{
	CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub ( this, szControl, UI_FLAG_XSIZE | UI_FLAG_YSIZE, ControlID );
	pLineBox->CreateSubControl( "SNS_WINDOW_TEXTINFO_DEFAULT_LINEBOX" );
	RegisterControl ( pLineBox );		

	return pLineBox;
}


void CSNSTwitterPage::SetEnableTweet ( const BOOL bEnable )
{
	if ( bEnable )
	{
		m_pButton_Tweet->SetOneLineText( ID2GAMEWORD( "SNS_FACEBOOK_PAGE_BUTTON", 0 ) );
		m_pButton_Tweet->SetNoMessage( FALSE );
		m_pButton_Tweet->SetDiffuse( CUIPage::cCOLOR_BUTTON_ENABLE );
	}
	else
	{

		m_pButton_Tweet->SetOneLineText( ID2GAMEINTEXT("SNS_WINDOW_PAGE_BUTTON_REQUIRE") );
		m_pButton_Tweet->SetNoMessage( TRUE );
		m_pButton_Tweet->SetDiffuse( CUIPage::cCOLOR_BUTTON_DISABLE );
	}
}

void CSNSTwitterPage::SetEnableFollower ( const BOOL bEnable )
{
	if ( bEnable )
	{
		m_pButton_Follow->SetOneLineText( ID2GAMEWORD( "SNS_TWITTER_PAGE_BUTTON", 1 ) );
		m_pButton_Follow->SetNoMessage( FALSE );
		m_pButton_Follow->SetDiffuse( CUIPage::cCOLOR_BUTTON_ENABLE );

		m_pButton_Unfollow->SetOneLineText( ID2GAMEWORD( "SNS_TWITTER_PAGE_BUTTON", 2 ) );
		m_pButton_Unfollow->SetNoMessage( FALSE );
		m_pButton_Unfollow->SetDiffuse( CUIPage::cCOLOR_BUTTON_ENABLE );
	}
	else
	{
		m_pButton_Follow->SetOneLineText( ID2GAMEINTEXT("SNS_WINDOW_PAGE_BUTTON_REQUIRE") );
		m_pButton_Follow->SetNoMessage( TRUE );
		m_pButton_Follow->SetDiffuse( CUIPage::cCOLOR_BUTTON_DISABLE );

		m_pButton_Unfollow->SetOneLineText( ID2GAMEINTEXT("SNS_WINDOW_PAGE_BUTTON_REQUIRE") );
		m_pButton_Unfollow->SetNoMessage( TRUE );
		m_pButton_Unfollow->SetDiffuse( CUIPage::cCOLOR_BUTTON_DISABLE );
	}
}
