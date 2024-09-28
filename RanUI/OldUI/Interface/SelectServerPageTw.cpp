#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/RENDERPARAM.h"


#include "../../../RanLogicClient/Network/s_NetClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../RanLogic/RANPARAM.h"

#include "../../OuterInterface.h"
#include "SelectServerPage.h"
#include "SelectServerChannel.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL CSelectServerPage::InitServerPageChannel ()
{
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

			struct S_SERVER_INFO
			{
				bool	bUSE;
				int		nIndex;
				int		nUserAmount;
                bool    bNewServer;

				S_SERVER_INFO () :
					bUSE ( false ),
					nIndex ( -1 ),
					nUserAmount ( 0 ),
                    bNewServer ( false )
				{
				}
			};

			S_SERVER_INFO sSERVERINFO[MAX_SERVER_GROUP];			

			//	카운트
			{
				for ( int i = 0; i < MAX_SERVER_GROUP; ++i )
				{
					sSERVERINFO[i].nIndex = i;
					for ( int j = 0, index = 0; j < MAX_SERVER_NUMBER; ++j )
					{
						LPG_SERVER_CUR_INFO_LOGIN pServerInfo = m_pServerInfo + ( i * MAX_SERVER_NUMBER ) + j;
						if ( !pServerInfo || !pServerInfo->nServerMaxClient ) continue;
						
						sSERVERINFO[i].bUSE = true;
						sSERVERINFO[i].nUserAmount += pServerInfo->nServerCurrentClient;
                        sSERVERINFO[i].bNewServer = pServerInfo->bNewServer;
					}
				}
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
                    for ( int i = 0; i < MAX_SERVER_GROUP; ++i )
				    {
					    for ( int j = 0; j < MAX_SERVER_GROUP - 1; ++j )
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
                    for ( int i = 0; i < MAX_SERVER_GROUP; ++i )
				    {
					    for ( int j = 0; j < MAX_SERVER_GROUP - 1; ++j )
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

			EMSERVICE_PROVIDER sp =	m_pGaeaClient->GetServiceProvider();

			//	리스트 구성
			{
				for ( int i = 0; i < MAX_SERVER_GROUP; ++i )
				{
					if ( !sSERVERINFO[i].bUSE ) continue;

					int nGROUPINDEX = sSERVERINFO[i].nIndex;

					{	//	서버 이름
						CString strServerName;

						//#ifdef CH_PARAM
						if (sp == SP_CHINA)
						{
							if( nGROUPINDEX <= 0 ) GASSERT( 0 && _T("nGROUPINDEX <= 0 is true.") );
							strServerName = RANPARAM::China_Region[RANPARAM::nChinaRegion].ServerName[nGROUPINDEX-1];
						}
						else
						{
						//#else
							strServerName = ID2GAMEEXTEXT("SERVER_NAME",nGROUPINDEX);
						}
						//#endif

						if( strServerName.IsEmpty() )
						{
							GASSERT( 0 && _T("A ServerName does not exist!") );
							break;
						}

						int nIndex = m_pServerList->AddTextNoSplit ( strServerName, NS_UITEXTCOLOR::DEFAULT );
						m_pServerList->SetTextData ( nIndex, nGROUPINDEX );
                        
                        //  신규 서버면
                        if ( sSERVERINFO[i].bNewServer )
                        {
                            m_pServerList->SetTextImage( nIndex, "NEW_SERVER_IMAGE" );
                        }
                        else
                        {
                            m_pServerList->SetTextImage( nIndex, "NEW_SERVER_IMAGE_NONE" );
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
					pGlobalStage->GetNetClient()->SndVersion();
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

BOOL CSelectServerPage::ConnectServerChannel ()
{
	if ( m_nIndex < 0 )
		return FALSE;

	int nChannelSelect = m_pSelectServerChannel->GetSelected ();
	if ( nChannelSelect < 0 )
		return FALSE;

	DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
    pGlobalStage->SetChannel( nChannelSelect );

	DWORD dwServerGroup = m_pServerList->GetTextData ( m_nIndex );
	DWORD dwServerNumber = m_pSelectServerChannel->GetSelectedChannel( nChannelSelect );

	if ( dwServerGroup == UINT_MAX || dwServerNumber == UINT_MAX )
		return FALSE;

	int nServerGroup = (int)dwServerGroup;
	int nServerNumber = (int)dwServerNumber;

	m_pInterface->SetConnectServerInfo ( nServerGroup, nServerNumber );

	m_pGaeaClient->SetConnServerGroup( nServerGroup );
	m_pGaeaClient->SetConnChannel( nServerNumber );

	//	접속할 서버 아이피,포트
	CNetClient* pNetClient = pGlobalStage->GetNetClient();
	if ( pNetClient->ConnectGameServer ( nServerGroup, nServerNumber ) <= NET_ERROR )
	{
		//접속 실패 에러
		m_pInterface->DoModal ( ID2GAMEEXTEXT ("SERVERSTAGE_5"), UI::MODAL_INFOMATION );					
		return FALSE;
	}
    
    m_pGaeaClient->SetServerGroup(nServerGroup);
	return TRUE;
}

 // 통합전장 서버 연결
BOOL CSelectServerPage::ConnectWorldBattleServerChannel ()
{
	int nServerGroup = 1;
	int nServerNumber = 0;
	int nServerChannel = 0;
	
	DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
    pGlobalStage->SetChannel( nServerChannel );

	m_pInterface->SetConnectServerInfo ( nServerGroup, nServerNumber );

	m_pGaeaClient->SetConnServerGroup( nServerGroup );
	m_pGaeaClient->SetConnChannel( nServerNumber );

	//	접속할 서버 아이피,포트
	CNetClient* pNetClient = pGlobalStage->GetNetClient();
	if ( pNetClient->ConnectGameServer ( nServerGroup, nServerNumber ) <= NET_ERROR )
	{
		//접속 실패 에러
		m_pInterface->DoModal ( ID2GAMEEXTEXT ("SERVERSTAGE_6"), UI::MODAL_INFOMATION );					
		return FALSE;
	}
    
    m_pGaeaClient->SetServerGroup(nServerGroup);
	return TRUE;
}

void CSelectServerPage::TranslateUIMessageChannel ( UIGUID ControlID, DWORD dwMsg )
{

	switch ( ControlID )
	{
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
					bool bDIFF = m_nIndex!=nIndex;
					m_nIndex = nIndex;

					if ( bDIFF ) LoadChannel ();					
				}

				if ( dwMsg & UIMSG_LB_DUP )
				{
//					Login ();
				}
			}
		}
		break;

	case SELECT_SERVER_CHANNEL:
		{
			if ( UIMSG_MOUSEIN_LOGIN & dwMsg )
			{
				int nIndex = m_pSelectServerChannel->GetSelected();
				if( m_nServerState[nIndex] == SERVER_NOVACANCY )
				{
					// 서버 접속 인원이 95%를 넘김, 접속 불가
					m_pInterface->DoModal( ID2GAMEEXTEXT ("LOGINSTAGE_FULL"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				}
				else 
				{
					Login ();
				}
			}
		}
		break;

	case SELECT_CHINA_AREA_BACK:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->ToChinaAreaPage( GetWndID() );
			}
		}
		break;
	}
}

void CSelectServerPage::LoadChannel ()
{
	m_pSelectServerChannel->ResetAll ();

	const DWORD dwGROUP_INDEX = m_pServerList->GetTextData ( m_nIndex );

	m_pSelectServerChannel->SetTitleName ( m_pServerList->GetText ( m_nIndex ) );

	for ( int j = 0; j < MAX_SERVER_NUMBER; ++j )
	{
		LPG_SERVER_CUR_INFO_LOGIN pServerInfo = m_pServerInfo + ( dwGROUP_INDEX * MAX_SERVER_NUMBER ) + j;
		if ( !pServerInfo || !pServerInfo->nServerMaxClient ) continue;

		CString strCombine;
		CString strPK( ID2GAMEEXTEXT("SERVER_PK") );
				
		if( pServerInfo->bPK )
		{
			if( RENDERPARAM::emSERVICE_TYPE == RENDERPARAM::EMSERVICE_JAPAN )
			{
				strCombine.Format ( "%s %d", ID2GAMEEXTEXT("SERVER_CHANNEL"), j );
			}
			else
			{
				strCombine.Format ( "%d %s%s", j, ID2GAMEEXTEXT("SERVER_CHANNEL"), strPK );
			}
		}
		else					
		{
			CString strSpace( ' ', strPK.GetLength() );

			if( RENDERPARAM::emSERVICE_TYPE == RENDERPARAM::EMSERVICE_JAPAN )
			{
				strCombine.Format ( "%s %d %s", ID2GAMEEXTEXT("SERVER_CHANNEL"), j, strSpace );
			}
			else
			{
				strCombine.Format ( "%d %s%s", j, ID2GAMEEXTEXT("SERVER_CHANNEL"), strSpace );
			}
		}
		
		CString strChannelState;
		DWORD dwStateColor;

		{	//	서버 상태
			float fPercent = float(pServerInfo->nServerCurrentClient) / float(pServerInfo->nServerMaxClient);

			if( fPercent < ( GLCONST_CHAR::wSERVER_NORMAL * 0.01f ) )			m_nServerState[j] = SERVER_SMOOTH;
			else if( fPercent < ( GLCONST_CHAR::wSERVER_CONGEST * 0.01f ) )		m_nServerState[j] = SERVER_NORMAL;
			else if( fPercent < ( GLCONST_CHAR::wSERVER_NOVACANCY * 0.01f ) )	m_nServerState[j] = SERVER_CONGEST;
			else																m_nServerState[j] = SERVER_NOVACANCY;
			
			D3DCOLOR dwStateColorArray[4] = 
			{
				NS_UITEXTCOLOR::PALEGREEN,
				NS_UITEXTCOLOR::GOLD,
				NS_UITEXTCOLOR::RED,
				NS_UITEXTCOLOR::RED
			};

			strChannelState = ID2GAMEWORD("SERVER_STATE", m_nServerState[j]);
			dwStateColor = dwStateColorArray[m_nServerState[j]];
		}

		// 서버 이름
		m_pSelectServerChannel->AddChannel( strCombine, NS_UITEXTCOLOR::DEFAULT, strChannelState, dwStateColor, j );
	}

	m_pSelectServerChannel->SetVisibleSingle ( TRUE );
}

void CSelectServerPage::SetChannelFull()
{
	int nSelectChannel = m_pSelectServerChannel->GetSelected();
	const DWORD dwGROUP_INDEX = m_pServerList->GetTextData ( m_nIndex );

	LPG_SERVER_CUR_INFO_LOGIN pServerInfo = m_pServerInfo + ( dwGROUP_INDEX * MAX_SERVER_NUMBER ) + nSelectChannel;
	if ( !pServerInfo || !pServerInfo->nServerMaxClient ) return;

	pServerInfo->nServerCurrentClient = pServerInfo->nServerMaxClient;

}
