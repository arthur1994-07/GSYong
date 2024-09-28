#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "PostNotifyButton.h"

#include "../../InnerInterface.h"

#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogicClient/Post/GLPostClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostNotifyButton::CPostNotifyButton(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) 
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pImage(NULL)
    , m_pText(NULL)
    , m_pImageTextBack(NULL)
    , m_emType(EMNOTIFY_TYPE_INIT)
    , m_dwNew(0)
    , m_dwTotal(0)
{
}

CPostNotifyButton::~CPostNotifyButton()
{
}

void CPostNotifyButton::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, _DEFAULT_FONT_FLAG );

	CSwapImage* pSwapImage = new CSwapImage(m_pEngineDevice);
	pSwapImage->CreateSub ( this, "POSTNOTIFY_BUTTON_IMAGE", UI_FLAG_DEFAULT, POSTNOTIFY_BUTTON_IMAGE );
	pSwapImage->SetImage( "POSTNOTIFY_BUTTON_IMAGE_DEFAULT" );
	RegisterControl ( pSwapImage );
	m_pImage = pSwapImage;

	pSwapImage = new CSwapImage(m_pEngineDevice);
	pSwapImage->CreateSub ( this, "POSTNOTIFY_BUTTON_BACKGROUND_IMAGE", UI_FLAG_DEFAULT );
	pSwapImage->SetImage( "POSTNOTIFY_BUTTON_BACKGROUND_IMAGE_RED" );
	RegisterControl ( pSwapImage );
	m_pImageTextBack = pSwapImage;

	CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);
	pText->CreateSub ( this, "POSTNOTIFY_BUTTON_TEXT", UI_FLAG_DEFAULT );
	pText->SetFont ( pFont );
	pText->SetTextAlign ( TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y );	
	RegisterControl ( pText );
	m_pText = pText;

	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		SetVisibleSingle( FALSE );
	}
}

void CPostNotifyButton::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	DWORD dwNew		= m_pGaeaClient->GetPostClient()->GetNewPost();
	DWORD dwWaiting = m_pGaeaClient->GetPostClient()->GetWaitingPost();
	DWORD dwTotal	= m_pGaeaClient->GetPostClient()->GetTotalPost();

	EMNOTIFY_TYPE emType = EMNOTIFY_TYPE_NONE;
	BOOL		  bDiff  = FALSE;
	BOOL		  bNew	 = FALSE;

	if ( dwNew || dwWaiting )
	{
		emType = EMNOTIFY_TYPE_UNREAD;
	}

	if ( emType != m_emType )
	{
		m_emType = emType;
		bDiff	 = TRUE;
	}

	if ( dwNew != m_dwNew )
	{
		if ( m_dwNew < dwNew )
		{
			bNew = TRUE;
		}

		m_dwNew = dwNew;
		bDiff   = TRUE;
	}

	if ( dwTotal != m_dwTotal )
	{
		m_dwTotal = dwTotal;
		bDiff     = TRUE;
	}

	if ( bDiff )
	{
		switch ( m_emType )
		{
		case EMNOTIFY_TYPE_NONE:
			{
				m_pText			 -> SetVisibleSingle ( FALSE );
				m_pImageTextBack -> SetVisibleSingle ( FALSE );

				// Note : 이미지
				{
					m_pImage->SetImage( "POSTNOTIFY_BUTTON_IMAGE_DEFAULT" );
				}
			}
			break;

		case EMNOTIFY_TYPE_UNREAD:
			{
				m_pText			 -> SetVisibleSingle ( TRUE );
				m_pImageTextBack -> SetVisibleSingle ( TRUE );

				// Note : 이미지
				{
					m_pImage->SetImage( "POSTNOTIFY_BUTTON_IMAGE_UNREAD" );
				}

				// Note : 우편 갯수
				{
					CString strText;
					strText.Format( "%d", dwNew );
					m_pText->SetOneLineText( strText );
				}
			}
			break;
		};

		// Note : 새로운 우편이 왔을 경우 메시지를 출력 한다.
		if ( bNew )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("POSTBOX_MSG_RECEIVE_NEW") );
		}	
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


void CPostNotifyButton::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case POSTNOTIFY_BUTTON_IMAGE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				DWORD dwNew = m_pGaeaClient->GetPostClient()->GetNewPost();

				if ( dwNew )
				{
					m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT( "POSTBOX_TOOLTIP_NOTIFYBUTTON" ), NS_UITEXTCOLOR::WHITE );
				}
			}
		}
		break;
	}

	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

MyPostNotifyButton::MyPostNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CPostNotifyButton( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyPostNotifyButton::CreateUIWindowAndRegisterOwnership()
{
    CPostNotifyButton::Create( POSTNOTIFY_BUTTON, "POSTNOTIFY_BUTTON", UI_FLAG_RIGHT | UI_FLAG_BOTTOM );
    CPostNotifyButton::CreateSubControl ();
    CPostNotifyButton::m_pInterface->UiRegisterControl( this );		
    CPostNotifyButton::m_pInterface->UiShowGroupBottom( POSTNOTIFY_BUTTON );
}