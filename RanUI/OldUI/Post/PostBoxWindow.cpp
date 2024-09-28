#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Post/GLPostClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"
#include "PostBoxWindow.h"
#include "PostBoxWindowMain.h"
#include "PostBoxWindowSub.h"
#include "PostBoxWindowNaked.h"
#include "PostBoxReceivePage.h"
#include "PostBoxReadPage.h"
#include "PostBoxSendPage.h"
#include "PostBoxHistoryPage.h"
#include "PostBoxFriendFindPage.h"
#include "PostBoxPreviewPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxWindow::CPostBoxWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
	, m_pd3dDevice(NULL)
	, m_pWindow_Main(NULL)
	, m_pWindow_Sub(NULL)
	, m_pWindow_Naked(NULL)
	, m_pPage_Receive(NULL)
	, m_pPage_Read(NULL)
	, m_pPage_Send(NULL)
	, m_pPage_History(NULL)
	, m_pPage_FriendFind(NULL)
{
	// Blank
}

CPostBoxWindow::~CPostBoxWindow()
{
	// Note : 페이지들을 제거
	if ( !m_vecPage.empty() )
	{
		VECPAGEITER iter = m_vecPage.begin();
		for ( ; iter!=m_vecPage.end(); ++iter )
		{
			CPostBoxPage* pPage = (*iter);

			if ( !pPage )
			{
				continue;
			}

			delete pPage;
		}

		m_vecPage.clear();
	}
}

HRESULT	CPostBoxWindow::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	HRESULT hr = S_OK;
	hr = CUIGroup::InitDeviceObjects( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	VECPAGEITER iter = m_vecPage.begin();
	for ( ; iter!=m_vecPage.end(); ++iter )
	{
		CPostBoxPage* pPage = (*iter);

		if ( !pPage )
		{
			continue;
		}

		hr = pPage->InitDeviceObjects( pd3dDevice );
		if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}

CBasicLineBox* CPostBoxWindow::CreateBackgroundControl( char* szConatrolKeyword )
{
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox(m_pEngineDevice);
	pLineBoxCombo->CreateSub( this, szConatrolKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	CUIControl* pBody = new CUIControl(m_pEngineDevice);
	pBody->CreateSub( pLineBoxCombo, szConatrolKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	RegisterControl( pBody );
	RegisterControl( pLineBoxCombo );

	return pLineBoxCombo;
}

CBasicTextBox* CPostBoxWindow::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont( pFont );
	pStaticText->SetTextAlign( nAlign );	
	RegisterControl( pStaticText );

	return pStaticText;
}

void CPostBoxWindow::CreateSubControl( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 슬롯 초기화
	if ( !m_vecSlot.empty() )
	{
		m_vecSlot.clear();
	}

	// Note : 윈도우
	{
		m_pWindow_Main = new CPostBoxWindowMain( m_pInterface, m_pEngineDevice );
		m_pWindow_Main->CreateSub( this,  "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POSTBOX_WINDOW_MAIN );
		m_pWindow_Main->CreateBaseWindowLightGray( "POSTBOX_WINDOW_SLOT_0", (char*)ID2GAMEWORD("POSTBOX_WINDOW_TITLE") );
		m_pWindow_Main->CreateSubControl( pd3dDevice );
		m_pWindow_Main->SetAlignFlag( UI_FLAG_LEFT | UI_FLAG_TOP );
		RegisterControl( m_pWindow_Main );
		m_pWindow_Main->SetVisibleSingle( FALSE );

		m_pWindow_Sub = new CPostBoxWindowSub( m_pInterface, m_pEngineDevice );
		m_pWindow_Sub->CreateSub( this,  "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POSTBOX_WINDOW_SUB );
		m_pWindow_Sub->CreateBaseWindowLightGray( "POSTBOX_WINDOW_SLOT_0", (char*)ID2GAMEWORD("POSTBOX_WINDOW_TITLE") );
		m_pWindow_Sub->CreateSubControl( pd3dDevice	);
		m_pWindow_Sub->SetAlignFlag( UI_FLAG_LEFT | UI_FLAG_TOP );
		RegisterControl( m_pWindow_Sub );
		m_pWindow_Sub->SetVisibleSingle( FALSE );

		m_pWindow_Naked = new CPostBoxWindowNaked( m_pInterface, m_pEngineDevice );
		m_pWindow_Naked->CreateSub( this,  "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POSTBOX_WINDOW_NAKED );
		m_pWindow_Naked->CreateBaseWindowLightGray( "POSTBOX_WINDOW_SLOT_0", (char*)ID2GAMEWORD("POSTBOX_WINDOW_TITLE") );
		m_pWindow_Naked->CreateSubControl( pd3dDevice );
		m_pWindow_Naked->SetAlignFlag( UI_FLAG_LEFT | UI_FLAG_TOP );
		RegisterControl( m_pWindow_Naked );
		m_pWindow_Naked->SetVisibleSingle( FALSE );
	}

	// Note : 페이지
	{
		if ( !m_vecPage.empty() )
		{
			m_vecPage.clear();
		}

		m_pPage_Receive = new CPostBoxReceivePage(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pPage_Receive->Create(
            "POSTBOX_PAGE_RECEIVE",
            (char*) ID2GAMEWORD("POSTBOX_PAGE_TITLE", 0),
            POSTBOX_PAGE_RECEIVE);
		m_vecPage.push_back(m_pPage_Receive);

		m_pPage_Read = new CPostBoxReadPage(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pPage_Read->Create(
            "POSTBOX_PAGE_READ",
            (char*) ID2GAMEWORD("POSTBOX_PAGE_TITLE", 1),
            POSTBOX_PAGE_READ);
		m_vecPage.push_back(m_pPage_Read);

		m_pPage_Send = new CPostBoxSendPage(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pPage_Send->Create(
            "POSTBOX_PAGE_SEND",
            (char*) ID2GAMEWORD("POSTBOX_PAGE_TITLE", 2),
            POSTBOX_PAGE_SEND);
		m_vecPage.push_back(m_pPage_Send);

		m_pPage_History = new CPostBoxHistoryPage(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pPage_History->Create(
            "POSTBOX_PAGE_HISTORY",
            (char*) ID2GAMEWORD("POSTBOX_PAGE_TITLE", 3),
            POSTBOX_PAGE_HISTORY);
		m_vecPage.push_back(m_pPage_History);

		m_pPage_FriendFind = new CPostBoxFriendFindPage(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pPage_FriendFind->Create(
            "POSTBOX_PAGE_FRIENDFIND",
            (char*) ID2GAMEWORD("POSTBOX_PAGE_TITLE", 4),
            POSTBOX_PAGE_FRIENDFIND);
		m_vecPage.push_back(m_pPage_FriendFind);

		m_pPage_Preview = new CPostBoxPreviewPage(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pPage_Preview->Create(
            "POSTBOX_PAGE_PREVIEW",
            (char*) ID2GAMEWORD("POSTBOX_PAGE_TITLE", 5),
            POSTBOX_PAGE_PREVIEW);
		m_vecPage.push_back(m_pPage_Preview);
	}
}

void CPostBoxWindow::SetVisibleSingle( BOOL bVisible )
{
	if ( bVisible )
	{
		WindowBegin();
	}
	else
	{
		WindowEnd();
        Close();
	}
}

void CPostBoxWindow::Close()
{
	if ( IsWindowCloseAble(TRUE) )
	{
		m_pInterface->UiHideGroup( GetWndID() );
	}
}

void CPostBoxWindow::RefreshMainPage()
{
	if ( m_pWindow_Main )
	{
		switch ( m_pWindow_Main->GetTapID() )
		{
		case CPostBoxWindowMain::TAP_RECEIVE:
			m_pWindow_Main->PageOpen( m_pPage_Receive );
			break;

		case CPostBoxWindowMain::TAP_SEND:
			m_pWindow_Main->PageOpen( m_pPage_Send );
			break;

		case CPostBoxWindowMain::TAP_HISTORY:
			m_pWindow_Main->PageOpen( m_pPage_History );
			break;
		}
	}
}

/*  //Old Style
void CPostBoxWindow::WindowPageOpen  ( UIGUID WndControlID, CPostBoxPage* pPage )
{
	if ( !m_pWindow_Main || !m_pWindow_Sub || !m_pWindow_Naked )
	{
		return;
	}

	switch ( WndControlID )
	{
	case POSTBOX_WINDOW_MAIN:
		{	
			if ( !m_pWindow_Main->IsWindowOpen() )
			{
				if ( m_pWindow_Sub->IsWindowOpen() )
				{
					if ( m_pWindow_Naked->IsWindowOpen() )
					{
						m_pWindow_Main->WindowOpen( "POSTBOX_WINDOW_SLOT_3" );
					}
					else
					{
						m_pWindow_Main->WindowOpen( "POSTBOX_WINDOW_SLOT_2" );
					}
				}
				else
				{
					m_pWindow_Main->WindowOpen( "POSTBOX_WINDOW" );
				}
			}

			m_pWindow_Main->PageOpen ( pPage );
		}
		break;

	case POSTBOX_WINDOW_SUB:
		{	
			if ( !m_pWindow_Main->IsWindowOpen() )
			{
				break;
			}

			if ( m_pWindow_Naked->IsWindowOpen() )
			{
				m_pWindow_Sub->WindowOpen  ( "POSTBOX_WINDOW_SLOT_2" );
				m_pWindow_Naked->SetResize ( "POSTBOX_WINDOW_NAKED_SLOT_3" );
			}
			else
			{
				m_pWindow_Sub->WindowOpen  ( "POSTBOX_WINDOW_SLOT_2" );
			}

			m_pWindow_Sub->PageOpen ( pPage );
		}
		break;

	case POSTBOX_WINDOW_NAKED:
		{	
			if ( !m_pWindow_Main->IsWindowOpen() )
			{
				break;
			}

			if ( m_pWindow_Sub->IsWindowOpen() )
			{
				m_pWindow_Naked->WindowOpen ( "POSTBOX_WINDOW_SLOT_3" );
				m_pWindow_Naked->SetResize  ( "POSTBOX_WINDOW_NAKED_SLOT_3" );
			}
			else
			{
				m_pWindow_Naked->WindowOpen ( "POSTBOX_WINDOW_SLOT_2" );
				m_pWindow_Naked->SetResize  ( "POSTBOX_WINDOW_NAKED_SLOT_2" );
			}

			m_pWindow_Naked->PageOpen ( pPage );
		}
		break;
	};
}
*/


void CPostBoxWindow::WindowResize()
{
	// Note : 활성화 되어있는 슬롯의 갯수를 가져온다
	DWORD dwSlotCount = GetEnableSlotCount();

	CString strSizeControl;
	strSizeControl.Format( "POSTBOX_WINDOW_SLOTSIZE_%d", dwSlotCount );

	CUIControl TempControl(m_pEngineDevice); 
	TempControl.Create ( 1, strSizeControl.GetString() ); // MEMO

	UIRECT rcParentLocalPos   = GetLocalPos();
	UIRECT rcControlLoacalPos = TempControl.GetLocalPos();

	AlignSubControl( rcParentLocalPos, rcControlLoacalPos );
	SetLocalPos( rcControlLoacalPos );

	UIRECT rcParentGlobalOldPos = GetGlobalPos();
	UIRECT rcParentGlobalNewPos = rcParentGlobalOldPos;

	rcParentGlobalNewPos.left   = rcParentGlobalOldPos.left;
	rcParentGlobalNewPos.top    = rcParentGlobalOldPos.top;
	rcParentGlobalNewPos.right  = rcParentGlobalNewPos.left + rcControlLoacalPos.sizeX;
	rcParentGlobalNewPos.bottom = rcParentGlobalNewPos.top  + rcControlLoacalPos.sizeY;
	rcParentGlobalNewPos.sizeX  = rcControlLoacalPos.sizeX;
	rcParentGlobalNewPos.sizeY  = rcControlLoacalPos.sizeY;

	SetGlobalPos( rcParentGlobalNewPos );
}

void CPostBoxWindow::WindowBegin()
{
	if ( m_pd3dDevice )
	{
		if ( !IsWindowOpenAble() )
		{
			return;
		}

		// Note : 활성화 된 슬롯이 있다면 전부삭제
		if ( GetEnableSlotCount() )
		{
			for ( DWORD i=0; i<(DWORD)m_vecSlot.size(); ++i )
			{
				CPostBoxWindowObject* &pSlot = m_vecSlot[ i ];

				if ( pSlot )
				{
					pSlot->SetSlot( SLOT_NULL );
					pSlot->PageClose();
					pSlot->WindowClose();
					pSlot = NULL;
				}
			}
		}

		if ( !m_bOpen && m_pd3dDevice )
		{
			VECPAGEITER iter = m_vecPage.begin();
			for ( ; iter!=m_vecPage.end(); ++iter )
			{
				CPostBoxPage* pPage = (*iter);

				if ( !pPage )
				{
					continue;
				}

				pPage->Begin();
			}

			// Note : 메인 페이지 오픈
			WindowPageOpenSlot( m_pWindow_Main, NULL, SLOT_MAIN );
			m_pWindow_Main->TapSelect( CPostBoxWindowMain::TAP_RECEIVE );
			RefreshMainPage();
		}
	}

	CUIGroup::SetVisibleSingle( TRUE );
	m_bOpen = TRUE;
}

void CPostBoxWindow::WindowEnd()
{
	if ( !m_bOpen )
	{
		return;
	}

	if ( m_pd3dDevice )
	{
		if ( !IsWindowCloseAble(TRUE) )
		{
			return;
		}

		// Note : 활성화 된 슬롯이 있다면 전부삭제
		if ( GetEnableSlotCount()  )
		{
			for ( DWORD i=0; i<(DWORD)m_vecSlot.size(); ++i )
			{
				CPostBoxWindowObject* &pSlot = m_vecSlot[ i ];

				if ( pSlot )
				{
					pSlot->SetSlot( SLOT_NULL );
					pSlot->PageClose();
					pSlot->WindowClose();
					pSlot = NULL;
				}
			}
		}

		if ( m_bOpen && m_pd3dDevice )
		{
			VECPAGEITER iter = m_vecPage.begin();
			for ( ; iter!=m_vecPage.end(); ++iter )
			{
				CPostBoxPage* pPage = (*iter);

				if ( !pPage )
				{
					continue;
				}

				pPage->End();
			}
		}

		// Note : 닫기 요청 보내기
		m_pGaeaClient->GetCharacter()->ReqPostBoxClose();
	}

	// Note : 컨트롤 독점 리셋
	ResetExclusiveControl();

	// Note : 숨기기
	CUIGroup::SetVisibleSingle( FALSE );
	m_bOpen = FALSE;
}

void CPostBoxWindow::WindowPageOpen( UIGUID WndControlID, CPostBoxPage* pPage )
{
	if ( !pPage )
	{
		return;
	}

	CPostBoxWindowObject* pWindow = NULL;

	switch ( WndControlID )
	{
	case POSTBOX_WINDOW_MAIN:
		pWindow = m_pWindow_Main;
		break;

	case POSTBOX_WINDOW_SUB:
		pWindow = m_pWindow_Sub;
		break;
	
	case POSTBOX_WINDOW_NAKED:
		pWindow = m_pWindow_Naked;
		break;
	};

	if ( !pWindow )
	{
		return;
	}

	// Note : 슬롯 설정
	DWORD dwSlot = 0;

	// Note : 이미 슬롯이 열려있다면 해당 슬롯을 가져온다.
	if ( pWindow->IsWindowOpen() )
	{
		dwSlot = pWindow->GetSlot();
	}
	// Note : 비어있는 슬롯을 찾는다.
	else
	{
		dwSlot = GetEmptySlot();
	}

	if ( dwSlot == SLOT_NULL )
	{
		return;
	}

	WindowPageOpenSlot( pWindow, pPage, dwSlot );
}

void CPostBoxWindow::WindowPageOpen	( CPostBoxWindowObject* pWindow, CPostBoxPage* pPage )
{
	if ( !pWindow )
	{
		return;
	}

	DWORD dwSlot = 0;

	// Note : 이미 슬롯이 열려있다면 해당 슬롯을 가져온다.
	if ( pWindow->IsWindowOpen() )
	{
		dwSlot = pWindow->GetSlot();
	}
	// Note : 비어있는 슬롯을 찾는다.
	else
	{
		dwSlot = GetEmptySlot();
	}

	if ( dwSlot == SLOT_NULL )
	{
		return;
	}

	WindowPageOpenSlot( pWindow, pPage, dwSlot );
}

void CPostBoxWindow::WindowPageOpenSlot( CPostBoxWindowObject* pWindow, CPostBoxPage* pPage, const DWORD dwSlot )
{
	if ( !pWindow )
	{
		return;
	}

	if ( SLOT_MAX <= dwSlot )
	{
		return;
	}

	// Note : 해당 윈도우가 이미 열려있는 윈도우라면 닫고 새로 연다.
	if ( pWindow->IsWindowOpen() )
	{
		DWORD dwCurSlot = pWindow->GetSlot();

		if ( dwCurSlot == SLOT_NULL )
		{
			return;
		}

		m_vecSlot[ dwCurSlot ] = NULL;

		pWindow->SetSlot( SLOT_NULL );
		pWindow->PageClose();
		pWindow->WindowClose();
	}

	// Note : 사이즈 동기화
	DWORD dwSlotSize = (DWORD)m_vecSlot.size();

	if ( dwSlotSize <= dwSlot )
	{
		m_vecSlot.resize( dwSlotSize + 1, NULL );
	}

	CPostBoxWindowObject* &pSlot = m_vecSlot[ dwSlot ];

	// Note : 해당슬롯에 사용하던 윈도우 닫기
	if ( pSlot && pSlot->IsWindowOpen() )
	{
		pSlot->SetSlot( SLOT_NULL );
		pSlot->PageClose();
		pSlot->WindowClose();
	}
	
	pSlot = pWindow;
	pSlot->SetSlot( dwSlot );

	// Note : 슬롯 리사이징
	CString strSlotControl;
	strSlotControl.Format( "POSTBOX_WINDOW_SLOT_%d", dwSlot );
	pSlot->SetResize( strSlotControl.GetString() );

	pSlot->WindowOpen();
	pSlot->PageOpen( pPage );

	WindowResize();
}

void CPostBoxWindow::WindowPageClose( CPostBoxPage* pPage )
{
	if ( !pPage )
	{
		return;
	}

	WindowPageClose( pPage->GetParentWindow() );
}

void CPostBoxWindow::WindowPageClose( UIGUID WndControlID )
{
	CPostBoxWindowObject* pWindow = NULL;

	switch ( WndControlID )
	{
	case POSTBOX_WINDOW_MAIN:
		pWindow = m_pWindow_Main;
		break;

	case POSTBOX_WINDOW_SUB:
		pWindow = m_pWindow_Sub;
		break;

	case POSTBOX_WINDOW_NAKED:
		pWindow = m_pWindow_Naked;
		break;
	};

	if ( !pWindow )
	{
		return;
	}

	WindowPageClose( pWindow );
}

void CPostBoxWindow::WindowPageClose( CPostBoxWindowObject* pWindow )
{
	if ( !pWindow )
	{
		return;
	}

	//  Note : 슬롯을 순서대로 제거한다 
	DWORD dwSlotSize = (DWORD)m_vecSlot.size();
	BOOL  dwBegin    = pWindow->GetSlot();

	// Note : 메인 슬롯을 닫으려할때는 닫기 메서드를 호출
	if ( dwBegin == SLOT_MAIN )
	{
		Close();
		return;
	}

	if ( dwBegin != SLOT_NULL )
	{
		for ( DWORD i=dwBegin; i<dwSlotSize; ++i )
		{
			CPostBoxWindowObject* &pSlot = m_vecSlot[ i ];

			if ( pSlot )
			{
				pSlot->PageClose();
				pSlot->WindowClose();
				pSlot = NULL;
			}
		}

		WindowResize();
	}

	// Note : 활성화 된 슬롯이 하나도 없다면
	if ( GetEnableSlotCount() == 0 )
	{
		// Note : 본창을 닫는다.
		Close();
	}
}

const BOOL CPostBoxWindow::IsWindowOpenAble( const BOOL bPrint )
{
	// Note : 우편함 사용 불가능이면
	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return FALSE;
	}

	// Note : 이미 열려 있다면
	if ( m_bOpen )
	{
		return FALSE;
	}

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	if ( !pChar || !pChar->IsValidBody() )
	{
		return FALSE;
	}

	// Note : 받기 쿨타임 체크
	if (m_pGaeaClient->GetPostClient()->IsRecvCoolTime())
	{
		if ( bPrint )
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_RECV_COOLTIME") );
		}

		return FALSE;
	}

	// Note : 대련중에 열 수 없음
	if ( pChar->GetConftType( pChar->GetGaeaID() ) != EMCONFT_NONE )
	{
		if ( bPrint )
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_OPEN_CONFT") );
		}

		return FALSE;
	}

	// Note : 개인 상점 중에 열수 없음
	if ( m_pInterface->UiIsVisibleGroup( PRIVATE_MARKET_WINDOW ) )
	{
		if ( bPrint )
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_OPEN_PMARKET") );
		}

		return FALSE;
	}

	// Note : 거래 중에 열수 없음
	if ( m_pInterface->UiIsVisibleGroup( TRADE_WINDOW ) )
	{
		if ( bPrint )
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_OPEN_TRADE") );
		}

		return FALSE;
	}

	// Note : 우편을 요청중이면 열 수 없다.
	if (m_pGaeaClient->GetPostClient()->IsRequirePost())
	{
		if ( bPrint )
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_OPEN_PROCESSING") );
		}

		return FALSE;
	}

	//// Note : 현황을 요청중이면 열 수 없다.
	//if (m_pGaeaClient->GetPostClient()->IsRequireHistory())
	//{
	//	if ( bPrint )
	//	{
	//		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_OPEN_PROCESSING") );
	//	}

	//	return FALSE;
	//}

	return TRUE;
}

const BOOL CPostBoxWindow::IsWindowCloseAble( const BOOL bPrint )
{
	return TRUE;
}

const BOOL CPostBoxWindow::IsPageOpen( CPostBoxPage* pPage )
{
	if ( !pPage )
	{
		return FALSE;
	}

	for ( DWORD i=0; i<(DWORD)m_vecSlot.size(); ++i )
	{
		CPostBoxWindowObject* &pSlot = m_vecSlot[ i ];

		if ( pSlot )
		{
			if ( pPage == pSlot->GetPage() )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

const DWORD	CPostBoxWindow::GetEnableSlotCount()
{
	// Note : 활성화 되어있는 슬롯의 갯수를 가져온다
	DWORD   dwSlotSize	   = (DWORD)m_vecSlot.size();
	DWORD   dwSlotCount	   = 0;

	for ( DWORD i=0; i<dwSlotSize; ++i )
	{
		if ( !m_vecSlot[i] )
		{
			continue;
		}

		++dwSlotCount;
	}

	return dwSlotCount;
}

const DWORD	CPostBoxWindow::GetEmptySlot()
{
	// Note : 비어있는 슬롯을 찾는다.
	DWORD dwSlotSize = (DWORD)m_vecSlot.size();
	DWORD dwSlot	 = dwSlotSize;

	for ( DWORD i=0; i<dwSlotSize; ++i )
	{
		if ( !m_vecSlot[i] )
		{
			dwSlot = i;
			break;
		}
	}

	if ( SLOT_MAX <= dwSlot )
	{
		dwSlot = SLOT_NULL;
	}

	return dwSlot;
}

// Note : 우편받기 완료 프로세스
void CPostBoxWindow::Process_ReceiveComplete()
{
	if ( m_bOpen )
	{
		DWORD dwWaitingPost		= m_pGaeaClient->GetPostClient()->GetWaitingPost();
		int nWaitingNewPost  = m_pGaeaClient->GetPostClient()->GetNewPost() - m_pGaeaClient->GetPostClient()->GetReceivedNewPost();

		if ( nWaitingNewPost > 0 )
		{
			std::string strText = sc::string::format( ID2GAMEINTEXT("POSTBOX_WARNING_MSG_UNREAD_POST"), dwWaitingPost, nWaitingNewPost );
			m_pInterface->DoModal( strText,  UI::MODAL_INFOMATION, UI::OK, UI::MODAL_POSTBOX_PAGE_READ_BUTTON_RETURN );
		}
	}
}

// Note : 에러 메세지 모달
void CPostBoxWindow::DoModalError( const std::string& strError )
{
	m_pInterface->DoModal( strError, UI::MODAL_ERROR, UI::OK );
}

// Note : 일반 메세지 모달
void CPostBoxWindow::DoModalMsg( const std::string& strMsg )
{
	m_pInterface->DoModal( strMsg, UI::MODAL_INFOMATION, UI::OK );
}

void CPostBoxWindow::PostBoxHistoryPageUpdateList()
{
    if ( m_pPage_History )
        m_pPage_History->UpdateList();
}

void CPostBoxWindow::RequestPushViewPage( const DWORD dwRequest )
{
    if ( m_pPage_Preview )
        m_pPage_Preview->RequestPush( dwRequest );
}

void CPostBoxWindow::ReqMultiDelete_Check()
{
    if ( m_pPage_Receive )
        m_pPage_Receive->ReqMultiDelete_Check();
}

void CPostBoxWindow::ReqMultiDelete_Commit()
{
    if( m_pPage_Receive )
        m_pPage_Receive->ReqMultiDelete_Commit();
}

void CPostBoxWindow::PostBoxReceivePageUpdateItem()
{
    if ( m_pPage_Receive )
        m_pPage_Receive->UpdateItem();
}

void CPostBoxWindow::ReqSendPost()
{
    if ( m_pPage_Send )
        m_pPage_Send->ReqSendPost();
}

void CPostBoxWindow::ResetSendPage()
{
    if ( m_pPage_Send )
        m_pPage_Send->Reset();
}

bool CPostBoxWindow::IsSendPageEnalbe()
{
    if ( m_pPage_Send )
        return true;

    return false;
}

const BOOL CPostBoxWindow::IsSendPageOpen()
{
    if ( m_pPage_Send )
        return IsPageOpen( m_pPage_Send );

    return false;
}

void CPostBoxWindow::Set_TO_Editbox( const UI::String& strText_To )
{
    if ( m_pPage_Send )
        m_pPage_Send->SetEditBoxTo( UI::ToString( strText_To ) );
}

void CPostBoxWindow::ReqPostDelete( bool bConfirmAttach )
{
    if ( m_pPage_Read )
        m_pPage_Read->ReqDelete( bConfirmAttach );
}

void CPostBoxWindow::ReqPostReturn()
{
    if ( m_pPage_Read )
        m_pPage_Read->ReqReturn();
}

void CPostBoxWindow::ReqPostTakeItem()
{
    if ( m_pPage_Read )
        m_pPage_Read->ReqTakeItem();
}

MyPostBoxWindow::MyPostBoxWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CPostBoxWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyPostBoxWindow::CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice )
{
    CPostBoxWindow::Create( POSTBOX_WINDOW, "POSTBOX_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CPostBoxWindow::CreateSubControl( pd3dDevice );
    CPostBoxWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
    CPostBoxWindow::m_pInterface->UiRegisterControl( this, true );
    CPostBoxWindow::m_pInterface->UiShowGroupFocus( POSTBOX_WINDOW );
}

void MyPostBoxWindow::Close()
{
    CPostBoxWindow::Close();
}

void MyPostBoxWindow::DoModalError( const std::string& strError )
{
    CPostBoxWindow::DoModalError( strError );
}

void MyPostBoxWindow::DoModalMsg( const std::string& strMsg )
{
    CPostBoxWindow::DoModalMsg( strMsg );
}

CPostBoxHistoryPage* MyPostBoxWindow::GetHistoryPage()
{
    return CPostBoxWindow::GetHistoryPage();
}

CPostBoxPreviewPage* MyPostBoxWindow::GetPreviewPage()
{
    return CPostBoxWindow::GetPreviewPage();
}

CPostBoxReceivePage* MyPostBoxWindow::GetReceivePage()
{
    return CPostBoxWindow::GetReceivePage();
}

CPostBoxSendPage* MyPostBoxWindow::GetSendPage()
{
    return CPostBoxWindow::GetSendPage();
}

BOOL MyPostBoxWindow::IsVisible()
{
    return CPostBoxWindow::IsVisible();
}

const BOOL MyPostBoxWindow::IsWindowOpenAble( const BOOL bPrint/*=FALSE */ )
{
    return CPostBoxWindow::IsWindowOpenAble( bPrint );
}

const BOOL MyPostBoxWindow::IsWindowCloseAble( const BOOL bPrint/*=FALSE */ )
{
    return CPostBoxWindow::IsWindowCloseAble( bPrint );
}

void MyPostBoxWindow::Process_ReceiveComplete()
{
    CPostBoxWindow::Process_ReceiveComplete();
}

CPostBoxReadPage* MyPostBoxWindow::GetReadPage()
{
    return CPostBoxWindow::GetReadPage();
}

const BOOL MyPostBoxWindow::IsSendPageOpen()
{
    return CPostBoxWindow::IsSendPageOpen();
}

void MyPostBoxWindow::PostBoxHistoryPageUpdateList()
{
    CPostBoxWindow::PostBoxHistoryPageUpdateList();
}

void MyPostBoxWindow::RequestPushViewPage( const DWORD dwRequest )
{
    CPostBoxWindow::RequestPushViewPage( dwRequest );
}

void MyPostBoxWindow::ReqMultiDelete_Check()
{
    CPostBoxWindow::ReqMultiDelete_Check();
}

void MyPostBoxWindow::ReqMultiDelete_Commit()
{
    CPostBoxWindow::ReqMultiDelete_Commit();
}

void MyPostBoxWindow::PostBoxReceivePageUpdateItem()
{
    CPostBoxWindow::PostBoxReceivePageUpdateItem();
}

void MyPostBoxWindow::ReqSendPost()
{
    CPostBoxWindow::ReqSendPost();
}

void MyPostBoxWindow::ResetSendPage()
{
    CPostBoxWindow::ResetSendPage();
}

bool MyPostBoxWindow::IsSendPageEnalbe()
{
    return CPostBoxWindow::IsSendPageEnalbe();
}

void MyPostBoxWindow::Set_TO_Editbox( const UI::String& strText_To )
{
    CPostBoxWindow::Set_TO_Editbox( strText_To );
}

void MyPostBoxWindow::ReqPostDelete( bool bConfirmAttach /* = true */ )
{
    CPostBoxWindow::ReqPostDelete( bConfirmAttach );
}

void MyPostBoxWindow::ReqPostReturn()
{
    CPostBoxWindow::ReqPostReturn();
}

void MyPostBoxWindow::ReqPostTakeItem()
{
    CPostBoxWindow::ReqPostTakeItem();
}