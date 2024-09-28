#include "../pch.h"
#include "./GLChar.h"

#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../FieldServer/GLGaeaServer.h"

#include "../../RanLogic/NpcTalk/NpcTalk.h"
#include "../Util/GLItemLimit.h"

#include "../../SigmaCore/DebugInclude.h"



HRESULT GLChar::MsgReqNpcQuestStart ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_QUEST_START *pNetMsg = (GLMSG::SNETPC_REQ_QUEST_START *) nmg;

	GLMSG::SNETPC_REQ_QUEST_START_FB NetMsgFB(EMQUEST_START_FB_OK);

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_OK;
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqNpcQuestStart", m_CharDbNum ) );
	}


	//	Note : 퀘스트 시작을 위한 검사.
	//
	GLCrow* pNPC = pLand->GetCrow( pNetMsg->dwNpcGlobID );
	if ( !pNPC || !pNPC->m_pCrowData )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_OK;
	}

    // Note : NPC 수행 가능 조사
    if ( !pNPC->IsNpcInteractionAble(GetPosition(), GETBODYRADIUS() ) )
    {
        NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
        SendToClient( &NetMsgFB );
        return S_OK;
    }

	//	Note : 퀘스트 시작을 부여 받는 대화 점검.
	std::tr1::shared_ptr<SNpcTalk> spTalk = pNPC->GetTalk(pNetMsg->dwTalkID);
	if (!spTalk)
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_OK;
	}

	//	Note : 해당 퀘스트가 맞는지 검사.
	DWORD dwQUESTID = pNetMsg->dwQuestID;

	DWORD dwIndex = UINT_MAX;

	for ( int i = 0; i < MAX_QUEST_START; ++i )
	{
		if ( spTalk->m_dwQuestStartID[i] == dwQUESTID )
		{
			dwIndex = i; 
			break;
		}
	}
	
	if ( dwIndex == UINT_MAX )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_OK;
	}

	//	Note : 퀘스트 시작을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	const DWORD nPartyMember = m_pGaeaServer->GetPartyManager()->GetNMember(GetPartyID());
	const DWORD nMember = nPartyMember ? nPartyMember : 1;
	if ( pQUEST->m_dwBeginPartyMemNum && (pQUEST->m_dwBeginPartyMemNum > nMember) )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_MINIMUM_PLAYER;
		NetMsgFB.dwVar = pQUEST->m_dwBeginPartyMemNum;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( pQUEST->m_dwLimitPartyMemNum && (pQUEST->m_dwLimitPartyMemNum < nMember) )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_MAXIMUM_PLAYER;
		NetMsgFB.dwVar = pQUEST->m_dwLimitPartyMemNum;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	이미 진행중인 퀘스트 인지 점검.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( pQUEST_PROG )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_ALREADY;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	이미 완료한 퀘스트인지 검사.
	GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd ( dwQUESTID );
	if ( pQUEST_END )
	{
		if ( pQUEST_END->m_bCOMPLETE )
		{
			//	여러번 시도 가능한지 검사.
			if ( !pQUEST->IsREPEAT() )
			{
				NetMsgFB.emFB = EMQUEST_START_FB_DONREPEAT;
				SendToClient( &NetMsgFB );
				return S_FALSE;
			}

			if ( pQUEST->IsDaily() )
			{
				CTime cTime = CTime::GetCurrentTime();
				CTime cTimeEnd(pQUEST_END->m_tEndTime);

				if (cTime.GetDay() == cTimeEnd.GetDay() &&
					cTime.GetMonth() == cTimeEnd.GetMonth() &&
					cTime.GetYear() == cTimeEnd.GetYear() )
				{
					NetMsgFB.emFB = EMQUEST_START_FB_DONREPEAT;
					SendToClient( &NetMsgFB );
					return S_FALSE;
				}
			}
		}
		else
		{
			//	포기(실패)한 퀘스트를 다시 시도 불가능한 경우.
			if ( !pQUEST->IsAGAIN() )
			{
				NetMsgFB.emFB = EMQUEST_START_FB_DONAGAIN;
				SendToClient( &NetMsgFB );
				return S_FALSE;
			}
		}
	}

	//	Note : 대화 조건에 부합하는지 점검.
	CTime cCurSvrTime = CTime::GetCurrentTime();
	if ( !spTalk->DoTEST( this, dwIndex, cCurSvrTime.GetHour() ) )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_CONDITION;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( GetInvenMoney() < pQUEST->m_dwBeginMoney )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_NEEDMONEY;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if( !CheckQuestProgressEvent( 0, pQUEST ) )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	Note : 가지고 있는 돈의 액수를 조절.
	CheckMoneyUpdate( GetInvenMoney(), pQUEST->m_dwBeginMoney, FALSE, "Quest Start Pay." );

	SubtractInvenMoneyServer( pQUEST->m_dwBeginMoney, SUBTRACT_INVEN_MONEY_QUEST_START );

	if ( pQUEST->m_dwBeginMoney )
	{
		// 금액 변화 로그.
		// 통계 및 로그 수정
        LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -LONGLONG(pQUEST->m_dwBeginMoney), ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_QUEST_START);
		// 통계 및 로그 수정
		LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT);
	}

	//	Note : 클라이언트에 돈 액수 변화를 알려줌.
	SendInvenMoneyUpdate();

	//	Note : 새로이 시작되는 퀘스트 설정.
	//
	GLQUESTPROG sQUEST_PROG_NEW;
	sQUEST_PROG_NEW.START ( pQUEST, pQUEST_END );

	//	리스트에 등록.
	m_cQuestPlay.InsertProc(sQUEST_PROG_NEW, false);

	//	Note : 추가된 퀘스트 정보 전송.
	//
	se::ByteStream sByteStream;
	sQUEST_PROG_NEW.SET_BYBUFFER ( sByteStream );
	
	LPBYTE pBuff(0);
	DWORD dwSize(0);
	sByteStream.GetBuffer ( pBuff, dwSize );
	
	if( dwSize <= MAX_QUEST_PACKET_SIZE )
	{
		GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew(sQUEST_PROG_NEW.m_sNID.dwID, 0);
		NetMsgQuestNew.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
		// QUEST_BUFFER_OVER
		sc::writeLogDebug(sc::string::format("MsgReqNpcQuestStart ( dwSize <= MAX_QUEST_PACKET_SIZE ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgQuestNew.dwQID, NetMsgQuestNew.dwSeq, NetMsgQuestNew.bLastSeq, NetMsgQuestNew.dwLengthStream, NetMsgQuestNew.dwSize));	
#endif

		SendToClient(&NetMsgQuestNew);
	}
	else
	{
		int nRepetIdx = (dwSize / MAX_QUEST_PACKET_SIZE) + ((dwSize%MAX_QUEST_PACKET_SIZE == 0) ? 0 : 1);
		
		for( int i = 0; i < nRepetIdx; i++ )
		{
			GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew(sQUEST_PROG_NEW.m_sNID.dwID, 0);
			NetMsgQuestNew.dwSeq = i+1;

			if( i == nRepetIdx-1 )
			{
				NetMsgQuestNew.bLastSeq = TRUE;
				NetMsgQuestNew.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE), dwSize );
			}
			else
			{
				NetMsgQuestNew.bLastSeq = FALSE;
				NetMsgQuestNew.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE) , MAX_QUEST_PACKET_SIZE );
				dwSize -= MAX_QUEST_PACKET_SIZE;
			}
#ifdef _TEMP_TRACING_LOG
			// QUEST_BUFFER_OVER
			sc::writeLogDebug(sc::string::format("MsgReqNpcQuestStart dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgQuestNew.dwQID, NetMsgQuestNew.dwSeq, NetMsgQuestNew.bLastSeq, NetMsgQuestNew.dwLengthStream, NetMsgQuestNew.dwSize));	
#endif

			SendToClient(&NetMsgQuestNew );
		}
	}

	//	만약 종전에 진행한 정보 있을때 삭제.
	if ( pQUEST_END )
	{
		//	Note : 종료된 퀘스트 리스트에서 퀘스트 정보 삭제.
		//
		m_cQuestPlay.DeleteEnd ( dwQUESTID );

		//	Note : 삭제된 퀘스트 정보 전송.
		//
		GLMSG::SNET_QUEST_END_DEL NetMsgDel(dwQUESTID);
		SendToClient(&NetMsgDel);
	}

	return S_OK;
}


HRESULT GLChar::MsgReqNpcQuestStartWithoutNPC ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_QUEST_START_WITHOUT_NPC *pNetMsg = (GLMSG::SNETPC_REQ_QUEST_START_WITHOUT_NPC *) nmg;
	GLMSG::SNETPC_REQ_QUEST_START_FB NetMsgFB(EMQUEST_START_FB_OK);

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_OK;
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqNpcQuestStartWithoutNPC", m_CharDbNum ) );
	}


	////	Note : 퀘스트 시작을 위한 검사.
	//
	//GLCrow* pNPC = pLand->GetCrow( pNetMsg->dwNpcGlobID );
	//if ( !pNPC || !pNPC->m_pCrowData )
	//{
	//	NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
	//	SendToClient( &NetMsgFB );
	//	return S_OK;
	//}

 //   // Note : NPC 수행 가능 조사
 //   if ( !pNPC->IsNpcInteractionAble(GetPosition(), GETBODYRADIUS() ) )
 //   {
 //       NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
 //       SendToClient( &NetMsgFB );
 //       return S_OK;
 //   }

	////	Note : 퀘스트 시작을 부여 받는 대화 점검.
	//std::tr1::shared_ptr<SNpcTalk> spTalk = pNPC->GetTalk(pNetMsg->dwTalkID);
	//if (!spTalk)
	//{
	//	NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
	//	SendToClient( &NetMsgFB );
	//	return S_OK;
	//}

	//	Note : 해당 퀘스트가 맞는지 검사.
	DWORD dwQUESTID = pNetMsg->dwQuestID;

	DWORD dwIndex = UINT_MAX;

	//for ( int i = 0; i < MAX_QUEST_START; ++i )
	//{
	//	if ( spTalk->m_dwQuestStartID[i] == dwQUESTID )
	//	{
	//		dwIndex = i; 
	//		break;
	//	}
	//}
	//
	if ( dwQUESTID >= UINT_MAX )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_OK;
	}

	//	Note : 퀘스트 시작을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	const DWORD nPartyMember = m_pGaeaServer->GetPartyManager()->GetNMember(GetPartyID());
	const DWORD nMember = nPartyMember ? nPartyMember : 1;
	if ( pQUEST->m_dwBeginPartyMemNum && (pQUEST->m_dwBeginPartyMemNum > nMember) )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_MINIMUM_PLAYER;
		NetMsgFB.dwVar = pQUEST->m_dwBeginPartyMemNum;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( pQUEST->m_dwLimitPartyMemNum && (pQUEST->m_dwLimitPartyMemNum < nMember) )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_MAXIMUM_PLAYER;
		NetMsgFB.dwVar = pQUEST->m_dwLimitPartyMemNum;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	이미 진행중인 퀘스트 인지 점검.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( pQUEST_PROG )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_ALREADY;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	이미 완료한 퀘스트인지 검사.
	GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd ( dwQUESTID );
	if ( pQUEST_END )
	{
		if ( pQUEST_END->m_bCOMPLETE )
		{
			//	여러번 시도 가능한지 검사.
			if ( !pQUEST->IsREPEAT() )
			{
				NetMsgFB.emFB = EMQUEST_START_FB_DONREPEAT;
				SendToClient( &NetMsgFB );
				return S_FALSE;
			}

			if ( pQUEST->IsDaily() )
			{
				CTime cTime = CTime::GetCurrentTime();
				CTime cTimeEnd(pQUEST_END->m_tEndTime);

				if (cTime.GetDay() == cTimeEnd.GetDay() &&
					cTime.GetMonth() == cTimeEnd.GetMonth() &&
					cTime.GetYear() == cTimeEnd.GetYear() )
				{
					NetMsgFB.emFB = EMQUEST_START_FB_DONREPEAT;
					SendToClient( &NetMsgFB );
					return S_FALSE;
				}
			}
		}
		else
		{
			//	포기(실패)한 퀘스트를 다시 시도 불가능한 경우.
			if ( !pQUEST->IsAGAIN() )
			{
				NetMsgFB.emFB = EMQUEST_START_FB_DONAGAIN;
				SendToClient( &NetMsgFB );
				return S_FALSE;
			}
		}
	}

	//	Note : 대화 조건에 부합하는지 점검.
	//CTime cCurSvrTime = CTime::GetCurrentTime();
	//if ( !spTalk->DoTEST( this, dwIndex, cCurSvrTime.GetHour() ) )
	//{
	//	NetMsgFB.emFB = EMQUEST_START_FB_CONDITION;
	//	SendToClient( &NetMsgFB );
	//	return S_FALSE;
	//}

	if ( GetInvenMoney() < pQUEST->m_dwBeginMoney )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_NEEDMONEY;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if( !CheckQuestProgressEvent( 0, pQUEST ) )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	Note : 가지고 있는 돈의 액수를 조절.
	CheckMoneyUpdate( GetInvenMoney(), pQUEST->m_dwBeginMoney, FALSE, "Quest Start Pay." );

	SubtractInvenMoneyServer( pQUEST->m_dwBeginMoney, SUBTRACT_INVEN_MONEY_QUEST_START );

	if ( pQUEST->m_dwBeginMoney )
	{
		// 금액 변화 로그.
		// 통계 및 로그 수정
        LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -LONGLONG(pQUEST->m_dwBeginMoney), ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_QUEST_START);
		// 통계 및 로그 수정
		LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT);
	}

	//	Note : 클라이언트에 돈 액수 변화를 알려줌.
	SendInvenMoneyUpdate();

	//	Note : 새로이 시작되는 퀘스트 설정.
	//
	GLQUESTPROG sQUEST_PROG_NEW;
	sQUEST_PROG_NEW.START ( pQUEST, pQUEST_END );

	//	리스트에 등록.
	m_cQuestPlay.InsertProc(sQUEST_PROG_NEW, false);

	//	Note : 추가된 퀘스트 정보 전송.
	//
	se::ByteStream sByteStream;
	sQUEST_PROG_NEW.SET_BYBUFFER ( sByteStream );
	
	LPBYTE pBuff(0);
	DWORD dwSize(0);
	sByteStream.GetBuffer ( pBuff, dwSize );
	
	if( dwSize <= MAX_QUEST_PACKET_SIZE )
	{
		GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew(sQUEST_PROG_NEW.m_sNID.dwID, 0);
		NetMsgQuestNew.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
		// QUEST_BUFFER_OVER
		sc::writeLogDebug(sc::string::format("MsgReqNpcQuestStart ( dwSize <= MAX_QUEST_PACKET_SIZE ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgQuestNew.dwQID, NetMsgQuestNew.dwSeq, NetMsgQuestNew.bLastSeq, NetMsgQuestNew.dwLengthStream, NetMsgQuestNew.dwSize));	
#endif

		SendToClient(&NetMsgQuestNew);
	}
	else
	{
		int nRepetIdx = (dwSize / MAX_QUEST_PACKET_SIZE) + ((dwSize%MAX_QUEST_PACKET_SIZE == 0) ? 0 : 1);
		
		for( int i = 0; i < nRepetIdx; i++ )
		{
			GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew(sQUEST_PROG_NEW.m_sNID.dwID, 0);
			NetMsgQuestNew.dwSeq = i+1;

			if( i == nRepetIdx-1 )
			{
				NetMsgQuestNew.bLastSeq = TRUE;
				NetMsgQuestNew.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE), dwSize );
			}
			else
			{
				NetMsgQuestNew.bLastSeq = FALSE;
				NetMsgQuestNew.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE) , MAX_QUEST_PACKET_SIZE );
				dwSize -= MAX_QUEST_PACKET_SIZE;
			}
#ifdef _TEMP_TRACING_LOG
			// QUEST_BUFFER_OVER
			sc::writeLogDebug(sc::string::format("MsgReqNpcQuestStart dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgQuestNew.dwQID, NetMsgQuestNew.dwSeq, NetMsgQuestNew.bLastSeq, NetMsgQuestNew.dwLengthStream, NetMsgQuestNew.dwSize));	
#endif

			SendToClient(&NetMsgQuestNew );
		}
	}

	//	만약 종전에 진행한 정보 있을때 삭제.
	if ( pQUEST_END )
	{
		//	Note : 종료된 퀘스트 리스트에서 퀘스트 정보 삭제.
		//
		m_cQuestPlay.DeleteEnd ( dwQUESTID );

		//	Note : 삭제된 퀘스트 정보 전송.
		//
		GLMSG::SNET_QUEST_END_DEL NetMsgDel(dwQUESTID);
		SendToClient(&NetMsgDel);
	}

	return S_OK;
}

HRESULT GLChar::MsgReqNpcQuestTalk ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_QUEST_STEP_NPC_TALK *pNetMsg = (GLMSG::SNET_QUEST_STEP_NPC_TALK *) nmg;

	//	Note : 퀘스트 진행 검사.
	//
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return S_OK;

	PGLCROW pNPC = pLand->GetCrow( pNetMsg->dwNpcGlobID );
	if ( !pNPC || !pNPC->m_pCrowData )
		return S_OK;

    // Note : NPC 수행 가능 조사
    if ( !pNPC->IsNpcInteractionAble(GetPosition(), GETBODYRADIUS() ) )
    {
        return S_OK;
    }

	//	Note : 퀘스트 대화 점검.
	std::tr1::shared_ptr<SNpcTalk> spTalk = pNPC->GetTalk(pNetMsg->dwTalkID);
	if (!spTalk)
		return S_OK;

	DWORD dwQUEST_STEP = spTalk->m_dwACTION_PARAM2;

	//	Note : 해당 퀘스트가 맞는지 검사.
	DWORD dwQUESTID = pNetMsg->dwQUESTID;
	if ( spTalk->m_dwACTION_PARAM1 != dwQUESTID )
		return S_OK;

	//	Note : 퀘스트 진행을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
		return S_FALSE;

	GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( dwQUEST_STEP );
	if ( !pSTEP )
		return S_FALSE;

	//	진행중인 퀘스트 인지 점검.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )
		return S_FALSE;

	if ( dwQUEST_STEP!=pQUEST_PROG->m_dwSTEP )
		return S_FALSE;

	if ( pNPC->m_sNativeID.dwID!=pSTEP->m_dwNID_NPCTALK )
	{
		return S_FALSE;
	}

	//	Note : 대화 조건 완료.
	pQUEST_PROG->m_sSTEP_PROG.m_bTALK = true;

	//	Note : 대화 조건 완료됫음을 알림.
	//
	GLMSG::SNET_QUEST_STEP_NPC_TALK_FB NetMsgFB(dwQUESTID, dwQUEST_STEP);
	SendToClient( &NetMsgFB );
	

	//	Note : 퀘스트 갱신. ( 주의 pQUEST_PROG 무효화 됬을 가능성 있음. )
	//
	DoQuestProgress ( pQUEST_PROG );
	pQUEST_PROG = NULL;

	return S_OK;
}

HRESULT GLChar::MsgReqQuestGiveUp ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_QUEST_PROG_GIVEUP *pNetMsg = (GLMSG::SNET_QUEST_PROG_GIVEUP *) nmg;

	DWORD dwQUESTID = pNetMsg->dwQUESTID;

	//	Note : 퀘스트 진행을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
		return S_FALSE;

	//	진행중인 퀘스트 인지 점검.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )
		return S_FALSE;

	if ( !pQUEST->IsGIVEUP() )
		return S_FALSE;

	// DoGIVEUP 호출전에 디비에서 퀘스트 아이템 삭제한다.
	GLInventory::CELL_MAP_CITER iter = pQUEST_PROG->m_sINVENTORY.GetItemList()->begin();
	GLInventory::CELL_MAP_CITER iter_end = pQUEST_PROG->m_sINVENTORY.GetItemList()->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		if ( pInvenItem )
		{
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemDelete( pInvenItem->GetGuid(), pInvenItem->GetDbState(), 89129 ) ) );
		}
	}

	//	리셋.
	pQUEST_PROG->DoGIVEUP();

	//	완료된 리스트에 추가.
	m_cQuestPlay.InsertEnd(*pQUEST_PROG, false);

	se::ByteStream cByteStream;
	pQUEST_PROG->SET_BYBUFFER ( cByteStream );

	LPBYTE pBuff(NULL);
	DWORD dwSize(0);
	cByteStream.GetBuffer ( pBuff, dwSize );

	//	진행정보 삭제.
	m_cQuestPlay.DeleteProc ( dwQUESTID );

	//	Note : 퀘스트 완료 정보 전송.
	//
	if( dwSize <= MAX_QUEST_PACKET_SIZE )
	{
		GLMSG::SNET_QUEST_END_STREAM NetMsgStream(pQUEST_PROG->m_sNID.dwID, 0);
		NetMsgStream.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
		// QUEST_BUFFER_OVER
		sc::writeLogDebug(sc::string::format("MsgReqQuestGiveUp ( dwSize <= MAX_QUEST_PACKET_SIZE ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgStream.dwQID, NetMsgStream.dwSeq, NetMsgStream.bLastSeq, NetMsgStream.dwLengthStream, NetMsgStream.dwSize));	
#endif

		SendToClient(&NetMsgStream );
	}
	else
	{
		int nRepetIdx = (dwSize / MAX_QUEST_PACKET_SIZE) + ((dwSize%MAX_QUEST_PACKET_SIZE == 0) ? 0 : 1);
		
		for( int i = 0; i < nRepetIdx; i++ )
		{
			GLMSG::SNET_QUEST_END_STREAM NetMsgStream(pQUEST_PROG->m_sNID.dwID, 0);
			NetMsgStream.dwSeq = i+1;

			if( i == nRepetIdx-1 )
			{
				NetMsgStream.bLastSeq = TRUE;
				NetMsgStream.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE), dwSize );
			}
			else
			{
				NetMsgStream.bLastSeq = FALSE;
				NetMsgStream.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE) , MAX_QUEST_PACKET_SIZE );
				dwSize -= MAX_QUEST_PACKET_SIZE;
			}
#ifdef _TEMP_TRACING_LOG
			// QUEST_BUFFER_OVER
			sc::writeLogDebug(sc::string::format("MsgReqQuestGiveUp dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgStream.dwQID, NetMsgStream.dwSeq, NetMsgStream.bLastSeq, NetMsgStream.dwLengthStream, NetMsgStream.dwSize));	
#endif

			SendToClient(&NetMsgStream );
		}
	}

	//	Note : 퀘스트 진행 정보 삭제 전송.
	//
	GLMSG::SNET_QUEST_PROG_DEL NetMsgDel(dwQUESTID);
	SendToClient(&NetMsgDel );

	return S_OK;
}

HRESULT GLChar::MsgReqQuestReadReset ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_QUEST_PROG_READ *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_READ *) nmg;

	DWORD dwQUESTID = pNetMsg->dwQID;

	//	Note : 퀘스트 진행을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
		return S_FALSE;

	//	진행중인 퀘스트 인지 점검.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )	
		return S_FALSE;

	if ( !pQUEST_PROG->IsReqREADING() )
		return S_FALSE;

	pQUEST_PROG->ResetReqREADING ();

	return S_OK;
}

HRESULT GLChar::MsgReqQuestComplete ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_QUEST_PROG_READ *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_READ *) nmg;

	DWORD dwQUESTID = pNetMsg->dwQID;

	GLMSG::SNETPC_QUEST_COMPLETE_FB NetMsgFB(dwQUESTID, EMQUEST_COMPLETE_FB_FAIL);

	//	Note : 퀘스트 진행을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
	{
		NetMsgFB.emFB = EMQUEST_COMPLETE_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	진행중인 퀘스트 인지 점검.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )
	{
		NetMsgFB.emFB = EMQUEST_COMPLETE_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

		//	Note : 퀘스트가 완료됫는지 검사.
	//
	if( m_bTracingUser == true )
		sc::writeLogError(sc::string::format("[QUEST TRACE] MsgReqQuestComplete : %1%", pQUEST_PROG->LogCheckCOMPLETE()));

	if ( !pQUEST_PROG->CheckCOMPLETE() )
	{
		NetMsgFB.emFB = EMQUEST_COMPLETE_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( !CheckQuestItemReward( pQUEST, pQUEST_PROG ) )
	{
		NetMsgFB.emFB = EMQUEST_COMPLETE_FB_NOINVEN;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	bool bOK = DoQuestProgress ( pQUEST_PROG );
	if ( bOK )
	{
		NetMsgFB.emFB = EMQUEST_COMPLETE_FB_OK;
		SendToClient( &NetMsgFB );
	}

	if ( bOK && pQUEST->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE )
	{
		std::vector<DWORD> vecAttendancAdditionalQuest;
		vecAttendancAdditionalQuest.clear();
		vecAttendancAdditionalQuest = GLQuestMan::GetInstance().GetAttendancAdditionalQuest();

		BOOST_FOREACH( DWORD AttendancAdditionalQuestID, vecAttendancAdditionalQuest )
		{
			StartAttendancAdditionalQuest( AttendancAdditionalQuestID );
		}
		
		CTime tStartTime(pQUEST_PROG->m_tStartTime);

		m_vecAttendanceSelectQuest.clear();
		m_vecAttendanceSelectQuest = GLQuestMan::GetInstance().FindAttendanceQuest( GETLEVEL(), 
																					3, 
																					static_cast<unsigned>(tStartTime.GetSecond()) );

		GLMSG::SNETPC_RNATTENDANCE_QUEST_LIST_FC msg;
		{
			BOOST_FOREACH( DWORD AttendanceSelectQuestID, m_vecAttendanceSelectQuest )
			{
				msg.vecQuestIDList.push_back( AttendanceSelectQuestID );
			}
		}

		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, msg );
		SendToClient( NET_MSG_RNATTENDANCE_QUEST_LIST_FC, PackBuffer);
	}

	return S_OK;
}

HRESULT GLChar::MsgReqRnattendanceQuestStart ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_RNATTENDANCE_QUEST_START *pNetMsg = (GLMSG::SNETPC_REQ_RNATTENDANCE_QUEST_START *) nmg;

	GLMSG::SNETPC_REQ_QUEST_START_FB NetMsgFB(EMQUEST_START_FB_OK);

	//	Note : 퀘스트 시작을 위한 검사.

	//	Note : 해당 퀘스트가 맞는지 검사.
	DWORD dwQUESTID = pNetMsg->dwQuestID;
	bool bIsAttendanceQuest = false;

	if ( static_cast<int>(m_vecAttendanceSelectQuest.size()) > 0 )
	{
		BOOST_FOREACH( DWORD dwSelectQuestID, m_vecAttendanceSelectQuest)
		{
			if ( dwSelectQuestID == dwQUESTID )
			{
				bIsAttendanceQuest = true;
				break;
			}
		}
	}

	if ( !bIsAttendanceQuest )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_OK;
	}

	//	Note : 퀘스트 시작을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	이미 진행중인 퀘스트 인지 점검.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( pQUEST_PROG )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_ALREADY;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	이미 완료한 퀘스트인지 검사.
	GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd ( dwQUESTID );
	//if ( pQUEST_END )
	//{
	//	if ( pQUEST_END->m_bCOMPLETE )
	//	{
	//		//	여러번 시도 가능한지 검사.
	//		if ( !pQUEST->IsREPEAT() )
	//		{
	//			NetMsgFB.emFB = EMQUEST_START_FB_DONREPEAT;
	//			SendToClient( &NetMsgFB );
	//			return S_FALSE;
	//		}

	//		if ( pQUEST->IsDaily() )
	//		{
	//			CTime cTime = CTime::GetCurrentTime();
	//			CTime cTimeEnd(pQUEST_END->m_tEndTime);

	//			if (cTime.GetDay() == cTimeEnd.GetDay() &&
	//				cTime.GetMonth() == cTimeEnd.GetMonth() &&
	//				cTime.GetYear() == cTimeEnd.GetYear() )
	//			{
	//				NetMsgFB.emFB = EMQUEST_START_FB_DONREPEAT;
	//				SendToClient( &NetMsgFB );
	//				return S_FALSE;
	//			}
	//		}
	//	}
	//	else
	//	{
	//		//	포기(실패)한 퀘스트를 다시 시도 불가능한 경우.
	//		if ( !pQUEST->IsAGAIN() )
	//		{
	//			NetMsgFB.emFB = EMQUEST_START_FB_DONAGAIN;
	//			SendToClient( &NetMsgFB );
	//			return S_FALSE;
	//		}
	//	}
	//}

	//	Note : 새로이 시작되는 퀘스트 설정.
	//
	GLQUESTPROG sQUEST_PROG_NEW;
	sQUEST_PROG_NEW.START ( pQUEST, pQUEST_END );

	//	리스트에 등록.
	m_cQuestPlay.InsertProc(sQUEST_PROG_NEW, false);

	//	Note : 추가된 퀘스트 정보 전송.
	//
	se::ByteStream sByteStream;
	sQUEST_PROG_NEW.SET_BYBUFFER ( sByteStream );

	LPBYTE pBuff(0);
	DWORD dwSize(0);
	sByteStream.GetBuffer ( pBuff, dwSize );

	if( dwSize <= MAX_QUEST_PACKET_SIZE )
	{
		GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew(sQUEST_PROG_NEW.m_sNID.dwID, 0);
		NetMsgQuestNew.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
		// QUEST_BUFFER_OVER
		sc::writeLogDebug(sc::string::format("MsgReqRnattendanceQuestStart ( dwSize <= MAX_QUEST_PACKET_SIZE ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgQuestNew.dwQID, NetMsgQuestNew.dwSeq, NetMsgQuestNew.bLastSeq, NetMsgQuestNew.dwLengthStream, NetMsgQuestNew.dwSize));	
#endif

		SendToClient(&NetMsgQuestNew);
	}
	else
	{
		int nRepetIdx = (dwSize / MAX_QUEST_PACKET_SIZE) + ((dwSize%MAX_QUEST_PACKET_SIZE == 0) ? 0 : 1);
		
		for( int i = 0; i < nRepetIdx; i++ )
		{
			GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew(sQUEST_PROG_NEW.m_sNID.dwID, 0);
			NetMsgQuestNew.dwSeq = i+1;

			if( i == nRepetIdx-1 )
			{
				NetMsgQuestNew.bLastSeq = TRUE;
				NetMsgQuestNew.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE), dwSize );
			}
			else
			{
				NetMsgQuestNew.bLastSeq = FALSE;
				NetMsgQuestNew.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE) , MAX_QUEST_PACKET_SIZE );
				dwSize -= MAX_QUEST_PACKET_SIZE;
			}
#ifdef _TEMP_TRACING_LOG
			// QUEST_BUFFER_OVER
			sc::writeLogDebug(sc::string::format("MsgReqRnattendanceQuestStart dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgQuestNew.dwQID, NetMsgQuestNew.dwSeq, NetMsgQuestNew.bLastSeq, NetMsgQuestNew.dwLengthStream, NetMsgQuestNew.dwSize));	
#endif

			SendToClient(&NetMsgQuestNew );
		}
	}

	//	만약 종전에 진행한 정보 있을때 삭제.
	if ( pQUEST_END )
	{
		//	Note : 종료된 퀘스트 리스트에서 퀘스트 정보 삭제.
		//
		m_cQuestPlay.DeleteEnd ( dwQUESTID );

		//	Note : 삭제된 퀘스트 정보 전송.
		//
		GLMSG::SNET_QUEST_END_DEL NetMsgDel(dwQUESTID);
		SendToClient(&NetMsgDel);
	}

	return S_OK;
}

HRESULT GLChar::StartAttendancAdditionalQuest( DWORD dwQUESTID )
{
	//	Note : 퀘스트 시작을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
	{
		return S_FALSE;
	}

	//	이미 진행중인 퀘스트 인지 점검.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( pQUEST_PROG )
	{
		return S_FALSE;
	}

	//	이미 완료한 퀘스트인지 검사.
	GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd ( dwQUESTID );


	WORD wStartYear = pQUEST->m_sAttendanceOption.wStartYear;
	WORD WStarMonth = pQUEST->m_sAttendanceOption.WStarMonth;
	WORD WstarDay = pQUEST->m_sAttendanceOption.WstarDay;

	WORD wEndYear = pQUEST->m_sAttendanceOption.wEndYear;
	WORD WEndMonth = pQUEST->m_sAttendanceOption.WEndMonth;
	WORD WEndDay = pQUEST->m_sAttendanceOption.WEndDay;

	CTime TToday = CTime::GetCurrentTime();

	if ( !(wStartYear <= TToday.GetYear()) ||
		 !(WStarMonth <= TToday.GetMonth()) ||
		 !(WstarDay <= TToday.GetDay()) ||
		 !(wEndYear >= TToday.GetYear()) ||
		 !(WEndMonth >= TToday.GetMonth()) ||
		 !(WEndDay >= TToday.GetDay()) )
		return  S_FALSE;

	//	Note : 새로이 시작되는 퀘스트 설정.
	//
	GLQUESTPROG sQUEST_PROG_NEW;
	sQUEST_PROG_NEW.START ( pQUEST, pQUEST_END );

	//	리스트에 등록.
	m_cQuestPlay.InsertProc(sQUEST_PROG_NEW, false);

	//	Note : 추가된 퀘스트 정보 전송.
	//
	se::ByteStream sByteStream;
	sQUEST_PROG_NEW.SET_BYBUFFER ( sByteStream );

	LPBYTE pBuff(0);
	DWORD dwSize(0);
	sByteStream.GetBuffer ( pBuff, dwSize );

	if( dwSize <= MAX_QUEST_PACKET_SIZE )
	{
		GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew(sQUEST_PROG_NEW.m_sNID.dwID, 0);
		NetMsgQuestNew.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
		// QUEST_BUFFER_OVER
		sc::writeLogDebug(sc::string::format("StartAttendancAdditionalQuest ( dwSize <= MAX_QUEST_PACKET_SIZE ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgQuestNew.dwQID, NetMsgQuestNew.dwSeq, NetMsgQuestNew.bLastSeq, NetMsgQuestNew.dwLengthStream, NetMsgQuestNew.dwSize));	
#endif

		SendToClient(&NetMsgQuestNew);
	}
	else
	{
		int nRepetIdx = (dwSize / MAX_QUEST_PACKET_SIZE) + ((dwSize%MAX_QUEST_PACKET_SIZE == 0) ? 0 : 1);
		
		for( int i = 0; i < nRepetIdx; i++ )
		{
			GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew(sQUEST_PROG_NEW.m_sNID.dwID, 0);
			NetMsgQuestNew.dwSeq = i+1;

			if( i == nRepetIdx-1 )
			{
				NetMsgQuestNew.bLastSeq = TRUE;
				NetMsgQuestNew.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE), dwSize );
			}
			else
			{
				NetMsgQuestNew.bLastSeq = FALSE;
				NetMsgQuestNew.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE) , MAX_QUEST_PACKET_SIZE );
				dwSize -= MAX_QUEST_PACKET_SIZE;
			}
#ifdef _TEMP_TRACING_LOG
			// QUEST_BUFFER_OVER
			sc::writeLogDebug(sc::string::format("StartAttendancAdditionalQuest dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgQuestNew.dwQID, NetMsgQuestNew.dwSeq, NetMsgQuestNew.bLastSeq, NetMsgQuestNew.dwLengthStream, NetMsgQuestNew.dwSize));	
#endif

			SendToClient(&NetMsgQuestNew );
		}
	}

	//	만약 종전에 진행한 정보 있을때 삭제.
	if ( pQUEST_END )
	{
		//	Note : 종료된 퀘스트 리스트에서 퀘스트 정보 삭제.
		//
		m_cQuestPlay.DeleteEnd ( dwQUESTID );

		//	Note : 삭제된 퀘스트 정보 전송.
		//
		GLMSG::SNET_QUEST_END_DEL NetMsgDel(dwQUESTID);
		SendToClient(&NetMsgDel);
	}

	return S_OK;
}