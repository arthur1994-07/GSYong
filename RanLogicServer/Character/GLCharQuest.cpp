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

		// �������� ����Ʈ ���� �˻�
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
					// DoGIVEUP ȣ������ ��񿡼� ����Ʈ ������ �����Ѵ�.
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

					//	����.
					pQUEST_PROG->DoGIVEUP();
					//	�Ϸ�� ����Ʈ�� �߰�.
					m_cQuestPlay.InsertEnd(*pQUEST_PROG, false);
					m_cQuestPlay.DeleteProc( dwQUEST );
				}
			}
			else
				continue;
		}

		//	�̹� �Ϸ��� ����Ʈ���� �˻�.
		pQUEST_END = m_cQuestPlay.FindEnd ( dwQUEST );
		if ( pQUEST_END )
		{
			if ( pQUEST_END->m_bCOMPLETE )
			{
				//	������ �õ� �������� �˻�.
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
				//	����(����)�� ����Ʈ�� �ٽ� �õ� �Ұ����� ���.
				if ( !pQUEST->IsAGAIN() &&
					pQUEST->m_sSTARTOPT.nJOIN != EMQJ_ATTENDANCE)
					continue;
			}
		}

		if( !CheckQuestProgressEvent( 0, pQUEST ) )
			continue;


		//	Note : ������ ���۵Ǵ� ����Ʈ ����.
		//
		GLQUESTPROG sQUEST_PROG_NEW;
		sQUEST_PROG_NEW.START ( pQUEST, pQUEST_END );

		//	����Ʈ�� ���.
		m_cQuestPlay.InsertProc(sQUEST_PROG_NEW, true);

		//	Note : �߰��� ����Ʈ ���� ����.
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

		//	���� ������ ������ ���� ������ ����.
		if ( pQUEST_END )
		{
			//	Note : ����� ����Ʈ ����Ʈ���� ����Ʈ ���� ����.
			//
			m_cQuestPlay.DeleteEnd ( dwQUEST );

			//	Note : ������ ����Ʈ ���� ����.
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


	//	Note : ����Ʈ ����. ( ���� �������� ����� �� ����. )
	//
	bool _bModifyInven(false);
	GLQUEST_PROGRESS_MAP stepMoveMap;
	std::vector< SINVENITEM > vecDeleted;
	bool bInserted = false;
	bool bPROG = pQUEST_PROG->DoPROGRESS( _bModifyInven, stepMoveMap, vecDeleted, bInserted );

	// ����Ʈ ������ ó��
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

	// ���� ������ �ְ� �ش� ������ �ٸ� ����Ʈ���
	// ���� ���� ������ ���� ����Ʈ�� �ݺ��ϵ��� �Ǿ��ִٸ� �Ʒ��� �Ϸ�ó���� �ٽ� �����ϵ��� �Ѵ�.
	if ( bPROG ) 
	{
		//	Note : ���� ���� ���� ����.
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
	//	Note : ����Ʈ�� �Ϸ�̴��� �˻�.
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

		//	�Ϸ�� ����Ʈ�� �߰�.
		m_cQuestPlay.InsertEnd(*pQUEST_PROG, false);

		se::ByteStream cByteStream;
		pQUEST_PROG->SET_BYBUFFER ( cByteStream );

		LPBYTE pBuff(NULL);
		DWORD dwSize(0);
		cByteStream.GetBuffer ( pBuff, dwSize );

		//	�������� ����.
		m_cQuestPlay.DeleteProc ( dwQID );

		//	Note : ����Ʈ �Ϸ� ���� ����.
		//
		if( dwSize <= MAX_QUEST_PACKET_SIZE )
		{
			GLMSG::SNET_QUEST_END_STREAM NetMsgStream(pQUEST_PROG->m_sNID.dwID, 0);
			NetMsgStream.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
			// QUEST_BUFFER_OVER
			sc::writeLogDebug(sc::string::format("DoQuestProgress ( �Ϸ�����, dwSize <= MAX_QUEST_PACKET_SIZE ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgStream.dwQID, NetMsgStream.dwSeq, NetMsgStream.bLastSeq, NetMsgStream.dwLengthStream, NetMsgStream.dwSize));	
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
				sc::writeLogDebug(sc::string::format("DoQuestProgress ( �Ϸ����� ) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%", NetMsgStream.dwQID, NetMsgStream.dwSeq, NetMsgStream.bLastSeq, NetMsgStream.dwLengthStream, NetMsgStream.dwSize));	
#endif

				SendToClient(&NetMsgStream );
			}
		}


		//	Note : ����Ʈ ���� ���� ���� ����.
		//
		GLMSG::SNET_QUEST_PROG_DEL NetMsgDel(dwQID);		
		SendToClient(&NetMsgDel );

		if ( !CheckQuestProgressEvent( 1, pQUEST ) )
		{
			GLMSG::SNETPC_QUEST_COMPLETE_FB NetMsgFB(dwQID, EMQUEST_COMPLETE_FB_FAIL);
			SendToClient( &NetMsgFB );
			return true;
		}

		//	Note : ����Ʈ�� ���� ����.
		//	������ ���� ����Ʈ�� �ݺ��ϵ��� ������ ��� �Ʒ����� ó��
		DoQuestGenerateReward ( pQUEST );

		return true;
	}

	return false;
}

//	Note : ������ �ֱ� �׽�Ʈ, ����Ʈ ���� �������� �� ������ ������ �ִ��� Ȯ��.
bool GLChar::CheckQuestItemReward ( GLQUEST* pQUEST, GLQUESTPROG* pQUEST_PROG )
{
	if (!pQUEST)
        return false;

	// ���ʿ��� Assign ���� 

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
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
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
			//	Note : ���� ����. ( ���� �޽��� �߻�. )
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
	
	//	Note : ������ �ݺ� �Ұ����Ҷ��� ������.
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

	//	������ ����.
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

			// ����Ʈ ������ ��쿡�� �����ɼ��� �ٵ��� �Ѵ�;
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
				//	�ι��丮�� ����.
				if ( m_cInventory.InsertItem( sITEM_NEW, wInsertPosX, wInsertPosY ) )
				{
					// �������� ���� ���� ��� ���
					// ��� �� �α� ����
					LogItemExchange(
						sITEM_NEW,
						gamelog::ID_CHAR, 0,
						gamelog::ID_CHAR, m_CharDbNum,
						ITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum, 
						( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_QUEST_REWARD,
						false );

					// ��� ������Ʈ
					InvenUpdateItem( m_cInventory.GetItem( wInsertPosX, wInsertPosY ) );

					//	�ι꿡 ������ �־��ִ� �޽���.
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

				// ������ �߻�.
				DropItem( sITEM_NEW, vCollision );
			}
		}
	}

	//	��ų ����.
	{
		DWQARRAY_ITER iter = pQUEST->m_vecGiftSKILL.begin();
		DWQARRAY_ITER iter_end = pQUEST->m_vecGiftSKILL.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SNATIVEID sNID = SNATIVEID( *iter );

			bool bLEARN = LEARN_SKILL_QUEST( sNID );
			if ( !bLEARN )	continue;

			//	��ų ��� ����.
			GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
			NetMsgFB.skill_id = sNID;
			NetMsgFB.emCHECK = EMSKILL_LEARN_OK;
			SendToAgent( &NetMsgFB );

			//	Note : passive skill �� ��� �ɸ��� ����Ǵ� �Ӽ����� Ŭ���̾�Ʈ�鿡 ����ȭ.
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
		// ����Ʈ ������ �߸��ϴ°�� ���ѷ����� ������ �����Ƿ� ������ �������� ������ �ֵ�����.
		//do
		//{
			DWORD dwQUEST = pQUEST->m_dwGiftQUEST;

			GLQUEST *pQUEST = glQuestMan.Find( dwQUEST );
			if ( !pQUEST )
			{
				sc::writeLogError( sc::string::format( "DoQuestGenerateReward Find ERROR m_dwGiftQUEST %1%", dwQUEST ) );	
				return;
			}

			//	Memo :	�־����� ����Ʈ�� Ŭ���� ���ǿ� �´��� �˻�.
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

			// �������� ����Ʈ����
			GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc( dwQUEST );
			if ( pQUEST_PROG )
			{
				sc::writeLogError( sc::string::format( "DoQuestGenerateReward Quest Process ERROR m_dwGiftQUEST %1%", dwQUEST ) );	
				return;
			}

			//	�̹� �Ϸ��� ����Ʈ���� �˻�.
			GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd( dwQUEST );
			if ( pQUEST_END )
			{
				if ( pQUEST_END->m_bCOMPLETE )
				{
					//	������ �õ� �������� �˻�.
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
					//	����(����)�� ����Ʈ�� �ٽ� �õ� �Ұ����� ���.
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

			//	Note : ������ �ִ� ���� �׼��� ����.
			// ������ �ƹ��� ó���� ���� ������ �ٸ� �κп��� ��� �˻��ϵ��� �Ǿ������Ƿ� ���ϼ��� ���ؼ� �߰���
			CheckMoneyUpdate( GetInvenMoney(), pQUEST->m_dwBeginMoney, FALSE, "Quest Repeat Start Pay." );

			SubtractInvenMoneyServer( pQUEST->m_dwBeginMoney, SUBTRACT_INVEN_MONEY_QUEST_START );

			if ( pQUEST->m_dwBeginMoney )
			{
				// �ݾ� ��ȭ �α�.
				// ��� �� �α� ����
				LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, -LONGLONG(pQUEST->m_dwBeginMoney), ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_QUEST_START );
				// ��� �� �α� ����
				LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
			}

			//	Note : Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���.
			SendInvenMoneyUpdate();
			
			//	Note : ������ ���۵Ǵ� ����Ʈ ����.
			//
			GLQUESTPROG sQUEST_PROG_NEW;
			sQUEST_PROG_NEW.START( pQUEST, pQUEST_END );

			//	����Ʈ�� ���.
			m_cQuestPlay.InsertProc( sQUEST_PROG_NEW, true );

			//	Note : �߰��� ����Ʈ ���� ����.
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

			//	���� ������ ������ ���� ������ ����.
			if ( pQUEST_END )
			{
				//	Note : ����� ����Ʈ ����Ʈ���� ����Ʈ ���� ����.
				//
				m_cQuestPlay.DeleteEnd( dwQUEST );

				//	Note : ������ ����Ʈ ���� ����.
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

		//�ڱ��ڽ��� �ƴҰ�쿣 ��Ƽ����Ʈ�� �����Ѵ�
		if( !bMySelf && !pQUEST->IsPARTYQUEST() )
			continue;
		
		pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
		if ( !pSTEP )
			continue;

		//������ ��ġ���� ��ƾ� �ϴ� ����Ʈ �̸�
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
				//	kill �� ����.
				bMOBKILL = true;
				bPROG = true;

				++pPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL;

				//	�����Ͽ����� �˻�.
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
			//	Note : ����Ʈ ����.
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

		//�ڱ��ڽ��� �ƴҰ�쿣 ��Ƽ����Ʈ�� �����Ѵ�
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

				//	�̹� �ش� �������� ��� �߻��ߴ��� �˻�.
				DWORD dwNUM = pPROG->m_sINVENTORY.CountTurnItem( sGENQITEM.sNID );

				if ( dwNUM >= sGENQITEM.wNUM )
					continue;

				//	�߻������� (Ȯ��)�� �߻�.
                if (sc::Random::getInstance().IsSucceed(sGENQITEM.fGEN_RATE))
				{
					//	����Ʈ �κ��丮�� �־���.
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
						//	����Ʈ �κ��� �ֱⰡ ������.
					}
					else
					{
						bPROG = true;

						//	������ Ŭ���̾�Ʈ�� �˸�.
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
			//	Note : ����Ʈ ����.
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
		// �ش� ���� ����Ʈ ���̳�?
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
				//	������ ��� ���� ����.
				bITEMUSE = true;
				bPROG = true;

				++pPROG->m_sSTEP_PROG.m_dwNUM_ITEMUSE;

				//	�����Ͽ����� �˻�.
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
			//	Note : ����Ʈ ����.
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
	// �ش� �������� ����Ʈ ������ �̳�
	GLQuestPlay::QUESTMOB_MAP &mapPROC = m_cQuestPlay.GetQuestItemUseProc();
	GLQuestPlay::QUESTMOB_MAP_ITER iter = mapPROC.find( nidITEM.dwID );
	if( iter == mapPROC.end() )
		return;

	DoQuestProgItemUse( nidITEM.dwID );
	//DoQuestProgQItem( nidMOB.dwID, bMySelf );
}

void GLChar::DoQuestItemGrinding( SNATIVEID nidITEM, EMGRINDING_CLASS emGrindingClass, bool IsSuccess  )
{
	//// �ش� �������� ����Ʈ ������ �̳�
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
				//���� ������ �ƴϰ� ���� �����̸� �Ѿ��
				if( !pSTEP->m_bConsecution && !IsSuccess )
					continue;

				//	������ ���� ���� ����.
				ITEMGRINDING = true;
				bPROG = true;

				++pPROG->m_sSTEP_PROG.m_dwNUM_ITEMGRINDING;

				// ���� �����̰� ���� �����̸� ����
				if ( pSTEP->m_bConsecution && !IsSuccess)
					pPROG->m_sSTEP_PROG.m_dwNUM_ITEMGRINDING = 0;

				//	�����Ͽ����� �˻�.
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
			//	Note : ����Ʈ ����.
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
			//	�����ؾ��� �� ����
			bPROG = true;

			++pPROG->m_sSTEP_PROG.m_dwNUM_LEVELUP;

			//	�����Ͽ����� �˻�.
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
			//	Note : ����Ʈ ����.
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

			//	Note : ����Ʈ ����.
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
				//	��� Ƚ�� ����
				bPROG = true;

				++pPROG->m_sSTEP_PROG.m_dwNUM_CONFTING;

				//	�����Ͽ����� �˻�.
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
			//	Note : ����Ʈ ����.
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
		//	��ħ �������� ���.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	�ֱ� ��û�� �����ۼ�. ( �ܿ���. )
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
		//	�ι��� �������� �������� ������ �����ϴ�.
		return E_FAIL;
	}
	
	if ( pItem->ISPILE() )
	{
		//	��ħ �������� ���.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	�ֱ� ��û�� �����ۼ�. ( �ܿ���. )
		WORD wREQINSRTNUM = ( sCUSTOM.wTurnNum );

		//	Note : �� �������� �ִ� �������� �ִ��� �˻��Ͽ� �κ��� �����ϴ�
		//		������ ���� ���ڸ� ���ҽ�Ŵ.
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
		
			//	���� ������ ��ħ ������.
			WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;

			if ( wREQINSRTNUM > wSURPLUSNUM )
			{
				//	Note : ���� �������� ��ħ�� ����. ( ���� )
				sITEMCUSTOM.wTurnNum = wPILENUM;

				//	Note : Ŭ���̾�Ʈ�� ���� ������ �˸�.
				GLMSG::SNETPC_QUEST_PROG_INVEN_TURNNUM NetMsg( dwQID, sINVENITEM.wPosX, sINVENITEM.wPosY, sITEMCUSTOM.wTurnNum );
				SendToClient( &NetMsg );

				//	Note : �ܿ� ���� ����.
				wREQINSRTNUM -= wSURPLUSNUM;

				// #item
				// ���� ���� ��� ������Ʈ �׽�Ʈ �뵵
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
				//	Note : ���� �����ۿ� ��ġ�� �� �����ε� ���� ���� �������� ����� ��.
				sITEMCUSTOM.wTurnNum += wREQINSRTNUM;

				//	Note : Ŭ���̾�Ʈ�� ���� ������ �˸�.
				GLMSG::SNETPC_QUEST_PROG_INVEN_TURNNUM NetMsg( dwQID, sINVENITEM.wPosX, sINVENITEM.wPosY, sITEMCUSTOM.wTurnNum );
				SendToClient( dwClientID, &NetMsg );

				// ���� ���� ��� ������Ʈ �׽�Ʈ �뵵
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

		//	Note : ������ �κ��� ������ �������� ���� �ľ��� �κ��� �� ������ �ִ��� �˻�.

		//	�ִ��ħ������ ����.
		WORD wONENUM = wREQINSRTNUM / wPILENUM;
		WORD wITEMNUM = wONENUM;

		//	���а�ħ�������� ��ħ��.
		WORD wSPLITNUM = wREQINSRTNUM % wPILENUM;
		if ( wSPLITNUM > 0 )				wITEMNUM += 1;
		if ( wSPLITNUM==0 && wITEMNUM>=1 )	wSPLITNUM = wPILENUM;

		for ( WORD i=0; i<wITEMNUM; ++i )
		{
			WORD wInsertPosX( 0 ), wInsertPosY( 0 );
			BOOL bSPACE = _sINVENTORY.FindInsertable( wInsertPosX, wInsertPosY );
						
			if ( !bSPACE )
			{
				sc::writeLogError( sc::string::format( "Char(%1%) inventory - ���� ������ �̸� üũ�� �ϰ� �������� �־����� ������ ������7", CharDbNum() ) );
				return E_FAIL;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.
			}

			//	Note : ���ο� �������� �־���.
			//
			SITEMCUSTOM sITEMCUSTOM = sCUSTOM;
			if ( wITEMNUM==(i+1) )	sITEMCUSTOM.wTurnNum = wSPLITNUM;	//	������ �������� �ܿ���.
			else					sITEMCUSTOM.wTurnNum = wPILENUM;	//	�ƴ� ���� ������.

			if ( i > 0 )
			{
				// �ű� guid ���� �ʿ�
				// �� ��찡 �Ǹ� ������ ���� ������ �α׸��� ���ο� �������� �����ȰŴ�. 
				sc::SeqUniqueGUID::Instance()->getGUID( sITEMCUSTOM.guid );
				sITEMCUSTOM.GenerateBasicStat( false );
				sITEMCUSTOM.GenerateLinkSkill();
				sITEMCUSTOM.GenerateAddOption();
			}

			//	�ι��丮�� �ֽ��ϴ�.
			_sINVENTORY.InsertItem( sITEMCUSTOM, wInsertPosX, wInsertPosY );
			SINVENITEM *pInvenItem = _sINVENTORY.GetItem( wInsertPosX, wInsertPosY );
			if ( !pInvenItem )
				return E_FAIL;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.

			{
				AddGameAdoJob(
					db::DbActionPtr( 
					new db::ItemInsert( m_CharDbNum, INVEN_QUEST_PROC, pInvenItem->sSaveData, 19771130 ) ) );
				pInvenItem->SetDbState( db::DB_UPDATE, true );
			}

			//	�ι꿡 ������ �־��ִ� �޽���.
			GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT NetMsgInven( dwQID, *pInvenItem );
			SendToClient( &NetMsgInven );
		}
	}
	else
	{
		WORD wPosX, wPosY;
		BOOL bOk = _sINVENTORY.FindInsertable( wPosX, wPosY );
		if ( !bOk )
			return E_FAIL;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.

		//	Note : ������ �ֱ�.
		_sINVENTORY.InsertItem( sCUSTOM, wPosX, wPosY );
		SINVENITEM *pInvenItem = _sINVENTORY.GetItem( wPosX, wPosY );
		if ( !pInvenItem )
			return E_FAIL;	//	�����δ� ������ ������ ������ �����߻� ���ɼ��� ����.

		{
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemInsert( m_CharDbNum, INVEN_QUEST_PROC, pInvenItem->sSaveData, 19771131 ) ) );
			pInvenItem->SetDbState( db::DB_UPDATE, true );
		}

		//	[�ڽſ���] �޽��� �߻�.
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
			//�Ϸ�� ����Ʈ ���� Ȯ��
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

			//	�ð� �ʰ��� �˸�.
			GLMSG::SNETPC_QUEST_PROG_TIMEOVER NetMsg(dwQID);
			SendToClient( &NetMsg );

			if (!bIsComplete )
			{
				if( pQUEST->m_ProgressEvent.bUseProgressEvent && pQUEST->m_ProgressEvent.IsTIMEOVER() )
					CheckQuestProgressEvent( 2, pQUEST );

				// DoGIVEUP ȣ������ ��񿡼� ����Ʈ ������ �����Ѵ�.
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

				//	�ڵ� ���� ó��.
				pQuestProg->DoGIVEUP ();

				//	�Ϸ�� ����Ʈ�� �߰�.
				m_cQuestPlay.InsertEnd(*pQuestProg, false);

				se::ByteStream cByteStream;
				pQuestProg->SET_BYBUFFER ( cByteStream );

				LPBYTE pBuff(NULL);
				DWORD dwSize(0);
				cByteStream.GetBuffer ( pBuff, dwSize );

				//	�������� ����.
				m_cQuestPlay.DeleteProc ( dwQID );

				//	Note : ����Ʈ �Ϸ� ���� ����.
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

				//	Note : ����Ʈ ���� ���� ���� ����.
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
			//	����Ͽ� ����Ʈ ���ǿ� �ɸ��� �˸�.
			GLMSG::SNETPC_QUEST_PROG_LEAVEMAP NetMsg(dwQID);
			SendToClient( &NetMsg );

			CheckQuestProgressEvent( 2, pQUEST );

			// DoGIVEUP ȣ������ ��񿡼� ����Ʈ ������ �����Ѵ�.
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

			//	�ڵ� ���� ó��.
			pQuestProg->DoGIVEUP ();

			//	�Ϸ�� ����Ʈ�� �߰�.
			m_cQuestPlay.InsertEnd(*pQuestProg, false);

			se::ByteStream cByteStream;
			pQuestProg->SET_BYBUFFER ( cByteStream );

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			cByteStream.GetBuffer ( pBuff, dwSize );

			//	�������� ����.
			m_cQuestPlay.DeleteProc ( dwQID );

			//	Note : ����Ʈ �Ϸ� ���� ����.
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

			//	Note : ����Ʈ ���� ���� ���� ����.
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
			//	����Ͽ� ����Ʈ ���ǿ� �ɸ��� �˸�.
			GLMSG::SNETPC_QUEST_PROG_NONDIE NetMsg(dwQID);
			SendToClient( &NetMsg );

			if( pQUEST->m_ProgressEvent.bUseProgressEvent && pQUEST->m_ProgressEvent.IsDIE() )
				CheckQuestProgressEvent( 2, pQUEST );

			// DoGIVEUP ȣ������ ��񿡼� ����Ʈ ������ �����Ѵ�.
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

			//	�ڵ� ���� ó��.
			pQuestProg->DoGIVEUP ();

			//	�Ϸ�� ����Ʈ�� �߰�.
			m_cQuestPlay.InsertEnd(*pQuestProg, false);

			se::ByteStream cByteStream;
			pQuestProg->SET_BYBUFFER ( cByteStream );

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			cByteStream.GetBuffer ( pBuff, dwSize );

			//	�������� ����.
			m_cQuestPlay.DeleteProc ( dwQID );

			//	Note : ����Ʈ �Ϸ� ���� ����.
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

			//	Note : ����Ʈ ���� ���� ���� ����.
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

		//	Note : ����Ʈ ����.
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
	/// �̵����� ����;
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

	/// �̵�;
	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

	return TRUE;

	/*// PET
	// ���̵��� Pet ����
	m_pGaeaServer->ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,false,true) );
	
	//VEHICLE
	m_pGaeaServer->SetActiveVehicle( m_ClientSlot, m_dwGaeaID, false );

	//SUMMON
	//SummonAllRemove();
	RemoveAllChildCrow();

	//	Note : �ٸ� �ʵ� ������ ���.
	
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
	//	Note : ���� �ʵ� ������ ���.
	else
	{
		GLMSG::SNETPC_REQ_RECALL_FB	NetMsgFB;
		NetMsgFB.dwChaNum = CharDbNum();
        
        // ����!: RequestInvenRecallThisSvr �Լ� ȣ�� �Ŀ��� �ɸ����� ���� mapID�� �̵��ϴ� mapID�� ����Ǿ����;
        // �׷��� ���� �� ������ pChar->GetCurrentMapID() �� ����ϸ� �ȵȴ�. const MapID sCurMapID(GetCurrentMap()); �� ����Ͽ��� �Ѵ�.;
        const MapID sCurMapID(GetCurrentMap());

		BOOL bOK = m_pGaeaServer->RequestInvenRecallThisSvr ( this, mapID, dwGATEID, vPOS );
		if ( !bOK )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
			SendToAgent(&NetMsgFB );
			return FALSE;
		}

		// ������ �����̻� ����
		if ( sCurMapID != mapID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )
				DISABLEBLOW ( i );
			for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
				RESETSKEFF  ( i );

			// CP �ʱ�ȭ ������ Ȯ��
			if (pMapNode->IsCombatPointReset())
				ResetCP();
		}

		// ���� �׼� �ʱ�ȭ
		ResetAction();

		//	Note : �� �̵� ������ �˸�.
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
	// ����
	std::sort( vecAttendanceCompletedDay.begin(), vecAttendanceCompletedDay.end(), std::less<int>() );

	// �������� �� ����
	std::vector<int>::iterator pos = std::unique( vecAttendanceCompletedDay.begin(), vecAttendanceCompletedDay.end() );

	// ���ο� ������ �� ��ġ�κ��� ������ ���� ����
	vecAttendanceCompletedDay.erase( pos, vecAttendanceCompletedDay.end() );

	nDayCount = static_cast<int>(vecAttendanceCompletedDay.size());

	return nDayCount;
}

// ����Ʈ ���࿡ ���� ����Ǵ� �̺�Ʈ�� 
bool GLChar::CheckQuestProgressEvent ( BYTE eventType, GLQUEST* pQUEST )
{
	if( !pQUEST->m_ProgressEvent.bUseProgressEvent )
		return TRUE;
	
	if( pQUEST->m_ProgressEvent.bUseProgressEvent )
	{
		DWORD dwProgress = 0;
		GLQUEST_PROGRESS_MAP progressMap;
		if( eventType == 0 ) // ���۽�
		{
			dwProgress  = pQUEST->m_ProgressEvent.dwStartProgress;
			progressMap = pQUEST->m_ProgressEvent.startMap;
		}
		else if( eventType == 1 ) // �Ϸ��
		{
			dwProgress  = pQUEST->m_ProgressEvent.dwEndProgress;
			progressMap = pQUEST->m_ProgressEvent.endMap;
		}
		else if( eventType == 2 ) // ���н�
		{
			dwProgress  = pQUEST->m_ProgressEvent.dwFailProgress;
			progressMap = pQUEST->m_ProgressEvent.failMap;
		}
		else
			return TRUE;

		//  Note : ���� ���۽� �� �̵��� �ʿ��� ���			
		if( dwProgress == EMQP_SELECT_MOVE )
		{
			if( !QuestMoveProgressEvent( progressMap.nidMAP, progressMap.wPosX, progressMap.wPosY, progressMap.dwGateID ) ) return FALSE;	
		}
		//  Note : ���� ���۽� ĳ������ ���� �������� �̵��� �ؾ��ϴ� ���
		if( dwProgress == EMQP_START_MOVE )
		{
			if( !QuestMoveProgressEvent( m_sStartMapID, USHRT_MAX, USHRT_MAX, 0, TRUE ) )
				return FALSE;
		}

		//  Note : ���� ���۽� ĳ���Ͱ� ����� �ؾ��ϴ� ����Ʈ �� ���
		if( dwProgress == EMQP_CHAR_DEAD )
			m_sHP.nNow = 0;
	}

	return TRUE;
}
