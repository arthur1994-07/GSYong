#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"

#include "PostBoxReceivePage.h"
#include "PostBoxReceiveList.h"
#include "PostBoxReceiveItem.h"
#include "PostBoxWindow.h"
#include "PostBoxWindowObject.h"
#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
//#include "../../ModalCallerID.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogicClient/Post/GLPostClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxReceivePage::CPostBoxReceivePage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CPostBoxPage(pGaeaClient, pInterface, pEngineDevice)
	, m_pList(NULL)
	, m_pButton_Delete(NULL)
	, m_pButton_Refresh(NULL)
	, m_pText_New(NULL)
	, m_pText_Page(NULL)
	, m_pImage_Arrow_Left(NULL)
	, m_pImage_Arrow_Right(NULL)
{


}

CPostBoxReceivePage::~CPostBoxReceivePage()
{

}

void CPostBoxReceivePage::CreateSubControl()
{	
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	// Note : 배경색
	{
		CreateBackgroundControl ( "POSTBOX_PAGE_RECEIVE_BACKGROUND" );
	}

	// Note : 받은 우편 리스트
	{
		m_pList = new CPostBoxReceiveList(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pList->CreateSub(this, "POSTBOX_PAGE_RECEIVE_LIST", UI_FLAG_DEFAULT, POSTBOX_PAGE_RECEIVE_LIST);
		m_pList->CreateSubControl();
		RegisterControl(m_pList);
	}

	// Note : 텍스트
	{
		m_pText_New	 = CreateStaticControl ( "POSTBOX_PAGE_RECEIVE_TEXT_NEW",		pFont, TEXT_ALIGN_LEFT	   |TEXT_ALIGN_TOP );
		m_pText_Page = CreateStaticControl ( "POSTBOX_PAGE_RECEIVE_TEXT_PAGE",		pFont, TEXT_ALIGN_CENTER_X |TEXT_ALIGN_TOP );
	}

	// Note : 이미지
	{
		m_pImage_Arrow_Left  = CreateSwapImage ( "POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_LEFT",  POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_LEFT  );
		m_pImage_Arrow_Right = CreateSwapImage ( "POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_RIGHT", POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_RIGHT );

		m_pImage_Arrow_Left	 -> SetImage( "POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_LEFT_DEFAULT"  );
		m_pImage_Arrow_Left  -> SetVisibleSingle( FALSE );

		m_pImage_Arrow_Right -> SetImage( "POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_RIGHT_DEFAULT" );
		m_pImage_Arrow_Right -> SetVisibleSingle( FALSE );
	}

	// Note : 버튼
	{
		CBasicTextButton* pTextButton;
		m_pButton_Delete = pTextButton = new CBasicTextButton(m_pEngineDevice);
		pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_RECEIVE_BUTTON_DELETE );
		pTextButton->CreateBaseButton( "POSTBOX_PAGE_RECEIVE_BUTTON_DELETE", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_PAGE_RECEIVE_BUTTON", 0 ) );
		RegisterControl( pTextButton );

		// Note : 새로고침 버튼
		CBasicButton* pButton;
		m_pButton_Refresh = pButton = new CBasicButton(m_pEngineDevice);
		pButton->CreateSub ( this, "POSTBOX_PAGE_RECEIVE_BUTTON_REFRESH", UI_FLAG_DEFAULT, POSTBOX_PAGE_RECEIVE_BUTTON_REFRESH );
		pButton->CreateFlip ( "POSTBOX_PAGE_RECEIVE_BUTTON_REFRESH_MOUSEIN", CBasicButton::MOUSEIN_FLIP );
		RegisterControl ( pButton );
	}
}

void CPostBoxReceivePage::BeginSubControl()
{
	m_pGaeaClient->GetPostClient()->ReqReceivePostInfo();
	m_pGaeaClient->GetPostClient()->ReqReceivePost();
}

void CPostBoxReceivePage::OpenSubControl()
{
	if ( m_pList )
	{
		m_pList->OpenSubControl();
		m_pList->UpdateItem();
	}

	// Note : 가변 컨트롤 업데이트
	UpdateVariableControl ();

	// Note : 가변 텍스트 업데이트
	UpdateVariableText ();
}

void CPostBoxReceivePage::CloseSubControl()
{
	if ( m_pList )
	{
		m_pList->CloseSubControl();
		RequestPush  ( CPostBoxWindow::POSTBOX_PAGE_REQ_READ_CLOSE );
	}
}

void CPostBoxReceivePage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CPostBoxPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	// Note : 가변 컨트롤 업데이트
	UpdateVariableControl ();

	// Note : 가변 텍스트 업데이트
	UpdateVariableText ();
}

void CPostBoxReceivePage::UpdateVariableText ()
{
	// Note : 읽지 않은 메일 수, 누적 메일 수
	{
		m_pText_New->ClearText();

		CString strNew;
		strNew.Format(
            ID2GAMEWORD( "POSTBOX_PAGE_RECEIVE_TEXT" ), 
			m_pGaeaClient->GetPostClient()->GetNewPost(), 
			m_pGaeaClient->GetPostClient()->GetWaitingPost());
		m_pText_New->AddText(strNew);
	}

	// Note : 페이지 넘버
	{
		CString strPage;

		DWORD dwMaxPage = m_pList->GetMaxPage();
		DWORD dwCurPage = m_pList->GetCurrentPage() + 1;

		if  ( dwMaxPage == 0 )
		{
			dwMaxPage = 1;
		}

		strPage.Format( "%2d/%d", dwCurPage, dwMaxPage );
		m_pText_Page->SetOneLineText( strPage );
	}
}

void CPostBoxReceivePage::UpdateVariableControl ()
{
	// Note : 화살표 이미지
	{
		DWORD dwMaxPage = m_pList->GetMaxPage();
		DWORD dwCurPage = m_pList->GetCurrentPage();

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

	// Note : 삭제 버튼
	if ( m_pList )
	{
		DWORD dwItemArray[ CPostBoxReceiveList::MAX_ITEM ];
		DWORD dwCount  = 0;

		m_pList->GetCheckedItems( dwItemArray, dwCount );

		if ( dwCount )
		{
			m_pButton_Delete->SetDiffuse( cCOLOR_BUTTON_ENABLE );
			m_pButton_Delete->SetNoMessage( FALSE );
			m_pButton_Delete->SetNoUpdate ( FALSE );
		}
		else
		{
			m_pButton_Delete->SetDiffuse( cCOLOR_BUTTON_DISABLE );
			m_pButton_Delete->SetNoMessage( TRUE );
			m_pButton_Delete->SetNoUpdate ( TRUE );
		}
	}
}

void CPostBoxReceivePage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case POSTBOX_PAGE_RECEIVE_LIST :
		{
			if ( dwMsg&UIMSG_POSTBOX_RECEIVE_LIST_READ )
			{
				RequestPush  ( CPostBoxWindow::POSTBOX_PAGE_REQ_READ_OPEN );
			}
		}
		break;

	case POSTBOX_PAGE_RECEIVE_BUTTON_DELETE:
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->DoModal( ID2GAMEINTEXT("MODAL_POSTBOX", 2),  UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_POSTBOX_PAGE_RECEIVE_BUTTON_DELETE_CHECK );
			}
		}
		break;

	case POSTBOX_PAGE_RECEIVE_BUTTON_REFRESH:
		{			
			if ( !m_pButton_Refresh )
			{
				break;
			}

			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pButton_Refresh->SetDiffuse( cCOLOR_REFRESH_BUTTON_ENABLE );

				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					m_pButton_Refresh->SetDiffuse( cCOLOR_REFRESH_BUTTON_DISABLE );
				}
				else if ( CHECK_LB_UP_LIKE( dwMsg ) )
				{
					RequestPush  ( CPostBoxWindow::POSTBOX_PAGE_REQ_RECEIVE_REFRESH );
				}

				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT( "POSTBOX_TOOLTIP_RECEIVE_REFRESH" ), NS_UITEXTCOLOR::WHITE );
			}
			else
			{
				m_pButton_Refresh->SetDiffuse( cCOLOR_REFRESH_BUTTON_DISABLE );
			}
		}
		break;

	case POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_LEFT :
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					m_pImage_Arrow_Left->SetImage( "POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_LEFT_DEFAULT" );
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{
					m_pList->PagePrev();
				}
				else
				{
					m_pImage_Arrow_Left->SetImage( "POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_LEFT_OVER" );
				}
			}
			else
			{
				m_pImage_Arrow_Left->SetImage( "POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_LEFT_DEFAULT" );
			}
		}
		break;

	case POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_RIGHT :
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					m_pImage_Arrow_Right->SetImage( "POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_RIGHT_DEFAULT" );
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{
					m_pList->PageNext();
				}
				else
				{
					m_pImage_Arrow_Right->SetImage( "POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_RIGHT_OVER" );
				}
			}
			else
			{
				m_pImage_Arrow_Right->SetImage( "POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_RIGHT_DEFAULT" );
			}
		}
		break;
	}

	CPostBoxPage::TranslateUIMessage ( ControlID, dwMsg );
}

void CPostBoxReceivePage::UpdateItem ()
{
	if ( m_pList )
	{
		m_pList->UpdateItem ();
	}
}

void CPostBoxReceivePage::UpdateItem ( SAPOSTCLIENT* pPostData )
{
	if ( m_pList )
	{
		m_pList->UpdateItem ( pPostData );
	}
}

const BOOL CPostBoxReceivePage::ReturnItem ( const DWORD dwItem )
{
	if ( m_pList )
	{
		return m_pList->ReturnItem ( dwItem );
	}

	return FALSE;
}

const BOOL CPostBoxReceivePage::DeleteItem ( const DWORD dwItem )
{
	if ( m_pList )
	{
		return m_pList->DeleteItem ( dwItem );
	}

	return FALSE;
}

const BOOL CPostBoxReceivePage::ReturnIndex ( const DWORD dwIndex )
{
	if ( m_pList )
	{
		return m_pList->ReturnIndex ( dwIndex );
	}

	return FALSE;
}

const BOOL CPostBoxReceivePage::DeleteIndex ( const DWORD dwIndex )
{
	if ( m_pList )
	{
		return m_pList->DeleteIndex ( dwIndex );
	}

	return FALSE;
}

void CPostBoxReceivePage::SelectPost ( const DWORD dwItem )
{
	if ( m_pList )
	{
		m_pList->SelectPost ( dwItem );
	}
}

SAPOSTCLIENT* CPostBoxReceivePage::GetSelectedPost ()
{
	if ( m_pList )
	{
		return m_pList->GetSelectedPost();
	}

	return NULL;
}

const DWORD	CPostBoxReceivePage::GetSelectedIndex ()
{
	if ( m_pList )
	{
		return m_pList->GetSelectedIndex();
	}

	return CPostBoxReceiveList::SELECT_NULL;
}

const DWORD	CPostBoxReceivePage::GetSelectedItem ()
{
	if ( m_pList )
	{
		return m_pList->GetSelectedItem();
	}

	return CPostBoxReceiveList::SELECT_NULL;
}

void CPostBoxReceivePage::ReqMultiDelete_Check ()
{

	if ( !m_pList )
	{
		return;
	}

	DWORD dwItemArray[ CPostBoxReceiveList::MAX_ITEM ];
	DWORD dwCount = 0;

	m_pList->GetCheckedItems( dwItemArray, dwCount );

	if ( dwCount == 0 )
	{
		return;
	}

	// Note : 첨부물이 존재하는 아이템을 삭제하려 할때
	for ( DWORD i=0; i<dwCount; ++i )
	{
		CPostBoxReceiveItem* pItem = m_pList->GetItem ( dwItemArray[i] );

		if ( !pItem )
		{
			continue;
		}

		SAPOSTCLIENT* pPost = pItem->GetPost();

		// Note : 아이템이나 금액이 첨부 되어있다면
		if ( pPost->m_byMoneyAccept == FALSE && pPost->m_llAttachMoney )
		{
			m_pInterface->DoModal( ID2GAMEINTEXT("MODAL_POSTBOX", 8),  UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_POSTBOX_PAGE_RECEIVE_BUTTON_DELETE_COMMIT );
			return;
		}

		if ( pPost->m_byAttach01Accept == FALSE && pPost->m_sAttach01.GetNativeID() != NATIVEID_NULL() )
		{
			m_pInterface->DoModal( ID2GAMEINTEXT("MODAL_POSTBOX", 8),  UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_POSTBOX_PAGE_RECEIVE_BUTTON_DELETE_COMMIT );
			return;
		}	

		// Note : 대금 청구이고 지불을 하지 않았을때
		if ( pPost->m_byChargePayment == FALSE && pPost->m_llCharge )
		{
			m_pInterface->DoModal( ID2GAMEINTEXT("MODAL_POSTBOX", 8),  UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_POSTBOX_PAGE_RECEIVE_BUTTON_DELETE_COMMIT );
			return;
		}
	}

	ReqMultiDelete_Commit ();
}

void CPostBoxReceivePage::ReqMultiDelete_Commit ( const BOOL bAttachDelete )
{
	if ( !m_pList )
	{
		return;
	}

	DWORD dwItemArray[ CPostBoxReceiveList::MAX_ITEM ];
	DWORD dwCount = 0;

	m_pList->GetCheckedItems( dwItemArray, dwCount );

	if ( dwCount == 0 )
	{
		return;
	}

	for ( DWORD i=0; i<dwCount; ++i )
	{
		CPostBoxReceiveItem* pItem = m_pList->GetItem ( dwItemArray[i] );

		if ( !pItem )
		{
			continue;
		}
		
		SAPOSTCLIENT* pPost = pItem->GetPost();

		if ( !pPost )
		{
			continue;
		}

		if (!m_pGaeaClient->GetPostClient()->ReqDeletePost(pPost, bAttachDelete))
		{
			// Note : 첨부물 삭제에 한에서 반송 가능한 메일은 반송한다.
			if ( bAttachDelete )
			{
				m_pGaeaClient->GetPostClient()->ReqReturnPost(pPost);
			}

			continue;
		}
	}

	m_pList->SelectPost( CPostBoxReceiveList::SELECT_NULL );
	RequestPush ( CPostBoxWindow::POSTBOX_PAGE_REQ_READ_CLOSE );
}
