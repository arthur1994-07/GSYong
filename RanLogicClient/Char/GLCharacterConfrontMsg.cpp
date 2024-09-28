#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "../Party/GLPartyClient.h"
#include "../Char/GLCharClient.h"
#include "../Pet/GLPetClient.h"
#include "../GLGaeaClient.h"
#include "../Stage/DxGlobalStage.h"
#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



void GLCharacter::MsgProcessConfront( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_CONFRONT_FB:		
		//const GLMSG::SNETPC_REQ_CONFRONT_FB *pNetMsg = (GLMSG::SNETPC_REQ_CONFRONT_FB *)nmg;
		GLCharacter::MsgProcessConfront_FB((GLMSG::SNETPC_REQ_CONFRONT_FB *)nmg);			
		break;

	case NET_MSG_GCTRL_CONFRONT_START2_CLT:
		{
			GLMSG::SNETPC_CONFRONT_START2_CLT *pNetMsg = (GLMSG::SNETPC_CONFRONT_START2_CLT *)nmg;

			m_sCONFTING.RESET();

			//	Note : 유효성 검사. strNAME이 비여 있다면 무효로 판단.
			//
			std::string strNAME;
			switch ( pNetMsg->emTYPE )
			{
			case EMCONFT_ONE:
				{
					strNAME = pNetMsg->szMasterName;
				}				
				break;

			case EMCONFT_PARTY:				
				break;

			case EMCONFT_GUILD:				
				break;
			};

			if ( strNAME.empty() )	return;

			std::string strText = sc::string::format( ID2GAMEINTEXT("CONFT_START"), strNAME.c_str() ).c_str();
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );

			//	Note : 대련 정보 설정.
			//
			m_sCONFTING.emTYPE = pNetMsg->emTYPE;
			m_sCONFTING.dwTAR_ID = pNetMsg->dwTARID;
			m_sCONFTING.strTAR_NAME = strNAME;
			m_sCONFTING.vPosition = pNetMsg->vPosition;
			m_sCONFTING.sOption = pNetMsg->sOption;

			//	Note : 경계 표시.
			//
			if ( m_sCONFTING.sOption.bBOUND )	NewConftBoundEffect(m_sCONFTING.emTYPE);

			// 대련이 시작되면 팻은 사라진다.
			PGLPETCLIENT pMyPet = m_pGaeaClient->GetPetClient ();
			if ( pMyPet->IsVALID () )
			{
				GLMSG::SNETPET_REQ_UNUSEPETCARD NetMsg( pMyPet->m_dwGUID, pMyPet->DbNum() );
				m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
			}

			// 대련이 시작되면 내린다.
			if ( m_emVehicle ) 
			{
				ReqSetVehicle( false );
			}

			// 대련이 시작되면 상대방의 음식버프를 비활성화 한다
			std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar( m_sCONFTING.dwTAR_ID );
			if (pChar)
				pChar->SETENABLEALLLUNCHBOX( FALSE );
		}
		break;

	case NET_MSG_GCTRL_CONFRONT_FIGHT2_CLT:
		{
			m_sCONFTING.DoFIGHTING();

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
				"-n", 0 );
		}
		break;

	case NET_MSG_GCTRL_CONFRONT_END2_CLT:
		{
			GLMSG::SNETPC_CONFRONT_END2_CLT *pNetMsg = (GLMSG::SNETPC_CONFRONT_END2_CLT *)nmg;

			if ( !m_sCONFTING.IsCONFRONTING () )		break;

            std::string strText;
			switch ( pNetMsg->emEND )
			{
			case EMCONFRONT_END_FAIL:
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_FAIL") );
				break;

			case EMCONFRONT_END_WIN:
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
					"-n", (int) NSWIDGET_SCRIPT::EMCONFT_DISP_WIN );


				strText = sc::string::format( ID2GAMEINTEXT("EMCONFRONT_END_WIN"), m_sCONFTING.GETTARNAME() ).c_str();
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );

				SetActState(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_LOSS:
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
					"-n", (int) NSWIDGET_SCRIPT::EMCONFT_DISP_LOSE );

				strText = sc::string::format( ID2GAMEINTEXT("EMCONFRONT_END_LOSS"), m_sCONFTING.GETTARNAME() ).c_str();
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );

				ReSetActState(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_NOTWIN:
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_NOTWIN") );

				ReSetActState(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_TIME:
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
					"-n", (int) NSWIDGET_SCRIPT::EMCONFT_DISP_DRAW );

				strText = sc::string::format( ID2GAMEINTEXT("EMCONFRONT_END_TIME"), m_sCONFTING.GETTARNAME() ).c_str();
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );

				ReSetActState(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_MOVEMAP:
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
					"-n", (int) NSWIDGET_SCRIPT::EMCONFT_DISP_DRAW );

				strText = sc::string::format( ID2GAMEINTEXT("EMCONFRONT_END_MOVEMAP"), m_sCONFTING.GETTARNAME() ).c_str();
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
				break;

			case EMCONFRONT_END_DISWIN:
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
					"-n", (int) NSWIDGET_SCRIPT::EMCONFT_DISP_DRAW );

				strText = sc::string::format( ID2GAMEINTEXT("EMCONFRONT_END_DISWIN"), m_sCONFTING.GETTARNAME() ).c_str();
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );

				SetActState(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_DISLOSS:
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
					"-n", (int) NSWIDGET_SCRIPT::EMCONFT_DISP_DRAW );

				strText = sc::string::format( ID2GAMEINTEXT("EMCONFRONT_END_DISLOSS"), m_sCONFTING.GETTARNAME() ).c_str();
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );

				ReSetActState(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_PARTY:
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
					"-n", (int) NSWIDGET_SCRIPT::EMCONFT_DISP_DRAW );

				strText = sc::string::format( ID2GAMEINTEXT("EMCONFRONT_END_PARTY"), m_sCONFTING.GETTARNAME() ).c_str();
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
				break;
			};

			// Note : 대련이 끝나면 상대방의 음식버프를 활성화 한다
			std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar( m_sCONFTING.dwTAR_ID );
			if (pChar)
				pChar->SETENABLEALLLUNCHBOX( TRUE );

			//	Note : 대련 종료처리.
            EndConfting();

			//	Note : 연속 강제 공격 취소.
			ResetReservationAction();

			//	Note : 학교간 대련 마크 표시 OFF.
			//!! SF_TODO
			//m_pInterface->SetAcademyFightEnd();
		}
		break;

	case NET_MSG_GCTRL_CONFRONTPTY_START2_CLT:
		{
			GLMSG::SNETPC_CONFRONTPTY_START2_CLT *pNetMsg = (GLMSG::SNETPC_CONFRONTPTY_START2_CLT *) nmg;

			m_pGaeaClient->GetMyPartyClient()->SetConfront((NET_MSG_GENERIC*) pNetMsg);

			std::string strText = sc::string::format( ID2GAMEINTEXT("CONFTPTY_START"), pNetMsg->szName ).c_str();
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );

			const GLPartySlotClient* const pMyInfo = m_pGaeaClient->GetMyPartyClient()->GetMemberByGaeaID(m_dwGaeaID);
			if ( pMyInfo && (pMyInfo->isMemberState(EMPARTY_MEMBER_STATE_CONFRONT) == true) )
			{
				m_sCONFTING.RESET();

				m_sCONFTING.emTYPE = EMCONFT_PARTY;
				m_sCONFTING.dwTAR_ID = pNetMsg->ConfrontPartyID.GetID();
				m_sCONFTING.strTAR_NAME = pNetMsg->szName;
				m_sCONFTING.vPosition = pNetMsg->vPosition;
				m_sCONFTING.sOption = pNetMsg->sOption;

				m_sCONFTING.setConftMember.clear();
				for ( int i=0; i<MAXPARTY; ++i )
				{
					DWORD dwMEMBER = pNetMsg->dwCONFRONT_TAR_MEMBER[i];
					if ( dwMEMBER==GAEAID_NULL )	continue;

					// Note : 대련이 시작하면 상대방의 음식버프를 비활성화 한다
					std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar( dwMEMBER );
					if ( pChar )
					{
						pChar->SETENABLEALLLUNCHBOX( FALSE );
					}					

					m_sCONFTING.setConftMember.insert(dwMEMBER);
				}
				

				//	Note : 경계 표시.
				//
				if ( m_sCONFTING.sOption.bBOUND )	NewConftBoundEffect(m_sCONFTING.emTYPE);

				//	Note : 학교간 대련 출력.
				//
				if ( m_sCONFTING.sOption.bSCHOOL )
				{
					//	Note : 학교간 대련 마크 표시 ON.
					//!! SF_TODO
					/*m_pInterface->SetAcademyFightBegin(
						m_sCONFTING.sOption.wMY_SCHOOL,
						m_pGaeaClient->GetMyPartyClient()->GetNConfrontMember(),
						m_sCONFTING.sOption.wTAR_SCHOOL,
						(int) m_sCONFTING.setConftMember.size());*/
				}

				// 대련이 시작되면 팻은 사라진다.
				PGLPETCLIENT pMyPet = m_pGaeaClient->GetPetClient ();
				if ( pMyPet->IsVALID () )
				{
					GLMSG::SNETPET_REQ_UNUSEPETCARD NetMsg( pMyPet->m_dwGUID, pMyPet->DbNum() );
					m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
				}

				// 대련이 시작되면 내린다.
				if ( m_emVehicle ) 
				{
					ReqSetVehicle( false );
				}
			}
			else
			{
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CONFTPTY_START_NOT") );
			}
		}
		break;

	case NET_MSG_GCTRL_CONFRONTPTY_END2_CLT:
		{
			GLMSG::SNETPC_CONFRONTPTY_END2_CLT *pNetMsg = (GLMSG::SNETPC_CONFRONTPTY_END2_CLT *) nmg;

			switch ( pNetMsg->emEND )
			{
			case EMCONFRONT_END_PWIN:
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
					"-n", (int) NSWIDGET_SCRIPT::EMCONFT_DISP_WIN );

				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCONFRONT_END_PWIN") );

				SetActState(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_PLOSS:
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
					"-n", (int) NSWIDGET_SCRIPT::EMCONFT_DISP_LOSE );

				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_PLOSS") );

				ReSetActState(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_PTIME:
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_PTIME") );

				ReSetActState(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_CWIN:
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
					"-n", (int) NSWIDGET_SCRIPT::EMCONFT_DISP_WIN );

				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCONFRONT_END_CWIN") );

				SetActState(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_CLOSS:
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
					"-n", (int) NSWIDGET_SCRIPT::EMCONFT_DISP_LOSE );

				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_CLOSS") );

				ReSetActState(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_CTIME:
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_CTIME") );

				ReSetActState(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;
			};

			// Note : 대련이 끝나면 상대방의 음식버프를 활성화 한다
			//
			SCONFTING_CLT::CONFT_MEM_ITER iter      = m_sCONFTING.setConftMember.begin();
			SCONFTING_CLT::CONFT_MEM_ITER iterEnd   = m_sCONFTING.setConftMember.end();

			for ( ; iter!=iterEnd; ++iter )
			{
				DWORD dwMEMBER = (*iter);
				if ( dwMEMBER==GAEAID_NULL )	
					continue;
			
				std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar( m_sCONFTING.dwTAR_ID );
				if ( pChar )
				{
					pChar->SETENABLEALLLUNCHBOX( TRUE );
				}
			}

			//	Note : 승리하였을 경우 아직 리셋이 안되어 있음.
			//
            //  대련 종료 처리
			EndConfting();

			//	Note : 연속 강제 공격 취소.
			ResetReservationAction();

			//	Note : 학교간 대련 마크 표시 OFF.
			//!! SF_TODO
			//m_pInterface->SetAcademyFightEnd();
		}
		break;

	case NET_MSG_GCTRL_CONFRONT_RECOVE:
		{
			GLMSG::SNETPC_CONFRONT_RECOVE *pNetMsg = (GLMSG::SNETPC_CONFRONT_RECOVE *) nmg;

			//	Note : 회복약 사용 횟수 업대이트.
			//
			m_sCONFTING.wRECOVER = pNetMsg->wRECOVER;

			int nRECOVER = m_sCONFTING.sOption.wRECOVER-m_sCONFTING.wRECOVER;
			const DWORD dwCOLOR = nRECOVER>0 ? NS_UITEXTCOLOR::PALEGREEN : NS_UITEXTCOLOR::DISABLE;

			std::string strText = sc::string::format( ID2GAMEINTEXT("CONFRONT_RECOVE"), nRECOVER ).c_str();
			m_pGaeaClient->PrintMsgText ( dwCOLOR, strText );
		}
		break;

	case NET_MSG_GCTRL_CONFRONT_END2_CLT_MBR:		
		{
			GLMSG::SNETPC_CONFRONT_END2_CLT_MBR *pNetMsg = (GLMSG::SNETPC_CONFRONT_END2_CLT_MBR *)nmg;

			DELCONFRONT_MEMBER(pNetMsg->dwID);
		}
		break;

	case NET_MSG_GCTRL_CONFRONTCLB_START2_CLT:
		{
			GLMSG::SNETPC_CONFRONTCLB_START2_CLT *pNetMsg = (GLMSG::SNETPC_CONFRONTCLB_START2_CLT *) nmg;

			std::string strText = sc::string::format( ID2GAMEINTEXT("CONFTCLB_START"), pNetMsg->szName ).c_str();
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );

			DWORD i(0);
			for ( i=0; i<pNetMsg->dwCLUB_MY_NUM; ++i )
			{
				m_pMyClub->ADDCONFT_MEMBER ( pNetMsg->dwCONFRONT_MY_MEMBER[i] );
			}

			m_sCONFTING.RESET();

			m_sCONFTING.emTYPE = EMCONFT_GUILD;
			m_sCONFTING.dwTAR_ID = pNetMsg->dwconftCLUB;
			m_sCONFTING.strTAR_NAME = pNetMsg->szName;
			m_sCONFTING.vPosition = pNetMsg->vPosition;
			m_sCONFTING.sOption = pNetMsg->sOption;

			m_sCONFTING.setConftMember.clear();
			for ( i=0; i<pNetMsg->dwCLUB_TAR_NUM; ++i )
			{
				// Note : 대련이 시작하면 상대방의 음식버프를 비활성화 한다
				std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar( pNetMsg->dwCONFRONT_MY_MEMBER[i] );
				if ( pChar )
				{
					pChar->SETENABLEALLLUNCHBOX( FALSE );
				}
		
				m_sCONFTING.setConftMember.insert(pNetMsg->dwCONFRONT_TAR_MEMBER[i]);
			}

			//	Note : 경계 표시.
			//
			if ( m_sCONFTING.sOption.bBOUND )	NewConftBoundEffect(m_sCONFTING.emTYPE);

			// 대련이 시작되면 팻은 사라진다.
			PGLPETCLIENT pMyPet = m_pGaeaClient->GetPetClient ();
			if ( pMyPet->IsVALID () )
			{
				GLMSG::SNETPET_REQ_UNUSEPETCARD NetMsg( pMyPet->m_dwGUID, pMyPet->DbNum() );
				m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
			}

			// 대련이 시작되면 내린다.
			if ( m_emVehicle ) 
			{
				ReqSetVehicle( false );
			}
		}
		break;

	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess unknown message type %d", nmg->nType );
		break;		
	}
}


void GLCharacter::MsgProcessConfront_FB(const GLMSG::SNETPC_REQ_CONFRONT_FB* pNetMsg){
	switch ( pNetMsg->emFB )
	{			
	case EMCONFRONT_FAIL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_FAIL") );
		break;
	case EMCONFRONT_REFUSE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_REFUSE") );
		break;
	case EMCONFRONT_PEACE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_PEACE") );
		break;
	case EMCONFRONT_MAX:
		{
			std::string strText = sc::string::format( ID2GAMEINTEXT("EMCONFRONT_MAX"), GLCONST_CHAR::dwCONFRONT_MAX, (int)pNetMsg->fTIME ).c_str();
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
		}
		break;
	case EMCONFRONT_ELAP:
		{
			std::string strText = sc::string::format( ID2GAMEINTEXT("EMCONFRONT_ELAP"), (int)pNetMsg->fTIME ).c_str();
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
		}
		break;
	case EMCONFRONT_ALREADY_ME:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_ALREADY_ME") );
		break;
	case EMCONFRONT_ALREADY_TAR:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_ALREADY_TAR") );
		break;
	case EMCONFRONT_PARTY:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_PARTY") );
		break;
	case EMCONFRONT_DIE_YOU:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_DIE_YOU") );
		break;
	case EMCONFRONT_DIE_ME:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_DIE_ME") );
		break;
	case EMCONFRONT_VEHICLE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_BIKE_FAIL") );
		break;
	case EMCONFRONT_NONCONFRONTMODE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_NONCONFRONTMODE") );
		break;
	case EMCONFRONT_EXPEDITION:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_EXPEDITION") );
		break;
	};

	// Note : 대련이 끝나거나 취소되면 상대방의 음식버프를 활성화 한다
	//
	GLCharacter::MsgProcessConfront_FB_ReEnableLunch(pNetMsg->emTYPE);	

	m_sCONFTING.RESET();
	DelConftBoundEffect();
}

void GLCharacter::MsgProcessConfront_FB_ReEnableLunch(const EMCONFT_TYPE emCONFT_TYPE){
	switch ( emCONFT_TYPE )
	{
	case EMCONFT_ONE:
		{
			std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar( m_sCONFTING.dwTAR_ID );
			if ( pChar )
			{
				pChar->SETENABLEALLLUNCHBOX( TRUE );
			}
		}
		break;

	case EMCONFT_PARTY:
	case EMCONFT_GUILD:
		{
			SCONFTING_CLT::CONFT_MEM_ITER iter      = m_sCONFTING.setConftMember.begin();
			SCONFTING_CLT::CONFT_MEM_ITER iterEnd   = m_sCONFTING.setConftMember.end();

			for ( ; iter!=iterEnd; ++iter )
			{
				DWORD dwMEMBER = (*iter);
				if ( dwMEMBER==GAEAID_NULL )	
					continue;

				std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar( m_sCONFTING.dwTAR_ID );
				if ( pChar )
				{
					pChar->SETENABLEALLLUNCHBOX( TRUE );
				}
			}
		}
		break;
	};
}