#include "StdAfx.h"
#include "../SigmaCore/Database/DbDefine.h"

#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/BasicButton.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/Msg/LoginMsg.h"

#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/GLGaeaClient.h"

#include "OldUI/Util/ModalWindow.h"
#include "OldUI/Interface/SelectCharacterPage.h"
#include "OldUI/Interface/SelectCharacterLeftPage.h"
#include "OldUI/Interface/CreateCharacterPage.h"
#include "OldUI/Interface/SelectServerPage.h"
#include "OldUI/Interface/NewCreateCharPage.h"
#include "OldUI/Interface/CreateCharacterWindow.h"
#include "OuterInterface.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void COuterInterface::MsgProcess ( LPVOID pBuffer )
{
	if( !pBuffer ) return ;

	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) pBuffer;

	EMSERVICE_PROVIDER sp = m_pGaeaClient->GetServiceProvider();

	switch ( nmg->nType )
	{
	case NET_MSG_LOGIN_FB:
		{
            DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
			if ( NULL == pGlobalStage )
				return;

			NET_LOGIN_FEEDBACK_DATA* pPacket = (NET_LOGIN_FEEDBACK_DATA *) nmg;

			pGlobalStage->SetWorldBattleID( pPacket->m_szDaumGID );

			int nServerPatchProgramVer = pPacket->m_LauncherVersion;
			int nServerGameProgramVer = pPacket->m_GameVersion;

			SetCreateExtreme( pPacket->m_Extreme );
			SetCharRemain(pPacket->m_ChaRemain);
            SetCountry(pPacket->m_Country);
            if ( pPacket->m_Result == EM_LOGIN_FB_SUB_OK )
            {
                if ( GetCountry() == wb::UNKNOWN_COUNTRY )
                    ToSelectCountryPage();
            }

            int nClientPatchProgramVer;
			int nClientGameProgramVer;

			BOOL bResult = FALSE;
			bResult = LoadClientVersion(nClientPatchProgramVer, nClientGameProgramVer);
			if (!bResult)
			{
				UiHideGroup ( MODAL_WINDOW_OUTER );
				DoModal ( ID2GAMEEXTEXT ("LOGIN_FAILED_VER_FILE"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				return;
			}

            BOOL bDifferentVer = FALSE;
            if ( (nClientPatchProgramVer < nServerPatchProgramVer) ||
				(nClientGameProgramVer < nServerGameProgramVer) )
			{
				bDifferentVer = TRUE;
			}
	
			switch ( pPacket->m_Result )
			{
			case EM_LOGIN_FB_SUB_OK:	// �α��� ����				
			case EM_LOGIN_FB_KR_OK_USE_PASS:	// �α��� ����
			case EM_LOGIN_FB_KR_OK_NEW_PASS:	// �α��� ����

				// 0 : 2����� ������. 1 : 2����� ��� , 2 : 2����� �űԼ���
				if(pPacket->m_Result == EM_LOGIN_FB_KR_OK_USE_PASS)
					m_pGaeaClient->SetUse2ndPass( 1 );
				else if(pPacket->m_Result == EM_LOGIN_FB_KR_OK_NEW_PASS)
					m_pGaeaClient->SetUse2ndPass( 2 );
				else
					m_pGaeaClient->SetUse2ndPass( 0 );

				UiHideGroup ( MODAL_WINDOW_OUTER );

				//	������ Ʋ�� ���
	
				if ( bDifferentVer )
				{
					DoModal ( ID2GAMEEXTEXT ("LOGIN_FAILED_INVALID_VER"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
					return ;
				}
/*
//#if defined(KRT_PARAM) || defined(KR_PARAM)
#if defined ( JP_PARAM )	// 2�� �н�����
				if ( RANPARAM::VALIDIDPARAM () )
				{
					SetDaumGID( CString(nlfd->szDaumGID) );
					ToSecPassPage ( SELECT_SERVER_PAGE, nlfd->nCheckFlag );
				}
				else
				{
					ToSecPassPage ( LOGIN_PAGE, nlfd->nCheckFlag );
				}
#else
*/

                if ( m_pGaeaClient->GetServiceProvider() == SP_WORLD_BATTLE )
                {
                    return;
                }

				//	�α��� ���� & ĳ���� ���� ȭ������ �̵�
				pGlobalStage->OnInitLogin();
                
                if (m_pGaeaClient)
				{
                    m_pGaeaClient->SetUserCountry(pPacket->m_Country);
					m_pGaeaClient->SetCountryInfo(pPacket->m_sCountryInfo);
				}

				pGlobalStage->GetNetClient()->SndChaAllBasicInfo();
				// �α��� ���μ��� Ȯ���� ���� �α�
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Request Basic Char Info ( Recv Ret )"));

                if ( RANPARAM::VALIDIDPARAM() )
					ToSelectCharacterPage( SELECT_SERVER_PAGE );
				else
					ToSelectCharacterPage( LOGIN_PAGE );

				if (sp == SP_CHINA)
				{
//#if defined ( CH_PARAM ) // �߱� MMOSPEED
					//RunMMOSpeed();
				}
//#endif

				break;

			case EM_LOGIN_FB_SUB_FAIL:		// �α��� ����				
				DoModal ( ID2GAMEEXTEXT ("LOGIN_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );				
				break;

			case EM_LOGIN_FB_SUB_SYSTEM:	// �ý��ۿ����� �α��� ����
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_8"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );				
				break;

			case EM_LOGIN_FB_SUB_USAGE:		// �㰡�� ���ð� ����
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_9"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );				
				break;

			case EM_LOGIN_FB_SUB_DUP:		// �ߺ��� ����
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_10"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );				
				break;

			case EM_LOGIN_FB_SUB_INCORRECT:	// ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_11"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );				
				break;

			case EM_LOGIN_FB_SUB_IP_BAN:	// ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_12"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				break;

			case EM_LOGIN_FB_CH_FULL: // �ִ� ���� �ο� ���޽� NET_MSG_SVR_FULL �޽����� �´�.
				m_pSelectServerPage->SetChannelFull();
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_FULL"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				ToSelectServerPage ( LOGIN_PAGE );
				break;

			// 2004-02-26 jgkim �߰�
			case EM_LOGIN_FB_SUB_BLOCK:		// ID/PWD ��ġ������ ID �� Block �� ����
                DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_BLOCK"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				break;

			case EM_LOGIN_FB_SUB_RANDOM_PASS:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_RANDOM_PASS_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				break;

			case EM_LOGIN_FB_SUB_ALREADYOFFLINE: // // �α��� �����̰� �̹� ���������϶�
				DoModal ( ID2GAMEEXTEXT ("LOGIN_ALREADYOFFLINE"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Daum : �������� �α��ε���Ÿ (���Ӱ� ������ �α����� ���ӽ���)
			case EM_LOGIN_FB_SUB_UNCON:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_INVALID_LOGIN"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Daum : �����ð��� �ʰ��� ������̱� ������, ���� �������� �϶�� �޽����� ������
			case EM_LOGIN_FB_SUB_EXPIRED:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_EXPIRED_USER"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Daum : GID�� ���� ������ ���Ӱ� �α���
			case EM_LOGIN_FB_SUB_GID_ERR:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_NOGID"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Daum : UID�� ���� ������ ���Ӱ� �α���
			case EM_LOGIN_FB_SUB_UID_ERR:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_NOUID"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Daum : ���� ���Խ���
			case EM_LOGIN_FB_SUB_UNKNOWN:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_UNKNOWN_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;		

			// Daum : �ֹι�ȣ����
			case EM_LOGIN_FB_SUB_SSNHEAD:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_SSN_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;		

			// Daum : ������������
			case EM_LOGIN_FB_SUB_ADULT:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_ADULT_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Thai : 18 ���̸� 3 �ð� ���
			case EM_LOGIN_FB_THAI_UNDER18_3HOUR:
				DoModal( ID2GAMEEXTEXT("LOGINSTAGE_18LESS_TIME_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Thai : 18 ���̸� ���ӺҰ��� �ð�
			case EM_LOGIN_FB_THAI_UNDER18_TIME:
				DoModal( ID2GAMEEXTEXT("LOGINSTAGE_18LESS_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Thai : 18 ���̻� ���ӺҰ��� �ð�
			case EM_LOGIN_FB_THAI_OVER18_TIME:
				DoModal( ID2GAMEEXTEXT("LOGINSTAGE_18MORE_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// ������ ip���� %%%
			// My : �����̽þ� ���� �Ұ����� ip
			case EM_LOGIN_FB_SUB_IMPOSSIBLEIP:
				DoModal( ID2GAMEEXTEXT("EM_LOGIN_FB_SUB_IMPOSSIBLEIP"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			case EM_LOGIN_FB_SUB_BETAKEY:
				DoModal( ID2GAMEEXTEXT("EM_LOGIN_FB_SUB_BETAKEY"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			//case WAIT_MSG_FROM_SERVER:
			//	DoModalOuter ( ID2GAMEEXTEXT ("LOGINSTAGE_6"), UI::MODAL_INFOMATION, UI::OK );				
			//	break;

            case EM_LOGIN_FB_KOR_SHUTDOWN:
                DoModal(ID2GAMEEXTEXT("EM_LOGIN_FB_KOR_SHUTDOWN"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME);
                break;

			default:
				break;
			};
		}
		break;

	case NET_MSG_PASSCHECK_FB:
		{
			NET_PASSCHECK_FEEDBACK_DATA* nlfd = (NET_PASSCHECK_FEEDBACK_DATA *) nmg;
			UiHideGroup(MODAL_WINDOW_OUTER);
            DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();

			switch ( nlfd->nResult )
			{
			case EM_LOGIN_FB_SUB_OK : // �α��� ����
				//	�α��� ���� & ĳ���� ���� ȭ������ �̵�                
				pGlobalStage->OnInitLogin();
                if (m_pGaeaClient)
                    m_pGaeaClient->SetUserCountry(m_Country);
				pGlobalStage->GetNetClient()->SndChaAllBasicInfo();
				// �α��� ���μ��� Ȯ���� ���� �α�
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Request Basic Char Info ( Success PassCheck )"));
				ToSelectCharacterPage ( SECPASS_CHECKPAGE );
				break;

			case EM_LOGIN_FB_SUB_FAIL : // ID / PWD ����ġ (�α��� ����)
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_11"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				SetModalCallWindowID( SECPASS_CHECKPAGE );
				break;

			case EM_LOGIN_FB_SUB_PASS_OK : // �ʱ� �н����� �Է� ����
				//	�α��� ���� & ĳ���� ���� ȭ������ �̵�
				pGlobalStage->OnInitLogin();
                if (m_pGaeaClient)
                    m_pGaeaClient->SetUserCountry(m_Country);
				pGlobalStage->GetNetClient()->SndChaAllBasicInfo();
				// �α��� ���μ��� Ȯ���� ���� �α�
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Request Basic Char Info ( Init PassCheck )"));

				ToSelectCharacterPage ( SECPASS_SETPAGE );
				break;
			case EM_LOGIN_FB_SUB_SECID_ALREADY:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_13"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				SetModalCallWindowID( SECPASS_SETPAGE );
				break;

            case sc::db::DB_ERROR : // �ʱ� �н����� ������Ʈ ����
			case USHRT_MAX :
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_11"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				SetModalCallWindowID( SECPASS_SETPAGE );
				break;	
			}
		}
		break;
	case JAPAN_NET_MSG_UUID:
		{
			JAPAN_NET_LOGIN_UUID* netMsg = (JAPAN_NET_LOGIN_UUID*) nmg;
			RANPARAM::SETJAPANUUID( netMsg->szUUID );
		}
		break;
	case NET_MSG_LOBBY_REQ_GAME_JOIN:
		{
			//// 2�����
			EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();

			if( sp == SP_HONGKONG )
			{
				DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_16"), UI::MODAL_INPUT, UI::EDITBOX, OUTER_MODAL_JOIN_SECOUNDPASSWORD, TRUE, TRUE );
			}
			else
			{
				// 2�����
				if( m_pGaeaClient->GetUse2ndPass() == 0 )
				{
					GetSelectCharacterPage()->SndGameJoin ();
				}
				else if( m_pGaeaClient->GetUse2ndPass() == 1 )
				{
					DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_16"), UI::MODAL_INPUT, UI::EDITBOX, OUTER_MODAL_JOIN_SECOUNDPASSWORD_GS, TRUE, TRUE );
				}
				else if( m_pGaeaClient->GetUse2ndPass() == 2 )
				{
					DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_20"), UI::MODAL_INPUT, UI::EDITBOX_CHECK_PWD, OUTER_MODAL_JOIN_SECOUNDPASSWORD_GS, TRUE, TRUE );
				}
				else
				{
					sc::writeLogError(std::string("NET_MSG_LOBBY_REQ_GAME_JOIN - ERROR"));
				}
			}
		}
		break;

	case NET_MSG_LOBBY_GAME_COMPLETE:
		{
			CDebugSet::ToListView ( "NET_MSG_LOBBY_GAME_COMPLETE" );

            DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
			if ( pGlobalStage )
			{
				pGlobalStage->ToChangeStage(DxGlobalStage::EM_STAGE_GAME);
			}
			else
			{
				sc::writeLogError( "Failed Join Game...!!!" );
			}
		}
		break;

	case NET_MSG_LOBBY_CHAR_JOIN_FB:
		{
			GLMSG::SNETLOBBY_CHARJOIN_FB *pNetMsg = (GLMSG::SNETLOBBY_CHARJOIN_FB *) nmg;
			DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
			
			switch (pNetMsg->emCharJoinFB )
			{
			case EMCJOIN_FB_NOWLOGIN: //������
				// �α��� ���μ��� Ȯ���� ���� �α�
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 0"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_10" ), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_ALREADYCONNECT );
				break;
			case EMCJOIN_FB_ERROR: //����
				// �α��� ���μ��� Ȯ���� ���� �α�
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 1"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_11" ), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CREATEFAILED );
				break;
			case EMCJOIN_FB_PKPOINT: //pk �������� ����
				// �α��� ���μ��� Ȯ���� ���� �α�
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 2"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_12" ), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_NON_PK_CHANNEL );
				break;
			case EMCJOIN_FB_SAVE_CHA: //���� �������� ĳ���Ͱ� �־ ���� �� �� ����.
				// �α��� ���μ��� Ȯ���� ���� �α�
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 3"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_14" ), UI::MODAL_INFOMATION, UI::OK );
				break;
			// 2�����
			case EMCJOIN_FB_WAIT: // ����� �ٽ� �õ����ּ���
				// �α��� ���μ��� Ȯ���� ���� �α�
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 4"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_15" ), UI::MODAL_INFOMATION, UI::OK );
				break;
			case EMCJOIN_FB_SAVE_CHA2: //���� �������� ĳ���Ͱ� �־ ���� �� �� ����.
				// �α��� ���μ��� Ȯ���� ���� �α�
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 5"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_14" ), UI::MODAL_INFOMATION, UI::OK );
				break;
			case EMCJOIN_FB_OVERSTAT_RANDOMOPTION: //�����ɼ��� ������ �ִ� ĳ�����̴�.
				// �α��� ���μ��� Ȯ���� ���� �α�
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 6"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_RANDOMOPTION_ERROR" ), UI::MODAL_INFOMATION, UI::OK );
				break;
			//////////////////////////////////////////
			};
		}
		break;

		// 2�����
	case NET_MSG_LOBBY_CHARJOIN_2NDPASS:
		{
			GLMSG::SNETLOBBY_CHARJOIN_2NDPASS *pNetMsg = (GLMSG::SNETLOBBY_CHARJOIN_2NDPASS *) nmg;

			switch (pNetMsg->nResult )
			{				
			case EMPASS_FAIL:
				DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_17"), UI::MODAL_INFOMATION, UI::OK );
				break;
			case EMPASS_BLOCK:
				DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_18"), UI::MODAL_INFOMATION, UI::OK );
				break;
			default:
				DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_19"), UI::MODAL_INFOMATION, UI::OK );
				break;
			}
		}
		break;
		/////////////////////////////////////////////////////

	case NET_MSG_CHA_DEL_FB_OK:
		{
			UiHideGroup( MODAL_WINDOW_OUTER, true );

			NET_CHA_DEL_FB* pCharDelFb = (NET_CHA_DEL_FB*) nmg;
			GASSERT( m_pDxLobyStage );
			m_pDxLobyStage->DelChar( pCharDelFb->nChaNum );

			//	ĳ���� ī��� ������
			SetCreateExtreme( pCharDelFb->bExtreme );
			SetCharRemain( static_cast<USHORT>(pCharDelFb->wChaRemain) );

			DoModal ( ID2GAMEEXTEXT ( "CHAR_DEL_SUCCEED" ), UI::MODAL_INFOMATION, UI::OK );
		}
		break;

	case NET_MSG_CHA_DEL_FB_CLUB:
		{
			UiHideGroup( MODAL_WINDOW_OUTER, true );
			DoModal ( ID2GAMEEXTEXT ( "CHAR_DEL_CLUB_FAILED" ), UI::MODAL_INFOMATION, UI::OK );
		}
		break;

	case NET_MSG_CHA_DEL_FB_ERROR:
		{
			UiHideGroup( MODAL_WINDOW_OUTER, true );
			DoModal ( ID2GAMEEXTEXT ( "CHAR_DEL_FAILED" ), UI::MODAL_INFOMATION, UI::OK );
		}
		break;

	case NET_MSG_CHA_NEW_FB:
		{
			NET_NEW_CHA_FB* pCharNewFb = (NET_NEW_CHA_FB*) nmg;
			//pCharNewFb->nChaNum;

			switch ( pCharNewFb->nResult )
			{
			case EM_CHA_NEW_FB_SUB_OK:	// ĳ���� ���� ����
				{						
					DoModal ( ID2GAMEEXTEXT ( "CREATESTAGE_4" ), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CREATESUCCEED );

					//	ĳ���� ī��� ������
					SetCreateExtreme( pCharNewFb->bExtreme );
					//SetCreateJumping( pCharNewFb->m_bJumping );
					SetCharRemain( static_cast<USHORT>(pCharNewFb->wChaRemain) );
				}
				break;		

			case EM_CHA_NEW_FB_SUB_DUP: // �̹� ���� �̸��� ĳ���Ͱ� ����
				DoModal(ID2GAMEEXTEXT("CREATESTAGE_5"), UI::MODAL_INFOMATION, UI::OK);
				break;

			case EM_CHA_NEW_FB_SUB_GM: // GM ������ ĳ���� ���� �Ұ�
				DoModal(ID2GAMEEXTEXT("CREATESTAGE_11"), UI::MODAL_INFOMATION, UI::OK);
				break;

			case EM_CHA_NEW_FB_SUB_LENGTH: // ���� ĳ���� �̸� ���̺��� ª��
				DoModal(ID2GAMEEXTEXT("NEWCHAR_NAME_TOO_SHORT"), UI::MODAL_INFOMATION, UI::OK);
				break;

			case EM_CHA_NEW_FB_SUB_MAX: // �����Ҽ� �ִ� �ִ� ĳ���ͼ� �ʰ�
				DoModal(ID2GAMEEXTEXT("CREATESTAGE_6"), UI::MODAL_INFOMATION, UI::OK);
				break;

			case EM_CHA_NEW_FB_SUB_ERROR: // �ý��ۿ��� �Ǵ� ������ �ʴ� ĳ���� �̸�
				DoModal(ID2GAMEEXTEXT("CREATESTAGE_7"), UI::MODAL_INFOMATION, UI::OK);
				break;
			case EM_CHA_NEW_FB_SUB_THAICHAR_ERROR:
				DoModal(ID2GAMEEXTEXT("CREATESTAGE_9"), UI::MODAL_INFOMATION, UI::OK);
				break;
			case EM_CHA_NEW_FB_SUB_VNCHAR_ERROR:
				DoModal(ID2GAMEEXTEXT("CREATESTAGE_10"), UI::MODAL_INFOMATION, UI::OK);
				break;
			case EM_CHA_NEW_FB_SUB_BAD_NAME:
				DoModal(ID2GAMEEXTEXT("CHARACTER_BADNAME"), UI::MODAL_INFOMATION, UI::OK);
				break;
            case EM_CHA_NEW_FB_SUB_BAD_NAME_KOREA:
                DoModal(ID2GAMEEXTEXT("CREATE_CHARACTER_NAME_ERROR_KOREA"), UI::MODAL_INFOMATION, UI::OK);
                break;
            case EM_CHA_NEW_FB_SUB_BAD_NAME_WB:
                DoModal(ID2GAMEEXTEXT("CREATE_CHARACTER_NAME_ERROR_WB"), UI::MODAL_INFOMATION, UI::OK);
                break;
            default:
                DoModal("NET_MSG_CHA_NEW_FB unknown type", UI::MODAL_INFOMATION, UI::OK);
                break;
			}
			m_pCreateCharacterWindow->UseCancelButton();
		}
		break;
	case GS_NET_MSG_WB_LOGIN_FB :
		if( sp == SP_GS )
		{
			GS_NET_WB_LOGIN_DATA_FB* pPacket = (GS_NET_WB_LOGIN_DATA_FB *) nmg;

			// ���弭���� ������ ���� �ʰų� ���弭���� �����ִ°��
			if( pPacket->bLoginWB == false || pPacket->bWBOff == true )
			{
				DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
				if ( NULL == pGlobalStage )
					return;

				CNetClient* pNetClient = pGlobalStage->GetNetClient();

				if( strcmp( pGlobalStage->GetWorldBattleID(), pPacket->szUserid ) == 0 )
				{
					pNetClient->GsSndLogin( pGlobalStage->GetWorldBattleID(), pGlobalStage->GetPWDForWB(), pPacket->nChannel );
				}
				else
				{
					DoModal ( ID2GAMEEXTEXT ("LOGIN_FAILED_ID_WB"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
					return;
				}
			}
			else
			{
				DoModal ( ID2GAMEEXTEXT ("LOGIN_FAILED_WB"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				return ;
			}

		}
		else
		{
			sc::writeLogError( std::string("WB Login can use only GS") );
		}
		break;
//#ifdef CH_PARAM_USEGAIN //chinaTest%%%
	case NET_MSG_LOBBY_CHINA_ERROR:
		if (sp == SP_CHINA)
		{
			DoModal ( ID2GAMEEXTEXT ( "CHINA_LOGIN_GAINTIME_ERROR" ), UI::MODAL_INFOMATION, UI::OK );
		}
//#endif
		break;
	};
}


void COuterInterface::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{	
	switch ( ControlID )
	{
	case SELECT_SERVER_PAGE:
	case LOGIN_PAGE:
	case CREATE_CHARACTER_PAGE:
	case CREATE_CHARACTER_BUTTON_PAGE:
	case CREATE_CHARACTER_WEB:
	case SELECT_CHINA_AREA_PAGE:
	case CHINA_TITLE:
	case COPYRIGHT:
	case SECPASS_SETPAGE:
	case SECPASS_CHECKPAGE:
	case PASS_KEYBOARD:
	case MODAL_WINDOW_OUTER:
	case OUTER_UP_IMAGE:
	case OUTER_DN_IMAGE:
    case SELECT_COUNTRY_CONFIRM_MODAL:
    case SELECT_COUNTRY_WINDOW:
    case SELECT_CLUB_WINDOW:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) ) SetOnTheInterface ();
		}
		break;

	case SELECT_COUNTRY_UI:
		{
			if ( UIMSG_MOUSEIN_SELECT_COUNTRYUI & dwMsg ) SetOnTheInterface ();
		}
		break;

	case SELECT_CONTINENT_LIST_UI:
		{
			if ( UIMSG_MOUSEIN_SELECT_CONTINENT_LIST_UI & dwMsg ) SetOnTheInterface ();
		}
		break;

	case SELECT_COUNTRY_LIST_UI:
		{
			if ( UIMSG_MOUSEIN_SELECT_COUNTRY_LIST_UI & dwMsg ) SetOnTheInterface ();
		}
		break;

	case SELECT_CHARACTER_PAGE:
		{
			if ( UIMSG_MOUSEIN_SELECT_CHARACTERPAGE & dwMsg ) SetOnTheInterface ();
		}
		break;

	case SELECT_CHARACTER_LEFTPAGE:
		{
			if ( UIMSG_MOUSEIN_SELECT_CHARACTERLEFTPAGE & dwMsg ) SetOnTheInterface ();
		}
		break;

	case FULLSCREEN_OUTER:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					const float fBUTTON_CLICK = CBasicButton::fBUTTON_CLICK;

					const UIRECT& rcPos = m_pFullScreenButtonDummy->GetGlobalPos ();				
					m_pFullScreenButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if (CHECK_MOUSEIN_LBUPLIKE(dwMsg))
				{
                    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
					pGlobalStage->ChangeWndMode();
				}
			}
		}
		break;
	}

	switch ( ControlID )
	{
	case MODAL_WINDOW_OUTER:
		{			
			if ( m_pModalWindow )
			{
				UIGUID CallerID = m_pModalWindow->GetCallerID ();
				ModalMsgProcess ( CallerID, dwMsg );
			}
		}
		break;		
	}
}