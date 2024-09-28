#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UIInfoCfg.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/RenderParam.h"

#include "../../../RanLogicClient/Network/s_NetClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../RanLogic/RANPARAM.h"

#include "../../OuterInterface.h"
#include "SelectServerChannel.h"
#include "SelectServerPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	float	CSelectServerPage::fLIST_PART_INTERVAL = 10.0f;
const D3DCOLOR	CSelectServerPage::dwCLICKCOLOR = D3DCOLOR_ARGB(140,255,255,255);
const	int		CSelectServerPage::nOUTOFRANGE = -1;
const	float	CSelectServerPage::fNORMAL_PERCENT = 0.4f;
const	float	CSelectServerPage::fMAX_PERCENT = 0.7f;

CSelectServerPage::CSelectServerPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIOuterWindow ( pInterface, pEngineDevice )
    , m_pGaeaClient(pGaeaClient)
    , m_pServerInfo ( NULL )
    , m_nIndex ( nOUTOFRANGE )
    , m_pServerList ( NULL )
    , m_pServerState ( NULL )
    , m_pSelectServerChannel ( NULL )
{
	BOOST_STATIC_ASSERT( sizeof(m_nServerState) == sizeof(int) *MAX_SERVER_NUMBER );
	memset( m_nServerState, SERVER_SMOOTH, sizeof(int) *MAX_SERVER_NUMBER );
}

CSelectServerPage::~CSelectServerPage ()
{
}

void CSelectServerPage::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	if ( m_pInterface->IsCHANNEL() ) // 채널
	{
		EMSERVICE_PROVIDER sp =	m_pGaeaClient->GetServiceProvider();

		if (sp == SP_CHINA)
		{		
//#ifdef CH_PARAM
			CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
			pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, SELECT_CHINA_AREA_BACK );
			pButton->CreateBaseButton(	"SELECT_CHINA_AREA_BACK", 
										CBasicTextButton::SIZE19, 
										CBasicButton::CLICK_FLIP, 
										(char*)ID2GAMEWORD( "SELECT_CHINA_AREA" ) );
			RegisterControl( pButton );
		}
//#endif

		m_pSelectServerChannel = new CSelectServerChannel ( m_pInterface, m_pEngineDevice );
		m_pSelectServerChannel->CreateSubEx ( this, "OUTER_WINDOW", SELECT_SERVER_CHANNEL );
		m_pSelectServerChannel->CreateBaseWidnow ( "SELECT_SERVER_PAGE", (char*) ID2GAMEWORD ( "SELECT_SERVER" ) );			
		m_pSelectServerChannel->CreateSubControl ();
		m_pSelectServerChannel->ResetAlignFlag ();
		m_pSelectServerChannel->SetLocalPos ( D3DXVECTOR2(GetLocalPos().right + 5.0f, GetLocalPos().top) );
		m_pSelectServerChannel->SetVisibleSingle ( FALSE );
		RegisterControl ( m_pSelectServerChannel );	
	}
	else
	{
		CBasicTextButton* pOKButton = new CBasicTextButton(m_pEngineDevice);
		pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, SELECT_SERVER_CONNECT );
		pOKButton->CreateBaseButton ( "SELECT_SERVER_CONNECT", CBasicTextButton::SIZE19, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SELECT_SERVER_CONNECTQUIT", 0 ) );
		pOKButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl ( pOKButton );

		m_pServerState = new CBasicTextBox(m_pEngineDevice);
		m_pServerState->CreateSub ( this, "SELECT_SERVER_LIST_STATE" );
		m_pServerState->SetFont ( pFont9 );
		m_pServerState->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		m_pServerState->SetPartInterval ( fLIST_PART_INTERVAL );
		RegisterControl ( m_pServerState );
	}

	CBasicTextButton* pQuitButton = new CBasicTextButton(m_pEngineDevice);
	pQuitButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, SELECT_SERVER_QUIT );
	pQuitButton->CreateBaseButton ( "SELECT_SERVER_QUIT", 
									CBasicTextButton::SIZE19, 
									CBasicButton::CLICK_FLIP, 
									(char*)ID2GAMEWORD ( "SELECT_SERVER_CONNECTQUIT", 1 ) );
	pQuitButton->SetShortcutKey ( DIK_ESCAPE );
	RegisterControl ( pQuitButton );

	{
		m_pServerList = new CBasicTextBoxEx(m_pEngineDevice);
		m_pServerList->CreateSub ( this, "SELECT_SERVER_LIST", UI_FLAG_DEFAULT, SELECT_SERVER_LIST );
		m_pServerList->SetFont ( pFont9 );
		m_pServerList->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		m_pServerList->SetPartInterval ( fLIST_PART_INTERVAL );
		m_pServerList->CreateMouseClick ( dwCLICKCOLOR );
		m_pServerList->SetSensitive ( TRUE );
		m_pServerList->SetSensitiveView ( false );
		RegisterControl ( m_pServerList );

		int nTotalLine = m_pServerList->GetVisibleLine ();

		//	스크롤바
		m_pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		m_pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, SERVER_SCROLLBAR );
		m_pScrollBar->CreateBaseScrollBar ( "SERVER_SCROLLBAR" );
		m_pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( m_pScrollBar );
		m_pScrollBar->SetVisibleSingle ( FALSE );
	}

	CreateControl ( "SELECT_SERVER_BOTTOM" );
}

BOOL CSelectServerPage::InitServerPage ()
{
    m_pGaeaClient->SetUserCountry( wb::UNKNOWN_COUNTRY ); // Init user country.

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();
	if ( pNetClient->IsOnline () )
	{
		if ( !pNetClient->IsGameSvrInfoEnd () )
		{
			if ( !m_pInterface->UiIsVisibleGroup ( MODAL_WINDOW_OUTER ) )
			{
				m_pInterface->DoModal ( ID2GAMEEXTEXT ( "SERVERSTAGE_1" ), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_WAITSERVERINFO );
			}
		}
		else
		{
			m_pServerInfo = pNetClient->GetSeverCurInfoLogin ();

			const int MAX_SERVER = MAX_SERVER_GROUP*MAX_SERVER_NUMBER;	//	전체 서버수
			int	nUSING_SERVER_AMOUNT = 0;	//	사용중인 서버수
			struct S_SERVER_INFO
			{
				int		nGroupIndex;
				int		nChannelIndex;
				int		nUserAmount;
				int		nMaxAmount;
                bool    bNewServer;

				S_SERVER_INFO () :
					nGroupIndex ( -1 ),
					nChannelIndex ( -1 ),
					nUserAmount ( 0 ),
					nMaxAmount ( 0 ),
                    bNewServer ( false )
				{
				}
			};
			S_SERVER_INFO sSERVERINFO[MAX_SERVER];			

			//	카운트
			{
				nUSING_SERVER_AMOUNT = 0;
				for ( int i = 0; i < MAX_SERVER_GROUP; ++i )
				{
					S_SERVER_INFO& sInfo = sSERVERINFO[nUSING_SERVER_AMOUNT];	//	레퍼런스로 인덱스 없앰

					sInfo.nGroupIndex = i;
					for ( int j = 0, index = 0; j < MAX_SERVER_NUMBER; ++j )
					{
						LPG_SERVER_CUR_INFO_LOGIN pServerInfo = m_pServerInfo + ( i * MAX_SERVER_NUMBER ) + j;
						if ( !pServerInfo || !pServerInfo->nServerMaxClient ) continue;
						
						sInfo.nChannelIndex = j;
						sInfo.nUserAmount = pServerInfo->nServerCurrentClient;
						sInfo.nMaxAmount = pServerInfo->nServerMaxClient;
                        sInfo.bNewServer = pServerInfo->bNewServer;

						//	존재한다면 인덱스 증가
						++nUSING_SERVER_AMOUNT;
					}
				}
			}

			if ( !nUSING_SERVER_AMOUNT )
			{
				m_pInterface->DoModal ( ID2GAMEEXTEXT ( "SERVER_INFO_RECV_FAILED" ), UI::MODAL_INFOMATION, UI::OK );
				return FALSE;
			}


            // 한국의 경우 접속자가 많은순으로 정렬
            switch ( RENDERPARAM::emSERVICE_TYPE )
            {
			// GSServer는 정렬하지않는다;
			// GS 프로바이더 바꿔야되기때문에 KR로 테스트한다
			case RENDERPARAM::EMSERVICE_GLOBAL :	break;
            case RENDERPARAM::EMSERVICE_DEFAULT:
            case RENDERPARAM::EMSERVICE_KOREA:
                {
                    for ( int i = 0; i < nUSING_SERVER_AMOUNT; ++i )
				    {
					    for ( int j = 0; j < nUSING_SERVER_AMOUNT - 1; ++j )
					    {
						    if ( sSERVERINFO[j].nUserAmount < sSERVERINFO[j+1].nUserAmount )
						    {
							    S_SERVER_INFO temp = sSERVERINFO[j];
							    sSERVERINFO[j] = sSERVERINFO[j+1];
							    sSERVERINFO[j+1] = temp;
						    }                        
					    }
				    }
                }
                  break;
            default:
                {
                    for ( int i = 0; i < nUSING_SERVER_AMOUNT; ++i )
				    {
					    for ( int j = 0; j < nUSING_SERVER_AMOUNT - 1; ++j )
					    {
						    if ( sSERVERINFO[j].nUserAmount > sSERVERINFO[j+1].nUserAmount )
						    {
							    S_SERVER_INFO temp = sSERVERINFO[j];
							    sSERVERINFO[j] = sSERVERINFO[j+1];
							    sSERVERINFO[j+1] = temp;
						    }                        
					    }
				    }
                }
                break;                
            }	

			//	리스트 구성
			{
				for ( int i = 0; i < nUSING_SERVER_AMOUNT; ++i )
				{
					const S_SERVER_INFO& sInfo = sSERVERINFO[i];	//	레퍼런스로 인덱스 없앰

					CString strCombine;
					strCombine.Format ( "%d %s", sInfo.nGroupIndex, ID2GAMEEXTEXT ("SERVER_NAME",0) );

					{	//	서버 이름
						int nIndex = m_pServerList->AddTextNoSplit ( strCombine, NS_UITEXTCOLOR::DEFAULT );
						m_pServerList->SetTextData ( nIndex, sInfo.nGroupIndex );

                        if ( sInfo.bNewServer ) 
                        {
                            m_pServerList->SetTextImage( nIndex, "NEW_SERVER_IMAGE" );
                        }
                        else
                        {
                            m_pServerList->SetTextImage( nIndex, "NEW_SERVER_IMAGE_NONE" );
                        }
					}

					{	//	서버 상태
						float fPercent = float(sInfo.nUserAmount) / float(sInfo.nMaxAmount);
						int nState = 0;
						if ( fPercent < GLCONST_CHAR::wSERVER_NORMAL * 0.01f )			nState = 0;
						else if ( fPercent < GLCONST_CHAR::wSERVER_CONGEST * 0.01f )	nState = 1;
						else															nState = 2;
						
						D3DCOLOR dwStateColor[3] = 
						{
							NS_UITEXTCOLOR::PALEGREEN,
							NS_UITEXTCOLOR::GOLD,
							NS_UITEXTCOLOR::RED
						};

                        if ( m_pServerState )
                        {
						    int nIndex = m_pServerState->AddTextNoSplit ( ID2GAMEWORD("SERVER_STATE", nState), dwStateColor[nState] );
						    m_pServerState->SetTextData ( nIndex, sInfo.nChannelIndex );
                        }
					}
				}
			}

			const int nTotalLine = m_pServerList->GetTotalLine ();
			const int nLinePerOneView = m_pServerList->GetVisibleLine ();
			m_pScrollBar->SetVisibleSingle ( FALSE );
			if ( nLinePerOneView < nTotalLine )
			{
				m_pScrollBar->SetVisibleSingle ( TRUE );

				m_pServerList->SetTextAlign ( TEXT_ALIGN_CENTER_X );
			}

/*
			m_pServerInfo = pNetClient->GetSeverCurInfoLogin ();

			for ( int i = 0; i < MAX_SERVER_GROUP; ++i )
			for ( int j = 0, index = 0; j < MAX_SERVER_NUMBER; ++j )
			{
				LPG_SERVER_CUR_INFO_LOGIN pServerInfo = m_pServerInfo + ( i * MAX_SERVER_NUMBER ) + j;
				if ( !pServerInfo || !pServerInfo->nServerMaxClient ) continue;

				CString strCombine;
				strCombine.Format ( "%s(%d/%d)", ID2GAMEEXTEXT ("SERVER_NAME"), i, j );

				{	//	서버 이름
					int nIndex = m_pServerList->AddTextNoSplit ( strCombine, NS_UITEXTCOLOR::DEFAULT );
					m_pServerList->SetTextData ( nIndex, i );
				}

				{	//	서버 상태
					float fPercent = float(pServerInfo->nServerCurrentClient) / float(pServerInfo->nServerMaxClient);
					int nState = 0;
					if ( fPercent < fNORMAL_PERCENT )	nState = 0;
					else if ( fPercent < fMAX_PERCENT )	nState = 1;
					else								nState = 2;
					
					D3DCOLOR dwStateColor[3] = 
					{
						NS_UITEXTCOLOR::PALEGREEN,
						NS_UITEXTCOLOR::GOLD,
						NS_UITEXTCOLOR::RED
					};

					int nIndex = m_pServerState->AddTextNoSplit ( ID2GAMEWORD("SERVER_STATE", nState), dwStateColor[nState] );
					m_pServerState->SetTextData ( nIndex, j );
				}
			}

			const int nTotalLine = m_pServerList->GetTotalLine ();
			const int nLinePerOneView = m_pServerList->GetLinePerOneView ();
			m_pScrollBar->SetVisibleSingle ( FALSE );
			if ( nLinePerOneView < nTotalLine )
			{
				m_pScrollBar->SetVisibleSingle ( TRUE );

				m_pServerList->SetTextAlign ( TEXT_ALIGN_CENTER_X );
				m_pServerState->SetTextAlign ( TEXT_ALIGN_CENTER_X );
			}
*/
		}
	}	
	else	//	네트워크 연결이 실패한 경우
	{
		if ( !m_pInterface->UiIsVisibleGroup ( MODAL_WINDOW_OUTER ) )
		{
			if( !m_pInterface->IsLoginCancel() )
			{
				m_pInterface->DoModal ( ID2GAMEEXTEXT("SERVERSTAGE_4"), UI::MODAL_QUESTION, UI::OKCANCEL, OUTER_MODAL_CLOSEGAME );
			}
			else
			{
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
                if (pGlobalStage->GetNetClient()->ConnectLoginServer(RANPARAM::LoginAddress, RANPARAM::LoginServerPort) > NET_ERROR )
				{
					pGlobalStage->GetNetClient()->SndReqServerInfo();				
				}
				else
				{
					m_pInterface->DoModal( ID2GAMEEXTEXT ( "SERVERSTAGE_3" ), UI::MODAL_INFOMATION, UI::OKCANCEL, OUTER_MODAL_CLOSEGAME );
				}
			}
		}
	}

	return TRUE;
}

void CSelectServerPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pInterface->IsCHANNEL() )
	{
		if ( !m_pServerInfo ) 
			InitServerPageChannel ();

	}

	{
		if ( !m_pServerInfo ) 
			InitServerPage ();

		{
			CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame ();

			const int nTotalLine = m_pServerList->GetTotalLine ();
			const int nLinePerOneView = m_pServerList->GetVisibleLine ();
			pThumbFrame->SetState ( nTotalLine, nLinePerOneView );		
			if ( nLinePerOneView < nTotalLine )
			{
				const int nMovableLine = nTotalLine - nLinePerOneView;
				float fPercent = pThumbFrame->GetPercent ();
				int nPos = (int)floor(fPercent * nMovableLine);
				m_pServerList->SetCurLine ( nPos );
				//m_pServerState->SetCurLine ( nPos );
			}
		}
	}
}

void CSelectServerPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( m_pInterface->IsCHANNEL() )
	{
		TranslateUIMessageChannel ( ControlID, dwMsg );
		return ;
	}

	switch ( ControlID )
	{
	case SELECT_SERVER_CONNECT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				Login ();
			}
		}
		break;

	case SELECT_SERVER_QUIT:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->DoModal ( ID2GAMEINTEXT("CLOSE_GAME"), UI::MODAL_QUESTION, UI::OKCANCEL, OUTER_MODAL_CLOSEGAME );
				SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus 문제로 메세지 삭제
			}
		}
		break;

	case SELECT_SERVER_LIST:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nIndex = m_pServerList->GetSelectPos ();				
				if ( nIndex < 0 || m_pServerList->GetCount () <= nIndex ) return ;

				if ( dwMsg & UIMSG_LB_UP )
				{
					m_nIndex = nIndex;
				}

				if ( dwMsg & UIMSG_LB_DUP )
				{
					Login ();
				}
			}
		}
		break;
	}
}

void	CSelectServerPage::Login ()
{
	if ( m_pInterface->IsCHANNEL() )		
	{
		if ( !ConnectServerChannel () )
			return ;
	}
	else
	{
		if ( !ConnectServer () ) 
			return ;
	}

    // 서버 인덱스 세팅
	RANPARAM::dwServerNum = (DWORD)m_nIndex;
	CUIInfoCfg::GetInstance().SetAddPathNameServerNum(RANPARAM::dwServerNum);
	//	NOTE
	//		'다음'이면, 로그인 페이지를 건너뛴다.
	if ( RANPARAM::VALIDIDPARAM () )
	{
		m_pInterface->START_WAIT_TIME ( 1.0f );
		m_pInterface->DoModal ( ID2GAMEEXTEXT ("DAUM_LOGIN_WAIT"), UI::MODAL_INFOMATION, UI::CANCEL, OUTER_MODAL_LOGIN_WAIT_DAUM );
	}
	else
	{
		m_pInterface->ToLoginPage ( GetWndID () );
	}
}

BOOL CSelectServerPage::ConnectServer ()
{
	if ( m_nIndex < 0 )
	{
//		DoModalOuter ( GetWndID (), ID2GAMEEXTEXT ("SERVERSTAGE_2"), UI::MODAL_INFOMATION );
		return FALSE;
	}

	DWORD dwServerGroup = m_pServerList->GetTextData ( m_nIndex );
	DWORD dwServerNumber = m_pServerState->GetTextData ( m_nIndex );

	if ( dwServerGroup == UINT_MAX || dwServerNumber == UINT_MAX ) return FALSE;

	int nServerGroup = (int)dwServerGroup;
	int nServerNumber = (int)dwServerNumber;	
	
	//	접속할 서버 아이피,포트
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient ();	
	if (pNetClient->ConnectGameServer(nServerGroup, nServerNumber) <= NET_ERROR)
	{
		//접속 실패 에러
		m_pInterface->DoModal ( ID2GAMEEXTEXT ("SERVERSTAGE_5"), UI::MODAL_INFOMATION );					
		return FALSE;
	}

    m_pGaeaClient->SetServerGroup(nServerGroup);
	return TRUE;
}

void CSelectServerPage::ResetAll ()
{
	if ( m_pServerList )	m_pServerList->ClearText ();
	if ( m_pServerState )	m_pServerState->ClearText ();
	if ( m_pSelectServerChannel ) m_pSelectServerChannel->SetVisibleSingle ( FALSE );

	m_nIndex = nOUTOFRANGE;
	m_pServerInfo = NULL;
}

// 통합전장 로그인 W.B.
BOOL CSelectServerPage::LoginWorldBattle()
{
	if ( !ConnectWorldBattleServerChannel() )
	{
		return FALSE;
	}
	return TRUE;
}
