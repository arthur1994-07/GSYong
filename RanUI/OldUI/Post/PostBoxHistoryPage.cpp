#include "StdAfx.h"

#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"

#include "PostBoxHistoryPage.h"
#include "PostBoxHistoryList.h"
#include "PostBoxHistoryItem.h"
#include "PostBoxWindow.h"
#include "PostBoxWindowObject.h"

#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogicClient/Post/GLPostClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxHistoryPage::CPostBoxHistoryPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CPostBoxPage(pGaeaClient, pInterface, pEngineDevice)
    //, m_pList(NULL)
    , m_pSendList(NULL)
    , m_pRecvList(NULL)
    , m_pListActive(NULL)
	, m_pText_Static_Describe(NULL)
	, m_pText_Page(NULL)
	, m_pImage_Arrow_Left(NULL)
	, m_pImage_Arrow_Right(NULL)
    //, m_bStateRequested(FALSE)
    , m_bSendStateRequested(FALSE)
    , m_bRecvStateRequested(FALSE)
{
}

CPostBoxHistoryPage::~CPostBoxHistoryPage()
{

}

void CPostBoxHistoryPage::CreateSubControl()
{	
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	// Note : 배경색
	{
		CreateBackgroundControl ( "POSTBOX_PAGE_HISTORY_BACKGROUND" );
	}

	// Note : 라인
	{
		CreateBackgroundControl ( "POSTBOX_PAGE_HISTORY_DESCRIBE_LINE" );
	}

	// Note : 받은 우편 리스트
	{
        //m_pList = new CPostBoxHistoryList ( m_pGaeaClient, m_pEngineDevice );
        //m_pList->CreateSub( this, "POSTBOX_PAGE_HISTORY_LIST", UI_FLAG_DEFAULT, POSTBOX_PAGE_HISTORY_LIST );
        //m_pList->CreateSubControl();
        //RegisterControl( m_pList );

        m_pSendList = new CPostBoxHistoryList ( m_pGaeaClient, m_pEngineDevice, CPostBoxHistoryList::POSTBOX_HISTORY_SEND );
        m_pSendList->CreateSub( this, "POSTBOX_PAGE_SEND_HISTORY_LIST", UI_FLAG_DEFAULT, POSTBOX_PAGE_SEND_HISTORY_LIST );
        m_pSendList->CreateSubControl();
        m_pSendList->SetVisibleSingle( FALSE );
        RegisterControl( m_pSendList );

        m_pRecvList = new CPostBoxHistoryList ( m_pGaeaClient, m_pEngineDevice, CPostBoxHistoryList::POSTBOX_HISTORY_RECV );
        m_pRecvList->CreateSub( this, "POSTBOX_PAGE_RECV_HISTORY_LIST", UI_FLAG_DEFAULT, POSTBOX_PAGE_RECV_HISTORY_LIST );
        m_pRecvList->CreateSubControl();
        m_pRecvList->SetVisibleSingle( FALSE );
        RegisterControl( m_pRecvList );

        ChangeActivePostBoxList( m_pSendList );
	}

	// Note : 텍스트
	{
		m_pText_Static_Describe = CreateStaticControl ( "POSTBOX_PAGE_HISTORY_TEXT_STATIC_DESCRIBE",	pFont, TEXT_ALIGN_LEFT	   |TEXT_ALIGN_TOP );
		m_pText_Page			= CreateStaticControl ( "POSTBOX_PAGE_HISTORY_TEXT_PAGE",				pFont, TEXT_ALIGN_CENTER_X |TEXT_ALIGN_TOP );

		CString strFormat;
		strFormat.Format( (char*)ID2GAMEWORD( "POSTBOX_PAGE_HISTORY_TEXT_STATIC_DESCRIBE" ), GLCONST_POST::dwPOST_STATE_HOW_MANY_GET );
		m_pText_Static_Describe->AddText( strFormat );
	}

	// Note : 이미지
	{
		m_pImage_Arrow_Left  = CreateSwapImage ( "POSTBOX_PAGE_HISTORY_IMAGE_ARROW_LEFT",  POSTBOX_PAGE_HISTORY_IMAGE_ARROW_LEFT  );
		m_pImage_Arrow_Right = CreateSwapImage ( "POSTBOX_PAGE_HISTORY_IMAGE_ARROW_RIGHT", POSTBOX_PAGE_HISTORY_IMAGE_ARROW_RIGHT );

		m_pImage_Arrow_Left	 -> SetImage( "POSTBOX_PAGE_HISTORY_IMAGE_ARROW_LEFT_DEFAULT"  );
		m_pImage_Arrow_Left  -> SetVisibleSingle( FALSE );

		m_pImage_Arrow_Right -> SetImage( "POSTBOX_PAGE_HISTORY_IMAGE_ARROW_RIGHT_DEFAULT" );
		m_pImage_Arrow_Right -> SetVisibleSingle( FALSE );
	}

    // Note : 버튼
    {
        CBasicTextButton* pButton;
        m_pButton_History_Send_View = pButton = new CBasicTextButton(m_pEngineDevice);
        pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_HISTORY_BUTTON_SEND_VIEW );
        pButton->CreateBaseButton( "POSTBOX_PAGE_HISTORY_BUTTON_SEND_VIEW", CBasicTextButton::SIZE19, 
            CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_PAGE_HISTORY_BUTTON", 0 ) );
        RegisterControl( pButton );

        m_pButton_History_Recv_View = pButton = new CBasicTextButton(m_pEngineDevice);
        pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_HISTORY_BUTTON_RECV_VIEW );
        pButton->CreateBaseButton( "POSTBOX_PAGE_HISTORY_BUTTON_RECV_VIEW", CBasicTextButton::SIZE19, 
            CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_PAGE_HISTORY_BUTTON", 1 ) );
        RegisterControl( pButton );    
    }
}

void CPostBoxHistoryPage::BeginSubControl()
{
    ChangeActivePostBoxList( m_pSendList );

    ResetRequestState();

    m_pSendList->ResetCurrentPage();
    m_pRecvList->ResetCurrentPage();
}

void CPostBoxHistoryPage::OpenSubControl()
{
    RequestCurrentState();

	if ( m_pListActive )
	{
		m_pListActive->UpdateItem( TRUE );
	}

	// Note : 가변 컨트롤 업데이트
	UpdateVariableControl ();

	// Note : 가변 텍스트 업데이트
	UpdateVariableText ();
}

void CPostBoxHistoryPage::EndSubControl()
{
	// Blank
}

void CPostBoxHistoryPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CPostBoxPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	// Note : 가변 컨트롤 업데이트
	UpdateVariableControl ();

	// Note : 가변 텍스트 업데이트
	UpdateVariableText ();
}

void CPostBoxHistoryPage::UpdateVariableText ()
{
	// Note : 페이지 넘버
	{
		CString strPage;

		DWORD dwMaxPage = m_pListActive->GetMaxPage();
		DWORD dwCurPage = m_pListActive->GetCurrentPage() + 1;

		if  ( dwMaxPage == 0 )
		{
			dwMaxPage = 1;
		}

		strPage.Format( "%2d/%d", dwCurPage, dwMaxPage );
		m_pText_Page->SetOneLineText( strPage );
	}
}

void CPostBoxHistoryPage::UpdateVariableControl ()
{
	// Note : 화살표 이미지
	{
		DWORD dwMaxPage = m_pListActive->GetMaxPage();
		DWORD dwCurPage = m_pListActive->GetCurrentPage();

		if ( dwCurPage+1 >= dwMaxPage )
		{ 
			m_pImage_Arrow_Right->SetVisibleSingle( FALSE );
		}
		else
		{
			m_pImage_Arrow_Right->SetVisibleSingle( TRUE );
		}

		if ( dwCurPage == 0 )
		{
			m_pImage_Arrow_Left->SetVisibleSingle( FALSE );
		}
		else
		{
			m_pImage_Arrow_Left->SetVisibleSingle( TRUE );
		}
	}
}

void CPostBoxHistoryPage::UpdateList ()
{
	if ( m_pListActive )
	{
		m_pListActive->UpdateItem( TRUE );
	}
}

void CPostBoxHistoryPage::ResetRequestState()
{
    //m_bStateRequested = FALSE;
    m_bSendStateRequested = FALSE;
    m_bRecvStateRequested = FALSE;
}

void CPostBoxHistoryPage::RequestCurrentState ()
{
    //if ( m_pListActive == m_pList )
    //{
    //    if( m_bStateRequested == FALSE )
    //    {
    //        m_pGaeaClient->GetPostClient()->ReqReceiveHistory();
    //        m_bStateRequested = TRUE;
    //    }
    //}
    if ( m_pListActive == m_pSendList )
    {
        if( m_bSendStateRequested == FALSE )
        {
            m_pGaeaClient->GetPostClient()->ReqReceiveSendHistory();
            m_bSendStateRequested = TRUE;
        }
    }
    if ( m_pListActive == m_pRecvList )
    {
        if( m_bRecvStateRequested == FALSE )
        {
            m_pGaeaClient->GetPostClient()->ReqReceiveRecvHistory();
            m_bRecvStateRequested = TRUE;
        }
    }
}

void CPostBoxHistoryPage::ChangeActivePostBoxList ( CPostBoxHistoryList* pNewList )
{
    if ( !pNewList )
    {
        sc::writeLogError("in CPostBoxHistoryPage::ChangeActivePostBoxList() function, pNewList == NULL error");
        return;
    }

    if ( m_pListActive != pNewList )
    {
        if( m_pListActive )
            m_pListActive->SetVisibleSingle( FALSE );

        m_pListActive = pNewList;

        m_pListActive->SetVisibleSingle( TRUE );
    }
}

void CPostBoxHistoryPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case POSTBOX_PAGE_HISTORY_IMAGE_ARROW_LEFT :
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					m_pImage_Arrow_Left->SetImage( "POSTBOX_PAGE_HISTORY_IMAGE_ARROW_LEFT_DEFAULT" );
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{
					m_pListActive->PagePrev();
				}
				else
				{
					m_pImage_Arrow_Left->SetImage( "POSTBOX_PAGE_HISTORY_IMAGE_ARROW_LEFT_OVER" );
				}
			}
			else
			{
				m_pImage_Arrow_Left->SetImage( "POSTBOX_PAGE_HISTORY_IMAGE_ARROW_LEFT_DEFAULT" );
			}
		}
		break;

	case POSTBOX_PAGE_HISTORY_IMAGE_ARROW_RIGHT :
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					m_pImage_Arrow_Right->SetImage( "POSTBOX_PAGE_HISTORY_IMAGE_ARROW_RIGHT_DEFAULT" );
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{
					m_pListActive->PageNext();
				}
				else
				{
					m_pImage_Arrow_Right->SetImage( "POSTBOX_PAGE_HISTORY_IMAGE_ARROW_RIGHT_OVER" );
				}
			}
			else
			{
				m_pImage_Arrow_Right->SetImage( "POSTBOX_PAGE_HISTORY_IMAGE_ARROW_RIGHT_DEFAULT" );
			}
		}
		break;

    case POSTBOX_PAGE_HISTORY_BUTTON_SEND_VIEW :
        {
            if ( CHECK_MOUSE_IN (dwMsg ) )
            {
                if ( CHECK_LB_UP_LIKE ( dwMsg ) )
                {
                    ChangeActivePostBoxList( m_pSendList );
                    RequestCurrentState();
                }
            }
        }
		break;

    case POSTBOX_PAGE_HISTORY_BUTTON_RECV_VIEW :
        {
            if ( CHECK_MOUSE_IN (dwMsg ) )
            {
                if ( CHECK_LB_UP_LIKE ( dwMsg ) )
                {
                    ChangeActivePostBoxList( m_pRecvList );
                    RequestCurrentState();
                }
            }
        }
        break;

	}

	CPostBoxPage::TranslateUIMessage ( ControlID, dwMsg );
}

