#include "StdAfx.h"

#include "OuterInterface.h"

#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"

#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogic/RANPARAM.h"

#include "OldUI/Util/ModalWindow.h"

#include "OldUI/Interface/SelectServerPage.h"
#include "OldUI/Interface/LoginPage.h"
#include "OldUI/Interface/CreateCharacterPage.h"
#include "OldUI/Interface/NewCreateCharPage.h"
#include "OldUI/Interface/SelectCharacterPage.h"
#include "OldUI/Interface/SelectCharacterLeftPage.h"
#include "OldUI/Interface/CreateCharacterWindow.h"
#include "OldUI/Interface/CreateCharacterMenu.h"
#include "OldUI/Interface/WaitDialogue.h"
#include "OldUI/Interface/SecPassCheckPage.h"
#include "OldUI/Interface/SecPassSetPage.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

void COuterInterface::ModalMsgProcess ( UIGUID nCallerID, DWORD dwMsg )
{
	switch ( nCallerID )
	{
    case OUTER_MODAL_JOIN_CLUB_SUCCESS:
        {
            if ( dwMsg & UIMSG_MODAL_OK )
            {
                SendGameJoinMessage();
            }
        }
        break;

    case OUTER_MODAL_JOIN_CLUB_FAIL:
        {
            if ( dwMsg & UIMSG_MODAL_OK )
            {
                ShowSelectClubWindow();
            }
        }
        break;

    case OUTER_MODAL_MAKE_CLUB_SUCCESS:
        {
            if ( dwMsg & UIMSG_MODAL_OK )
            {
                SendGameJoinMessage();
            }
        }
        break;

    case OUTER_MODAL_MAKE_CLUB_FAIL:
        {
            if ( dwMsg & UIMSG_MODAL_OK )
            {
                ShowSelectClubWindow();
            }
        }
        break;

    case OUTER_MODAL_INPUT_MAKE_CLUB_NAME:
        {
            if ( dwMsg & UIMSG_MODAL_OK )
            {
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
                if ( !pGlobalStage )
                    return;

                SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetSelectCharInfo();
                if ( !pCharInfo )
                    return;

                DWORD dwDbCharNum = pCharInfo->ChaDbNum();
                std::string strCharName = sc::string::format( "%1%", pCharInfo->m_ChaName );
                std::string strEditString = sc::string::format( "%1%", GetModalWindow()->GetEditString().GetString() );

                m_pGaeaClient->MakeNewClubLobby( dwDbCharNum,
                    strEditString, strCharName );
            }
            else if ( dwMsg & UIMSG_MODAL_CANCEL )
                ShowSelectClubWindow();
        }
        break;

    case OUTER_MODAL_SUCCESS_CHANGE_COUNTRY:
        {
            if ( dwMsg & UIMSG_MODAL_OK )
            {
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
                pGlobalStage->SetUserCountry( m_pGaeaClient->GetUserCountry() );
            }
        }
        break;

    case OUTER_MODAL_FAIL_CHANGE_COUNTRY:
        {
            if ( dwMsg & UIMSG_MODAL_OK )
            {
                ToSelectServerPage( GetModalCallWindowID() );
            }
        }
        break;

	case OUTER_MODAL_CLOSEGAME:
		{
            DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
			if ( dwMsg & UIMSG_MODAL_OK )
			{
                pGlobalStage->CloseGame();
			}
			else if ( dwMsg & UIMSG_MODAL_CANCEL )
			{
                if (pGlobalStage->GetNetClient()->ConnectLoginServer(RANPARAM::LoginAddress, RANPARAM::LoginServerPort) > NET_ERROR)
				{
 					// ���� ����
					pGlobalStage->GetNetClient()->SndReqServerInfo();
				}
				else
				{
					//���� ����
					DoModal(ID2GAMEEXTEXT("SERVERSTAGE_3"), UI::MODAL_INFOMATION, UI::OKCANCEL, OUTER_MODAL_CLOSEGAME);
				}
			}
		}
		break;

	case OUTER_MODAL_WAITSERVERINFO:
		{
            DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
			CNetClient* pNetClient = pGlobalStage->GetNetClient();
			if ( pNetClient->IsOnline() )		//��Ʈ��ũ ������ ������ ���
			{
				if ( pNetClient->IsGameSvrInfoEnd() )
				{
					UiHideGroup ( MODAL_WINDOW_OUTER );
					return ;
				}
			}

			if ( dwMsg & UIMSG_MODAL_CANCEL )
			{
				DoModal ( ID2GAMEEXTEXT ( "SERVERSTAGE_3" ), UI::MODAL_INFOMATION, UI::OKCANCEL, OUTER_MODAL_CLOSEGAME );				
			}
		}
		break;

	case OUTER_MODAL_RECONNECT:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				ToSelectServerPage( GetModalCallWindowID() );
			}
		}
		break;

	case OUTER_MODAL_WAITCONFIRM:
		{
			if ( dwMsg & UIMSG_MODAL_CANCEL )
			{
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
				pGlobalStage->GetNetClient()->CloseConnect();
				ToSelectServerPage( GetModalCallWindowID() );
			}
		}	
		break;

	case OUTER_MODAL_ALREADYCONNECT:
	case OUTER_MODAL_CREATEFAILED:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
				pGlobalStage->CloseGame();
			}
		}
		break;

	case OUTER_MODAL_NON_PK_CHANNEL:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
				{
					if ( UiIsVisibleGroup ( SELECT_COUNTRY_UI ) )				UiHideGroup ( SELECT_COUNTRY_UI );
					if ( UiIsVisibleGroup ( SELECT_CONTINENT_LIST_UI ) )		UiHideGroup ( SELECT_CONTINENT_LIST_UI );
					if ( UiIsVisibleGroup ( SELECT_COUNTRY_LIST_UI ) )			UiHideGroup ( SELECT_COUNTRY_LIST_UI );
				}

				if ( UiIsVisibleGroup ( SELECT_CHARACTER_PAGE ) )			UiHideGroup ( SELECT_CHARACTER_PAGE );
				if ( UiIsVisibleGroup ( SELECT_CHARACTER_LEFTPAGE ) )		UiHideGroup ( SELECT_CHARACTER_LEFTPAGE );
				if ( UiIsVisibleGroup ( CREATE_CHARACTER_PAGE ) )			UiHideGroup ( CREATE_CHARACTER_PAGE );
				if ( UiIsVisibleGroup ( CREATE_CHARACTER_BUTTON_PAGE ) )	UiHideGroup ( CREATE_CHARACTER_BUTTON_PAGE );
				if ( UiIsVisibleGroup ( CREATE_CHARACTER_WEB ) )			UiHideGroup ( CREATE_CHARACTER_WEB );

				//	Note : ��ſ� �������
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
				CNetClient* pNetClient = pGlobalStage->GetNetClient();
				if ( pNetClient->IsOnline() == true )
					pNetClient->CloseConnect ();

				ToSelectServerPage ( SELECT_SERVER_PAGE );
			}
		}
		break;

	case OUTER_MODAL_SECONDPASSWORD:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				WAITSERVER_DIALOGUE_OPEN ( ID2GAMEEXTEXT("WAITSERVER_DEL_CHARACTER"), WAITSERVER_DEL_CHARACTER, 10.0f );
			}
			else if( dwMsg & UIMSG_MODAL_CANCEL )
			{
				//// 2�����
				// bGameJoin �÷��׸� ����. �ش� �÷��װ� ĳ���� ���� Ŭ�� ���� �˻� ������ �ϳ�
				m_pGaeaClient->GetGlobalStage()->GetLobyStage()->m_bGameJoin = FALSE;
			}
		}
		break;

	// 2�����
	// 2����� �Է� ���â�� �ߴ� ��츸 �������� ���⶧���� ������ �˻簡 �ʿ� ����
	case OUTER_MODAL_JOIN_SECOUNDPASSWORD:
	case OUTER_MODAL_JOIN_SECOUNDPASSWORD_GS:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				if( GetModalWindow()->Is2ndPWDModal() == TRUE )
				{
					int nRetPasswordCheck = GetModalWindow()->CheckNew2ndPassString();

					if( nRetPasswordCheck == 1 )
					{
						if ( !m_pSelectCharacterLeftPage->SndGameJoin( GetModalWindow()->GetNew2ndPassString().GetString()) )
						{
							//// 2�� ��� 				
							DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_8"), UI::MODAL_INFOMATION, UI::OK, FALSE, TRUE );
						}
					}
					else if( nRetPasswordCheck == 0 ) // 2����� ����ġ
					{
						//// 2�����
						// bGameJoin �÷��׸� ����. �ش� �÷��װ� ĳ���� ���� Ŭ�� ���� �˻� ������ �ϳ�
						m_pGaeaClient->GetGlobalStage()->GetLobyStage()->m_bGameJoin = FALSE;

						DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_MISMATCH_2ND_PWD"), UI::MODAL_INFOMATION, UI::OK, FALSE, TRUE );
					}
					else if( nRetPasswordCheck == -1 ) // ����� 4���� ���ڸ� ���� // ����� ���� ���� ������ �Ұ��� ( ex: 1111 )
					{
						//// 2�����
						// bGameJoin �÷��׸� ����. �ش� �÷��װ� ĳ���� ���� Ŭ�� ���� �˻� ������ �ϳ�
						m_pGaeaClient->GetGlobalStage()->GetLobyStage()->m_bGameJoin = FALSE;

						DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_ERROR_LEN_2ND_PWD"), UI::MODAL_INFOMATION, UI::OK, FALSE, TRUE );
					}
					else if( nRetPasswordCheck == -2 ) // ����� 4���� ���ڸ� ���� // ����� ���� ���� ������ �Ұ��� ( ex: 1111 )
					{
						//// 2�����
						// bGameJoin �÷��׸� ����. �ش� �÷��װ� ĳ���� ���� Ŭ�� ���� �˻� ������ �ϳ�
						m_pGaeaClient->GetGlobalStage()->GetLobyStage()->m_bGameJoin = FALSE;

						DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_ERROR_SAME_2ND_PWD"), UI::MODAL_INFOMATION, UI::OK, FALSE, TRUE );
					}
					else // �˼� ���� ����
					{
						//// 2�����
						m_pGaeaClient->GetGlobalStage()->GetLobyStage()->m_bGameJoin = FALSE;

						DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_UNKNOWN_ERROR_2ND_PWD"), UI::MODAL_INFOMATION, UI::OK, FALSE, TRUE );
					}
				}
				else
				{
					if ( !m_pSelectCharacterLeftPage->SndGameJoin(GetModalWindow()->GetEditString().GetString()) )
					{
						//// 2�� ��� 				
						DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_8"), UI::MODAL_INFOMATION, UI::OK, FALSE, TRUE );
					}
				}
			}
			else if( dwMsg & UIMSG_MODAL_CANCEL )
			{
				//// 2�����
				// bGameJoin �÷��׸� ����. �ش� �÷��װ� ĳ���� ���� Ŭ�� ���� �˻� ������ �ϳ�
				m_pGaeaClient->GetGlobalStage()->GetLobyStage()->m_bGameJoin = FALSE;
			}
		}
		break;
	////////////////////////////////

	case OUTER_MODAL_SHOW_COUNTRY_SELECT_LIST_UI:
		if ( dwMsg & UIMSG_MODAL_OK )
		{
			UiShowGroupFocus( SELECT_CONTINENT_LIST_UI );
			UiShowGroupFocus( SELECT_COUNTRY_LIST_UI );
		}
		break;

	case OUTER_MODAL_SELECTED_COUNTRY:
		if ( dwMsg & UIMSG_MODAL_OK )
		{
			UiHideGroup( SELECT_CONTINENT_LIST_UI );
			UiHideGroup( SELECT_COUNTRY_LIST_UI );

			if ( m_pSelectContinentListUI && m_pSelectCountryListUI )
			{
				Country::SCOUNTRY_INFO sCountryInfo;
				sCountryInfo.wContinentID = m_pSelectContinentListUI->GetSelectedContinentID();
				sCountryInfo.wCountryID = m_pSelectCountryListUI->GetSelectedCountryID();

				m_pGaeaClient->ChangeCountry( sCountryInfo );
			}
			else
			{
				// UI�� ����;
			}
		}
		break;

	case OUTER_MODAL_LOGOUT:
	case OUTER_MODAL_CONNECTCLOSED:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				//	Note : ��ſ� �������
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
				pGlobalStage->GameToLobbyStage();
				SetCancelToLogin();

				//				if ( pNetClient->ConnectLoginServer(RANPARAM::LoginAddress) <= NET_ERROR )
				//					CDebugSet::ToListView ( "CCharacterStage::Logout ConnectLoginServer NET_ERROR" );
				//				else
				//					CDebugSet::ToListView ( "CCharacterStage::Logout ConnectLoginServer NET_OK" );				

				if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
				{
					if ( UiIsVisibleGroup ( SELECT_COUNTRY_UI ) )				UiHideGroup ( SELECT_COUNTRY_UI );
					if ( UiIsVisibleGroup ( SELECT_CONTINENT_LIST_UI ) )		UiHideGroup ( SELECT_CONTINENT_LIST_UI );
					if ( UiIsVisibleGroup ( SELECT_COUNTRY_LIST_UI ) )			UiHideGroup ( SELECT_COUNTRY_LIST_UI );
				}

				if ( UiIsVisibleGroup( SELECT_CHARACTER_PAGE ) )			UiHideGroup( SELECT_CHARACTER_PAGE );
				if ( UiIsVisibleGroup( SELECT_CHARACTER_LEFTPAGE ) )		UiHideGroup( SELECT_CHARACTER_LEFTPAGE );
				if ( UiIsVisibleGroup( CREATE_CHARACTER_PAGE ) )			UiHideGroup( CREATE_CHARACTER_PAGE );
				if ( UiIsVisibleGroup( CREATE_CHARACTER_BUTTON_PAGE ) )	    UiHideGroup( CREATE_CHARACTER_BUTTON_PAGE );
				if ( UiIsVisibleGroup( CREATE_CHARACTER_WEB ) )			    UiHideGroup( CREATE_CHARACTER_WEB );
                if ( UiIsVisibleGroup( SELECT_COUNTRY_CONFIRM_MODAL ) )     UiHideGroup( SELECT_COUNTRY_CONFIRM_MODAL );
                if ( UiIsVisibleGroup( SELECT_CLUB_WINDOW ) )               UiHideGroup( SELECT_CLUB_WINDOW );

				if( UiIsVisibleGroup( SELECT_CHINA_AREA_PAGE ) )			
				{
					ToChinaAreaPage( SELECT_CHINA_AREA_PAGE );
				}
				else
				{
					ToSelectServerPage ( SELECT_CHARACTER_PAGE );
				}
			}
		}
		break;

	case OUTER_MODAL_CREATESUCCEED:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				ToSelectCharacterPage ( CREATE_CHARACTER_PAGE );
			}
		}
		break;

	case OUTER_MODAL_GENERAL_LOGINFAIL:
		{
			if( dwMsg & UIMSG_MODAL_OK )
			{
				if( RANPARAM::VALIDIDPARAM () )
				{
					UIGUID uiGUID = GetModalCallWindowID();

					if( uiGUID == SECPASS_SETPAGE )
						GetSecPassSetPage()->ResetAll();
					else if( uiGUID == SECPASS_CHECKPAGE )
						GetSecPassCheckPage()->ResetAll();
					else
						ToSelectServerPage ( SELECT_SERVER_PAGE );
				}
				else
				{
					ToLoginPage ( LOGIN_PAGE );
					GetLoginPage()->ResetAll();
				}
			}
		}
		break;

	case OUTER_MODAL_LOGIN_WAIT_DAUM:
		{
			// ���� ������ ��ȣ Ű�� ���� ������ ����Ѵ�.
            DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
			CNetClient* pNetClient = pGlobalStage->GetNetClient();
			if ( pNetClient->IsKeyReceived () )
			{
				CString strTemp;

				//	��Ʈ�ѷ� ���� ���̵�� ��ȣ�� ������ ������ ����
				switch ( RENDERPARAM::emSERVICE_TYPE )
				{
				case RENDERPARAM::EMSERVICE_KOREA:
					pNetClient->DaumSndLogin ( RANPARAM::GETIDPARAM(), m_nServerChannel );
					break;

				//case EMSERVICE_GSP:
				//	pNetClient->GspSndLogin ( RANPARAM::GETIDPARAM(), m_nServerChannel );
				//	break;

				case RENDERPARAM::EMSERVICE_MALAYSIA_CN:
				case RENDERPARAM::EMSERVICE_MALAYSIA_EN:
				case RENDERPARAM::EMSERVICE_PHILIPPINES:
				case RENDERPARAM::EMSERVICE_VIETNAM:
					pNetClient->TerraSndLogin ( RANPARAM::GETIDPARAM(), m_nServerChannel );
					break;

//		Japan �α��� ��� ����
//				case EMSERVICE_JAPAN:
//					pNetClient->ExciteSndLogin ( RANPARAM::GETIDPARAM(), RANPARAM::GETIDPARAM2(), RANPARAM::GETIDPARAM3(), m_nServerChannel );
//					break;

				default:
					CString strTemp;
					strTemp.Format( "OUTER_MODAL_LOGIN_WAIT_DAUM::default:ServiceType:%d", RENDERPARAM::emSERVICE_TYPE );
					AfxMessageBox( strTemp.GetString() );
					break;
				};

                START_WAIT_TIME ( 10.0f );
				DoModal ( ID2GAMEEXTEXT ("DAUM_LOGIN_WAIT"), UI::MODAL_INFOMATION, UI::CANCEL, OUTER_MODAL_LOGIN_WAIT_DAUM_AFTER_KEYRECEIVED );
			}
			else			
			{
				//	NOTE
				//		�ð��� �ʰ��� ���
				//		�Ʒ��� ó������ �ٷ� ������ �� á�ٴ� �޽����� �Ѹ���.
				if ( GET_WAIT_TIME() < 0.0f )
				{
					DoModal ( ID2GAMEEXTEXT ("DAUM_LOGIN_WAIT_SERVERFULL"), UI::MODAL_INFOMATION, UI::YESNO, OUTER_MODAL_LOGIN_WAIT_DAUM_SERVERFULL );
				}
			}
		}
		break;

	case OUTER_MODAL_LOGIN_WAIT_DAUM_AFTER_KEYRECEIVED:
		{
			//	NOTE
			//		�ð��� �ʰ��� ���
			//		�Ʒ��� ó������ �ٷ� ������ �� á�ٴ� �޽����� �Ѹ���.
			if ( GET_WAIT_TIME() < 0.0f )
			{
				DoModal ( ID2GAMEEXTEXT ("DAUM_LOGIN_WAIT_SERVERFULL"), UI::MODAL_INFOMATION, UI::YESNO, OUTER_MODAL_LOGIN_WAIT_DAUM_SERVERFULL );
			}
		}
		break;

	case OUTER_MODAL_LOGIN_WAIT_DAUM_SERVERFULL:
		{
			//	NOTE
			//		�ٽ� �õ��غ���
			if ( dwMsg & UIMSG_MODAL_OK )
			{
				GetSelectServerPage()->Login ();
			}
		}
		break;

	case OUTER_MODAL_CHARACTERSTAGE_GAME_JOIN_WAIT:
		{
			if( dwMsg & UIMSG_MODAL_CANCEL )
			{
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
				pGlobalStage->SetGameJoin(FALSE);
				//	Note : ��ſ� �������
				CNetClient* pNetClient = pGlobalStage->GetNetClient();
				if ( pNetClient->IsOnline() == true )
				{
					pNetClient->CloseConnect ();
					SetCancelToLogin();
				}

				//				if ( pNetClient->ConnectLoginServer(RANPARAM::LoginAddress) <= NET_ERROR )
				//					CDebugSet::ToListView ( "CCharacterStage::Logout ConnectLoginServer NET_ERROR" );
				//				else
				//					CDebugSet::ToListView ( "CCharacterStage::Logout ConnectLoginServer NET_OK" );				

				if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
				{
					if ( UiIsVisibleGroup ( SELECT_COUNTRY_UI ) )				UiHideGroup ( SELECT_COUNTRY_UI );
					if ( UiIsVisibleGroup ( SELECT_CONTINENT_LIST_UI ) )		UiHideGroup ( SELECT_CONTINENT_LIST_UI );
					if ( UiIsVisibleGroup ( SELECT_COUNTRY_LIST_UI ) )			UiHideGroup ( SELECT_COUNTRY_LIST_UI );
				}

				if ( UiIsVisibleGroup ( SELECT_CHARACTER_PAGE ) )			UiHideGroup ( SELECT_CHARACTER_PAGE );
				if ( UiIsVisibleGroup ( SELECT_CHARACTER_LEFTPAGE ) )		UiHideGroup ( SELECT_CHARACTER_LEFTPAGE );
				if ( UiIsVisibleGroup ( CREATE_CHARACTER_PAGE ) )			UiHideGroup ( CREATE_CHARACTER_PAGE );
				if ( UiIsVisibleGroup ( CREATE_CHARACTER_BUTTON_PAGE ) )	UiHideGroup ( CREATE_CHARACTER_BUTTON_PAGE );
				if ( UiIsVisibleGroup ( CREATE_CHARACTER_WEB ) )			UiHideGroup ( CREATE_CHARACTER_WEB );
				if( UiIsVisibleGroup( SELECT_CHINA_AREA_PAGE ) )			
				{
					ToChinaAreaPage( SELECT_CHINA_AREA_PAGE );
				}
				else
				{
					ToSelectServerPage ( SELECT_CHARACTER_PAGE );
				}
			}
			else if ( GET_WAIT_TIME() < 0.0f )
			{
				//	NOTE
				//		�ð��� �ʰ��� ���
				//		�Ʒ��� ó������ �ٷ� ������ �� á�ٴ� �޽����� �Ѹ���.
				DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_DAUM_GAME_JOIN_FAIL"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CONNECTCLOSED );
			}
		}
		break;

	case OUTER_MODAL_NAME_ERROR:
	case OUTER_MODAL_THAI_CHECK_STRING_ERROR:
		{
			if ( dwMsg & UIMSG_MODAL_OK )
			{
//				GetNewCreateCharacterPage()->ResetAll ();

				GetCreateCharacterWindow()->GetMenu()->ResetEdit();
			}
		}
		break;

	case OUTER_MODAL_CHARACTER_SELECT:
		{
			if( dwMsg & UIMSG_MODAL_OK )
			{
				ToSelectCharacterPage ( CREATE_CHARACTER_PAGE );
			}
		}
	}
}
