#include "../pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../Util/GLItemLimit.h"
#include "./GLChar.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLChar::QuestStartFromGetITEM ( SNATIVEID nidITEM )
{
	GLQuestMan::QUESTGEN_RANGE range = GLQuestMan::GetInstance().FindStartFromGetITEM(nidITEM);
	SetAvailableQuest( range );
}

void GLChar::QuestStartFromGetSKILL ( SNATIVEID nidSKILL )
{
	GLQuestMan::QUESTGEN_RANGE range = GLQuestMan::GetInstance().FindStartFromGetSKILL(nidSKILL);
	SetAvailableQuest( range );
}

void GLChar::QuestStartFromGetLEVEL ( WORD wLEVEL )
{
	GLQuestMan::QUESTGEN_RANGE range = GLQuestMan::GetInstance().FindStartFromGetLEVEL(wLEVEL);
	SetAvailableQuest( range );
}

void GLChar::QuestStartFromCharJOIN ( int nFlag )
{
	GLQuestMan::QUESTGEN_RANGE range = GLQuestMan::GetInstance().FindStartFromGetCharJOIN(nFlag);
	SetAvailableQuest( range );
}

void GLChar::SetAvailableQuest( GLQuestMan::QUESTGEN_RANGE range )
{
	if(range.first == range.second)
		return;

	DWORD dwQUEST = 0;
	GLQUEST* pQUEST = NULL;
	GLQUESTPROG* pQUEST_PROG = NULL;
	GLQUESTPROG* pQUEST_END = NULL;

	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestMan::QUESTGEN_ITER pos = range.first;
	for ( ; pos!=range.second; ++pos )
	{
		dwQUEST = (*pos).second;

		pQUEST = glQuestMan.Find ( dwQUEST );
		if ( !pQUEST )
			continue;

		if ( (pQUEST->m_sSTARTOPT.dwCLASS&m_emClass)==NULL )
			continue;

		if ( MAX_SCHOOL!=pQUEST->m_sSTARTOPT.wSCHOOL )
		{
			if ( pQUEST->m_sSTARTOPT.wSCHOOL!=m_wSchool )
				continue;
		}

		// 진행중인 퀘스트 인지 검사
		pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUEST );
		if ( pQUEST_PROG )
		{
			if ( pQUEST->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE )
			{
				CTime TToday = CTime::GetCurrentTime();
				bool bIsTodayAttendacceQuest = false;
				BOOST_FOREACH(LONGLONG QuestStart , pQUEST_PROG->m_vectQuestStart)
				{
					CTime TQuestStart = QuestStart;

					if( TToday.GetYear() == TQuestStart.GetYear() &&
						TToday.GetMonth() == TQuestStart.GetMonth() &&
						TToday.GetDay() == TQuestStart.GetDay() )
					{
						bIsTodayAttendacceQuest = true;
					}

				}

				if ( bIsTodayAttendacceQuest )
				{
					continue;
				}
				else
				{
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
								new db::ItemDelete( pInvenItem->GetGuid(), pInvenItem->GetDbState(), 89126 ) ) );
						}
					}

					//	리셋.
					pQUEST_PROG->DoGIVEUP();
					//	완료된 리스트에 추가.
					m_cQuestPlay.InsertEnd(*pQUEST_PROG, false);
					m_cQuestPlay.DeleteProc( dwQUEST );
				}
			}
			else
				continue;
		}

		//	이미 완료한 퀘스트인지 검사.
		pQUEST_END = m_cQuestPlay.FindEnd ( dwQUEST );
		if ( pQUEST_END )
		{
			if ( pQUEST_END->m_bCOMPLETE )
			{
				//	여러번 시도 가능한지 검사.
				if ( !pQUEST->IsREPEAT() )
					continue;

				if ( pQUEST->IsDaily() )
				{
					CTime cTime = CTime::GetCurrentTime();
					CTime cTimeEnd(pQUEST_END->m_tEndTime);

					if (cTime.GetDay() == cTimeEnd.GetDay() &&
						cTime.GetMonth() == cTimeEnd.GetMonth() &&
						cTime.GetYear() == cTimeEnd.GetYear() )
						continue;
				}

			}
			else
			{
				//	포기(실패)한 퀘스트를 다시 시도 불가능한 경우.
				if ( !pQUEST->IsAGAIN() &&
					pQUEST->m_sSTARTOPT.nJOIN != EMQJ_ATTENDANCE)
					continue;
			}
		}

		if( !CheckQuestProgressEvent( 0, pQUEST ) )
			continue;


		//	Note : 새로이 시작되는 퀘스트 설정.
		//
		GLQUESTPROG sQUEST_PROG_NEW;
		sQUEST_PROG_NEW.START ( pQUEST, pQUEST_END );

		//	리스트에 등록.
		m_cQuestPlay.InsertProc(sQUEST_PROG_NEW, true);

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
			sc::writeLogDebug(sc::string::format("SetAvailableQuest (dwSize <= MAX_QUEST_PACKET_SIZE) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgQuestNew.dwQID, NetMsgQuestNew.dwSeq, NetMsgQuestNew.bLastSeq, NetMsgQuestNew.dwLengthStream, NetMsgQuestNew.dwSize));
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
				sc::writeLogDebug(sc::string::format("SetAvailableQuest dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgQuestNew.dwQID, NetMsgQuestNew.dwSeq, NetMsgQuestNew.bLastSeq, NetMsgQuestNew.dwLengthStream, NetMsgQuestNew.dwSize));
#endif

				SendToClient(&NetMsgQuestNew);
			}
		}

		//	만약 종전에 진행한 정보 있을때 삭제.
		if ( pQUEST_END )
		{
			//	Note : 종료된 퀘스트 리스트에서 퀘스트 정보 삭제.
			//
			m_cQuestPlay.DeleteEnd ( dwQUEST );

			//	Note : 삭제된 퀘스트 정보 전송.
			//
			GLMSG::SNET_QUEST_END_DEL	NetMsgDel(dwQUEST);
			SendToClient(&NetMsgDel);
		}
	}
}

bool GLChar::DoQuestProgress ( GLQUESTPROG* pQUEST_PROG )
{
	GASSERT(pQUEST_PROG&&"GLChar::DoQuestProgress");
	if ( !pQUEST_PROG )
		return false;

	DWORD dwQID = pQUEST_PROG->m_sNID.dwID;

	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQID );
	if ( !pQUEST )
		return false;


	//	Note : 퀘스트 진행. ( 다음 스텝으로 진행될 수 있음. )
	//
	bool _bModifyInven(false);
	GLQUEST_PROGRESS_MAP stepMoveMap;
	std::vector< SINVENITEM > vecDeleted;
	bool bInserted = false;
	bool bPROG = pQUEST_PROG->DoPROGRESS( _bModifyInven, stepMoveMap, vecDeleted, bInserted );

	// 퀘스트 아이템 처리
	{
		size_t DeletedSize = vecDeleted.size();
		for ( size_t loop = 0; loop < DeletedSize; loop++ )
		{
			SINVENITEM& sItem = vecDeleted[loop];
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemDelete( sItem.GetGuid(), sItem.GetDbState(), 19785 ) ) );
		}

		if ( bInserted )
		{
			GLInventory::CELL_MAP_CITER iter = pQUEST_PROG->m_sINVENTORY.GetItemList()->begin();
			GLInventory::CELL_MAP_CITER iter_end = pQUEST_PROG->m_sINVENTORY.GetItemList()->end();
			for ( ; iter!=iter_end; ++iter )
			{
				SINVENITEM* pItem = ( *iter ).second;
				if ( !pItem )
				{
					continue;
				}

				int DbState = pItem->GetDbState();
				if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
				{
					AddGameAdoJob(
						db::DbActionPtr( 
						new db::ItemInsert( m_CharDbNum, INVEN_QUEST_PROC, pItem->sSaveData, 89125 ) ) );
					pItem->SetDbState( db::DB_UPDATE, true );
				}
				else if ( db::DB_UPDATE == DbState )
				{
					AddGameAdoJob(
						db::DbActionPtr( 
						new db::ItemUpdate( m_CharDbNum, INVEN_QUEST_PROC, pItem->sSaveData, 89125 ) ) );
				}
			}
		}
	}

	// 다음 스텝이 있고 해당 스텝이 다른 퀘스트라면
	// 만약 다음 스텝이 동일 퀘스트를 반복하도록 되어있다면 아래의 완료처리후 다시 시작하도록 한다.
	if ( bPROG ) 
	{
		//	Note : 다음 스텝 정보 전송.
		//
		se::ByteStream cByteStream;
		pQUEST_PROG->m_sSTEP_PROG.SET_BYBUFFER ( cByteStream );

		LPBYTE pBuff(NULL);
		DWORD dwSize(0);
		cByteStream.GetBuffer ( pBuff, dwSize );

		if( dwSize <= MAX_QUEST_PACKET_SIZE )
		{
			GLMSG::SNET_QUEST_PROG_STEP_STREAM NetMsgStep(dwQID, pQUEST_PROG->m_dwSTEP, pQUEST_PROG->m_dwFlags, 0);
			NetMsgStep.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
			// QUEST_BUFFER_OVER
			sc::writeLogDebug(sc::string::format("DoQuestProgress ( bPROG, dwSize <= MAX_QUEST_PACKET_SIZE ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwQSTEP %6%", NetMsgStep.dwQID, NetMsgStep.dwSeq, NetMsgStep.bLastSeq, NetMsgStep.dwLengthStream, NetMsgStep.dwSize, NetMsgStep.dwQSTEP));	
#endif

			SendToClient(&NetMsgStep );
		}
		else
		{
			int nRepetIdx = (dwSize / MAX_QUEST_PACKET_SIZE) + ((dwSize%MAX_QUEST_PACKET_SIZE == 0) ? 0 : 1);
			
			for( int i = 0; i < nRepetIdx; i++ )
			{
				GLMSG::SNET_QUEST_PROG_STEP_STREAM NetMsgStep(dwQID, pQUEST_PROG->m_dwSTEP, pQUEST_PROG->m_dwFlags, 0);
				NetMsgStep.dwSeq = i+1;

				if( i == nRepetIdx-1 )
				{
					NetMsgStep.bLastSeq = TRUE;
					NetMsgStep.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE), dwSize );
				}
				else
				{
					NetMsgStep.bLastSeq = FALSE;
					NetMsgStep.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE) , MAX_QUEST_PACKET_SIZE );
					dwSize -= MAX_QUEST_PACKET_SIZE;
				}
#ifdef _TEMP_TRACING_LOG
				// QUEST_BUFFER_OVER
				sc::writeLogDebug(sc::string::format("DoQuestProgress ( bPROG ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwQSTEP %6%", NetMsgStep.dwQID, NetMsgStep.dwSeq, NetMsgStep.bLastSeq, NetMsgStep.dwLengthStream, NetMsgStep.dwSize, NetMsgStep.dwQSTEP));	
#endif

				SendToClient(&NetMsgStep );
			}
		}

		if ( _bModifyInven )
		{
			se::ByteStream cByteStream;
			pQUEST_PROG->m_sINVENTORY.GETITEM_BYBUFFER ( cByteStream );

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			cByteStream.GetBuffer ( pBuff, dwSize );

			if( dwSize <= MAX_QUEST_PACKET_SIZE )
			{
				GLMSG::SNET_QUEST_PROG_INVEN NetMsgInven(dwQID, 0);
				NetMsgInven.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
				// QUEST_BUFFER_OVER
				sc::writeLogDebug(sc::string::format("DoQuestProgress ( _bModifyInven, dwSize <= MAX_QUEST_PACKET_SIZE ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgInven.dwQID, NetMsgInven.dwSeq, NetMsgInven.bLastSeq, NetMsgInven.dwLengthStream, NetMsgInven.dwSize));	
#endif

				SendToClient(&NetMsgInven );
			}
			else
			{
				int nRepetIdx = (dwSize / MAX_QUEST_PACKET_SIZE) + ((dwSize%MAX_QUEST_PACKET_SIZE == 0) ? 0 : 1);
				
				for( int i = 0; i < nRepetIdx; i++ )
				{
					GLMSG::SNET_QUEST_PROG_INVEN NetMsgInven(dwQID, 0);
					NetMsgInven.dwSeq = i+1;

					if( i == nRepetIdx-1 )
					{
						NetMsgInven.bLastSeq = TRUE;
						NetMsgInven.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE), dwSize );
					}
					else
					{
						NetMsgInven.bLastSeq = FALSE;
						NetMsgInven.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE) , MAX_QUEST_PACKET_SIZE );
						dwSize -= MAX_QUEST_PACKET_SIZE;
					}
#ifdef _TEMP_TRACING_LOG
					// QUEST_BUFFER_OVER
					sc::writeLogDebug(sc::string::format("DoQuestProgress ( _bModifyInven ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgInven.dwQID, NetMsgInven.dwSeq, NetMsgInven.bLastSeq, NetMsgInven.dwLengthStream, NetMsgInven.dwSize));	
#endif

					SendToClient(&NetMsgInven );
				}
			}
		}

		// always true
        //if( stepMoveMap.nidMAP.wMainID  != UINT_MAX &&
		//	stepMoveMap.nidMAP.wSubID   != UINT_MAX )
		//{
			QuestMoveProgressEvent( stepMoveMap.nidMAP, stepMoveMap.wPosX, stepMoveMap.wPosY, stepMoveMap.dwGateID );
		//}

		return true;
	}
	

	if( m_bTracingUser == true )
		sc::writeLogError(sc::string::format("[QUEST TRACE] DoQuestProgress : %1%", pQUEST_PROG->LogCheckCOMPLETE()));
	//	Note : 퀘스트가 완료됫는지 검사.
	//
	if ( pQUEST_PROG->CheckCOMPLETE() )
	{
		//if (pQUEST->m_sAttendanceOption.nType >= EMATTENDANCE_TYPE_CONNECTION &&
		//	pQUEST->m_sAttendanceOption.nType < EMATTENDANCE_TYPE_SIZE)
		//{
		//	GLMSG::SNETPC_QUEST_COMPLETE_FB NetMsgFB(dwQID, EMQUEST_COMPLETE_FB_NOINVEN);
		//	SendToClient( &NetMsgFB );
		//	return true;
		//}

		if ( !CheckQuestItemReward( pQUEST, pQUEST_PROG ) )
		{
			GLMSG::SNETPC_QUEST_COMPLETE_FB NetMsgFB(dwQID, EMQUEST_COMPLETE_FB_NOINVEN);
			SendToClient( &NetMsgFB );
			return true;
		}

		std::vector< SINVENITEM > vecDeleted;
		pQUEST_PROG->DoCOMPLETE( vecDeleted, m_bTracingUser );
		
		size_t DeletedSize = vecDeleted.size();
		for ( size_t loop = 0; loop < DeletedSize; loop++ )
		{
			SINVENITEM& sItem = vecDeleted[loop];
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemDelete( sItem.GetGuid(), sItem.GetDbState(), 19786 ) ) );
		}

		//	완료된 리스트에 추가.
		m_cQuestPlay.InsertEnd(*pQUEST_PROG, false);

		se::ByteStream cByteStream;
		pQUEST_PROG->SET_BYBUFFER ( cByteStream );

		LPBYTE pBuff(NULL);
		DWORD dwSize(0);
		cByteStream.GetBuffer ( pBuff, dwSize );

		//	진행정보 삭제.
		m_cQuestPlay.DeleteProc ( dwQID );

		//	Note : 퀘스트 완료 정보 전송.
		//
		if( dwSize <= MAX_QUEST_PACKET_SIZE )
		{
			GLMSG::SNET_QUEST_END_STREAM NetMsgStream(pQUEST_PROG->m_sNID.dwID, 0);
			NetMsgStream.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
			// QUEST_BUFFER_OVER
			sc::writeLogDebug(sc::string::format("DoQuestProgress ( 완료정보, dwSize <= MAX_QUEST_PACKET_SIZE ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgStream.dwQID, NetMsgStream.dwSeq, NetMsgStream.bLastSeq, NetMsgStream.dwLengthStream, NetMsgStream.dwSize));	
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
				sc::writeLogDebug(sc::string::format("DoQuestProgress ( 완료정보 ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgStream.dwQID, NetMsgStream.dwSeq, NetMsgStream.bLastSeq, NetMsgStream.dwLengthStream, NetMsgStream.dwSize));	
#endif

				SendToClient(&NetMsgStream );
			}
		}


		//	Note : 퀘스트 진행 정보 삭제 전송.
		//
		GLMSG::SNET_QUEST_PROG_DEL NetMsgDel(dwQID);		
		SendToClient(&NetMsgDel );

		if ( !CheckQuestProgressEvent( 1, pQUEST ) )
		{
			GLMSG::SNETPC_QUEST_COMPLETE_FB NetMsgFB(dwQID, EMQUEST_COMPLETE_FB_FAIL);
			SendToClient( &NetMsgFB );
			return true;
		}

		//	Note : 퀘스트에 따른 보상.
		//	연퀘로 동일 퀘스트를 반복하도록 설정한 경우 아래에서 처리
		DoQuestGenerateReward ( pQUEST );

		return true;
	}

	return false;
}

//	Note : 아이템 넣기 테스트, 퀘스트 보상 아이템이 들어갈 공간에 여유가 있는지 확인.
bool GLChar::CheckQuestItemReward ( GLQUEST* pQUEST, GLQUESTPROG* pQUEST_PROG )
{
	if (!pQUEST)
        return false;

	// 불필요한 Assign 삭제 

	GLItemMan & glItemMan = GLItemMan::GetInstance();

	if ( pQUEST->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE )
	{
		int nDayCount = GetAttendanceCompletedDay( pQUEST, pQUEST_PROG );
		DWORD dwItemId = GLQuestMan::GetInstance().GetRnattendanceRewardNDay( nDayCount ) ;

		if (dwItemId != UINT_MAX)
		{
			SNATIVEID sItemId( dwItemId );
			SITEMCUSTOM sItemCustom( sItemId );
			pQUEST->m_vecGiftITEM.clear();
			pQUEST->m_vecGiftITEM.push_back(sItemCustom);
		}
	}
	
	DWORD ItemCount = 0;
	INVENQARRAY_ITER iter = pQUEST->m_vecGiftITEM.begin();
	INVENQARRAY_ITER iter_end = pQUEST->m_vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sITEM_NEW = (*iter);

		SITEM* pItem = glItemMan.GetItem( sITEM_NEW.GetNativeID() );
		if ( !pItem )
			continue;

		ItemCount++;
// 		BOOL bOK = cInven.InsertItem( sITEM_NEW );
// 		if ( !bOK )
//             return false;
	}

	if ( ItemCount > m_cInventory.GetAmountEmpty( false ) )
	{
		return false;
	}
	
	return true;
}

void GLChar::DoQuestGenerateReward( GLQUEST* pQUEST )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if ( !pQUEST )
        return;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on DoQuestGenerateReward", m_CharDbNum ) );
	}

	GLItemMan & glItemMan = GLItemMan::GetInstance();
	GLSkillMan & glSkillMan = GLSkillMan::GetInstance();
	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	if ( pQUEST->m_lnGiftEXP>0 )
	{
		LONGLONG nMemExp = pQUEST->m_lnGiftEXP;

		if ( GLHIDESET::CheckHideSet( EMHIDE_UPDATE_EXP ) )
		{
			RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
			RestoreActStateByInvisible();
		}

		m_sExperience.lnNow += nMemExp;
	
		GLMSG::SNETPC_UPDATE_EXP NetMsgExp;
		NetMsgExp.lnNowExp = m_sExperience.lnNow;
		SendToClient( &NetMsgExp );

		if ( m_sExperience.ISOVER() )
		{
			if ( GLHIDESET::CheckHideSet( EMHIDE_LEVELUP ) )
			{
				RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
				RestoreActStateByInvisible();
			}
			//	Note : 랩업 수행. ( 로컬 메시지 발생. )
			GLMSG::SNETPC_REQ_LEVELUP NetMsg;
			MsgReqLevelUp ( ( NET_MSG_GENERIC* ) &NetMsg );
		}
	}

	if ( pQUEST->m_dwGiftMONEY > 0 )
	{
		LONGLONG lnAmount = pQUEST->m_dwGiftMONEY;

		CheckMoneyUpdate( GetInvenMoney(), lnAmount, TRUE, "Quest Gain Money." );

		AddInvenMoneyServer( lnAmount, ADD_INVEN_MONEY_QUEST_REWARD );

		GLMSG::SNETPC_PICKUP_MONEY NetMsgMoney;
		NetMsgMoney.lnMoney = GetInvenMoney();
		NetMsgMoney.lnPickUp = ( LONGLONG )pQUEST->m_dwGiftMONEY;
		SendToClient( &NetMsgMoney );
	}
	
	//	Note : 성향은 반복 불가능할때만 수여됨.
	if ( pQUEST->m_dwGiftELEMENT > 0 )
	{
		int nBrightMAX = GLCONST_CHAR::nBrightMAX;
		int nBrightMIN = GLCONST_CHAR::nBrightMIN;

		if ( m_nBright >= nBrightMIN && m_nBright <= nBrightMAX)
		{
			m_nBright += pQUEST->m_dwGiftELEMENT;

			GLMSG::SNETPC_UPDATE_BRIGHT NetMsgBright;
			NetMsgBright.nBright = m_nBright;
			SendToClient( &NetMsgBright );


			GLMSG::SNETPC_UPDATE_BRIGHT_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;
			NetMsgBrd.nBright = m_nBright;
			SendMsgViewAround( &NetMsgBrd );
		}
	}

	if ( pQUEST->m_dwGiftLIFEPOINT > 0 )
	{
		ReceiveLivingPoint( pQUEST->m_dwGiftLIFEPOINT );
	}

	if ( pQUEST->m_dwGiftDisPK > 0 )
	{
		if ( m_wPK>pQUEST->m_dwGiftDisPK )	m_wPK -= WORD( pQUEST->m_dwGiftDisPK );
		else								m_wPK = 0;
	}

	if ( pQUEST->m_dwGiftSKILLPOINT > 0 )
	{
		m_dwSkillPoint += pQUEST->m_dwGiftSKILLPOINT;

		GLCharSlotMan::Instance()->SkillPointUp( CharDbNum(), pQUEST->m_dwGiftSKILLPOINT );
		
		GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
		NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
		SendToClient( &NetMsgSkp );
	}

	if ( pQUEST->m_dwGiftSTATSPOINT > 0 )
	{
		m_wStatsPoint += ( WORD )pQUEST->m_dwGiftSTATSPOINT;

		GLCharSlotMan::Instance()->StatPointUp( CharDbNum(), pQUEST->m_dwGiftSTATSPOINT );
		
		GLMSG::SNETPC_UPDATE_STATS NetMsgStats;
		NetMsgStats.wStatsPoint = m_wStatsPoint;
		SendToClient( &NetMsgStats );
	}

	//	아이템 증정.
	{
		INVENQARRAY_ITER iter = pQUEST->m_vecGiftITEM.begin();
		INVENQARRAY_ITER iter_end = pQUEST->m_vecGiftITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sITEM_NEW = (*iter);

			CTime cTIME = CTime::GetCurrentTime();
			sITEM_NEW.tBORNTIME = cTIME.GetTime();
			sITEM_NEW.cGenType = EMGEN_QUEST;
			sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
			sITEM_NEW.GenerateBasicStat( false );
			sITEM_NEW.GenerateLinkSkill();
			sITEM_NEW.GenerateAddOption();

			LogItemBasicStatOption( sITEM_NEW );
			LogItemLinkSkillOption( sITEM_NEW );
			LogItemAddonOption( sITEM_NEW );

			// 퀘스트 보상의 경우에도 랜덤옵션이 붙도록 한다;
			if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
				LogItemRandomOption( sITEM_NEW );

			SITEM* pItem = glItemMan.GetItem( sITEM_NEW.GetNativeID() );
			if ( !pItem )
                continue;

			WORD wInsertPosX( 0 ), wInsertPosY( 0 );
			BOOL bITEM_SPACE = TRUE;
			bITEM_SPACE = m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );
			if ( bITEM_SPACE )
			{
				//	인밴토리에 넣음.
				if ( m_cInventory.InsertItem( sITEM_NEW, wInsertPosX, wInsertPosY ) )
				{
					// 아이템의 소유 이전 경로 기록
					// 통계 및 로그 수정
					LogItemExchange(
						sITEM_NEW,
						gamelog::ID_CHAR, 0,
						gamelog::ID_CHAR, m_CharDbNum,
						ITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum, 
						( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_QUEST_REWARD,
						false );

					// 디비 업데이트
					InvenUpdateItem( m_cInventory.GetItem( wInsertPosX, wInsertPosY ) );

					//	인밴에 아이탬 넣어주는 메시지.
					InvenInsertSend( m_cInventory.GetItem( wInsertPosX, wInsertPosY ), FALSE, false, true );
				}
			}
            else
			{
				D3DXVECTOR3 vGenPos = D3DXVECTOR3( m_vPosition ) + D3DXVECTOR3( sc::Random::RANDOM_NUM() * 4.0f, 0.0f, sc::Random::RANDOM_NUM() * 4.0f );
				
				BOOL bCollision;
				D3DXVECTOR3 vCollision;
				bCollision = pLand->IsCollisionNavi
				(
					D3DXVECTOR3( vGenPos ) + D3DXVECTOR3( 0, +5,0 ),
					D3DXVECTOR3( vGenPos ) + D3DXVECTOR3( 0, -5,0 ),
					vCollision
				);

				if ( !bCollision )
					vCollision = m_vPosition;

				// 아이템 발생.
				DropItem( sITEM_NEW, vCollision );
			}
		}
	}

	//	스킬 증정.
	{
		DWQARRAY_ITER iter = pQUEST->m_vecGiftSKILL.begin();
		DWQARRAY_ITER iter_end = pQUEST->m_vecGiftSKILL.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SNATIVEID sNID = SNATIVEID( *iter );

			bool bLEARN = LEARN_SKILL_QUEST( sNID );
			if ( !bLEARN )	continue;

			//	스킬 배움 성공.
			GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
			NetMsgFB.skill_id = sNID;
			NetMsgFB.emCHECK = EMSKILL_LEARN_OK;
			SendToAgent( &NetMsgFB );

			//	Note : passive skill 의 경우 케릭에 적용되는 속성값을 클라이언트들에 동기화.
			//
			PGLSKILL pSkill = glSkillMan.GetData( sNID );
			if ( !pSkill ) continue;

			if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
			{
				GLMSG::SNETPC_UPDATE_PASSIVE_BRD NetMsgBrd;
				NetMsgBrd.dwGaeaID = m_dwGaeaID;
				NetMsgBrd.sSKILL_DATA = m_sSUM_PASSIVE;
				SendMsgViewAround( &NetMsgBrd );
			}
		}
	}

	if ( pQUEST->m_dwGiftQUEST != UINT_MAX )
	{
		// 퀘스트 설정을 잘못하는경우 무한루프에 빠질수 있으므로 연퀘는 한종류만 넣을수 있도록함.
		//do
		//{
			DWORD dwQUEST = pQUEST->m_dwGiftQUEST;

			GLQUEST *pQUEST = glQuestMan.Find( dwQUEST );
			if ( !pQUEST )
			{
				sc::writeLogError( sc::string::format( "DoQuestGenerateReward Find ERROR m_dwGiftQUEST %1%", dwQUEST ) );	
				return;
			}

			//	Memo :	주어지는 퀘스트가 클래스 조건에 맞는지 검사.
			if ( ( pQUEST->m_sSTARTOPT.dwCLASS & m_emClass ) == NULL )
			{
				sc::writeLogError( sc::string::format( "DoQuestGenerateReward Class ERROR m_dwGiftQUEST %1%", dwQUEST ) );	
				return;
			}

			const DWORD nPartyMember = m_pGaeaServer->GetPartyManager()->GetNMember( GetPartyID() );
			const DWORD nMember = nPartyMember ? nPartyMember : 1;
			if ( pQUEST->m_dwBeginPartyMemNum && ( pQUEST->m_dwBeginPartyMemNum > nMember ) )
			{
				sc::writeLogError( sc::string::format( "DoQuestGenerateReward Party ERROR m_dwGiftQUEST %1%", dwQUEST ) );	
				return;
			}

			if ( pQUEST->m_dwLimitPartyMemNum && ( pQUEST->m_dwLimitPartyMemNum < nMember ) )
			{
				sc::writeLogError( sc::string::format( "DoQuestGenerateReward Member ERROR m_dwGiftQUEST %1%", dwQUEST ) );	
				return;
			}

			// 진행중인 퀘스트인지
			GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc( dwQUEST );
			if ( pQUEST_PROG )
			{
				sc::writeLogError( sc::string::format( "DoQuestGenerateReward Quest Process ERROR m_dwGiftQUEST %1%", dwQUEST ) );	
				return;
			}

			//	이미 완료한 퀘스트인지 검사.
			GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd( dwQUEST );
			if ( pQUEST_END )
			{
				if ( pQUEST_END->m_bCOMPLETE )
				{
					//	여러번 시도 가능한지 검사.
					if ( !pQUEST->IsREPEAT() )
					{
						sc::writeLogError( sc::string::format( "DoQuestGenerateReward Repeat ERROR m_dwGiftQUEST %1%", dwQUEST ) );	
						return;
					}

					if ( pQUEST->IsDaily() )
					{
						CTime cTime = CTime::GetCurrentTime();
						CTime cTimeEnd( pQUEST_END->m_tEndTime );

						if ( cTime.GetDay() == cTimeEnd.GetDay() &&
							 cTime.GetMonth() == cTimeEnd.GetMonth() &&
							 cTime.GetYear() == cTimeEnd.GetYear() )
						{
							sc::writeLogError( sc::string::format( "DoQuestGenerateReward Daily Time ERROR m_dwGiftQUEST %1%", dwQUEST ) );	
							return;
						}
					}
				}
				else
				{
					//	포기(실패)한 퀘스트를 다시 시도 불가능한 경우.
					if ( !pQUEST->IsAGAIN() )
					{
						sc::writeLogError( sc::string::format( "DoQuestGenerateReward Again ERROR m_dwGiftQUEST %1%", dwQUEST ) );	
						return;
					}
				}
			}

			if ( !CheckQuestProgressEvent( 0, pQUEST ) )
			{
				sc::writeLogError( sc::string::format( "DoQuestGenerateReward Quest Process Event ERROR m_dwGiftQUEST %1%", dwQUEST ) );	
				return;
			}

			//	Note : 가지고 있는 돈의 액수를 조절.
			// 실제로 아무런 처리를 하지 않지만 다른 부분에서 모두 검사하도록 되어있으므로 동일성을 위해서 추가함
			CheckMoneyUpdate( GetInvenMoney(), pQUEST->m_dwBeginMoney, FALSE, "Quest Repeat Start Pay." );

			SubtractInvenMoneyServer( pQUEST->m_dwBeginMoney, SUBTRACT_INVEN_MONEY_QUEST_START );

			if ( pQUEST->m_dwBeginMoney )
			{
				// 금액 변화 로그.
				// 통계 및 로그 수정
				LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -LONGLONG(pQUEST->m_dwBeginMoney), ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_QUEST_START );
				// 통계 및 로그 수정
				LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
			}

			//	Note : 클라이언트에 돈 액수 변화를 알려줌.
			SendInvenMoneyUpdate();
			
			//	Note : 새로이 시작되는 퀘스트 설정.
			//
			GLQUESTPROG sQUEST_PROG_NEW;
			sQUEST_PROG_NEW.START( pQUEST, pQUEST_END );

			//	리스트에 등록.
			m_cQuestPlay.InsertProc( sQUEST_PROG_NEW, true );

			//	Note : 추가된 퀘스트 정보 전송.
			//
			se::ByteStream sByteStream;
			sQUEST_PROG_NEW.SET_BYBUFFER( sByteStream );
			
			LPBYTE pBuff( 0 );
			DWORD dwSize( 0 );
			sByteStream.GetBuffer( pBuff, dwSize );
			
			if( dwSize <= MAX_QUEST_PACKET_SIZE )
			{
				GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew( sQUEST_PROG_NEW.m_sNID.dwID, 0 );
				NetMsgQuestNew.SETSTREAM( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
				// QUEST_BUFFER_OVER
				sc::writeLogDebug( sc::string::format( "DoQuestGenerateReward ( dwSize <= MAX_QUEST_PACKET_SIZE ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgQuestNew.dwQID, NetMsgQuestNew.dwSeq, NetMsgQuestNew.bLastSeq, NetMsgQuestNew.dwLengthStream, NetMsgQuestNew.dwSize ) );	
#endif

				SendToClient( &NetMsgQuestNew );
			}
			else
			{
				int nRepetIdx = ( dwSize / MAX_QUEST_PACKET_SIZE ) + ( ( dwSize % MAX_QUEST_PACKET_SIZE == 0 ) ? 0 : 1 );
				
				for( int i = 0; i < nRepetIdx; i++ )
				{
					GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew( sQUEST_PROG_NEW.m_sNID.dwID, 0 );
					NetMsgQuestNew.dwSeq = i+1;

					if ( i == nRepetIdx-1 )
					{
						NetMsgQuestNew.bLastSeq = TRUE;
						NetMsgQuestNew.SETSTREAM( pBuff + ( i * MAX_QUEST_PACKET_SIZE ), dwSize );
					}
					else
					{
						NetMsgQuestNew.bLastSeq = FALSE;
						NetMsgQuestNew.SETSTREAM( pBuff + ( i * MAX_QUEST_PACKET_SIZE ) , MAX_QUEST_PACKET_SIZE );
						dwSize -= MAX_QUEST_PACKET_SIZE;
					}
#ifdef _TEMP_TRACING_LOG
					// QUEST_BUFFER_OVER
					sc::writeLogDebug( sc::string::format( "DoQuestGenerateReward dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgQuestNew.dwQID, NetMsgQuestNew.dwSeq, NetMsgQuestNew.bLastSeq, NetMsgQuestNew.dwLengthStream, NetMsgQuestNew.dwSize ) );	
#endif
					
					SendToClient( &NetMsgQuestNew );
				}
			}

			//	만약 종전에 진행한 정보 있을때 삭제.
			if ( pQUEST_END )
			{
				//	Note : 종료된 퀘스트 리스트에서 퀘스트 정보 삭제.
				//
				m_cQuestPlay.DeleteEnd( dwQUEST );

				//	Note : 삭제된 퀘스트 정보 전송.
				//
				GLMSG::SNET_QUEST_END_DEL NetMsgDel( dwQUEST );
				SendToClient( &NetMsgDel );
			}

		//} while(0);
	}
}

void GLChar::DoQuestProgMob( DWORD dwMOBID, bool bMySelf, bool bPlayerKill )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	bool bPROG(false);
	bool bMOBKILL(false);
	bool bMOBGEN(false);

	GLQUESTPROG* pPROG = NULL;
	GLQUEST* pQUEST = NULL;
	GLQUEST_STEP* pSTEP = NULL;

	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestPlay::MAPQUEST &mapPROC = m_cQuestPlay.GetQuestProc();
	GLQuestPlay::MAPQUEST_ITER iter = mapPROC.begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = mapPROC.end();
	for ( ; iter!=iter_end; ++iter )
	{
		pPROG = (*iter).second;
		pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
		if( !pQUEST )
			continue;

		//자기자신이 아닐경우엔 파티퀘스트만 점검한다
		if( !bMySelf && !pQUEST->IsPARTYQUEST() )
			continue;
		
		pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
		if ( !pSTEP )
			continue;

		//지정된 위치에서 잡아야 하는 퀘스트 이면
		bool bIsReachZone = false;
		if (pSTEP->IsNEED_REACHZONE())
		{			
			if ( pSTEP->m_sMAPID_REACH == GetCurrentMap().getBaseMapID() )
			{
				bIsReachZone = true;
			}
		}
		else
		{
			bIsReachZone = true;
		}

		if ( !pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL )
		{
			if ( ( ( dwMOBID == pSTEP->m_dwNID_MOBKILL || ( pSTEP->m_bIsPLAYERKILL && bPlayerKill ) ) && bIsReachZone  ) )
			{
				//	kill 수 증가.
				bMOBKILL = true;
				bPROG = true;

				++pPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL;

				//	충족하였는지 검사.
				if ( pPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL >= pSTEP->m_dwNUM_MOBKILL )
				{
					pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL = true;
				}

				GLMSG::SNET_QUEST_PROG_MOBKILL NetMsgMobKill(
                    pPROG->m_sNID.dwID, 
                    pPROG->m_dwSTEP,
                    pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL,
                    pPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL);
				NetMsgMobKill.dwChaNum = CharDbNum();

				SendToAgent(&NetMsgMobKill );
			}
		}

		if ( bPROG && 
			!(pQUEST->m_sAttendanceOption.nType >= EMATTENDANCE_TYPE_CONNECTION &&
			pQUEST->m_sAttendanceOption.nType < EMATTENDANCE_TYPE_SIZE) )
		{
			//	Note : 퀘스트 진행.
			//
			bool bPROG = DoQuestProgress ( pPROG );
			if ( bPROG )
			{
				pPROG = NULL;
				break;
			}
		}
	}
}

void GLChar::DoQuestProgQItem( DWORD dwMOBID, bool bMySelf )
{
	HRESULT hr(S_OK);

	bool bPROG(false);
	bool bMOBKILL(false);
	bool bMOBGEN(false);

	GLQUESTPROG* pPROG = NULL;
	GLQUEST* pQUEST = NULL;
	GLQUEST_STEP* pSTEP = NULL;

	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestPlay::MAPQUEST &mapPROC = m_cQuestPlay.GetQuestProc();
	GLQuestPlay::MAPQUEST_ITER iter = mapPROC.begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = mapPROC.end();
	for ( ; iter!=iter_end; ++iter )
	{
		pPROG = (*iter).second;
		pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
		if( !pQUEST )
			continue;

		//자기자신이 아닐경우엔 파티퀘스트만 점검한다
		if( !bMySelf && !pQUEST->IsPARTYQUEST() )
			continue;
		
		pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
		if ( !pSTEP )
			continue;

		if ( !pPROG->m_sSTEP_PROG.m_bQITEM )
		{
			GENMOBITEMARRAY_ITER iter = pSTEP->m_vecMOBGEN_QITEM.begin();
			GENMOBITEMARRAY_ITER iter_end = pSTEP->m_vecMOBGEN_QITEM.end();
			for ( ; iter!=iter_end; ++iter )
			{
				SGENQUESTITEM &sGENQITEM = (*iter);

				bool bFOUND = sGENQITEM.Find ( dwMOBID );
				if ( !bFOUND )
					continue;

				bMOBGEN = true;

				//	이미 해당 아이템이 모두 발생했는지 검사.
				DWORD dwNUM = pPROG->m_sINVENTORY.CountTurnItem( sGENQITEM.sNID );

				if ( dwNUM >= sGENQITEM.wNUM )
					continue;

				//	발생아이템 (확율)로 발생.
                if (sc::Random::getInstance().IsSucceed(sGENQITEM.fGEN_RATE))
				{
					//	퀘스트 인벤토리에 넣어줌.
					SITEMCUSTOM sCUSTOM( sGENQITEM.sNID );
					sc::SeqUniqueGUID::Instance()->getGUID( sCUSTOM.guid );
					sCUSTOM.GenerateBasicStat( false );
					sCUSTOM.GenerateLinkSkill();
					sCUSTOM.GenerateAddOption();

					sCUSTOM.SetDbState( db::DB_INSERT );
					sCUSTOM.nidDISGUISE.wMainID = pPROG->m_sNID.wMainID;
					sCUSTOM.nidDISGUISE.wSubID = pPROG->m_sNID.wSubID;
					
					hr = InsertToQuestInven( m_ClientSlot, pPROG->m_sNID.dwID, pPROG->m_sINVENTORY, sCUSTOM );
					if ( FAILED( hr ) )
					{
						//	퀘스트 인벤에 넣기가 실패함.
					}
					else
					{
						bPROG = true;

						//	습득을 클라이언트에 알림.
						GLMSG::SNETPC_QUEST_PROG_INVEN_PICKUP NetMsgPickUp( (SNATIVEID)sCUSTOM.GetNativeID() );
						SendToClient(&NetMsgPickUp);
					}
				}
			}

			if ( bMOBGEN )
			{
				bool bGEMITEM_COMPLETE(true);

				GLInventory sTEMP_INVEN;
				sTEMP_INVEN.Assign( pPROG->m_sINVENTORY );

				GENMOBITEMARRAY_ITER iter = pSTEP->m_vecMOBGEN_QITEM.begin();
				GENMOBITEMARRAY_ITER iter_end = pSTEP->m_vecMOBGEN_QITEM.end();
				for ( ; iter!=iter_end; ++iter )
				{
					SGENQUESTITEM &sGENQITEM = (*iter);

					if ( sGENQITEM.wNUM > 0 )
					{
						DWORD dwNUM = sTEMP_INVEN.CountTurnItem( sGENQITEM.sNID );
						if ( dwNUM<sGENQITEM.wNUM )
						{
							bGEMITEM_COMPLETE = false;
							break;
						}
					}
					else
					{
						SINVENITEM* pINVENITEM = sTEMP_INVEN.FindItem( sGENQITEM.sNID );
						if ( !pINVENITEM )
						{
							bGEMITEM_COMPLETE = false;
							break;
						}

						sTEMP_INVEN.DeleteItem( pINVENITEM->wPosX, pINVENITEM->wPosY );
					}
				}

				if ( bGEMITEM_COMPLETE )
				{
					pPROG->m_sSTEP_PROG.m_bQITEM = true;

					GLMSG::SNET_QUEST_PROG_QITEM NetMsg(pPROG->m_sNID.dwID, pPROG->m_dwSTEP, pPROG->m_sSTEP_PROG.m_bQITEM);
					SendToClient( &NetMsg );
				}
			}
		}

		if ( bPROG && 
			!(pQUEST->m_sAttendanceOption.nType >= EMATTENDANCE_TYPE_CONNECTION &&
			pQUEST->m_sAttendanceOption.nType < EMATTENDANCE_TYPE_SIZE) )
		{
			//	Note : 퀘스트 진행.
			//
			bool bPROG = DoQuestProgress( pPROG );
			if ( bPROG )
			{
				pPROG = NULL;
				break;
			}
		}
	}
}

void GLChar::DoQuestMobKill( PGLCHAR pKillChar, SNATIVEID nidMOB, bool bMySelf, bool bPlayerKill )
{
	if( !pKillChar )
		return;

	if ( !pKillChar->IsValidBody() )
		return;
	if ( pKillChar->GetCurrentMap() != GetCurrentMap() )
		return;

	bool bIsCONFRONTING = pKillChar->GETCONFTING()->IsCONFRONTING();
	if ( bPlayerKill && bIsCONFRONTING )
		return;
	
	D3DXVECTOR3 vDis = GetPosition() - pKillChar->GetPosition();
	float fDis = D3DXVec3Length ( &vDis );
	if ( fDis > GLCONST_CHAR::fPARTYEXPRANGE )
		return;

	if ( !bPlayerKill )
	{
		// 해당 몹이 퀘스트 몹이냐?
		GLQuestPlay::QUESTMOB_MAP &mapPROC = m_cQuestPlay.GetQuestMobProc();
		GLQuestPlay::QUESTMOB_MAP_ITER iter = mapPROC.find( nidMOB.dwID );
		if( iter == mapPROC.end() )
			return;
	}

	DoQuestProgMob( nidMOB.dwID, bMySelf, bPlayerKill );
	DoQuestProgQItem( nidMOB.dwID, bMySelf );
}

void GLChar::DoQuestProgItemUse( DWORD dwITEMID )
{
	bool bPROG(false);
	bool bITEMUSE(false);

	GLQUESTPROG* pPROG = NULL;
	GLQUEST* pQUEST = NULL;
	GLQUEST_STEP* pSTEP = NULL;

	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestPlay::MAPQUEST &mapPROC = m_cQuestPlay.GetQuestProc();
	GLQuestPlay::MAPQUEST_ITER iter = mapPROC.begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = mapPROC.end();
	for ( ; iter!=iter_end; ++iter )
	{
		pPROG = (*iter).second;
		pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
		if( !pQUEST )
			continue;

		pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
		if ( !pSTEP )
			continue;

		if ( !pPROG->m_sSTEP_PROG.m_bITEMUSE_ALL )
		{
			if ( dwITEMID == pSTEP->m_dwNID_ITEMUSE )
			{
				//	아이템 사용 개수 증가.
				bITEMUSE = true;
				bPROG = true;

				++pPROG->m_sSTEP_PROG.m_dwNUM_ITEMUSE;

				//	충족하였는지 검사.
				if ( pPROG->m_sSTEP_PROG.m_dwNUM_ITEMUSE >= pSTEP->m_dwNUM_ITEMUSE )
				{
					pPROG->m_sSTEP_PROG.m_bITEMUSE_ALL = true;
				}

				GLMSG::SNET_QUEST_PROG_ITEMUSE NetMsgItemUse(
					pPROG->m_sNID.dwID, 
					pPROG->m_dwSTEP,
					pPROG->m_sSTEP_PROG.m_bITEMUSE_ALL,
					pPROG->m_sSTEP_PROG.m_dwNUM_ITEMUSE);
				NetMsgItemUse.dwChaNum = CharDbNum();

				SendToAgent(&NetMsgItemUse );
			}
		}

		if ( bPROG && 
			!(pQUEST->m_sAttendanceOption.nType >= EMATTENDANCE_TYPE_CONNECTION &&
			pQUEST->m_sAttendanceOption.nType < EMATTENDANCE_TYPE_SIZE) )
		{
			//	Note : 퀘스트 진행.
			//
			bool bPROG = DoQuestProgress ( pPROG );
			if ( bPROG )
			{
				pPROG = NULL;
				break;
			}
		}
	}
}

void GLChar::DoQuestItemUse( SNATIVEID nidITEM )
{
	// 해당 아이템이 퀘스트 아이템 이냐
	GLQuestPlay::QUESTMOB_MAP &mapPROC = m_cQuestPlay.GetQuestItemUseProc();
	GLQuestPlay::QUESTMOB_MAP_ITER iter = mapPROC.find( nidITEM.dwID );
	if( iter == mapPROC.end() )
		return;

	DoQuestProgItemUse( nidITEM.dwID );
	//DoQuestProgQItem( nidMOB.dwID, bMySelf );
}

void GLChar::DoQuestItemGrinding( SNATIVEID nidITEM, EMGRINDING_CLASS emGrindingClass, bool IsSuccess  )
{
	//// 해당 아이템이 퀘스트 아이템 이냐
	//GLQuestPlay::QUESTMOB_MAP &mapPROC = m_cQuestPlay.GetQuestItemGrindingProc();
	//GLQuestPlay::QUESTMOB_MAP_ITER iter = mapPROC.find( nidITEM.dwID );
	//if( iter == mapPROC.end() )
	//	return;

	DoQuestProgItemGrinding( nidITEM.dwID, emGrindingClass, IsSuccess );
	//DoQuestProgQItem( nidMOB.dwID, bMySelf );
}

void GLChar::DoQuestProgItemGrinding( DWORD dwITEMID, EMGRINDING_CLASS emGrindingClass, bool IsSuccess )
{
	bool bPROG(false);
	bool ITEMGRINDING(false);

	GLQUESTPROG* pPROG = NULL;
	GLQUEST* pQUEST = NULL;
	GLQUEST_STEP* pSTEP = NULL;

	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestPlay::MAPQUEST &mapPROC = m_cQuestPlay.GetQuestProc();
	GLQuestPlay::MAPQUEST_ITER iter = mapPROC.begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = mapPROC.end();
	for ( ; iter!=iter_end; ++iter )
	{
		pPROG = (*iter).second;
		pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
		if( !pQUEST )
			continue;

		pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
		if ( !pSTEP )
			continue;

		bool bIsItemTypeCheck = false;

		switch(pSTEP->m_dwItemType)
		{
		case EMGRINDING_NONE:
			{
				bIsItemTypeCheck = false;
			}
			break;

		case EMGRINDING_WEAPON:
			{
				if( emGrindingClass == EMGRINDING_CLASS_ARM)
					bIsItemTypeCheck = true;
			}
			break;

		case EMGRINDING_ARMOR:
			{
				if( emGrindingClass == EMGRINDING_CLASS_CLOTH)
					bIsItemTypeCheck = true;
			}
			break;

		case EMGRINDING_ALL:
			{
				bIsItemTypeCheck = true;
			}
			break;
		}

		if ( !pPROG->m_sSTEP_PROG.m_bITEMGRINDING_ALL )
		{
			if ( dwITEMID == pSTEP->m_dwNID_ITEMGRINDING || bIsItemTypeCheck )
			{
				//연속 연마가 아니고 연마 실패이면 넘어간다
				if( !pSTEP->m_bConsecution && !IsSuccess )
					continue;

				//	아이템 연마 개수 증가.
				ITEMGRINDING = true;
				bPROG = true;

				++pPROG->m_sSTEP_PROG.m_dwNUM_ITEMGRINDING;

				// 연속 연마이고 연마 실패이면 리셋
				if ( pSTEP->m_bConsecution && !IsSuccess)
					pPROG->m_sSTEP_PROG.m_dwNUM_ITEMGRINDING = 0;

				//	충족하였는지 검사.
				if ( pPROG->m_sSTEP_PROG.m_dwNUM_ITEMGRINDING >= pSTEP->m_dwNUM_ITEMGRINDING )
				{
					pPROG->m_sSTEP_PROG.m_bITEMGRINDING_ALL = true;
				}

				GLMSG::SNET_QUEST_PROG_ITEMGRINDING NetMsgItemUse(
					pPROG->m_sNID.dwID, 
					pPROG->m_dwSTEP,
					pPROG->m_sSTEP_PROG.m_bITEMGRINDING_ALL,
					pPROG->m_sSTEP_PROG.m_dwNUM_ITEMGRINDING);
				NetMsgItemUse.dwChaNum = CharDbNum();

				SendToAgent(&NetMsgItemUse );
			}
		}

		if ( bPROG && 
			!(pQUEST->m_sAttendanceOption.nType >= EMATTENDANCE_TYPE_CONNECTION &&
			pQUEST->m_sAttendanceOption.nType < EMATTENDANCE_TYPE_SIZE)  )
		{
			//	Note : 퀘스트 진행.
			//
			bool bPROG = DoQuestProgress ( pPROG );
			if ( bPROG )
			{
				pPROG = NULL;
				break;
			}
		}
	}
}
void GLChar::DoQuestLevelUp()
{
	bool bPROG(false);

	GLQUESTPROG* pPROG = NULL;
	GLQUEST* pQUEST = NULL;
	GLQUEST_STEP* pSTEP = NULL;

	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestPlay::MAPQUEST &mapPROC = m_cQuestPlay.GetQuestProc();
	GLQuestPlay::MAPQUEST_ITER iter = mapPROC.begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = mapPROC.end();
	for ( ; iter!=iter_end; ++iter )
	{
		pPROG = (*iter).second;
		pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
		if( !pQUEST )
			continue;

		pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
		if ( !pSTEP )
			continue;

		if ( !pPROG->m_sSTEP_PROG.m_bLEVELUP_ALL )
		{
			//	랩업해야할 수 증가
			bPROG = true;

			++pPROG->m_sSTEP_PROG.m_dwNUM_LEVELUP;

			//	충족하였는지 검사.
			if ( pPROG->m_sSTEP_PROG.m_dwNUM_LEVELUP >= pSTEP->m_dwNUM_LEVELUP )
			{
				pPROG->m_sSTEP_PROG.m_bLEVELUP_ALL = true;
			}

			GLMSG::SNET_QUEST_PROG_LEVEL_UP NetMsgItemUse(
				pPROG->m_sNID.dwID, 
				pPROG->m_dwSTEP,
				pPROG->m_sSTEP_PROG.m_bLEVELUP_ALL,
				pPROG->m_sSTEP_PROG.m_dwNUM_LEVELUP);
			NetMsgItemUse.dwChaNum = CharDbNum();

			SendToAgent(&NetMsgItemUse );
		}

		if ( bPROG && 
			!(pQUEST->m_sAttendanceOption.nType >= EMATTENDANCE_TYPE_CONNECTION &&
			pQUEST->m_sAttendanceOption.nType < EMATTENDANCE_TYPE_SIZE)  )
		{
			//	Note : 퀘스트 진행.
			//
			bool bPROG = DoQuestProgress ( pPROG );
			if ( bPROG )
			{
				pPROG = NULL;
				break;
			}
		}
	}
}

void GLChar::DoQuestReachZone ()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestPlay::MAPQUEST &mapPROC = m_cQuestPlay.GetQuestProc();
	GLQuestPlay::MAPQUEST_ITER iter = mapPROC.begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = mapPROC.end();
	for ( ; iter!=iter_end; ++iter )
	{
		GLQUESTPROG* pPROG = (*iter).second;
		if ( !pPROG )
			continue;
		if ( pPROG->m_sSTEP_PROG.m_bREACH_ZONE )
			continue;

		GLQUEST *pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
		if ( !pQUEST )
			continue;

		GLQUEST_STEP* pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
		if ( !pSTEP )
			continue;
		if (pSTEP->m_sMAPID_REACH != GetCurrentMap().getBaseMapID())
			continue;

		int nPosX(0);
		int nPosY(0);
		pLand->GetMapAxisInfo().Convert2MapPos ( m_vPosition.x, m_vPosition.z, nPosX, nPosY );

		WORD wMINX_REACH(0), wMINY_REACH(0);
		WORD wMAXX_REACH(0), wMAXY_REACH(0);
		bool bAllSpace = false;

		if( pSTEP->m_wPOSX_REACH == 0 &&
			pSTEP->m_wPOSY_REACH == 0 &&
			pSTEP->m_wRADIUS_REACH == 0 )
		{
			bAllSpace = true;
		}

		if ( pSTEP->m_wPOSX_REACH > pSTEP->m_wRADIUS_REACH )
			wMINX_REACH = pSTEP->m_wPOSX_REACH - pSTEP->m_wRADIUS_REACH;
		else
			wMINX_REACH = 0;

		if ( pSTEP->m_wPOSY_REACH > pSTEP->m_wRADIUS_REACH )
			wMINY_REACH = pSTEP->m_wPOSY_REACH - pSTEP->m_wRADIUS_REACH;
		else
			wMINY_REACH = 0;

		wMAXX_REACH = pSTEP->m_wPOSX_REACH + pSTEP->m_wRADIUS_REACH;
		wMAXY_REACH = pSTEP->m_wPOSY_REACH + pSTEP->m_wRADIUS_REACH;

		if ( ( wMINX_REACH < nPosX &&
			   nPosX < wMAXX_REACH &&
			   wMINY_REACH < nPosY && 
			   nPosY < wMAXY_REACH ) || 
			   bAllSpace )
		{
			pPROG->m_sSTEP_PROG.m_bREACH_ZONE = true;
			
			GLMSG::SNET_QUEST_PROG_REACHZONE NetMsg(pPROG->m_sNID.dwID, pPROG->m_dwSTEP, pPROG->m_sSTEP_PROG.m_bREACH_ZONE);
			SendToClient( &NetMsg );

			//	Note : 퀘스트 진행.
			//
			if( pQUEST->m_sAttendanceOption.nType >= EMATTENDANCE_TYPE_CONNECTION &&
				pQUEST->m_sAttendanceOption.nType < EMATTENDANCE_TYPE_SIZE )
				break;

			bool bPROG = DoQuestProgress ( pPROG );
			if ( bPROG )
			{
				pPROG = NULL;
				break;
			}
		}
	}
}

void GLChar::DoQuestConfting( EMCONFT_TYPE emConftType, bool bSCHOOL )
{
	bool bPROG(false);

	GLQUESTPROG* pPROG = NULL;
	GLQUEST* pQUEST = NULL;
	GLQUEST_STEP* pSTEP = NULL;

	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestPlay::MAPQUEST &mapPROC = m_cQuestPlay.GetQuestProc();
	GLQuestPlay::MAPQUEST_ITER iter = mapPROC.begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = mapPROC.end();
	for ( ; iter!=iter_end; ++iter )
	{
		pPROG = (*iter).second;
		pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
		if( !pQUEST )
			continue;

		pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
		if ( !pSTEP )
			continue;


		if ( !pPROG->m_sSTEP_PROG.m_bCONFTING_ALL)
		{
			if ( emConftType == pSTEP->m_dwType_CONFTING && pSTEP->m_bSCHOOL == bSCHOOL )
			{
				//	대련 횟수 증가
				bPROG = true;

				++pPROG->m_sSTEP_PROG.m_dwNUM_CONFTING;

				//	충족하였는지 검사.
				if ( pPROG->m_sSTEP_PROG.m_dwNUM_CONFTING >= pSTEP->m_dwNUM_CONFTING )
				{
					pPROG->m_sSTEP_PROG.m_bCONFTING_ALL = true;
				}

				GLMSG::SNET_QUEST_PROG_CONFTING NetMsgItemUse(
					pPROG->m_sNID.dwID, 
					pPROG->m_dwSTEP,
					pPROG->m_sSTEP_PROG.m_bCONFTING_ALL,
					pPROG->m_sSTEP_PROG.m_dwNUM_CONFTING);
				NetMsgItemUse.dwChaNum = CharDbNum();

				SendToAgent(&NetMsgItemUse );
			}
		}

		if ( bPROG && 
			!(pQUEST->m_sAttendanceOption.nType >= EMATTENDANCE_TYPE_CONNECTION &&
			pQUEST->m_sAttendanceOption.nType < EMATTENDANCE_TYPE_SIZE) )
		{
			//	Note : 퀘스트 진행.
			//
			bool bPROG = DoQuestProgress ( pPROG );
			if ( bPROG )
			{
				pPROG = NULL;
				break;
			}
		}
	}
}

inline bool GLChar::IsInsertToQuestInven ( GLInventory &_sINVENTORY, SITEMCUSTOM &sCUSTOM )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sCUSTOM.GetNativeID() );
	if ( !pItem )
		return false;

	if ( pItem->ISPILE() )
	{
		//	겹침 아이템일 경우.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	넣기 요청된 아이템수. ( 잔여량. )
		WORD wREQINSRTNUM = ( sCUSTOM.wTurnNum );

		BOOL bITEM_SPACE = _sINVENTORY.ValidPileInsrt( wREQINSRTNUM, sNID, wPILENUM );
		if ( !bITEM_SPACE )	
			return false;
	}
	else
	{
		WORD wPosX, wPosY;
		BOOL bOk = _sINVENTORY.FindInsertable( wPosX, wPosY );
		if ( !bOk )	
			return false;
	}

	return true;
}

HRESULT GLChar::InsertToQuestInven( DWORD dwClientID, DWORD dwQID, GLInventory &_sINVENTORY, SITEMCUSTOM &sCUSTOM )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sCUSTOM.GetNativeID() );
	if ( !pItem )
		return E_FAIL;

	bool bITEM_SPACE = IsInsertToQuestInven( _sINVENTORY, sCUSTOM );
	if ( !bITEM_SPACE )
	{
		//	인밴이 가득차서 아이탬을 넣을수 없습니다.
		return E_FAIL;
	}
	
	if ( pItem->ISPILE() )
	{
		//	겹침 아이템일 경우.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	넣기 요청된 아이템수. ( 잔여량. )
		WORD wREQINSRTNUM = ( sCUSTOM.wTurnNum );

		//	Note : 더 겹쳐질수 있는 아이템이 있는지 검사하여 인벤에 들어가야하는
		//		아이템 점검 숫자를 감소시킴.
		const GLInventory::CELL_MAP &ItemMap = *_sINVENTORY.GetItemList();
		GLInventory::CELL_MAP_CITER iter = ItemMap.begin();
		for ( ; iter!=ItemMap.end(); ++iter )
		{
			SINVENITEM &sINVENITEM = *(*iter).second;
			SITEMCUSTOM &sITEMCUSTOM = sINVENITEM.sItemCustom;
			if ( sITEMCUSTOM.GetNativeID() != sNID )
				continue;
			if ( sITEMCUSTOM.wTurnNum>=wPILENUM )
				continue;
		
			//	기존 아이템 겹침 여유분.
			WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;

			if ( wREQINSRTNUM > wSURPLUSNUM )
			{
				//	Note : 기존 아이템의 겹침을 변경. ( 꽉참 )
				sITEMCUSTOM.wTurnNum = wPILENUM;

				//	Note : 클라이언트에 갯수 변경을 알림.
				GLMSG::SNETPC_QUEST_PROG_INVEN_TURNNUM NetMsg( dwQID, sINVENITEM.wPosX, sINVENITEM.wPosY, sITEMCUSTOM.wTurnNum );
				SendToClient( &NetMsg );

				//	Note : 잔여 수량 감소.
				wREQINSRTNUM -= wSURPLUSNUM;

				// #item
				// 남은 수량 디비 업데이트 테스트 용도
				{
					if ( db::DB_NONE != sITEMCUSTOM.GetDbState() && db::DB_INSERT != sITEMCUSTOM.GetDbState() )
					{
						AddGameAdoJob(
							db::DbActionPtr( 
							new db::ItemUpdateTurnNum( sITEMCUSTOM.GetGuid(), sITEMCUSTOM.wTurnNum, 19771129 ) ) );
					}
				}
			}
			else
			{
				//	Note : 기존 아이템에 겹치는 것 만으로도 새로 넣을 아이템이 충분히 들어감.
				sITEMCUSTOM.wTurnNum += wREQINSRTNUM;

				//	Note : 클라이언트에 갯수 변경을 알림.
				GLMSG::SNETPC_QUEST_PROG_INVEN_TURNNUM NetMsg( dwQID, sINVENITEM.wPosX, sINVENITEM.wPosY, sITEMCUSTOM.wTurnNum );
				SendToClient( dwClientID, &NetMsg );

				// 남은 수량 디비 업데이트 테스트 용도
				{
					if ( db::DB_NONE != sITEMCUSTOM.GetDbState() && db::DB_INSERT != sITEMCUSTOM.GetDbState() )
					{
						AddGameAdoJob(
							db::DbActionPtr( 
							new db::ItemUpdateTurnNum( sITEMCUSTOM.GetGuid(), sITEMCUSTOM.wTurnNum, 19771129 ) ) );
					}
				}

				return S_OK;
			}
		}

		//	Note : 새로이 인벤에 들어가야할 아이템의 갯수 파악후 인벤에 들어갈 공간이 있는지 검사.

		//	최대겹침아이템 수량.
		WORD wONENUM = wREQINSRTNUM / wPILENUM;
		WORD wITEMNUM = wONENUM;

		//	여분겹침아이템의 겹침수.
		WORD wSPLITNUM = wREQINSRTNUM % wPILENUM;
		if ( wSPLITNUM > 0 )				wITEMNUM += 1;
		if ( wSPLITNUM==0 && wITEMNUM>=1 )	wSPLITNUM = wPILENUM;

		for ( WORD i=0; i<wITEMNUM; ++i )
		{
			WORD wInsertPosX( 0 ), wInsertPosY( 0 );
			BOOL bSPACE = _sINVENTORY.FindInsertable( wInsertPosX, wInsertPosY );
						
			if ( !bSPACE )
			{
				sc::writeLogError( sc::string::format( "Char(%1%) inventory - 넣을 공간을 미리 체크를 하고 아이템을 넣었으나 공간이 부족함7", CharDbNum() ) );
				return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.
			}

			//	Note : 새로운 아이템을 넣어줌.
			//
			SITEMCUSTOM sITEMCUSTOM = sCUSTOM;
			if ( wITEMNUM==(i+1) )	sITEMCUSTOM.wTurnNum = wSPLITNUM;	//	마지막 아이템은 잔여량.
			else					sITEMCUSTOM.wTurnNum = wPILENUM;	//	아닐 경우는 꽉찬량.

			if ( i > 0 )
			{
				// 신규 guid 생성 필요
				// 이 경우가 되면 위에서 남긴 아이템 로그말고 새로운 아이템이 생성된거다. 
				sc::SeqUniqueGUID::Instance()->getGUID( sITEMCUSTOM.guid );
				sITEMCUSTOM.GenerateBasicStat( false );
				sITEMCUSTOM.GenerateLinkSkill();
				sITEMCUSTOM.GenerateAddOption();
			}

			//	인밴토리에 넣습니다.
			_sINVENTORY.InsertItem( sITEMCUSTOM, wInsertPosX, wInsertPosY );
			SINVENITEM *pInvenItem = _sINVENTORY.GetItem( wInsertPosX, wInsertPosY );
			if ( !pInvenItem )
				return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

			{
				AddGameAdoJob(
					db::DbActionPtr( 
					new db::ItemInsert( m_CharDbNum, INVEN_QUEST_PROC, pInvenItem->sSaveData, 19771130 ) ) );
				pInvenItem->SetDbState( db::DB_UPDATE, true );
			}

			//	인밴에 아이탬 넣어주는 메시지.
			GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT NetMsgInven( dwQID, *pInvenItem );
			SendToClient( &NetMsgInven );
		}
	}
	else
	{
		WORD wPosX, wPosY;
		BOOL bOk = _sINVENTORY.FindInsertable( wPosX, wPosY );
		if ( !bOk )
			return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

		//	Note : 아이탬 넣기.
		_sINVENTORY.InsertItem( sCUSTOM, wPosX, wPosY );
		SINVENITEM *pInvenItem = _sINVENTORY.GetItem( wPosX, wPosY );
		if ( !pInvenItem )
			return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

		{
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemInsert( m_CharDbNum, INVEN_QUEST_PROC, pInvenItem->sSaveData, 19771131 ) ) );
			pInvenItem->SetDbState( db::DB_UPDATE, true );
		}

		//	[자신에게] 메시지 발생.
		GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT NetMsg_Inven( dwQID, *pInvenItem );
		SendToClient( &NetMsg_Inven );
	}

	return S_OK;
}

void GLChar::MsgSendQuestTime ()
{
	GLQUESTPROG* pQuestProg = NULL;

	GLQuestPlay::MAPQUEST_ITER iter_cur;
	GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
	for ( ; iter!=iter_end;  )
	{
		iter_cur = iter++;

		pQuestProg = (*iter_cur).second;

		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( pQuestProg->m_sNID.dwID );
		if ( !pQUEST )
			continue;

		if ( pQUEST->m_dwLimitTime<=0 )
			continue;

		GLMSG::SNETPC_QUEST_PROG_TIME NetMsgTime(pQuestProg->m_sNID.dwID, pQuestProg->m_fLAPSTIME);
		SendToClient(&NetMsgTime);
	}
}

void GLChar::DoQuestCheckLimitTime ( float fTime, float fElapsedTime )
{
	m_fQUEST_TIMER += fElapsedTime;

	bool bTIME_UPDATE(false);
	if ( m_fQUEST_TIMER > 60.0f )
	{
		bTIME_UPDATE = true;
		m_fQUEST_TIMER = 0;
	}

	if ( !bTIME_UPDATE )
		return;

	GLMSG::SNETPC_QUEST_PROG_TIME NetMsgTime(0,0);
	GLQUESTPROG* pQuestProg = NULL;
	DWORD dwQID = 0;
	bool bOVER = false;

	m_cQuestPlay.FrameMove ( fTime, fElapsedTime );

	GLQuestPlay::MAPQUEST_ITER iter_cur;
	GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
	for ( ; iter!=iter_end;  )
	{
		iter_cur = iter++;

		pQuestProg = (*iter_cur).second;
		dwQID = pQuestProg->m_sNID.dwID;

		if ( pQuestProg->m_bCOMPLETE )
			continue;

		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQID  );
		if ( !pQUEST )
			continue;

		if ( pQUEST->m_dwLimitTime<=0 )
			continue;

		NetMsgTime.dwQID = dwQID;
		NetMsgTime.fELAPS = pQuestProg->m_fLAPSTIME;
		SendToClient(&NetMsgTime);
        
		bOVER = pQuestProg->CheckTIMEOVER();

		bool bIsComplete = false;

		if ( bOVER )
		{
			//완료된 퀘스트 인지 확인
			std::vector< SINVENITEM > vecDeleted;
			bIsComplete = pQuestProg->DoCOMPLETE( vecDeleted, m_bTracingUser );

			size_t DeletedSize = vecDeleted.size();
			for ( size_t loop = 0; loop < DeletedSize; loop++ )
			{
				SINVENITEM& sItem = vecDeleted[loop];
				AddGameAdoJob(
					db::DbActionPtr( 
					new db::ItemDelete( sItem.GetGuid(), sItem.GetDbState(), 19787 ) ) );
			}

			//	시간 초과를 알림.
			GLMSG::SNETPC_QUEST_PROG_TIMEOVER NetMsg(dwQID);
			SendToClient( &NetMsg );

			if (!bIsComplete )
			{
				if( pQUEST->m_ProgressEvent.bUseProgressEvent && pQUEST->m_ProgressEvent.IsTIMEOVER() )
					CheckQuestProgressEvent( 2, pQUEST );

				// DoGIVEUP 호출전에 디비에서 퀘스트 아이템 삭제한다.
				GLInventory::CELL_MAP_CITER iter = pQuestProg->m_sINVENTORY.GetItemList()->begin();
				GLInventory::CELL_MAP_CITER iter_end = pQuestProg->m_sINVENTORY.GetItemList()->end();
				for ( ; iter!=iter_end; ++iter )
				{
					SINVENITEM* pInvenItem = (*iter).second;
					if ( pInvenItem )
					{
						AddGameAdoJob(
							db::DbActionPtr( 
							new db::ItemDelete( pInvenItem->GetGuid(), pInvenItem->GetDbState(), 89127 ) ) );
					}
				}

				//	자동 포기 처리.
				pQuestProg->DoGIVEUP ();

				//	완료된 리스트에 추가.
				m_cQuestPlay.InsertEnd(*pQuestProg, false);

				se::ByteStream cByteStream;
				pQuestProg->SET_BYBUFFER ( cByteStream );

				LPBYTE pBuff(NULL);
				DWORD dwSize(0);
				cByteStream.GetBuffer ( pBuff, dwSize );

				//	진행정보 삭제.
				m_cQuestPlay.DeleteProc ( dwQID );

				//	Note : 퀘스트 완료 정보 전송.
				//
				if( dwSize <= MAX_QUEST_PACKET_SIZE )
				{
					GLMSG::SNET_QUEST_END_STREAM NetMsgStream(pQuestProg->m_sNID.dwID, 0);
					NetMsgStream.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
					// QUEST_BUFFER_OVER
					sc::writeLogDebug(sc::string::format("DoQuestCheckLimitTime ( dwSize <= MAX_QUEST_PACKET_SIZE ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgStream.dwQID, NetMsgStream.dwSeq, NetMsgStream.bLastSeq, NetMsgStream.dwLengthStream, NetMsgStream.dwSize));	
#endif

					SendToClient(&NetMsgStream );
				}
				else
				{
					int nRepetIdx = (dwSize / MAX_QUEST_PACKET_SIZE) + ((dwSize%MAX_QUEST_PACKET_SIZE == 0) ? 0 : 1);
					
					for( int i = 0; i < nRepetIdx; i++ )
					{
						GLMSG::SNET_QUEST_END_STREAM NetMsgStream(pQuestProg->m_sNID.dwID, 0);
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
						sc::writeLogDebug(sc::string::format("DoQuestCheckLimitTime dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgStream.dwQID, NetMsgStream.dwSeq, NetMsgStream.bLastSeq, NetMsgStream.dwLengthStream, NetMsgStream.dwSize));	
#endif

						SendToClient(&NetMsgStream );
					}
				}

				//	Note : 퀘스트 진행 정보 삭제 전송.
				//
				GLMSG::SNET_QUEST_PROG_DEL NetMsgDel(dwQID);
				SendToClient(&NetMsgDel );
			}
		}
	}
	
	return;
}

void GLChar::DoQuestCheckLeaveMap ()
{
	GLQUESTPROG* pQuestProg = NULL;
	DWORD dwQID = 0;
	bool bNON = false;

	GLQuestPlay::MAPQUEST_ITER iter_cur;
	GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
	for ( ; iter!=iter_end;  )
	{
		iter_cur = iter++;

		pQuestProg = (*iter_cur).second;
		dwQID = pQuestProg->m_sNID.dwID;

		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQID  );
		if ( !pQUEST )
			continue;

		if( !pQUEST->m_ProgressEvent.bUseProgressEvent )
			continue;
		if( pQUEST->m_ProgressEvent.dwStartProgress != EMQP_SELECT_MOVE || !pQUEST->m_ProgressEvent.IsLEAVE() )
			continue;

		if( pQUEST->m_ProgressEvent.startMap.nidMAP != m_mapID.getBaseMapID() )
		{
			//	사망하여 퀘스트 조건에 걸림을 알림.
			GLMSG::SNETPC_QUEST_PROG_LEAVEMAP NetMsg(dwQID);
			SendToClient( &NetMsg );

			CheckQuestProgressEvent( 2, pQUEST );

			// DoGIVEUP 호출전에 디비에서 퀘스트 아이템 삭제한다.
			GLInventory::CELL_MAP_CITER iter = pQuestProg->m_sINVENTORY.GetItemList()->begin();
			GLInventory::CELL_MAP_CITER iter_end = pQuestProg->m_sINVENTORY.GetItemList()->end();
			for ( ; iter!=iter_end; ++iter )
			{
				SINVENITEM* pInvenItem = (*iter).second;
				if ( pInvenItem )
				{
					AddGameAdoJob(
						db::DbActionPtr( 
						new db::ItemDelete( pInvenItem->GetGuid(), pInvenItem->GetDbState(), 89128 ) ) );
				}
			}

			//	자동 포기 처리.
			pQuestProg->DoGIVEUP ();

			//	완료된 리스트에 추가.
			m_cQuestPlay.InsertEnd(*pQuestProg, false);

			se::ByteStream cByteStream;
			pQuestProg->SET_BYBUFFER ( cByteStream );

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			cByteStream.GetBuffer ( pBuff, dwSize );

			//	진행정보 삭제.
			m_cQuestPlay.DeleteProc ( dwQID );

			//	Note : 퀘스트 완료 정보 전송.
			//
			if( dwSize <= MAX_QUEST_PACKET_SIZE )
			{
				GLMSG::SNET_QUEST_END_STREAM NetMsgStream(pQuestProg->m_sNID.dwID, 0);
				NetMsgStream.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
				// QUEST_BUFFER_OVER
				sc::writeLogDebug(sc::string::format("DoQuestCheckLeaveMap ( dwSize <= MAX_QUEST_PACKET_SIZE ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgStream.dwQID, NetMsgStream.dwSeq, NetMsgStream.bLastSeq, NetMsgStream.dwLengthStream, NetMsgStream.dwSize));	
#endif

				SendToClient(&NetMsgStream );
			}
			else
			{
				int nRepetIdx = (dwSize / MAX_QUEST_PACKET_SIZE) + ((dwSize%MAX_QUEST_PACKET_SIZE == 0) ? 0 : 1);
				
				for( int i = 0; i < nRepetIdx; i++ )
				{
					GLMSG::SNET_QUEST_END_STREAM NetMsgStream(pQuestProg->m_sNID.dwID, 0);
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
					sc::writeLogDebug(sc::string::format("DoQuestCheckLeaveMap dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgStream.dwQID, NetMsgStream.dwSeq, NetMsgStream.bLastSeq, NetMsgStream.dwLengthStream, NetMsgStream.dwSize));	
#endif

					SendToClient(&NetMsgStream );
				}
			}

			//	Note : 퀘스트 진행 정보 삭제 전송.
			//
			GLMSG::SNET_QUEST_PROG_DEL NetMsgDel(dwQID);
			SendToClient(&NetMsgDel );
		}
	}

	return;
}


void GLChar::DoQuestCheckLimitDie ()
{
	GLQUESTPROG* pQuestProg = NULL;
	DWORD dwQID = 0;
	bool bNON = false;

	GLQuestPlay::MAPQUEST_ITER iter_cur;
	GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
	for ( ; iter!=iter_end;  )
	{
		iter_cur = iter++;

		pQuestProg = (*iter_cur).second;
		dwQID = pQuestProg->m_sNID.dwID;

		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQID  );
		if ( !pQUEST )
			continue;

		bNON = pQuestProg->IsNonDIE();
		if ( bNON )
		{
			//	사망하여 퀘스트 조건에 걸림을 알림.
			GLMSG::SNETPC_QUEST_PROG_NONDIE NetMsg(dwQID);
			SendToClient( &NetMsg );

			if( pQUEST->m_ProgressEvent.bUseProgressEvent && pQUEST->m_ProgressEvent.IsDIE() )
				CheckQuestProgressEvent( 2, pQUEST );

			// DoGIVEUP 호출전에 디비에서 퀘스트 아이템 삭제한다.
			GLInventory::CELL_MAP_CITER iter = pQuestProg->m_sINVENTORY.GetItemList()->begin();
			GLInventory::CELL_MAP_CITER iter_end = pQuestProg->m_sINVENTORY.GetItemList()->end();
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

			//	자동 포기 처리.
			pQuestProg->DoGIVEUP ();

			//	완료된 리스트에 추가.
			m_cQuestPlay.InsertEnd(*pQuestProg, false);

			se::ByteStream cByteStream;
			pQuestProg->SET_BYBUFFER ( cByteStream );

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			cByteStream.GetBuffer ( pBuff, dwSize );

			//	진행정보 삭제.
			m_cQuestPlay.DeleteProc ( dwQID );

			//	Note : 퀘스트 완료 정보 전송.
			//
			if( dwSize <= MAX_QUEST_PACKET_SIZE )
			{
				GLMSG::SNET_QUEST_END_STREAM NetMsgStream(pQuestProg->m_sNID.dwID, 0);
				NetMsgStream.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
				// QUEST_BUFFER_OVER
				sc::writeLogDebug(sc::string::format("DoQuestCheckLimitDie ( dwSize <= MAX_QUEST_PACKET_SIZE ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgStream.dwQID, NetMsgStream.dwSeq, NetMsgStream.bLastSeq, NetMsgStream.dwLengthStream, NetMsgStream.dwSize));	
#endif

				SendToClient(&NetMsgStream );
			}
			else
			{
				int nRepetIdx = (dwSize / MAX_QUEST_PACKET_SIZE) + ((dwSize%MAX_QUEST_PACKET_SIZE == 0) ? 0 : 1);
				
				for( int i = 0; i < nRepetIdx; i++ )
				{
					GLMSG::SNET_QUEST_END_STREAM NetMsgStream(pQuestProg->m_sNID.dwID, 0);
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
					sc::writeLogDebug(sc::string::format("DoQuestCheckLimitDie dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgStream.dwQID, NetMsgStream.dwSeq, NetMsgStream.bLastSeq, NetMsgStream.dwLengthStream, NetMsgStream.dwSize));	
#endif

					SendToClient(&NetMsgStream );
				}
			}

			//	Note : 퀘스트 진행 정보 삭제 전송.
			//
			GLMSG::SNET_QUEST_PROG_DEL NetMsgDel(dwQID);
			SendToClient(&NetMsgDel );
		}
	}
	
	return;
}

void GLChar::DoQuestLevel ()
{
	GLQUESTPROG* pPROG = NULL;
	DWORD dwQID = 0;
	GLQUEST* pQUEST = NULL;
	GLQUEST_STEP* pSTEP = NULL;

	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	GLQuestPlay::MAPQUEST_ITER iter_cur;
	GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
	for ( ; iter!=iter_end;  )
	{
		iter_cur = iter++;

		pPROG = (*iter_cur).second;
		dwQID = pPROG->m_sNID.dwID;

		if ( pPROG->m_sSTEP_PROG.m_bLevel )	
			continue;

		pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
		if ( !pQUEST )
			continue;

		pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
		if ( !pSTEP )
			continue;

		if ( pSTEP->m_wLevel > m_wLevel )
			continue;

		pPROG->m_sSTEP_PROG.m_bLevel = true;

		GLMSG::SNETPC_QUEST_PROG_LEVEL NetMsg(dwQID);
		SendToClient( &NetMsg );

		//	Note : 퀘스트 진행.
		//
		if (pQUEST->m_sAttendanceOption.nType >= EMATTENDANCE_TYPE_CONNECTION &&
			pQUEST->m_sAttendanceOption.nType < EMATTENDANCE_TYPE_SIZE)
			continue;

		bool bPROG = DoQuestProgress ( pPROG );
		if ( bPROG )
		{
			pPROG = NULL;
		}
	}
}

bool GLChar::QuestMoveProgressEvent (const MapID& mapID, DWORD wPosX, DWORD wPosY, DWORD dwGateID, bool bStartMove /*= FALSE*/ )
{
	/// 이동정보 세팅;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.dwChaDBNum = CharDbNum();
	sMoveInfo.sMapID = mapID;
	
	DWORD wPOSX = USHRT_MAX;
	DWORD wPOSY = USHRT_MAX;
	if ( wPosX != USHRT_MAX && wPosY != USHRT_MAX )
	{
		wPOSX = wPosX;
		wPOSY = wPosY;
	}
	else if ( dwGateID != 0 )
	{
		sMoveInfo.dwGateID = dwGateID;
	}

	SMAPNODE* pMapNode = m_pGaeaServer->FindMapNode( mapID );
	if ( NULL == pMapNode )
		return FALSE;	

	if( !bStartMove && wPOSX != USHRT_MAX && wPosY != USHRT_MAX )
	{
        std::tr1::shared_ptr<GLMapAxisInfo> spAxisInfo = m_pGaeaServer->GetAxisInfo(pMapNode->m_LevelFile);
        if (spAxisInfo)
            spAxisInfo->MapPos2MiniPos( wPOSX, wPOSY, sMoveInfo.vPosition.x, sMoveInfo.vPosition.z );
	}
	else if( bStartMove )
	{
		sMoveInfo.dwGateID = m_dwStartGate;	
	}

	/// 이동;
	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

	return TRUE;

	/*// PET
	// 맵이동시 Pet 삭제
	m_pGaeaServer->ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,false,true) );
	
	//VEHICLE
	m_pGaeaServer->SetActiveVehicle( m_ClientSlot, m_dwGaeaID, false );

	//SUMMON
	//SummonAllRemove();
	RemoveAllChildCrow();

	//	Note : 다른 필드 서버일 경우.
	
	if (pMapNode->GetFieldServerNum() != GetFieldSvrID())
	{

		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;		
		NetMsgAg.dwChaNum = CharDbNum();
		NetMsgAg.dwFieldServerIDTo = pMapNode->GetFieldServerNum();
		NetMsgAg.mapIDTo = mapID;
		NetMsgAg.dwGATEID = dwGATEID;
		NetMsgAg.vPosition = vPOS;
		SendToAgent(&NetMsgAg );
	}
	//	Note : 같은 필드 서버일 경우.
	else
	{
		GLMSG::SNETPC_REQ_RECALL_FB	NetMsgFB;
		NetMsgFB.dwChaNum = CharDbNum();
        
        // 주의!: RequestInvenRecallThisSvr 함수 호출 후에는 케릭터의 현재 mapID가 이동하는 mapID로 변경되어버림;
        // 그래서 현재 맵 정보는 pChar->GetCurrentMapID() 를 사용하면 안된다. const MapID sCurMapID(GetCurrentMap()); 를 사용하여야 한다.;
        const MapID sCurMapID(GetCurrentMap());

		BOOL bOK = m_pGaeaServer->RequestInvenRecallThisSvr ( this, mapID, dwGATEID, vPOS );
		if ( !bOK )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
			SendToAgent(&NetMsgFB );
			return FALSE;
		}

		// 버프와 상태이상 제거
		if ( sCurMapID != mapID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )
				DISABLEBLOW ( i );
			for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
				RESETSKEFF  ( i );

			// CP 초기화 맵인지 확인
			if (pMapNode->IsCombatPointReset())
				ResetCP();
		}

		// 현재 액션 초기화
		ResetAction();

		//	Note : 멥 이동 성공을 알림.
		//
		GLLandMan* const pLand( GetLandMan() );
		NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.bCullByObjectMap = pLand == NULL ? true : pLand->IsCullByObjectMap();
		NetMsgFB.sMAPID = mapID;
		NetMsgFB.vPOS = m_vPosition;
		SendToAgent(&NetMsgFB );
	}

	return TRUE;*/
}

int GLChar::GetAttendanceCompletedDay( GLQUEST* pQUEST, GLQUESTPROG* pQUEST_PROG )
{
	std::vector<int> vecAttendanceCompletedDay;
	int nDayCount = 0;
	CTime TToday = CTime::GetCurrentTime();
	BOOST_FOREACH(LONGLONG QuestStart , pQUEST_PROG->m_vectQuestStart)
	{
		CTime TQuestStart = QuestStart;

		int a = TQuestStart.GetMonth();

		if( TToday.GetYear() == TQuestStart.GetYear() &&
			TToday.GetMonth() == TQuestStart.GetMonth() )
		{
			vecAttendanceCompletedDay.push_back(TQuestStart.GetDay());
		}
	}
	// 정렬
	std::sort( vecAttendanceCompletedDay.begin(), vecAttendanceCompletedDay.end(), std::less<int>() );

	// 연속적인 값 제거
	std::vector<int>::iterator pos = std::unique( vecAttendanceCompletedDay.begin(), vecAttendanceCompletedDay.end() );

	// 새로운 논리적인 끝 위치로부터 나머지 원소 제거
	vecAttendanceCompletedDay.erase( pos, vecAttendanceCompletedDay.end() );

	nDayCount = static_cast<int>(vecAttendanceCompletedDay.size());

	return nDayCount;
}

// 퀘스트 진행에 따라 실행되는 이벤트를 
bool GLChar::CheckQuestProgressEvent ( BYTE eventType, GLQUEST* pQUEST )
{
	if( !pQUEST->m_ProgressEvent.bUseProgressEvent )
		return TRUE;
	
	if( pQUEST->m_ProgressEvent.bUseProgressEvent )
	{
		DWORD dwProgress = 0;
		GLQUEST_PROGRESS_MAP progressMap;
		if( eventType == 0 ) // 시작시
		{
			dwProgress  = pQUEST->m_ProgressEvent.dwStartProgress;
			progressMap = pQUEST->m_ProgressEvent.startMap;
		}
		else if( eventType == 1 ) // 완료시
		{
			dwProgress  = pQUEST->m_ProgressEvent.dwEndProgress;
			progressMap = pQUEST->m_ProgressEvent.endMap;
		}
		else if( eventType == 2 ) // 실패시
		{
			dwProgress  = pQUEST->m_ProgressEvent.dwFailProgress;
			progressMap = pQUEST->m_ProgressEvent.failMap;
		}
		else
			return TRUE;

		//  Note : 만약 시작시 맵 이동이 필요한 경우			
		if( dwProgress == EMQP_SELECT_MOVE )
		{
			if( !QuestMoveProgressEvent( progressMap.nidMAP, progressMap.wPosX, progressMap.wPosY, progressMap.dwGateID ) ) return FALSE;	
		}
		//  Note : 만약 시작시 캐릭터의 시작 지점으로 이동을 해야하는 경우
		if( dwProgress == EMQP_START_MOVE )
		{
			if( !QuestMoveProgressEvent( m_sStartMapID, USHRT_MAX, USHRT_MAX, 0, TRUE ) )
				return FALSE;
		}

		//  Note : 만약 시작시 캐릭터가 사망을 해야하는 퀘스트 일 경우
		if( dwProgress == EMQP_CHAR_DEAD )
			m_sHP.nNow = 0;
	}

	return TRUE;
}
