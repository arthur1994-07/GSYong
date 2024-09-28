#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxSound/DxSoundLib.h"

#include "../../RanLogic/Quest/GLQUEST.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../GLGaeaClient.h"
#include "../Stage/DxGlobalStage.h"
#include "../Crow/GLCrowClient.h"
#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLCharacter::MsgProcessQuest( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_REQ_QUEST_START_FB:
		{
			GLMSG::SNETPC_REQ_QUEST_START_FB *pNetNsg = (GLMSG::SNETPC_REQ_QUEST_START_FB *)nmg;
			switch ( pNetNsg->emFB )
			{
			case EMQUEST_START_FB_FAIL:
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMQUEST_START_FB_FAIL") );
				break;
			case EMQUEST_START_FB_MINIMUM_PLAYER:
				m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, sc::string::format(ID2GAMEINTEXT("EMQUEST_START_FB_MINIMUM_PLAYER"), pNetNsg->dwVar));
				break;
			case EMQUEST_START_FB_MAXIMUM_PLAYER:
				m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, sc::string::format(ID2GAMEINTEXT("EMQUEST_START_FB_MAXIMUM_PLAYER"), pNetNsg->dwVar));
				break;
			case EMQUEST_START_FB_ALREADY:
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_ALREADY") );
				break;
			case EMQUEST_START_FB_DONAGAIN:
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_DONAGAIN") );
				break;
			case EMQUEST_START_FB_DONREPEAT:
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_DONREPEAT") );
				break;
			case EMQUEST_START_FB_NEEDMONEY:
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_NEEDMONEY") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_STREAM:
		{
			GLMSG::SNET_QUEST_PROG_STREAM *pNetMsg = (GLMSG::SNET_QUEST_PROG_STREAM *) nmg;

#ifdef _TEMP_TRACING_LOG
			// QUEST_BUFFER_OVER
			sc::writeLogDebug(sc::string::format("NET_MSG_GCTRL_QUEST_PROG_STREAM dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", pNetMsg->dwQID, pNetMsg->dwSeq, pNetMsg->bLastSeq, pNetMsg->dwLengthStream, pNetMsg->dwSize));
#endif

			GLQUESTPROG sQUEST_PROG_NEW;

			if( pNetMsg->dwSeq != 0 && pNetMsg->bLastSeq == FALSE )
			{
				// 분할 메세지중에서 처음을 받으면 기존 데이터 초기화
				// 서버의 워커 스레드가 하나이므로 상관없음. 만약 순서가 섞여서 들어온다면 아래에서 에러 로그 기록후 리턴
				if( pNetMsg->dwSeq == 1 )
				{
					GLQuestMan::GetInstance().m_QuestByteStreamID = 0;
					GLQuestMan::GetInstance().m_sQuestByteStream.ClearBuffer();
				}

				GLQuestMan::GetInstance().m_QuestByteStreamID = pNetMsg->dwQID;
				GLQuestMan::GetInstance().m_sQuestByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );
				
				return;
			}
			else if( pNetMsg->dwSeq != 0 && pNetMsg->bLastSeq == TRUE )
			{
				if( GLQuestMan::GetInstance().m_QuestByteStreamID != pNetMsg->dwQID )
				{
					sc::writeLogError(sc::string::format("NET_MSG_GCTRL_QUEST_PROG_STREAM ID is not match - m_QuestByteStreamID : %1%, pNetMsg->dwQID : %2%", GLQuestMan::GetInstance().m_QuestByteStreamID, pNetMsg->dwQID));

					GLQuestMan::GetInstance().m_QuestByteStreamID = 0;
					GLQuestMan::GetInstance().m_sQuestByteStream.ClearBuffer();

					return;
				}

				GLQuestMan::GetInstance().m_sQuestByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

				//	Note : 퀘스트 생성.
				//
				sQUEST_PROG_NEW.GET_BYBUFFER ( GLQuestMan::GetInstance().m_sQuestByteStream );
			}
			else
			{
				se::ByteStream sByteStream;
				sByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

				//  퀘스트 생성.

				sQUEST_PROG_NEW.GET_BYBUFFER ( sByteStream );
			}

			// 리스트에 등록.
			m_cQuestPlay.InsertProc(sQUEST_PROG_NEW, false);

			GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( sQUEST_PROG_NEW.m_sNID.dwID );
			if (!pQUEST)
                break;

			//: 새로운 퀘스트 시작 메시지 출력.	
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
				m_pGaeaClient->PrintMsgTextDlg(
					NS_UITEXTCOLOR::PALEGREEN,
					sc::string::format(ID2GAMEINTEXT("QUEST_START"), CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString()));
			}
			else
			{
				m_pGaeaClient->PrintMsgTextDlg(
					NS_UITEXTCOLOR::PALEGREEN,
					sc::string::format(ID2GAMEINTEXT("QUEST_START"), pQUEST->GetTITLE()));
			}

			// Note : 캐릭터 머리위에서 나오는 효과들이 있다.
			//			그 녀석들을 위한 것.
			D3DXMATRIX matCharHeight = GetTransMatrix();
			matCharHeight._42 += m_fHeight;

			STARGETID sTargetID(CROW_PC,m_dwGaeaID, GetPosition());
			DxEffGroupPlayer::GetInstance().NewEffGroup(
				GLCONST_CHAR::strQUEST_START.c_str(),
				matCharHeight,
				&sTargetID, 
				FALSE, TRUE);

			//	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
            //m_pInterface->AUTO_LOAD_QUEST( sQUEST_PROG_NEW.m_sNID.dwID );
			//m_pInterface->REFRESH_QUEST_WINDOW ();


			// 첫번째 스템 퀘스트 검사 
			GLQUEST_STEP* pSTEP = pQUEST->GetSTEP ( 0 );
			if ( !pSTEP )	break;


			if( pSTEP->IsNEED_NPCTALK() )
			{
				std::tr1::shared_ptr<GLCrowClient> spCrow = m_pGaeaClient->GETQUSETNPC(*pSTEP);
				if (spCrow)
                    spCrow->SetQuestCompleteNPC(pQUEST->m_sNID.dwID);
			}

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_Start, "-dw", pQUEST->m_sNID.dwID);
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_STEP_STREAM:
		{
			GLMSG::SNET_QUEST_PROG_STEP_STREAM *pNetMsg = (GLMSG::SNET_QUEST_PROG_STEP_STREAM *) nmg;

#ifdef _TEMP_TRACING_LOG
			// QUEST_BUFFER_OVER
			sc::writeLogDebug(sc::string::format("NET_MSG_GCTRL_QUEST_PROG_STEP_STREAM dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwQSTEP %6%", pNetMsg->dwQID, pNetMsg->dwSeq, pNetMsg->bLastSeq, pNetMsg->dwLengthStream, pNetMsg->dwSize, pNetMsg->dwQSTEP));
#endif

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		return;

			if( pNetMsg->dwSeq != 0 && pNetMsg->bLastSeq == FALSE )
			{
				// 분할 메세지중에서 처음을 받으면 기존 데이터 초기화
				// 서버의 워커 스레드가 하나이므로 상관없음. 만약 순서가 섞여서 들어온다면 아래에서 에러 로그 기록후 리턴
				if( pNetMsg->dwSeq == 1 )
				{
					GLQuestMan::GetInstance().m_QuestByteStreamID = 0;
					GLQuestMan::GetInstance().m_sQuestByteStream.ClearBuffer();
				}

				GLQuestMan::GetInstance().m_QuestByteStreamID = pNetMsg->dwQID;
				GLQuestMan::GetInstance().m_sQuestByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );
				
				return;
			}
			else if( pNetMsg->dwSeq != 0 && pNetMsg->bLastSeq == TRUE )
			{
				if( GLQuestMan::GetInstance().m_QuestByteStreamID != pNetMsg->dwQID )
				{
					sc::writeLogError(sc::string::format("NET_MSG_GCTRL_QUEST_PROG_STEP_STREAM ID is not match - m_QuestByteStreamID : %1%, pNetMsg->dwQID : %2%", GLQuestMan::GetInstance().m_QuestByteStreamID, pNetMsg->dwQID));

					GLQuestMan::GetInstance().m_QuestByteStreamID = 0;
					GLQuestMan::GetInstance().m_sQuestByteStream.ClearBuffer();

					return;
				}

				pPROG->m_dwFlags = pNetMsg->dwFlag;
				pPROG->m_dwSTEP = pNetMsg->dwQSTEP;

				GLQuestMan::GetInstance().m_sQuestByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

				pPROG->m_sSTEP_PROG.GET_BYBUFFER ( GLQuestMan::GetInstance().m_sQuestByteStream );
			}
			else
			{
				pPROG->m_dwFlags = pNetMsg->dwFlag;
				pPROG->m_dwSTEP = pNetMsg->dwQSTEP;

				se::ByteStream cByteStream;
				cByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

				pPROG->m_sSTEP_PROG.GET_BYBUFFER ( cByteStream );
			}

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
			if ( !pQUEST )	break;

			GLQUEST_STEP* pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
			if ( !pSTEP )	break;

			//	Note : 완료 메시지 출력.	
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d_P_%d", pPROG->m_sNID.wMainID, pPROG->m_sNID.wSubID, pPROG->m_dwSTEP);
				m_pGaeaClient->PrintMsgText(
					NS_UITEXTCOLOR::PALEGREEN,
					sc::string::format(ID2GAMEINTEXT("QUEST_STEP"), CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString()));
			}
			else
			{
				m_pGaeaClient->PrintMsgText(
					NS_UITEXTCOLOR::PALEGREEN,
					sc::string::format(ID2GAMEINTEXT("QUEST_STEP"), pSTEP->GetTITLE()));
			}

			// Note : 캐릭터 머리위에서 나오는 효과들이 있다.
			//			그 녀석들을 위한 것.
			D3DXMATRIX matCharHeight = GetTransMatrix();
			matCharHeight._42 += m_fHeight;

			STARGETID sTargetID(CROW_PC,m_dwGaeaID, GetPosition());
			DxEffGroupPlayer::GetInstance().NewEffGroup(
				GLCONST_CHAR::strQUEST_START.c_str(),
				matCharHeight,
				&sTargetID,
				FALSE, TRUE);

			//	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();

			if (pSTEP->IsNEED_NPCTALK())
			{
				std::tr1::shared_ptr<GLCrowClient> spCrow = m_pGaeaClient->GETQUSETNPC(*pSTEP);
				if (spCrow)
                    spCrow->SetQuestCompleteNPC(pQUEST->m_sNID.dwID);
			}
			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgChange, "-dw", pQUEST->m_sNID.dwID);
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_INVEN:
		{
			GLMSG::SNET_QUEST_PROG_INVEN *pNetMsg = (GLMSG::SNET_QUEST_PROG_INVEN *) nmg;

#ifdef _TEMP_TRACING_LOG
			// QUEST_BUFFER_OVER
			sc::writeLogDebug(sc::string::format("NET_MSG_GCTRL_QUEST_PROG_INVEN dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", pNetMsg->dwQID, pNetMsg->dwSeq, pNetMsg->bLastSeq, pNetMsg->dwLengthStream, pNetMsg->dwSize));
#endif

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		return;

			if( pNetMsg->dwSeq != 0 && pNetMsg->bLastSeq == FALSE )
			{
				// 분할 메세지중에서 처음을 받으면 기존 데이터 초기화
				// 서버의 워커 스레드가 하나이므로 상관없음. 만약 순서가 섞여서 들어온다면 아래에서 에러 로그 기록후 리턴
				if( pNetMsg->dwSeq == 1 )
				{
					GLQuestMan::GetInstance().m_QuestByteStreamID = 0;
					GLQuestMan::GetInstance().m_sQuestByteStream.ClearBuffer();
				}

				GLQuestMan::GetInstance().m_QuestByteStreamID = pNetMsg->dwQID;
				GLQuestMan::GetInstance().m_sQuestByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );
				
				return;
			}
			else if( pNetMsg->dwSeq != 0 && pNetMsg->bLastSeq == TRUE )
			{
				if( GLQuestMan::GetInstance().m_QuestByteStreamID != pNetMsg->dwQID )
				{
					sc::writeLogError(sc::string::format("NET_MSG_GCTRL_QUEST_PROG_INVEN ID is not match - m_QuestByteStreamID : %1%, pNetMsg->dwQID : %2%", GLQuestMan::GetInstance().m_QuestByteStreamID, pNetMsg->dwQID));

					GLQuestMan::GetInstance().m_QuestByteStreamID = 0;
					GLQuestMan::GetInstance().m_sQuestByteStream.ClearBuffer();

					return;
				}

				GLQuestMan::GetInstance().m_sQuestByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

				pPROG->m_sINVENTORY.SETITEM_BYBUFFER ( GLQuestMan::GetInstance().m_sQuestByteStream );
			}
			else
			{
				se::ByteStream cByteStream;
				cByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

				pPROG->m_sINVENTORY.SETITEM_BYBUFFER ( cByteStream );
			}
			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgQItem, "-dw", pNetMsg->dwQID);
            //	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
            //m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_DEL:
		{
			GLMSG::SNET_QUEST_PROG_DEL *pNetMsg = (GLMSG::SNET_QUEST_PROG_DEL *) nmg;

			//	Note : '진행퀘스트 리스트'에서 퀘스트 정보 삭제.
			//
			m_cQuestPlay.DeleteProc ( pNetMsg->dwQUESTID );
			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_DelProg, "-dw", pNetMsg->dwQUESTID);
            //	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_END_STREAM:
		{
			GLMSG::SNET_QUEST_END_STREAM *pNetMsg = (GLMSG::SNET_QUEST_END_STREAM *) nmg;

#ifdef _TEMP_TRACING_LOG
			// QUEST_BUFFER_OVER
			sc::writeLogDebug(sc::string::format("NET_MSG_GCTRL_QUEST_END_STREAM dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", pNetMsg->dwQID, pNetMsg->dwSeq, pNetMsg->bLastSeq, pNetMsg->dwLengthStream, pNetMsg->dwSize));
#endif

			GLQUESTPROG sQUEST_PROG_END;

			if( pNetMsg->dwSeq != 0 && pNetMsg->bLastSeq == FALSE )
			{
				// 분할 메세지중에서 처음을 받으면 기존 데이터 초기화
				// 서버의 워커 스레드가 하나이므로 상관없음. 만약 순서가 섞여서 들어온다면 아래에서 에러 로그 기록후 리턴
				if( pNetMsg->dwSeq == 1 )
				{
					GLQuestMan::GetInstance().m_QuestByteStreamID = 0;
					GLQuestMan::GetInstance().m_sQuestByteStream.ClearBuffer();
				}

				GLQuestMan::GetInstance().m_QuestByteStreamID = pNetMsg->dwQID;
				GLQuestMan::GetInstance().m_sQuestByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );
				
				return;
			}
			else if( pNetMsg->dwSeq != 0 && pNetMsg->bLastSeq == TRUE )
			{
				if( GLQuestMan::GetInstance().m_QuestByteStreamID != pNetMsg->dwQID )
				{
					sc::writeLogError(sc::string::format("NET_MSG_GCTRL_QUEST_END_STREAM ID is not match - m_QuestByteStreamID : %1%, pNetMsg->dwQID : %2%", GLQuestMan::GetInstance().m_QuestByteStreamID, pNetMsg->dwQID));

					GLQuestMan::GetInstance().m_QuestByteStreamID = 0;
					GLQuestMan::GetInstance().m_sQuestByteStream.ClearBuffer();

					return;
				}

				GLQuestMan::GetInstance().m_sQuestByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

				//	Note : 퀘스트 생성.
				//
				sQUEST_PROG_END.GET_BYBUFFER ( GLQuestMan::GetInstance().m_sQuestByteStream );
			}
			else
			{
				//	Note : 버퍼 생성.
				//
				se::ByteStream sByteStream;
				sByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

				//	Note : 퀘스트 생성.
				//
				sQUEST_PROG_END.GET_BYBUFFER ( sByteStream );
			}

			//	Note : 리스트에 등록.
			//
            GLQUEST *pQUEST = GLQuestMan::GetInstance().Find(sQUEST_PROG_END.m_sNID.dwID);
            if (!pQUEST)
                break;

			m_cQuestPlay.InsertEnd(sQUEST_PROG_END, false);

			std::string strEND_QUEST;
			CString strText;
			//	Note : 완료 메시지 출력.
			if ( sQUEST_PROG_END.m_bCOMPLETE )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					strEND_QUEST = GLCONST_CHAR::strQUEST_END_SUCCEED;				
					CString strKey;
					strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
					m_pGaeaClient->PrintMsgTextDlg(
						NS_UITEXTCOLOR::PALEGREEN,
						sc::string::format(ID2GAMEINTEXT("QUEST_COMPLETE"), CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString()));
				
					GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_End, "-dw", pQUEST->m_sNID);
				}
				else
				{
					strEND_QUEST = GLCONST_CHAR::strQUEST_END_SUCCEED;				
					m_pGaeaClient->PrintMsgTextDlg(
						NS_UITEXTCOLOR::PALEGREEN,
						sc::string::format(ID2GAMEINTEXT("QUEST_COMPLETE"), pQUEST->GetTITLE()));

					GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_End, "-dw", pQUEST->m_sNID);
				}
			}
			else
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					strEND_QUEST = GLCONST_CHAR::strQUEST_END_FAIL;
					CString strKey;
					strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
					m_pGaeaClient->PrintMsgTextDlg(
						NS_UITEXTCOLOR::NEGATIVE,
						sc::string::format(ID2GAMEINTEXT("QUEST_COMPLETE"), CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString()));

					GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_End, "-dw", pQUEST->m_sNID);
				}
				else
				{
					strEND_QUEST = GLCONST_CHAR::strQUEST_END_FAIL;				
					m_pGaeaClient->PrintMsgTextDlg(
						NS_UITEXTCOLOR::NEGATIVE,
						sc::string::format(ID2GAMEINTEXT("QUEST_FAIL"), pQUEST->GetTITLE()));
				}
			}

			

			// Note : 캐릭터 머리위에서 나오는 효과들이 있다.
			//			그 녀석들을 위한 것.
			D3DXMATRIX matCharHeight = GetTransMatrix();
			matCharHeight._42 += m_fHeight;

			STARGETID sTargetID(CROW_PC,m_dwGaeaID, GetPosition());
			DxEffGroupPlayer::GetInstance().NewEffGroup(
				strEND_QUEST.c_str(),
				matCharHeight,
				&sTargetID, 
				FALSE, TRUE);

            //	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
            //m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_END_DEL:
		{
			GLMSG::SNET_QUEST_END_DEL *pNetMsg = (GLMSG::SNET_QUEST_END_DEL *) nmg;

			//	Note : '종료퀘스트 리스트'에서 퀘스트 정보 삭제.
			//
			m_cQuestPlay.DeleteEnd ( pNetMsg->dwQUESTID );
			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_DelEnd, "-dw", pNetMsg->dwQUESTID);
            //	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
            //m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_NPCTALK_FB:
		{
			GLMSG::SNET_QUEST_STEP_NPC_TALK_FB *pNetMsg = (GLMSG::SNET_QUEST_STEP_NPC_TALK_FB *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bTALK = true;

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
			if (!pQUEST)
                break;

			GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
			if (!pSTEP)
                break;

// 			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 			{
// 				SNATIVEID sNPCID = pSTEP->m_dwNID_NPCTALK;
// 				CString strKey;
// 				strKey.Format("CN_%03d_%03d", sNPCID.Mid(), sNPCID.Sid());
// 				CString strCrowName		= CGameTextMan::GetInstance().GetText(strKey.GetString(), 0, CGameTextMan::EM_CROW_TEXT).GetString();
// 				CString strCommentText	= CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_NPCTALK, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 				CString strText;	
// 				strText.Format("%s \'%s\'%s"
// 					, ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE")
// 					, strCrowName
// 					, strCommentText);
// 
// 				m_pGaeaClient->PrintMsgText(	NS_UITEXTCOLOR::PALEGREEN, strText.GetString());
// 			}
// 			else
			{
				m_pGaeaClient->PrintMsgText(
					NS_UITEXTCOLOR::PALEGREEN,
					sc::string::format(ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE"), pSTEP->GetOBJ_NPCTALK()));
			}

            // 인터페이스에 갱신 요청.
			//!! SF_TODO
            //m_pInterface->REFRESH_QUEST_WINDOW ();


			if (pSTEP->IsNEED_NPCTALK())
			{
				std::tr1::shared_ptr<GLCrowClient> spCrow = m_pGaeaClient->GETQUSETNPC(*pSTEP);
				if (spCrow)
                    spCrow->SetQuestCompleteNPC(m_pGaeaClient->CHECKQUESTNPCSTATE(spCrow, pSTEP, pPROG, pPROG->m_dwSTEP));
			}

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgNpcTalk, "-dw", pNetMsg->dwQID);

		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_MOBKILL:
		{
			GLMSG::SNET_QUEST_PROG_MOBKILL *pNetMsg = (GLMSG::SNET_QUEST_PROG_MOBKILL *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL = pNetMsg->bMOBKILL_ALL;
			pPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL = pNetMsg->dwMOBKILL;

			if ( pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL )
			{
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
				if (!pQUEST)
                    break;

				GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
				if (!pSTEP)
                    break;
				
				CString strCommentText;
// 				if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
// 				{
// 					strCommentText = CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_MOBKILL, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 				}
// 				else
				{
					strCommentText = pSTEP->GetOBJ_MOBKILL();
				}
				m_pGaeaClient->PrintMsgText(
                    NS_UITEXTCOLOR::PALEGREEN,
                    sc::string::format(ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE"), strCommentText.GetString()));
			}
			
			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgMobKill, "-dw", pNetMsg->dwQID);
			//	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	//case NET_MSG_GCTRL_QUEST_PARTY_PROG_MOBKILL:
	//	{
	//		GLMSG::SNET_QUEST_PARTY_PROG_MOBKILL *pNetMsg = (GLMSG::SNET_QUEST_PARTY_PROG_MOBKILL *) nmg;

	//		GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
	//		if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
	//			break;

	//		pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL = pNetMsg->bMOBKILL_ALL;

	//		if ( pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL )
	//		{
	//			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
	//			if ( !pQUEST )	break;

	//			GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
	//			if ( !pSTEP )	break;

	//			GLMSG::SNETPC_REQ_QUEST_COMPLETE NetMsg;
	//			NetMsg.dwQID = pNetMsg->dwQID;

	//			NETSENDTOFIELD ( &NetMsg );                
	//		}
	//	}
	//	break;


	case NET_MSG_GCTRL_QUEST_PROG_QITEM:
		{
			GLMSG::SNET_QUEST_PROG_QITEM *pNetMsg = (GLMSG::SNET_QUEST_PROG_QITEM *) nmg;
			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bQITEM = pNetMsg->bQITEM;

			if ( pPROG->m_sSTEP_PROG.m_bQITEM )
			{
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
				if ( !pQUEST )	break;

				GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
				if ( !pSTEP )	break;
				
				//m_pGaeaClient->PrintMsgText(
    //                NS_UITEXTCOLOR::PALEGREEN,
    //                sc::string::format(ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE"), pSTEP->GetOBJ_MOBGEN_QITEM()));
			}

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgQItem, "-dw", pNetMsg->dwQID);
			////	Note : 인터페이스에 갱신 요청.
			//m_pInterface->REFRESH_QUEST_WINDOW ();;
		}
		break;

	//case NET_MSG_GCTRL_QUEST_PARTY_PROG_QITEM:
	//	{
	//		GLMSG::SNET_QUEST_PARTY_PROG_QITEM *pNetMsg = (GLMSG::SNET_QUEST_PARTY_PROG_QITEM *) nmg;

	//		GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
	//		if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
	//			break;

	//		pPROG->m_sSTEP_PROG.m_bQITEM = pNetMsg->bQITEM;

	//		if ( pPROG->m_sSTEP_PROG.m_bQITEM )
	//		{
	//			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
	//			if ( !pQUEST )	break;

	//			GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
	//			if ( !pSTEP )	break;

	//			GLMSG::SNETPC_REQ_QUEST_COMPLETE NetMsg;
	//			NetMsg.dwQID = pNetMsg->dwQID;

	//			NETSENDTOFIELD ( &NetMsg );                
	//		}
	//	}
	//	break;
	//	
	case NET_MSG_GCTRL_QUEST_PROG_ITEMUSE:
		{
			GLMSG::SNET_QUEST_PROG_ITEMUSE *pNetMsg = (GLMSG::SNET_QUEST_PROG_ITEMUSE *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bITEMUSE_ALL = pNetMsg->bITEMUSE_ALL;
			pPROG->m_sSTEP_PROG.m_dwNUM_ITEMUSE = pNetMsg->dwNUM_ITEMUSE;

			if ( pPROG->m_sSTEP_PROG.m_bITEMUSE_ALL )
			{
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
				if (!pQUEST)
					break;

				GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
				if (!pSTEP)
					break;

// 				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 				{
// 					m_pGaeaClient->PrintMsgText(
// 						NS_UITEXTCOLOR::PALEGREEN,
// 						CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_ITEM_USE, CGameTextMan::EM_COMMENT_TEXT).GetString());
// 				}
// 				else
				{
					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::PALEGREEN,
						sc::string::format(ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE"), pSTEP->GetOBJ_ITEMUSE()));
				}
			}

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgItemUse, "-dw", pNetMsg->dwQID);
			//	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_CONFTING:
		{
			GLMSG::SNET_QUEST_PROG_CONFTING *pNetMsg = (GLMSG::SNET_QUEST_PROG_CONFTING *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bCONFTING_ALL = pNetMsg->bCONFTING_ALL;
			pPROG->m_sSTEP_PROG.m_dwNUM_CONFTING = pNetMsg->dwNUM_CONFTING;

			if ( pPROG->m_sSTEP_PROG.m_bCONFTING_ALL )
			{
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
				if (!pQUEST)
					break;

				GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
				if (!pSTEP)
					break;

				m_pGaeaClient->PrintMsgText(
					NS_UITEXTCOLOR::PALEGREEN,
					sc::string::format(ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE"), pSTEP->GetOBJ_CONFTING()));
			}

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgConfting, "-dw", pNetMsg->dwQID);
			//	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_LEVEL_UP:
		{
			GLMSG::SNET_QUEST_PROG_LEVEL_UP *pNetMsg = (GLMSG::SNET_QUEST_PROG_LEVEL_UP *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bLEVELUP_ALL = pNetMsg->bLEVELUP_ALL;
			pPROG->m_sSTEP_PROG.m_dwNUM_LEVELUP = pNetMsg->dwNUM_LEVELUP;

			if ( pPROG->m_sSTEP_PROG.m_bLEVELUP_ALL )
			{
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
				if (!pQUEST)
					break;

				GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
				if (!pSTEP)
					break;

// 				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 				{
// 					m_pGaeaClient->PrintMsgText(
// 						NS_UITEXTCOLOR::PALEGREEN,
// 						sc::string::format(ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE")
// 							,CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_LEVEL_UP_COMMENT, CGameTextMan::EM_COMMENT_TEXT).GetString()));
// 				}
// 				else
				{
					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::PALEGREEN,
						sc::string::format(ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE"), pSTEP->GetOBJ_LEVEL_UP()));
				}
			}

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgLevelUp, "-dw", pNetMsg->dwQID);
			//	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_ITEMGRINDING:
		{
			GLMSG::SNET_QUEST_PROG_ITEMGRINDING *pNetMsg = (GLMSG::SNET_QUEST_PROG_ITEMGRINDING *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bITEMGRINDING_ALL = pNetMsg->bITEMGRINDING_ALL;
			pPROG->m_sSTEP_PROG.m_dwNUM_ITEMGRINDING = pNetMsg->dwNUM_ITEMGRINDING;

			if ( pPROG->m_sSTEP_PROG.m_bITEMGRINDING_ALL )
			{
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
				if (!pQUEST)
					break;

				GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
				if (!pSTEP)
					break;

// 				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 				{
// 					m_pGaeaClient->PrintMsgText(
// 						NS_UITEXTCOLOR::PALEGREEN,
// 						CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_ITEM_GRINDING, CGameTextMan::EM_COMMENT_TEXT).GetString());
// 				}
// 				else
				{
					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::PALEGREEN,
						sc::string::format(ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE"), pSTEP->GetOBJ_ITEMGRINDING()));
				}
			}

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgItemGrinding, "-dw", pNetMsg->dwQID);
			//	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_REACHZONE:
		{
			GLMSG::SNET_QUEST_PROG_REACHZONE *pNetMsg = (GLMSG::SNET_QUEST_PROG_REACHZONE *) nmg;
			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bREACH_ZONE = pNetMsg->bREACHZONE;

			if ( pPROG->m_sSTEP_PROG.m_bREACH_ZONE )
			{
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
				if ( !pQUEST )	break;

				GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
				if ( !pSTEP )	break;
				
				CString strComment;
// 				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 				{
// 					strComment =
// 						CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_REACH_ZONE, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 				}
// 				else
				{
					strComment = pSTEP->GetOBJ_REACH_ZONE();
				}
				m_pGaeaClient->PrintMsgText(
                    NS_UITEXTCOLOR::PALEGREEN,
                    sc::string::format(ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE"), strComment.GetString()));
			}

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgReachZone, "-dw", pNetMsg->dwQID);
			//	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_TIMEOVER:
		{
			GLMSG::SNETPC_QUEST_PROG_TIMEOVER *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_TIMEOVER *) nmg;
 
			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		break;

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pNetMsg->dwQID );
			if ( !pQUEST )		break;
			
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
				m_pGaeaClient->PrintMsgText(
					NS_UITEXTCOLOR::NEGATIVE,
					sc::string::format(ID2GAMEINTEXT("QUEST_TIMEOVER"),CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString()));
			}
			else
			{
				m_pGaeaClient->PrintMsgText(
					NS_UITEXTCOLOR::NEGATIVE,
					sc::string::format(ID2GAMEINTEXT("QUEST_TIMEOVER"), pQUEST->GetTITLE()));
			}

			if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() && (pQUEST->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE) )
			{
				std::string strText = ID2GAMEINTEXT("RNATTENDANCE_QUEST_COMPLETE_TEXT");
				strText = strText.substr( 0, strText.find_last_of(':') );
				strText = sc::string::format ( ID2GAMEINTEXT("ATTENDANCE_UPDATE_COMPLETE_PROGRESSDISPLAY"), strText );

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_AttendanceComplete_Show,
					"-s", strText.c_str() );
			}

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgTimeOver, "-dw", pNetMsg->dwQID);
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_TIME:
		{
			GLMSG::SNETPC_QUEST_PROG_TIME *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_TIME *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		break;

			pPROG->m_fLAPSTIME = pNetMsg->fELAPS;
			
			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgTime, "-dw", pNetMsg->dwQID);
			//	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_NONDIE:
		{
			GLMSG::SNETPC_QUEST_PROG_NONDIE *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_NONDIE *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		break;

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pNetMsg->dwQID );
			if ( !pQUEST )		break;
			

			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
				m_pGaeaClient->PrintMsgText(
					NS_UITEXTCOLOR::NEGATIVE,
					sc::string::format(ID2GAMEINTEXT("QUEST_NONDIE"),CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString()));
			}
			else
			{
				m_pGaeaClient->PrintMsgText(
					NS_UITEXTCOLOR::NEGATIVE,
					sc::string::format(ID2GAMEINTEXT("QUEST_NONDIE"), pQUEST->GetTITLE()));
			}
			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgNonDie, "-dw", pNetMsg->dwQID);
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_LEAVEMAP:
		{
			GLMSG::SNETPC_QUEST_PROG_NONDIE *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_NONDIE *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		break;

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pNetMsg->dwQID );
			if ( !pQUEST )		break;
			
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
				m_pGaeaClient->PrintMsgText(
					NS_UITEXTCOLOR::NEGATIVE,
					sc::string::format(ID2GAMEINTEXT("QUEST_LEAVEMAP"),CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString()));
			}
			else
			{
				m_pGaeaClient->PrintMsgText(
					NS_UITEXTCOLOR::NEGATIVE,
					sc::string::format(ID2GAMEINTEXT("QUEST_LEAVEMAP"), pQUEST->GetTITLE()));
			}

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgLeaveMap, "-dw", pNetMsg->dwQID);
		}
		break;
		

	case NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT:
		{
			GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		break;

			pPROG->m_sINVENTORY.InsertItem ( pNetMsg->Data.sItemCustom, pNetMsg->Data.wPosX, pNetMsg->Data.wPosY );

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgItemInsert, "-dw", pNetMsg->dwQID);
			//	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_INVEN_DELETE:
		{
			GLMSG::SNETPC_QUEST_PROG_INVEN_DELETE *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_INVEN_DELETE *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )	break;

			pPROG->m_sINVENTORY.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgItemDelete, "-dw", pNetMsg->dwQID);
			//	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_INVEN_TURN:
		{
			GLMSG::SNETPC_QUEST_PROG_INVEN_TURNNUM *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_INVEN_TURNNUM *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		break;

			SINVENITEM* pInvenItem = pPROG->m_sINVENTORY.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
			if ( pInvenItem )
			{
				pInvenItem->sItemCustom.wTurnNum = pNetMsg->wTurnNum;
			}

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgItemCount, "-dw", pNetMsg->dwQID);
			//	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_INVEN_PICKUP:
		{
			GLMSG::SNETPC_QUEST_PROG_INVEN_PICKUP *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_INVEN_PICKUP *) nmg;

			SITEM * pITEM = GLogicData::GetInstance().GetItem ( pNetMsg->sNID_ITEM );
			if ( pITEM )
			{				
				m_pGaeaClient->PrintMsgText(
                    NS_UITEXTCOLOR::PALEGREEN,
                    sc::string::format(ID2GAMEINTEXT("PICKUP_QUESTITEM"), pITEM->GetName()));
			}

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Quest_ProgItemPickUp, "-dw", pNetMsg->sNID_ITEM);
			//	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_COMPLETE_FB:
		{
			GLMSG::SNETPC_QUEST_COMPLETE_FB *pNetMsg = (GLMSG::SNETPC_QUEST_COMPLETE_FB *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( pPROG == NULL )
			{
				// 클라이언트에 퀘스트가 없다면 초기화 
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_Quest_Claer );
				break;
			}

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pNetMsg->dwQID );
			if ( !pQUEST )		break;

			CString strText;
			switch ( pNetMsg->emFB )
			{
			case EMQUEST_COMPLETE_FB_FAIL:				
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						CString strKey;
						strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
						m_pGaeaClient->PrintMsgText(
							NS_UITEXTCOLOR::NEGATIVE,
							sc::string::format(ID2GAMEINTEXT("EMQUEST_COMPLETE_FB_FAIL"),CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString()));
					}
					else
					{
						m_pGaeaClient->PrintMsgText(
							NS_UITEXTCOLOR::NEGATIVE,
							sc::string::format(ID2GAMEINTEXT("EMQUEST_COMPLETE_FB_FAIL"), pQUEST->GetTITLE()));
					}
				}
				break;
			case EMQUEST_COMPLETE_FB_OK:				
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						CString strKey;
						strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
						m_pGaeaClient->PrintMsgText(
							NS_UITEXTCOLOR::PALEGREEN,
							sc::string::format(ID2GAMEINTEXT("EMQUEST_COMPLETE_FB_OK"),CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString()));
					}
					else
					{
						m_pGaeaClient->PrintMsgText(
							NS_UITEXTCOLOR::PALEGREEN,
							sc::string::format(ID2GAMEINTEXT("EMQUEST_COMPLETE_FB_OK"), pQUEST->GetTITLE()));
					}


					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_Quest_Claer );

				}
				break;
			case EMQUEST_COMPLETE_FB_NOINVEN:				
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						CString strKey;
						strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
						m_pGaeaClient->PrintMsgTextDlg(
							NS_UITEXTCOLOR::NEGATIVE,
							sc::string::format(ID2GAMEINTEXT("EMQUEST_COMPLETE_FB_NOINVEN"),CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString()));
						m_pGaeaClient->PrintMsgTextDlg(
							NS_UITEXTCOLOR::NEGATIVE,
							sc::string::format(ID2GAMEINTEXT("EMQUEST_COMPLETE_FB_NOINVEN_EX"),CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString()));
					}
					else
					{
						m_pGaeaClient->PrintMsgTextDlg(
							NS_UITEXTCOLOR::NEGATIVE,
							sc::string::format(ID2GAMEINTEXT("EMQUEST_COMPLETE_FB_NOINVEN"), pQUEST->GetTITLE()));

						m_pGaeaClient->PrintConsoleTextDlg(
							sc::string::format(ID2GAMEINTEXT("EMQUEST_COMPLETE_FB_NOINVEN_EX"), pQUEST->GetTITLE()));
					}
				}
				break;
			};

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Attendance );

            //	Note : 인터페이스에 갱신 요청.
			//!! SF_TODO
			//m_pInterface->REFRESH_QUEST_WINDOW ();

		}
		break;

	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess unknown message type %d", nmg->nType );
		break;
	};

	if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Attendance );
	}

}


