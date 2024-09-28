#include "../pch.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/Common/profile.h"
#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ProductMsg.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogic/Msg/GLContrlTexasHoldemMsg.h"
#include "../../RanLogic/HideSet/GLHideSet.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Event/GLIPEvent.h"

#include "../Util/GLogicExServer.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../PK/GLSchoolFreePK.h"
#include "../PrivateMarket/PrivateMarketField.h"
#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "../Club/GLClubServer.h"
#include "../Party/GLPartyField.h"
#include "../Vehicle/GLVehicleField.h"
#include "../TriggerSystem/ServerExitMessage.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "../TexasHoldem/GLTexasHoldemField.h"

#include "./GLChar.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

HRESULT GLChar::MsgGameJoin()
{
	// 클라이언트에 전송 : 캐릭터정보 + 스킬 + 아이템 + 인스턴스 인던 목록;	
	
	// DB에서 정보를 읽고나면 캐릭터에게 전송한다.
	// 동기화 문제로 인하여 위치가 수정되었다.
	LoadExtraInfo();
	ActivityLoad();
	AttendanceLoad();

	GLMSG::SNETLOBBY_CHARJOIN NetCharJoin;
	NetCharJoin.dwChaNum = CharDbNum();

	StringCchCopy ( NetCharJoin.szUserID , USR_ID_LENGTH+1, m_szUID );

	NetCharJoin.dwClientID	= m_ClientSlot;
	NetCharJoin.dwGaeaID	= m_dwGaeaID;
	NetCharJoin.sMapID		= m_mapID;
	NetCharJoin.vPos		= m_vPosition;

	GLLandMan* const pLand( GetLandMan() );
	NetCharJoin.bCullByObjectMap = pLand == NULL ? true : pLand->IsCullByObjectMap();
	NetCharJoin.Data		= GETCHARDATA();

	NetCharJoin.wSKILLQUICK_ACT = m_wSKILLQUICK_ACT;
	memcpy ( NetCharJoin.sSKILLQUICK, m_sSKILLQUICK, sizeof(SNATIVEID)*EMSKILLQUICK_SIZE );
	memcpy ( NetCharJoin.sACTIONQUICK, m_sACTIONQUICK, sizeof(SACTION_SLOT)*EMACTIONQUICK_SIZE );

	NetCharJoin.dwNumInvenItems = m_cInventory.GetNumItems();


	//EMSERVICE_PROVIDER sp = GetServiceProvider();	
	if (m_ServiceProvider == SP_VIETNAM)
	{
//#if defined(VN_PARAM) //vietnamtest%%%
		NetCharJoin.dwNumVNInvenItems = m_cVietnamInventory.GetNumItems();
//#endif
	}
	// 모든 국가에서 Tracing 기능을 사용할 수 있도록 변경
	//else if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
	//{
//#if defined(TW_PARAM) || defined(_RELEASED) || defined(HK_PARAM) || defined ( TH_PARAM ) // ***Tracing Log print
		//NetCharJoin.bTracingChar	  = m_bTracingUser;
//#endif
	//}

	NetCharJoin.dwCountryID = m_sCountryInfo.dwID;

	NetCharJoin.dwNumSkill = static_cast<DWORD>(m_ExpSkills.size());
	NetCharJoin.dwNumQuestProc = static_cast<DWORD>(m_cQuestPlay.GetQuestProc().size());
	NetCharJoin.dwNumQuestEnd = static_cast<DWORD>(m_cQuestPlay.GetQuestEnd().size());
	// RM# 1197 에 따른 요청사항을 더이상 필요하지않아서 제거 (20160714)
	// 이벤트용 퀘스트 버그로 인해서 특정 퀘스트 번호만 예외처리. 차후에 반드시 소스에서 정리할것
	/*
		DWORD dwEndQuetNum = 0;
		GLQuestPlay::MAPQUEST_ITER eiter = m_cQuestPlay.GetQuestEnd().begin();
		GLQuestPlay::MAPQUEST_ITER eiter_end = m_cQuestPlay.GetQuestEnd().end();
		for ( ; eiter!=eiter_end; ++eiter )
		{
			GLQUESTPROG *pPROG = (*eiter).second;

			if( pPROG != NULL )
			{
				if( (pPROG->m_sNID.dwID == 3107 || pPROG->m_sNID.dwID == 3108 || pPROG->m_sNID.dwID == 3109 || pPROG->m_sNID.dwID == 3110 || pPROG->m_sNID.dwID == 3111 || pPROG->m_sNID.dwID == 3112 || pPROG->m_sNID.dwID == 3113 || pPROG->m_sNID.dwID == 3114 ) )
				{
					CTime cTime = CTime::GetCurrentTime();
					CTime cTimeEnd(pPROG->m_tEndTime);

					if (cTime.GetDay() != cTimeEnd.GetDay() && cTime.GetMonth() == cTimeEnd.GetMonth() && cTime.GetYear() == cTimeEnd.GetYear() )
					{
						continue;
					}

				}
				dwEndQuetNum++;
			}
			else
			{
				sc::writeLogError(std::string("[EVENT_QUEST] GLChar::MsgGameJoin - m_mapQuestEnd Quest count ERROR"));
			}
		}
		NetCharJoin.dwNumQuestEnd = dwEndQuetNum;
	*/
	NetCharJoin.dwChargeItem = static_cast<DWORD>(m_mapCharged.size());
	NetCharJoin.dwItemCoolTime = static_cast<DWORD> ( m_mapCoolTimeID.size() + m_mapCoolTimeType.size() );

	NetCharJoin.bBIGHEAD = m_pGaeaServer->m_bBigHead;
	NetCharJoin.bBIGHAND = m_pGaeaServer->m_bBigHand;
	NetCharJoin.fBIGHEAD = m_pGaeaServer->m_fBigHead;
	NetCharJoin.fBIGHAND = m_pGaeaServer->m_fBigHand;

	NetCharJoin.sStartMapID		= m_sStartMapID;
	NetCharJoin.dwStartGate		= m_dwStartGate;
	NetCharJoin.sLastCallMapID	= m_sLastCallMapID;
	NetCharJoin.vLastCallPos	= m_vLastCallPos;
	NetCharJoin.emIPBonus		= m_emIPBonus;

	SendToAgent(&NetCharJoin);

	GLMSG::SNETLOBBY_CHAR_DEFAULT NetCharJoinDefault;
	NetCharJoinDefault.dwChaNum = CharDbNum();

	NetCharJoinDefault.fInvisiblePerceiveAngle = GLHIDESET::fInvisiblePerceiveAngle;
	NetCharJoinDefault.fInvisiblePerceiveRadius = GLHIDESET::fInvisiblePerceiveRadius;
	NetCharJoinDefault.fInvisiblePerceiveLevel = GLHIDESET::fInvisiblePerceiveLevel;
	NetCharJoinDefault.fInvisibleUpdateTime = GLHIDESET::fInvisibleUpdateTime;

	SendToAgent(&NetCharJoinDefault);


    {
        // 자동으로 묶어서 보낸다.
        MSGPACK_AUTO_PACK<GLMSG::CHARPUTON_EX_PACK_DATA> autoPack(CharDbNum());
        for ( int i = 0; i < SLOT_TSIZE; ++i )
        {
            GLMSG::CHARPUTON_EX_PACK_DATA data;
            data.PutOnItem = m_PutOnItems[i];
            data.nSlot = i;

            autoPack.PushData( data );
        }

        SendToAgent( NET_MSG_LOBBY_CHAR_PUTON_EX, autoPack.GetVecBuffer() );
    }

    {
        // 클라이언트에 인벤토리 전송
        // 자동으로 묶어서 보낸다.
        MSGPACK_AUTO_PACK<GLMSG::SNETLOBBY_INVENITEM_PACK> autoPack(CharDbNum());
        {
            GLInventory::CELL_MAP_CITER iter = m_cInventory.GetItemList()->begin();
            GLInventory::CELL_MAP_CITER iter_end = m_cInventory.GetItemList()->end();
            for ( ; iter!=iter_end; ++iter )
            {
                SINVENITEM* pInvenItem = (*iter).second;

                GLMSG::SNETLOBBY_INVENITEM_PACK data;
                data.Data = *pInvenItem;

                autoPack.PushData( data );
            }
        }
        SendToAgent( NET_MSG_LOBBY_CHAR_ITEM, autoPack.GetVecBuffer() );
    }

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		// 클라이언트에 베트남 인벤토리 전송
		// 루프를 돌면서 하나씩 전송
		{
			GLInventory::CELL_MAP_CITER iter = m_cVietnamInventory.GetItemList()->begin();
			GLInventory::CELL_MAP_CITER iter_end = m_cVietnamInventory.GetItemList()->end();
			for ( ; iter!=iter_end; ++iter )
			{
				SINVENITEM* pInvenItem = (*iter).second;

				GLMSG::SNETLOBBY_VIETNAM_INVENITEM NetInvenItem;
				NetInvenItem.Data = *pInvenItem;

				SendToAgent(&NetInvenItem);
			}
		}
	}
//#endif

	//	배운 스킬목록 모두 전송.
	//
	{
		GLMSG::SNETLOBBY_CHARSKILL NetSkill;
		NetSkill.dwChaNum = CharDbNum();

		SKILL_MAP_ITER iter = m_ExpSkills.begin();
		SKILL_MAP_ITER iter_end = m_ExpSkills.end();
		for ( ; iter!=iter_end; ++iter )
		{
			const SCHARSKILL &sSkill = (*iter).second;

			NetSkill.ADD ( sSkill );
			if ( NetSkill.wNum == EMGLMSG_SKILLMAX )
			{
				SendToAgent(&NetSkill);
				NetSkill.wNum = 0;
			}
		}

		if ( NetSkill.wNum != 0 )
            SendToAgent(&NetSkill);
	}

	bool bIsPlayRnattendanceQuest = false;
	//	진행중인 퀘스트 모두 전송.
	{
		GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
		GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG *pPROG = (*iter).second;

			//bReset = ResetRnattendanceQuest(pPROG);
			bIsPlayRnattendanceQuest = IsPlayRnattendanceQuest(pPROG);

			se::ByteStream cByteStream;
			pPROG->SET_BYBUFFER ( cByteStream );

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			cByteStream.GetBuffer ( pBuff, dwSize );

			if( dwSize <= MAX_QUEST_PACKET_SIZE )
			{
				GLMSG::SNETLOBBY_QUEST_PROG NetMsg(pPROG->m_sNID.dwID);
				NetMsg.dwChaNum = CharDbNum();
				NetMsg.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
				// QUEST_BUFFER_OVER
				sc::writeLogDebug(sc::string::format("MsgGameJoin (dwSize <= MAX_QUEST_PACKET_SIZE, 진행중) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", NetMsg.dwQID, NetMsg.dwSeq, NetMsg.bLastSeq, NetMsg.dwLengthStream, NetMsg.dwSize, NetMsg.dwChaNum));
#endif

				SendToAgent(&NetMsg);
			}
			else
			{
				int nRepetIdx = (dwSize / MAX_QUEST_PACKET_SIZE) + ((dwSize%MAX_QUEST_PACKET_SIZE == 0) ? 0 : 1);
				
				for( int i = 0; i < nRepetIdx; i++ )
				{
					GLMSG::SNETLOBBY_QUEST_PROG NetMsg(pPROG->m_sNID.dwID);
					NetMsg.dwSeq = i+1;
					NetMsg.dwChaNum = CharDbNum();

					if( i == nRepetIdx-1 )
					{
						NetMsg.bLastSeq = TRUE;
						NetMsg.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE), dwSize );
					}
					else
					{
						NetMsg.bLastSeq = FALSE;
						NetMsg.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE) , MAX_QUEST_PACKET_SIZE );
						dwSize -= MAX_QUEST_PACKET_SIZE;
					}
#ifdef _TEMP_TRACING_LOG
					// QUEST_BUFFER_OVER
					sc::writeLogDebug(sc::string::format("MsgGameJoin (진행중) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", NetMsg.dwQID, NetMsg.dwSeq, NetMsg.bLastSeq, NetMsg.dwLengthStream, NetMsg.dwSize, NetMsg.dwChaNum));
#endif

					SendToAgent(&NetMsg);
				}
			}

			// #item
			// 퀘스트 아이템 전송
			GLInventory::CELL_MAP_CITER iter = pPROG->m_sINVENTORY.GetItemList()->begin();
			GLInventory::CELL_MAP_CITER iter_end = pPROG->m_sINVENTORY.GetItemList()->end();
			for ( ; iter!=iter_end; ++iter )
			{
				SINVENITEM* pInvenItem = (*iter).second;
				if ( pInvenItem )
				{
					GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT2 NetMsgInven( pPROG->m_sNID.dwID, *pInvenItem );
					NetMsgInven.dwChaNum = CharDbNum();
					SendToAgent( &NetMsgInven );
				}
			}
		}
	}

	bool bIsRnAttendanceCompleted = false;
	bool bIsEndRnattendanceQuest = false;

	//	완료된 퀘스트 모두 전송.
	{
		GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestEnd().begin();
		GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestEnd().end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG *pPROG = (*iter).second;
			
			// RM# 1197 에 따른 요청사항을 더이상 필요하지않아서 제거 (20160714)
			// 이벤트용 퀘스트 버그로 인해서 특정 퀘스트 번호만 예외처리. 차후에 반드시 소스에서 정리할것
			/*
			if( pPROG != NULL )
			{
				if( (pPROG->m_sNID.dwID == 3107 || pPROG->m_sNID.dwID == 3108 || pPROG->m_sNID.dwID == 3109 || pPROG->m_sNID.dwID == 3110 || pPROG->m_sNID.dwID == 3111 || pPROG->m_sNID.dwID == 3112 || pPROG->m_sNID.dwID == 3113 || pPROG->m_sNID.dwID == 3114 ) )
				{
					CTime cTime = CTime::GetCurrentTime();
					CTime cTimeEnd(pPROG->m_tEndTime);

					if (cTime.GetDay() != cTimeEnd.GetDay() && cTime.GetMonth() == cTimeEnd.GetMonth() && cTime.GetYear() == cTimeEnd.GetYear() )
					{
						continue;
					}
				}
			}
			else
			{
				sc::writeLogError(std::string("[EVENT_QUEST] GLChar::MsgGameJoin - m_mapQuestEnd is NULL"));
			}
			*/

			

			GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );

			if ( pQUEST )
			{
				if (pQUEST->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE)
					bIsRnAttendanceCompleted = true;

				GLQuestMan::GetInstance().SetRnattendanceGQuestProg(pPROG);
			}
			bIsEndRnattendanceQuest = IsPlayRnattendanceQuest(pPROG);

			se::ByteStream cByteStream;
			pPROG->SET_BYBUFFER ( cByteStream );

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			cByteStream.GetBuffer ( pBuff, dwSize );

			if( dwSize <= MAX_QUEST_PACKET_SIZE )
			{
				GLMSG::SNETLOBBY_QUEST_END NetMsg(pPROG->m_sNID.dwID);
				NetMsg.dwChaNum = CharDbNum();
				NetMsg.SETSTREAM ( pBuff, dwSize );
#ifdef _TEMP_TRACING_LOG
				// QUEST_BUFFER_OVER
				sc::writeLogDebug(sc::string::format("MsgGameJoin (dwSize <= MAX_QUEST_PACKET_SIZE, 진행완료) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", NetMsg.dwQID, NetMsg.dwSeq, NetMsg.bLastSeq, NetMsg.dwLengthStream, NetMsg.dwSize, NetMsg.dwChaNum));
#endif
				SendToAgent(&NetMsg);
			}
			else
			{
				int nRepetIdx = (dwSize / MAX_QUEST_PACKET_SIZE) + ((dwSize%MAX_QUEST_PACKET_SIZE == 0) ? 0 : 1);
				
				for( int i = 0; i < nRepetIdx; i++ )
				{
					GLMSG::SNETLOBBY_QUEST_END NetMsg(pPROG->m_sNID.dwID);
					NetMsg.dwSeq = i+1;
					NetMsg.dwChaNum = CharDbNum();

					if( i == nRepetIdx-1 )
					{
						NetMsg.bLastSeq = TRUE;
						NetMsg.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE), dwSize );
					}
					else
					{
						NetMsg.bLastSeq = FALSE;
						NetMsg.SETSTREAM ( pBuff + (i*MAX_QUEST_PACKET_SIZE) , MAX_QUEST_PACKET_SIZE );
						dwSize -= MAX_QUEST_PACKET_SIZE;
					}
#ifdef _TEMP_TRACING_LOG
					// QUEST_BUFFER_OVER
					sc::writeLogDebug(sc::string::format("MsgGameJoin (진행완료) dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", NetMsg.dwQID, NetMsg.dwSeq, NetMsg.bLastSeq, NetMsg.dwLengthStream, NetMsg.dwSize, NetMsg.dwChaNum));
#endif

					SendToAgent(&NetMsg);
				}
			}
		}
	}
	if (bIsRnAttendanceCompleted && !bIsPlayRnattendanceQuest && !bIsEndRnattendanceQuest )
	{
		DWORD dwQuestId = GLQuestMan::GetInstance().GetRnattendanceQuestId();
		GLQUESTPROG *pQUEST_PROG = m_cQuestPlay.FindEnd( dwQuestId );
		if (pQUEST_PROG)
		{
			CTime tStartTime(pQUEST_PROG->m_tStartTime);

			m_vecAttendanceSelectQuest.clear();
			m_vecAttendanceSelectQuest = GLQuestMan::GetInstance().FindAttendanceQuest( GETLEVEL(), 
				3, 
				static_cast<unsigned>(tStartTime.GetSecond()) );

			GLMSG::SNETPC_RNATTENDANCE_QUEST_LIST_FC msg;
			{
				BOOST_FOREACH( DWORD vecAttendanceSelectQuest, m_vecAttendanceSelectQuest )
				{
					msg.vecQuestIDList.push_back( vecAttendanceSelectQuest );
				}
			}

			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, msg );
			SendToClient( NET_MSG_RNATTENDANCE_QUEST_LIST_FC, PackBuffer);
		}
	}

	//	Rnattendance 퀘스트 리셋
	if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
		GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
		for ( ; iter!=iter_end; )
		{
			GLQUESTPROG *pPROG = (*iter).second;
			if (!ResetRnattendanceQuest( pPROG, iter ))
			{
				++iter;
			}
		}
	}

	//	아이템 쿨타임 전송
	//
	{
		GLMSG::SNETLOBBY_ITEM_COOLTIME NetCoolTime;
		NetCoolTime.dwChaNum = CharDbNum();

		COOLTIME_MAP_ITER iter = m_mapCoolTimeID.begin();
		COOLTIME_MAP_ITER iter_end = m_mapCoolTimeID.end();

		NetCoolTime.emCoolType = EMCOOL_ITEMID;

		for ( ; iter!=iter_end; ++iter )
		{
			const ITEM_COOLTIME &sCoolTime = (*iter).second;

			NetCoolTime.ADD ( sCoolTime );
			if ( NetCoolTime.dwNum == EMGLMSG_COOLTIMEMAX )
			{
				SendToAgent(&NetCoolTime);
				NetCoolTime.dwNum = 0;
			}
		}

		if ( NetCoolTime.dwNum != 0 )
            SendToAgent(&NetCoolTime);

		iter = m_mapCoolTimeType.begin();
		iter_end = m_mapCoolTimeType.end();

		NetCoolTime.emCoolType = EMCOOL_ITEMTYPE;
		NetCoolTime.dwNum = 0;

		for ( ; iter!=iter_end; ++iter )
		{
			const ITEM_COOLTIME &sCoolTime = (*iter).second;

			NetCoolTime.ADD ( sCoolTime );
			if ( NetCoolTime.dwNum == EMGLMSG_COOLTIMEMAX )
			{
				SendToAgent(&NetCoolTime);
				NetCoolTime.dwNum = 0;
			}
		}

		if ( NetCoolTime.dwNum != 0 )
            SendToAgent(&NetCoolTime);
	}	

	// 인스턴스 던전 정보 전송;
	//
	{
		InstanceSystem::ScriptManager* const _pScriptManager(InstanceSystem::ScriptManager::getInstance());

		GLMSG::SNETLOBBY_INSTANCE_LIST NetInstance;
		NetInstance.dwChaNum = CharDbNum();
		
		const InstanceSystem::MapIDVector& _keyMapIDVector = _pScriptManager->getKeyMapIDVector();
		for ( InstanceSystem::MapIDVectorCIter _iteratorKeyMapID(_keyMapIDVector.begin()); _iteratorKeyMapID != _keyMapIDVector.end(); ++_iteratorKeyMapID )
		{
			const SNATIVEID& _keyMapID(*_iteratorKeyMapID);

			InstanceSystem::InstanceInformationClient _instanceInformation;
			_pScriptManager->getInstanceInformation(_keyMapID, _instanceInformation);
			if ( NetInstance.IsAddable() == false )
			{
				SendToAgent(&NetInstance);
				NetInstance.reset();
			}

			if ( false == NetInstance.add(_instanceInformation) )
			{
				sc::writeLogError( "Failed Load Instance List Key MapID!!!!!" );
			}
		}

		if ( NetInstance.nInstanceInformation != 0 )
		{
			SendToAgent(&NetInstance);
		}
	}	

	// TexasHoldem
	// 처음 필드서버에 접속할 때 칩정보 보내준다.
	m_pGaeaServer->GetTexasHoldemManager()->SendChipMap( GETCLIENTID(), true );

	// Update Hire Summon State;
	MsgSendUpdateHireSummonState( 0, m_sBasicSummonID );

	// Update Character Slot State;
	MsgSendUpdateCharacterSlotState();

	return S_OK;
}
bool GLChar::ResetRnattendanceQuest(GLQUESTPROG* pQUEST_PROG, GLQuestPlay::MAPQUEST_ITER& Iter)
{
	// Note : Rnattendance
	if ( !GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		return false;
	}

	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( pQUEST_PROG->m_sNID.dwID );
	if ( pQUEST )
	{
		CTime TToday = CTime::GetCurrentTime();

		if (pQUEST->m_sAttendanceOption.nType >= EMATTENDANCE_TYPE_CONNECTION &&
			pQUEST->m_sAttendanceOption.nType < EMATTENDANCE_TYPE_SIZE)
		{
			CTime CQuestStartTime(pQUEST_PROG->m_tStartTime);

			if ( CQuestStartTime.GetYear() != TToday.GetYear() ||
				CQuestStartTime.GetMonth() != TToday.GetMonth() ||
				CQuestStartTime.GetDay() != TToday.GetDay() )
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
							new db::ItemDelete( pInvenItem->GetGuid(), pInvenItem->GetDbState(), 89125 ) ) );
					}
				}

				pQUEST_PROG->DoGIVEUP();

				//	완료된 리스트에 추가.
				m_cQuestPlay.InsertEnd(*pQUEST_PROG, false);

				//	진행정보 삭제.
				//return m_cQuestPlay.DeleteProc ( pQUEST_PROG->m_sNID.dwID  );
				m_cQuestPlay.DeleteMobProc( pQUEST_PROG->m_sNID.dwID );
				m_cQuestPlay.DeleteItemUseProc(pQUEST_PROG->m_sNID.dwID);
				SAFE_DELETE ( (*Iter).second );
				m_cQuestPlay.GetQuestProc().erase ( Iter++ );
				return true;
			}
		}
	}
	return false;
}

bool GLChar::IsPlayRnattendanceQuest( GLQUESTPROG* pQUEST_PROG )
{
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( pQUEST_PROG->m_sNID.dwID );
	if ( pQUEST )
	{
		CTime TToday = CTime::GetCurrentTime();

		if (pQUEST->m_sAttendanceOption.nType >= EMATTENDANCE_TYPE_CONNECTION &&
			pQUEST->m_sAttendanceOption.nType < EMATTENDANCE_TYPE_SIZE &&
			pQUEST->m_sSTARTOPT.nJOIN != EMQJ_ATTENDANCE )
		{
			CTime CQuestStartTime(pQUEST_PROG->m_tStartTime);

			if ( CQuestStartTime.GetYear() == TToday.GetYear() &&
				CQuestStartTime.GetMonth() == TToday.GetMonth() &&
				CQuestStartTime.GetDay() == TToday.GetDay() )
			{
				return true;
			}
		}
	}
	return false;
}
/*
void GLChar::MsgGameJoinSendClubInfo() 
{
    std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();

		GLMSG::SNETLOBBY_CLUB_INFO NetMsgInfo;
		NetMsgInfo.dwChaNum = CharDbNum();
		NetMsgInfo.dwClubID = m_ClubDbNum;

    if ( pCLUB )
    {
        StringCchCopy ( NetMsgInfo.szClubName, CHAR_SZNAME, pCLUB->Name() );

        NetMsgInfo.dwMasterID = pCLUB->MasterCharDbNum();
        StringCchCopy ( NetMsgInfo.szMasterName, CHAR_SZNAME, pCLUB->GetMasterName() );
        NetMsgInfo.dwCDCertifior = pCLUB->m_dwCDCertifior;

        NetMsgInfo.dwRank = pCLUB->m_dwRank;;

        NetMsgInfo.dwMarkVER = pCLUB->m_dwMarkVER;

        NetMsgInfo.tOrganize = pCLUB->m_tOrganize;
        NetMsgInfo.tDissolution = pCLUB->m_tDissolution;

        NetMsgInfo.dwAlliance = pCLUB->m_dwAlliance;

        NetMsgInfo.dwMEMBER_NUM = pCLUB->GetMemberNum();
        NetMsgInfo.dwALLIANCE_NUM = pCLUB->GetAllianceNum();
        NetMsgInfo.dwBATTLE_NUM = pCLUB->GetBattleNum();

        NetMsgInfo.dwBattleWin	= pCLUB->m_dwBattleWin;
        NetMsgInfo.dwBattleLose	= pCLUB->m_dwBattleLose;
        NetMsgInfo.dwBattleDraw	= pCLUB->m_dwBattleDraw;

        if ( pCLUB->m_dwAlliance!=CLUB_NULL )
        {
            std::tr1::shared_ptr<GLClubField> pCLUB_A = GetClub(pCLUB->m_dwAlliance);
            if ( pCLUB_A )
            {
                NetMsgInfo.dwALLIANCE_NUM = pCLUB_A->GetAllianceNum();
                NetMsgInfo.dwAllianceBattleWin	= pCLUB_A->m_dwAllianceBattleWin;
                NetMsgInfo.dwAllianceBattleLose	= pCLUB_A->m_dwAllianceBattleLose;
                NetMsgInfo.dwAllianceBattleDraw	= pCLUB_A->m_dwAllianceBattleDraw;
            }
        }

        StringCchCopy ( NetMsgInfo.szNotice, EMCLUB_NOTICE_LEN+1, pCLUB->m_szNotice );
    }

    SendToAgent(&NetMsgInfo );

		if ( pCLUB )
		{
			GLMSG::SNETLOBBY_CLUB_MEMBER NetMsgMember;
			NetMsgMember.dwChaNum = CharDbNum();

        CLUBMEMBERS& Members = pCLUB->GetMembers();
        CLUBMEMBERS_ITER pos = Members.begin();
        CLUBMEMBERS_ITER end = Members.end();
        for ( ; pos!=end; ++pos)
        {
            const GLCLUBMEMBER& sMEMBER = pos->second;
            bool bOK = NetMsgMember.ADDMEMBER(sMEMBER.dwID, sMEMBER.m_dwFlags, sMEMBER.m_szName, sMEMBER.m_ChaLevel);
            if (!bOK)
            {
                // 메시지 전송.
                SendToAgent(&NetMsgMember);
                NetMsgMember.RESET();
                NetMsgMember.ADDMEMBER(sMEMBER.dwID, sMEMBER.m_dwFlags, sMEMBER.m_szName, sMEMBER.m_ChaLevel);
            }
        }

        //	Note : 메시지 전송.
        if (NetMsgMember.dwMemberNum > 0)
        {
            SendToAgent(&NetMsgMember);
            NetMsgMember.RESET();
        }
    }

    if ( pCLUB )
    {
        if ( pCLUB->m_dwAlliance!=CLUB_NULL )
        {				
            std::tr1::shared_ptr<GLClubField> pCLUB_A = GetClub(pCLUB->m_dwAlliance);
            if ( pCLUB_A )
            {
                GLMSG::SNETLOBY_CLUB_ANCE_2CLT NetMsgAlliance;

                CLUB_ALLIANCE_ITER pos = pCLUB_A->m_setAlliance.begin();
                CLUB_ALLIANCE_ITER end = pCLUB_A->m_setAlliance.end();
                for ( ; pos!=end; ++pos )
                {
                    const GLCLUBALLIANCE &sALLIANCE = (*pos);
		if ( pCLUB )
		{
			if ( pCLUB->m_dwAlliance!=CLUB_NULL )
			{				
				GLClubField* pCLUB_A = GetClub(pCLUB->m_dwAlliance);
				if ( pCLUB_A )
				{
					GLMSG::SNETLOBY_CLUB_ANCE_2CLT NetMsgAlliance;
					NetMsgAlliance.dwChaNum = CharDbNum();

                    bool bOK = NetMsgAlliance.ADDALLIANCE ( sALLIANCE.m_dwID, sALLIANCE.m_szName );
                    if ( !bOK )
                    {
                        //	Note : 메시지 전송.
                        SendToAgent(&NetMsgAlliance );
                        NetMsgAlliance.RESET();

                        NetMsgAlliance.ADDALLIANCE ( sALLIANCE.m_dwID, sALLIANCE.m_szName );
                    }
                }

                //	Note : 메시지 전송.
                if ( NetMsgAlliance.dwAllianceNum > 0 )
                {
                    SendToAgent(&NetMsgAlliance );
                    NetMsgAlliance.RESET();
                }
            }
        }
    }

    if ( pCLUB )
    {
        GLMSG::SNETLOBBY_CLUB_BATTLE NetMsgBattle;

        CLUB_BATTLE_ITER pos = pCLUB->m_mapBattle.begin();
        CLUB_BATTLE_ITER end = pCLUB->m_mapBattle.end();
        for ( ; pos!=end; ++pos )
        {
            const GLCLUBBATTLE &sBattle = (*pos).second;
            GLCLUBBATTLE_LOBY sBattleLoby = sBattle;
            bool bOK = NetMsgBattle.ADDBATTLE ( sBattleLoby );
            if ( !bOK )
            {
                //	Note : 메시지 전송.
                SendToAgent(&NetMsgBattle );
                NetMsgBattle.RESET();
		if ( pCLUB )
		{
			GLMSG::SNETLOBBY_CLUB_BATTLE NetMsgBattle;
			NetMsgBattle.dwChaNum = CharDbNum();

                NetMsgBattle.ADDBATTLE ( sBattleLoby );
            }
        }

        //	Note : 메시지 전송.
        if ( NetMsgBattle.dwBattleNum > 0 )
        {
            SendToAgent(&NetMsgBattle );
            NetMsgBattle.RESET();
        }
    }
}
*/
/*
HRESULT GLChar::MsgClubInfo()
{
	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if (!pCLUB)
        return S_OK;

	GLMSG::SNET_CLUB_INFO_2CLT NetMsgInfo;
	NetMsgInfo.dwChaNum = CharDbNum();
	NetMsgInfo.dwClubID = m_ClubDbNum;
	NetMsgInfo.SetClubName(pCLUB->Name());
	NetMsgInfo.dwMasterID = pCLUB->MasterCharDbNum();
	NetMsgInfo.SetMasterName(pCLUB->GetMasterName());
	NetMsgInfo.dwCDCertifior = pCLUB->m_dwCDCertifior;
	NetMsgInfo.SetNickName(m_szNick);
	NetMsgInfo.dwMarkVER = pCLUB->m_dwMarkVER;
	NetMsgInfo.dwRank = pCLUB->m_dwRank;
	NetMsgInfo.tOrganize = pCLUB->m_tOrganize;
	NetMsgInfo.tDissolution = pCLUB->m_tDissolution;
	NetMsgInfo.SetNotice(pCLUB->m_szNotice);

	SendToAgent(&NetMsgInfo);

	GLMSG::SNET_CLUB_MEMBER_2CLT NetMsgMember;    
	NetMsgMember.dwChaNum = CharDbNum();	
    CLUBMEMBERS& Members = pCLUB->GetMembers();
    CLUBMEMBERS_CITER pos = Members.begin();
    CLUBMEMBERS_CITER end = Members.end();
	for ( ; pos!=end; ++pos)
	{
		const GLCLUBMEMBER& sMEMBER = pos->second;
		bool bFULL = NetMsgMember.ADDMEMBER(sMEMBER.m_ChaDbNum, sMEMBER.m_Grade, sMEMBER.m_ChaName.c_str(), sMEMBER.m_ChaLevel, sMEMBER.m_LastPlayTime);
		if (bFULL)
		{
			//	Note : 메시지 전송.
			SendToAgent(&NetMsgMember);
			NetMsgMember.RESET();
			NetMsgMember.ADDMEMBER(sMEMBER.m_ChaDbNum, sMEMBER.m_Grade, sMEMBER.m_ChaName.c_str(), sMEMBER.m_ChaLevel, sMEMBER.m_LastPlayTime);
		}
	}

	// 메시지 전송
	if (NetMsgMember.dwMemberNum > 0)
	{
		NetMsgMember.dwChaNum = CharDbNum();
		SendToAgent(&NetMsgMember);
		NetMsgMember.RESET();
	}
	return S_OK;
}
*/
GLMSG::SNETDROP_PC_BASE GLChar::ReqNetMsg_Drop_Base()
{
    GLMSG::SNETDROP_PC_BASE NetMsg;
    NetMsg = GLMSG::SNETDROP_PC_BASE();
    SDROP_CHAR_BASE& BaseData = NetMsg.m_Data;

	BaseData.SetName(m_szName);
	BaseData.emTribe = m_emTribe;
	BaseData.emClass = m_emClass;
	BaseData.wSchool = m_wSchool;
	BaseData.wHair = m_wHair;
	BaseData.wHairColor = m_wHairColor;
	BaseData.wFace = m_wFace;
	BaseData.wSex = m_wSex;
	BaseData.nBright = m_nBright;
	BaseData.dwCharID = m_CharDbNum;
	BaseData.wLevel = m_wLevel;
	BaseData.dwGuild = m_ClubDbNum;

	//
	//mjeon.sns
	//
	BaseData.byFB = (BYTE)(BOOL)(strlen(m_AUTHFB.SKey) > 0 && strlen(m_AUTHFB.UID) > 0);
	BaseData.byTW = (BYTE)(BOOL)(strlen(m_AUTHTW.AToken) > 0 && strlen(m_AUTHTW.ATokenS) > 0 && strlen(m_AUTHTW.UID) > 0);

	TITLEINFO ti;
	m_pTitleMan->GetCurrentTitle(ti);;
	
	BaseData.byActivityClass = (BYTE)(UINT)ti.nClass;
	BaseData.SetActivityTitle(ti.strTitle);

	BaseData.m_bItemShopOpen = m_bItemShopOpen;
    switch ( GetAction() )
    {
    case GLAT_MOTION:
        BaseData.m_dwANIMAINTYPE = (DWORD)m_eMotionMID;
        BaseData.m_dwANISUBTYPE = (DWORD)m_eMotionSID;
        BaseData.m_fAniRemainTime = m_fMotionSec;
        break;
    default:
        BaseData.m_dwANISUBTYPE = m_dwANISUBSELECT;
        break;
    }

	if (BaseData.dwGuild != CLUB_NULL)
	{
		std::tr1::shared_ptr<GLClubField> pCLUB = GetClub(BaseData.dwGuild);
		if (pCLUB)
		{
			BaseData.dwGuildMarkVer = pCLUB->m_dwMarkVER;
			BaseData.dwGuildMaster = pCLUB->m_MasterCharDbNum;			
			if (pCLUB->IsMemberFlgCDCertify(m_CharDbNum))
                BaseData.dwFLAGS |= SDROP_CHAR_BASE::CLUB_CD;

			//BaseData.SetClubName(pCLUB->m_szName);			
			BaseData.dwAlliance = pCLUB->m_dwAlliance;
		}
//		else
//		{
//			TEXTCONSOLEMSG_WRITE ( _T("ERROR : CLUBID_ERROR CharID %d ClubID %d"), m_dwCharID, m_dwGuild );
//		}

		BaseData.SetNickName(m_szNick);

	}

	if ( isValidParty() )
	{
		const GLPartyField* const pParty = GetMyParty();
		if ( pParty )
		{
			BaseData.PartyID = GetPartyID();
			BaseData.dwPMasterID = pParty->GetMasterGaeaID();
		}
	}

	BaseData.sHP = m_sHP;
	BaseData.dwGaeaID = m_dwGaeaID;
	BaseData.sMapID = m_mapID.getBaseMapID();
	BaseData.dwCeID = m_dwCeID;
	BaseData.vPos = m_vPosition;
	BaseData.vDir = m_vDirect;

	BaseData.Action = GetAction();
	BaseData.dwActState = GetActState();
	BaseData.vTarPos	= m_TargetID.vPos;
	BaseData.m_fCDMSafeTime = m_fCDMSafeTime;

	BaseData.m_bGM = (m_dwUserLvl >= USER_USER_GM) ? TRUE : FALSE;

	if (BaseData.Action == GLAT_MOVE)
	{
		if (m_actorMove.PathIsActive())
			BaseData.vTarPos = m_actorMove.GetTargetPosition();
		else
			BaseData.Action = GLAT_IDLE;
	}

	//if ( m_sVehicle.IsActiveValue() )
	//{
	//	dropChar.m_bVehicle = m_emVehicle;
	//	dropChar.m_sVehicle.m_dwGUID = m_sVehicle.m_dwGUID;
	//	dropChar.m_sVehicle.m_emTYPE = m_sVehicle.m_emTYPE;
	//	dropChar.m_sVehicle.m_sVehicleID = m_sVehicle.m_sVehicleID;

	//	for ( int i =0; i < ACCE_TYPE_SIZE; ++i )
	//	{
	//		dropChar.m_sVehicle.m_PutOnItems[i].Assign( m_sVehicle.m_PutOnItems[i] );
	//	}
	//}

	BaseData.m_bUseArmSub = m_bUseArmSub;

	BaseData.m_bGM = (m_dwUserLvl >= USER_USER_GM) ? TRUE : FALSE;

	if (m_fGenAge < 0.6f)
		BaseData.dwFLAGS |= SDROP_CHAR_BASE::CHAR_GEN;

	BaseData.sPASSIVE_SKILL = m_sSUM_PASSIVE;
	BaseData.sQITEMFACT = m_sQITEMFACT;
	BaseData.sEVENTFACT = m_pGaeaServer->m_sEVENTFACT;	

	BaseData.dwCountryID = m_sCountryInfo.dwID;

    return NetMsg;
}

GLMSG::SNETDROP_PC_STATEBLOW_SKILLFACT GLChar::ReqNetMsg_Drop_StateBlow_SkillFact()
{
    GLMSG::SNETDROP_PC_STATEBLOW_SKILLFACT NetMsg( CharDbNum() );

    for (int i=0; i<SKILLFACT_SIZE; ++i)
        NetMsg.sSKILLFACT[i].Assign(m_sSKILLFACT[i], i);

    for (int i=0; i<EMBLOW_MULTI; ++i)
        NetMsg.sSTATEBLOWS[i] = m_sSTATEBLOWS[i];

    return NetMsg;
}

GLMSG::SNETDROP_PC_PUT_ON_ITEMS GLChar::ReqNetMsg_Drop_PutOnItems()
{
    GLMSG::SNETDROP_PC_PUT_ON_ITEMS NetMsg( CharDbNum() );

    for (int i=0; i<SLOT_NSIZE_S_2; ++i)
        NetMsg.m_PutOnItems[i].Assign(m_PutOnItems[i]);

    return NetMsg;
}

bool GLChar::RegisterViewCrow(EMCROW emCrow, DWORD Id, SFIELDCROW* pCrow)
{
    if ( !pCrow )
        return false;
    
    if (emCrow == CROW_MOB || emCrow == CROW_ITEM || emCrow == CROW_MONEY ||
        emCrow == CROW_PET || emCrow == CROW_SUMMON || emCrow == CROW_MATERIAL ||
        emCrow == CROW_PC)
    {
        m_ViewCrow.erase(CROW_PAIR(emCrow, Id));
        m_ViewCrow.insert(VIEW_CROW_MAP_VALUE(CROW_PAIR(emCrow, Id), pCrow));
        return true;
    }
    else
    {
        return false;
    }    
}

void GLChar::RegisterViewFlyCamChar(EMCROW emCrow, DWORD Id)
{
	GLChar* pFlyCamChar = GetChar(Id);
	if ( !pFlyCamChar || emCrow != CROW_PC )
		return ;

	if ( !pFlyCamChar->m_bFlyCameraUse )
		return;

	SFIELDCROW* pCharCrow = GetViewCrow(CROW_PC, Id);
	if ( pCharCrow == NULL )
	{
		PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();
		pFIELDCROW->emCROW = CROW_PC;
		pFIELDCROW->dwID = Id;
		pFIELDCROW->invisible = pFlyCamChar->IsActState(EM_REQ_VISIBLENONE + EM_REQ_VISIBLEOFF + EM_ACT_OBSERVE) == true ? false : true;
		m_ViewCrow.erase(CROW_PAIR(emCrow, Id));
		m_ViewCrow.insert(VIEW_CROW_MAP_VALUE(CROW_PAIR(emCrow, Id), pFIELDCROW));
	}
}

//void GLChar::UnregisterViewCrow(EMCROW emCrow, DWORD Id)
//{
//    m_ViewCrow.erase(CROW_PAIR(emCrow, Id));
//}

GLChar::VIEW_CROW_MAP_CITER GLChar::GetViewCrowBegin()
{
    return m_ViewCrow.begin();
}

GLChar::VIEW_CROW_MAP_CITER GLChar::GetViewCrowEnd()
{
    return m_ViewCrow.end();
}

SFIELDCROW* GLChar::GetViewCrow(EMCROW emCrow, DWORD GaeaId)
{
    VIEW_CROW_MAP_ITER iter = m_ViewCrow.find(CROW_PAIR(emCrow, GaeaId));
    if (iter != m_ViewCrow.end())
        return iter->second;
    else
        return NULL;    
}

void GLChar::SETFIELDLANDEVENT(SFIELDLANDEVENT* pFIELDLANDEVENT)
{
	GASSERT(pFIELDLANDEVENT);
    if (!pFIELDLANDEVENT)
        return;

	GASSERT(pFIELDLANDEVENT->dwID < MAXLANDEVENT);	
	if (pFIELDLANDEVENT->dwID >= MAXLANDEVENT)
		return;

	//m_arrayFieldLandEvent[pFIELDLANDEVENT->dwID] = pFIELDLANDEVENT;
    LAND_EVENT_MAP_ITER iter = m_LandEventMap.find(pFIELDLANDEVENT->dwID);
    if (iter != m_LandEventMap.end())
        m_LandEventMap.erase(iter);
    m_LandEventMap.insert(LAND_EVENT_MAP_VALUE(pFIELDLANDEVENT->dwID, pFIELDLANDEVENT));
}

SFIELDLANDEVENT* GLChar::GetLandEvent(DWORD Id)
{
    LAND_EVENT_MAP_ITER iter = m_LandEventMap.find(Id);
    if (iter != m_LandEventMap.end())
        return iter->second;
    else
        return NULL;
}

void GLChar::RESETFIELDLANDEVENT ( DWORD dwID )
{
	GASSERT(dwID < MAXLANDEVENT);
	if (dwID >= MAXLANDEVENT)
		return;

	//m_arrayFieldLandEvent[dwID] = NULL;
    LAND_EVENT_MAP_ITER iter = m_LandEventMap.find(dwID);
    if (iter != m_LandEventMap.end())
        m_LandEventMap.erase(iter);
}

const BOOL GLChar::AddFieldCrow ( const EMCROW emCrow, const DWORD dwGUID )
{
	if (!m_pGaeaServer)
		return FALSE;

	SFIELDCROW* pFIELDCROW = NEW_FIELDCROW();

	// 값 설정
	pFIELDCROW->emCROW  = emCrow;
	pFIELDCROW->dwID    = dwGUID;
	pFIELDCROW->dwFRAME = m_dwViewFrame;
	//pFIELDCROW->pNODE   = pNODE;

    // 리스트에 등록.
    if (RegisterViewCrow(emCrow, dwGUID, pFIELDCROW))
    {
        return TRUE;		
	}
    else
    {
        RELEASE_FIELDCROW( pFIELDCROW );
        return FALSE;
    }
}

const BOOL GLChar::AddFieldLandEvent ( const DWORD dwGUID )
{
	if (!m_pGaeaServer)
		return FALSE;

	SFIELDLANDEVENT* pFIELDLANDEVENT = m_pGaeaServer->NEW_FIELDLANDEVENT();

	//	리스트에 등록.
	//sc::SGLNODE<SFIELDLANDEVENT*>* pNODE = m_ArrayFIELDLANDEVENTLIST.ADDTAIL ( pFIELDLANDEVENT );

	//	값 설정.
	pFIELDLANDEVENT->dwID    = dwGUID;
	pFIELDLANDEVENT->dwFRAME = m_dwViewFrame;
	//pFIELDLANDEVENT->pNODE   = pNODE;

	//	배열에 등록.
	SETFIELDLANDEVENT ( pFIELDLANDEVENT );

	return TRUE;
}

//	Note : 새로 게임에 참가시, 다른 맵으로 진입시에 PC 주변의
//		모든 개체에 대한 정보를 인지하기 위해서 호출함.
//		( 클라이언트 측의 개체에 이전 정보가 모두 리샛되엇음을 가정한다. )
//
HRESULT GLChar::GetViewAround ()
{
	DxMsgServer *pMsgServer = m_pGaeaServer->GetMsgServer();
	if ( pMsgServer==NULL )
		return E_FAIL;

	const MapID& mapID(GetCurrentMap());
	const SNATIVEID baseMapID(mapID.getBaseMapID());
	const SNATIVEID gaeaMapID(mapID.getGaeaMapID());	
	GLLandMan* const pLand(m_pGaeaServer->GetLand(gaeaMapID));
	if ( pLand == NULL )
	{
		sc::writeLogError(sc::string::format("GetViewAround() : pLand == NULL, dbNum:%1% CharMapID(%2%/%3%, %4%/%5%)", 
			m_CharDbNum, 
			baseMapID.Mid(), baseMapID.Sid(), gaeaMapID.Mid(), gaeaMapID.Sid()));		
	}
	else
	{
		int nX = int ( GetPosition().x );
		int nZ = int ( GetPosition().z );	

		LANDQUADTREE* pLandTree = pLand->GetLandTree();
		LANDQUADNODE* pQuadNode = NULL;
		const int _maxViewRange( MAX_VIEWRANGE );
		BOUDRECT bRect(nX+_maxViewRange,nZ+_maxViewRange,nX-_maxViewRange,nZ-_maxViewRange);
		pLandTree->FindNodes ( bRect, pLandTree->GetRootNode(), &pQuadNode );

		// 가시 영역에 있는 모든 셀의 오브젝트들을 탐색한다
		for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
		{
			GLLandNode *pLandNode = pQuadNode->pData;
			GASSERT(pLandTree->GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");
			GetViewAroundSummon(pLandNode);
			GetViewAroundPet(pLandNode);
			GetViewAroundChar(pLandNode);
			GetViewAroundCrow(pLandNode);
			GetViewAroundMaterial(pLandNode);
			GetViewAroundItem(pLandNode);
			GetViewAroundMoney(pLandNode);
			GetViewAroundLandEvent(pLandNode);
		}

		if (pLand->m_bClubDeathMatchMap||IsCTFPlayer())
			m_fMoveDelay = 0.0f;
		else
			m_fMoveDelay = GLCONST_CHAR::fPK_SAFE_TIME;

		//	Note : GLLandMan의 셀에 등록하는 작업.
		//
		pLand->DropChar(this);

		//	Note : 날씨 초기화.
		//
		GLMSG::SNETPC_WEATHER NetMsg;
		DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( baseMapID.wMainID, baseMapID.wSubID );
		NetMsg.dwWeather = pLand->IsWeatherActive() ? dwWeather : NULL;
		SendToClient( &NetMsg );

		//	지역 특별 정보.
		GLMSG::SNETPC_LAND_INFO NetMsgLandInfo;
		NetMsgLandInfo.nidMAP = pLand->GetBaseMapID();
		NetMsgLandInfo.bClubBattle = pLand->m_bGuidBattleMap;
		NetMsgLandInfo.bClubBattleHall = pLand->m_bGuidBattleMapHall;
		NetMsgLandInfo.bClubDeathMatch = pLand->m_bClubDeathMatchMap;
		NetMsgLandInfo.bClubDeathMatchHall = pLand->m_bClubDeathMatchMapHall;
		NetMsgLandInfo.bPK = pLand->IsPKZone();
		NetMsgLandInfo.fCommission = pLand->GetCommissionRate();
		NetMsgLandInfo.dwGuidClub = pLand->m_dwGuidClubID;
		NetMsgLandInfo.bLunchBoxForbid = pLand->IsLunchBoxForbid();

		// http://crm.mincoms.com/Ticket/TicketView/275
		// 클럽락커 기능 수정 요청
		if ( pLand->m_bGuidBattleMap )
		{
			NetMsgLandInfo.dwClubMapID = pLand->m_dwClubMapID;
			GLGuidance* pGuid = m_pGaeaServer->GuidanceFind( pLand->m_dwClubMapID );
			if ( pGuid && pGuid->IsBattle() )
			{
				NetMsgLandInfo.bClubBattleInProgress = true;
			}
		}

		std::tr1::shared_ptr<GLClubField> pCLUB = GetClub(pLand->m_dwGuidClubID);
		if ( pCLUB )
		{
			NetMsgLandInfo.dwGuidClubMarkVer = pCLUB->m_dwMarkVER;
			StringCchCopy ( NetMsgLandInfo.szGuidClubName, CLUB_NAME_LENGTH, pCLUB->Name() );
		}
		SendToClient(&NetMsgLandInfo );
	}

	SetActState(EM_GETVA_AFTER);


	//	Note : Preiod Sync
	//
	MsgSendPeriod ();
	

	//	퀘스트의 경과된 시간 업데이트.
	MsgSendQuestTime ();

	//	기간이 만료된 아이템을 소멸.
	RESET_TIMELMT_ITEM ();
	RESET_CHECK_ITEM ();

	//	Memo :	무기 스왑 때문에 아이템 값을 다시 계산한다.
	INIT_DATA( FALSE, FALSE );

	//	PK 관련 이벤트 정보를 전송한다. ( 학원, 성향, PK서버 ) 
	GLMSG::SNETPC_SERVER_PKEVENT_INFO	NetMsgPKInfo;
	NetMsgPKInfo.bSCHOOL_FREEPK = SchoolFreePkOn();
	NetMsgPKInfo.bBRIGHTEVENT = IsBRIGHTEVENT();
	NetMsgPKInfo.bPKServer = IsPKServer();
	SendToClient(&NetMsgPKInfo );


	// 선도전 시작 유무
	if ( m_pGaeaServer->IsGuidanceBattle() )
	{
		GLMSG::SNETPC_SERVER_CLUB_GUID_BATTLE_INFO NetMsgClubBattle;
		NetMsgClubBattle.bClubBattle = true;
		SendToClient(&NetMsgClubBattle );

		GLMSG::SNETPC_SERVER_CLUB_GUID_BATTLE_REMAIN_AG NetMsgReq;
		NetMsgReq.dwChaNum = CharDbNum();
		NetMsgReq.dwGaeaID = m_dwGaeaID;
		SendToAgent(&NetMsgReq );
	}

	if ( m_pGaeaServer->IsClubDMStarted() )
	{
		GLMSG::SNETPC_SERVER_CLUB_DEATHMATCH_INFO NetMsgClubDM;
		NetMsgClubDM.bClubDeathMatch = true;
		SendToClient(&NetMsgClubDM );

		GLMSG::SNETPC_SERVER_CLUB_DEATHMATCH_REMAIN_AG NetMsgReq;
		NetMsgReq.dwGaeaID = m_dwGaeaID;
		SendToAgent(&NetMsgReq );
	}

	

	//	Memo :	?아이템 이벤트 유무
	GLMSG::SNETPC_EVENTFACT_INFO NetMsgEvent;
	NetMsgEvent.sEVENTFACT = m_sEVENTFACT = m_pGaeaServer->m_sEVENTFACT;

    // Check IP Event.
    if ( m_pGaeaServer->m_sEVENTFACT.bIPEvent )
    {
        bool bIPEvent;
        std::string& ClinetIP = m_pGaeaServer->GetClientIP( GETCLIENTID() );
        if ( ran_event::GLIPEvent::Instance()->IsAbleToEvent( ClinetIP ) )
            bIPEvent = true;
        else
            bIPEvent = false;

        NetMsgEvent.sEVENTFACT.bIPEvent = m_sEVENTFACT.bIPEvent = bIPEvent;

        ReceiveIPEvent( bIPEvent );
    }
    //
	SendToClient(&NetMsgEvent);
	
	//	?아이템 리셋.
	GLMSG::SNETPC_QITEMFACT_END_BRD	NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	if ( m_sQITEMFACT.IsACTIVE() )
	{
		m_sQITEMFACT.RESET();

		//	Note : 종료되었을 경우 주변 사람에게 알림.
		SendMsgViewAround(&NetMsgBrd);
	}

	//	Note : 종료되었을 경우 자신에게 알림.
	SendToClient(&NetMsgBrd );

	// Note : 걸려있는 SAVE SKILLFACT 정보를 보낸다.
	SaveSkillFact();

	GLMSG::SNETPC_SAVESKILLFACT_BRD NetSkillFactInfo;
	NetSkillFactInfo.dwID   = m_dwGaeaID;
	NetSkillFactInfo.emCrow = CROW_PC;

	for ( int i=0; i<SKILLFACT_SIZE; ++i )
	{
		SCHARSAVESKILLFACT &sSAVESKILLFACT  = m_sSAVESKILLFACT[i];
		WORD			   wSlot            = m_sSAVESKILLFACT[i].wSLOT;

		if ( sSAVESKILLFACT.sNATIVEID == NATIVEID_NULL() )
		{
			continue;
		}

		/// 도시락의 경우 무제한으로 존재할 수 없다;
		/// Age 가 0 이하일 수 없다;
		EMSKILLFACT_TYPE emType = EMSKILLFACTTYPE_BY_INDEX ( sSAVESKILLFACT.wSLOT );
		SSKILLFACT::STIME sTime( sSAVESKILLFACT );
		if ( sTime.fAGE < 0.f ||
			( _SKILLFACT_INFINITY == sTime.fAGE &&
			( EMSKILLFACT_TYPE_EXP_LUNCHBOX == emType || EMSKILLFACT_TYPE_LUNCHBOX == emType ) ) )
		{
			sc::writeLogError( sc::string::format(
				"[ Skill ] [ Invalid SAVESKILLFACT %1%. ID : %2%/%3%, Age : %4%, Life : %5%, IsEXPIREDTIME : %6% ]",
				sSAVESKILLFACT.wSLOT,
				sSAVESKILLFACT.sNATIVEID.Mid(),
				sSAVESKILLFACT.sNATIVEID.Sid(),
				sTime.fAGE,
				sTime.fLIFE,
				sTime.IsEXPIREDTIME ? "TRUE" : "FALSE" ) );

			sSAVESKILLFACT.Reset();

			continue;
		}

		if ( NetSkillFactInfo.addSaveSkillFact(sSAVESKILLFACT) == false )
		{
			SendToClient(&NetSkillFactInfo);
			NetSkillFactInfo.reset();
		}
	}

	if ( NetSkillFactInfo.isEmpty() == false )
		SendToClient(&NetSkillFactInfo );
	SendMsgViewAround(&NetMsgBrd);

	// 서버의 현재시간을 클라이언트에 알림
	GLMSG::SNET_MSG_SERVERTIME_BRD NetMsgTime;
	CTime cServerTime = CTime::GetCurrentTime();
	
	NetMsgTime.t64Time = cServerTime.GetTime();


	tm t1, t2;
	cServerTime.GetLocalTm(&t1);
	cServerTime.GetGmtTm(&t2);
	
	__time64_t tLocalTime = _mktime64( &t1 );
	__time64_t tUtcTime = _mktime64( &t2 );;
	__time64_t tSpan = tLocalTime - tUtcTime;
	
	NetMsgTime.nTimeBias = (int)(tSpan / 3600); 

	SendToClient(&NetMsgTime );
	return S_OK;
}

HRESULT GLChar::MsgFieldMoveReset ()
{
	//	Note : 클럽 정보 리셋.
	if ( m_ClubDbNum!=CLUB_NULL )
	{
		std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
		if ( pCLUB && pCLUB->MasterCharDbNum()==m_CharDbNum )
		{
			GLMSG::SNET_CLUB_STORAGE_RESET NetMsgClubStorageReset;
			SendToClient(&NetMsgClubStorageReset );
		}
	}

	return S_OK;
}

void GLChar::SendMsgSkillSucessResult( const SNATIVEID& idSkill, const WORD wLevel, const D3DXVECTOR3& vTARPOS, const STARGETID& targetID )
{
	GLMSG::SNETPC_REQ_SKILL_BRD		NetMsgBRD;
	NetMsgBRD.emCrow	= GETCROW();
	NetMsgBRD.dwID		= m_dwGaeaID;		//- 시전자 정보. 
	NetMsgBRD.skill_id	= idSkill;
	NetMsgBRD.wLEVEL	= wLevel;		//- 시전 스킬 종류.
	NetMsgBRD.vTARPOS	= vTARPOS;

	for( WORD i=0; i<m_wTARNUM; ++i )
		NetMsgBRD.ADDTARGET ( m_sTARIDS[i] );

	SendMsgViewAround(&NetMsgBRD);

	//	Note : 스킬 구동 성공 FB 메시지.
	GLMSG::SNETPC_REQ_SKILL_FB NetMsgFB;
	NetMsgFB.sTargetID = targetID;
	NetMsgFB.sSkillID = idSkill;
	NetMsgFB.wLevel = wLevel;
	NetMsgFB.emSKILL_FB = EMSKILL_OK;
	
	SendToClient( &NetMsgFB );

	//CONSOLEMSG_WRITE( "MsgReqSkill End [%u]", skill_id.dwID );

	// 몹을 공격하면 팻이 공격모션을 취함
	if( targetID.emCrow==CROW_MOB )
	{
		PGLPETFIELD pMyPet = GetMyPet();
		if( pMyPet && pMyPet->IsValid () && !pMyPet->IsSTATE ( EM_PETACT_ATTACK ) )
		{
			D3DXVECTOR3 vDist;
			float fDist;
			vDist = pMyPet->m_vPos - m_vPosition;
			fDist = D3DXVec3Length(&vDist);

			// 일정거리 안에 있으면 
			if( fDist <= GLCONST_PET::fWalkArea )
			{
				GLMSG::SNETPET_ATTACK NetMsg(targetID);
				SendToClient( &NetMsg );

				GLMSG::SNETPET_ATTACK_BRD NetMsgBRD(targetID);
				NetMsgBRD.dwGUID = m_dwPetGUID;
				SendMsgViewAround(&NetMsgBRD);
			} //if
		} //if
	} //if
} //GLChar::SendMsgSkillSucessResult

bool GLChar::CheckBelongViewAround( GLChar* pTarChar )
{
	if ( NULL == pTarChar )
		return false;

	for ( VIEW_CROW_MAP_ITER iter = m_ViewCrow.begin();
		iter!=m_ViewCrow.end(); ++iter)
	{        
		SFIELDCROW* pCharCrow = iter->second;
		if ( pCharCrow->emCROW != CROW_PC )
			continue;

		GLChar* pChar = GetChar( pCharCrow->dwID );
		if ( NULL == pChar )
			continue;

		if ( pChar->CharDbNum() == pTarChar->CharDbNum() )
			return true;
	}

	return false;
}

HRESULT GLChar::SendMsgViewAround( NET_MSG_GENERIC* nmg )
{  
	/*if ( (nmg->nType == NET_MSG_CHAT_LINK_FB) && (m_bTracingUser == true) )
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		GLMSG::NET_CHAT_LINK_FB* pPacket = (GLMSG::NET_CHAT_LINK_FB*) nmg;
		TracingMsg.nUserNum = GetUserID();
		TracingMsg.SetAccount(m_szUID);

		CString strTemp;
		strTemp.Format("**Normal Send**, [%s][%s], %s", m_szUID, m_szName, pPacket->szChatMsg);

		TracingMsg.SetLog(strTemp.GetString());

		SendToAgent(&TracingMsg);
	}*/

    for (VIEW_CROW_MAP_ITER iter=m_ViewCrow.begin(); iter!=m_ViewCrow.end(); ++iter)
    {        
        SFIELDCROW* pCharCrow = iter->second;
        if (pCharCrow->emCROW != CROW_PC)
            continue;

        GLChar* pPChar = GetChar(pCharCrow->dwID);
        //	Note : 캐릭터의 삭제 시점과 UpdateViewAround ()과의 불일치로 캐릭터가 존재하지 않을 수 있음.
        if (pPChar)
        {
            pPChar->SendToClient(nmg);

            // 만일 일반 체팅 메시지일 경우에 추적로그를 남겨야하므로 체크를 해서 agent에 로그를 보낸다.
            if (nmg->nType == NET_MSG_CHAT_LINK_FB)
            {
				// 모든 국가에서 Tracing 기능을 사용할 수 있도록 변경
                //if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
                //{
                    /*if (pPChar->m_bTracingUser)
                    {
                        GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
                        GLMSG::NET_CHAT_LINK_FB* pPacket = (GLMSG::NET_CHAT_LINK_FB*) nmg;
                        TracingMsg.nUserNum = pPChar->GetUserID();
                        TracingMsg.SetAccount(pPChar->m_szUID);

                        CString strTemp;
                        strTemp.Format("**Normal Recv**, [%s][%s]->[%s][%s], %s", m_szUID, m_szName, pPChar->m_szUID, pPChar->m_szName, pPacket->szChatMsg);

                        TracingMsg.SetLog(strTemp.GetString());

                        pPChar->SendToAgent(&TracingMsg);
                    }*/
                //}
            }
        }
    }
	return S_OK;
}

HRESULT GLChar::SendMsgViewAround(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, Buffer, UseCrc))
        return SendMsgViewAround(&MsgPack);
    else
        return E_FAIL;
}

HRESULT GLChar::SendMsgViewAroundRelation(const Faction::EMRELATION_FLAG_TYPE emRelation, NET_MSG_GENERIC* nmg )
{  
   /* if ( (nmg->nType == NET_MSG_CHAT_LINK_FB) && (m_bTracingUser == true) )
    {
        GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
        GLMSG::NET_CHAT_LINK_FB* pPacket = (GLMSG::NET_CHAT_LINK_FB*) nmg;
        TracingMsg.nUserNum = GetUserID();
        TracingMsg.SetAccount(m_szUID);

        CString strTemp;
        strTemp.Format("**Normal Send**, [%s][%s], %s", m_szUID, m_szName, pPacket->szChatMsg);

        TracingMsg.SetLog(strTemp.GetString());

        SendToAgent(&TracingMsg);
    }*/

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

    Faction::ManagerField* pFactionMan = pLand->GetFactionManager();
    if (pFactionMan == NULL )
        return E_FAIL;

    for (VIEW_CROW_MAP_ITER iter=m_ViewCrow.begin(); iter!=m_ViewCrow.end(); ++iter)
    {        
        SFIELDCROW* pCharCrow = iter->second;
        if (pCharCrow->emCROW != CROW_PC)
            continue;

        GLChar* pPChar = GetChar(pCharCrow->dwID);
        //	Note : 캐릭터의 삭제 시점과 UpdateViewAround ()과의 불일치로 캐릭터가 존재하지 않을 수 있음.
        if (pPChar == NULL )
            continue;

        const Faction::GLActorID _ActorID(CROW_PC, GetCharID());
        const Faction::GLActorID _TargetActorID(CROW_PC, pPChar->GetCharID());
        const Faction::EMRELATION_TYPE emRelaition = pFactionMan->getRelation(_ActorID, _TargetActorID);
        switch ( emRelaition )
        {
        case Faction::EMRELATION_ALLY:
            {
                if ( emRelation & Faction::EM_RELATION_FLAG_ALLY )
                    SendToClient(nmg);
            }
            break;
        case Faction::EMRELATION_ENEMY:
            {
                if ( emRelation & Faction::EM_RELATION_FLAG_ENEMY )
                    SendToClient(nmg);
            }
            break;
        case Faction::EMRELATION_NEUTRAL_ENEMY:
		case Faction::EMRELATION_NEUTRAL_ALLY:
            {
                if ( emRelation & Faction::EM_RELATION_FLAG_NEUTRAL )
                    SendToClient(nmg);
            }
            break;
        default:
            continue;
        }
        // 만일 일반 체팅 메시지일 경우에 추적로그를 남겨야하므로 체크를 해서 agent에 로그를 보낸다.
        if (nmg->nType == NET_MSG_CHAT_LINK_FB)
        {
			// 모든 국가에서 Tracing 기능을 사용할 수 있도록 변경
            //if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
            //{
                /*if (pPChar->m_bTracingUser)
                {
                    GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
                    GLMSG::NET_CHAT_LINK_FB* pPacket = (GLMSG::NET_CHAT_LINK_FB*) nmg;
                    TracingMsg.nUserNum = pPChar->GetUserID();
                    TracingMsg.SetAccount(pPChar->m_szUID);

                    CString strTemp;
                    strTemp.Format("**Normal Recv**, [%s][%s]->[%s][%s], %s", m_szUID, m_szName, pPChar->m_szUID, pPChar->m_szName, pPacket->szChatMsg);

                    TracingMsg.SetLog(strTemp.GetString());

                    pPChar->SendToAgent(&TracingMsg);
                }*/
            //}
        }
    }

    for (VIEW_CROW_MAP_ITER iter=m_ViewCrow.begin(); iter!=m_ViewCrow.end(); ++iter)
    {        
        SFIELDCROW* pCharCrow = iter->second;
        if (pCharCrow->emCROW != CROW_PC)
            continue;

        GLChar* pPChar = GetChar(pCharCrow->dwID);
        //	Note : 캐릭터의 삭제 시점과 UpdateViewAround ()과의 불일치로 캐릭터가 존재하지 않을 수 있음.
        if (pPChar)
        {
            pPChar->SendToClient(nmg);

            
        }
    }
    return S_OK;
}

HRESULT GLChar::SendMsgViewAroundRelation(const Faction::EMRELATION_FLAG_TYPE emRelation, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, Buffer, UseCrc))
        return SendMsgViewAroundRelation(emRelation, &MsgPack);
    else
        return E_FAIL;
}

BOOL GLChar::IsInViewAround ( DWORD dwGAEAID )
{
	//return m_arrayFieldPC[dwGAEAID]!=NULL;
    SFIELDCROW* pCharCrow = GetViewCrow(CROW_PC, dwGAEAID);
    if (pCharCrow)
        return TRUE;
    else
        return FALSE;
}

HRESULT GLChar::ResetViewAround ()
{
	ReSetActState(EM_GETVA_AFTER);
	SetActState(EM_ACT_WAITING);

	m_dwViewFrame = 0;

    // 퇴출 메시지 전송
    TriggerSystem::SServerExitMessage msg;
    msg.dwCharDbNum = GetCharID();
    SendTriggerMessage(&msg);

	// FIELD CROW 정리.	
    for (VIEW_CROW_MAP_ITER iter=m_ViewCrow.begin(); iter!=m_ViewCrow.end(); ++iter)
    {
        SFIELDCROW* pCrow = iter->second;
        RELEASE_FIELDCROW(pCrow);
    }
    m_ViewCrow.clear();
    
    for (LAND_EVENT_MAP_ITER iter=m_LandEventMap.begin(); iter!=m_LandEventMap.end(); ++iter)
    {
        SFIELDLANDEVENT* pFIELD = iter->second;
        // 리스트에서 삭제
        m_pGaeaServer->RELEASE_FIELDLANDEVENT(pFIELD);
    }
    m_LandEventMap.clear();

	return S_OK;
}

bool GLChar::MoveTo( const D3DXVECTOR3* pTarPos, bool bJump, DWORD dwFlags )
{
	D3DXVECTOR3			vPos, vP0, vP1;
	if( pTarPos )
		vPos = (*pTarPos);
	else
		GetRandomPos( &vPos, &m_vPosition, m_fJumpRange );

	vP0 = vPos;
	vP1 = vPos;
	vP0.y = vPos.y + 10.0f;
	vP1.y = vPos.y - 10.0f;
	if( !m_actorMove.GotoLocation( vP0, vP1 ) )
		return false;

	// 점프라면 충돌메쉬와 충돌되지는 확인해야 한다.
	if ( bJump )
	{
		GLLandMan* const pLand( GetLandMan() );
		if ( pLand == NULL )
			return false;

		// 충돌이 된다면 이동을 못하게 한다.
		if ( pLand->IsLayerSimpleCollision( m_vPosition, vPos, 5.f ) )
		{
			return false;
		}
	}

	m_TargetID.vPos = vPos;

	if( bJump )
	{		
		SetPosition(m_TargetID.vPos);
		SendJumpMsg( m_vPosition, true, dwFlags );
	}
	else
	{
		TurnAction( GLAT_MOVE );
	} //if..else

	return true;
} //GLChar::MoveTo

// 피격 데미지 함수 통합 [9/26/2013 hsshin];
const DWORD GLChar::ReceiveDamage ( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

    /*DWORD _dwActualDamage(dwDamage);  // 실제 데미지 ( script에 의하여 데미지가 변경될 수 있다.);

    InstanceSystem::InstanceField* const _pInstance = pLand->GetScriptInstance();
    if ( _pInstance != NULL )  // 인던이 아닐 경우 NULL;
    {
        // dwID는 플레이어일경우 DBNUM, 몹,Summon등은 Land내에서 유효한 GloabalID를 사용;
        DWORD dwID(m_sAssault.GaeaId);
        if ( m_sAssault.emCrow == CROW_PC )
        {
            GLChar* pChar = m_pGaeaServer->GetChar(m_sAssault.GaeaId);
            if ( pChar )
                dwID = pChar->CharDbNum();
        }

        // 주의! : dwDamage는 reference로 인자를 넘기기때문에 값이 변경될수 있다. ( 스크립트에서 데미지값 변경을 할 수 있게 하기 위함 );
        if ( _pInstance->EventReceiveDamage(CROW_PC, CharDbNum(), emACrow, dwID, _dwActualDamage, dwDamageFlag) == false )  // return값이 false일 경우 기존의 데미지 처리를 하지 않겠다는 뜻;
            return _dwActualDamage;
    }*/

    return ReceiveDamage_Procedure(emACrow, dwAID, dwDamage, dwDamageFlag);
}

const DWORD GLChar::ReceiveDamage_Procedure( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

	if ( !IsValidBody() )
		return 0;

	if ( m_sHP.nNow == 0 )
		return 0;

    m_sAssault.emCrow = emACrow;
    m_sAssault.GaeaId = dwAID;

	// 인던 Script 에서의 Receive Damage 처리;
	DWORD _dwActualDamage = dwDamage;
	InstanceSystem::InstanceField* const pInstance =
		pLand->GetScriptInstance();
	if ( NULL != pInstance )
	{
		// dwID는 플레이어일경우 DBNUM, 몹, Summon등은 Land내에서 유효한 GloabalID를 사용;
		DWORD dwID = m_sAssault.GaeaId;
		if ( CROW_PC == m_sAssault.emCrow )
		{
			GLChar* pChar = m_pGaeaServer->GetChar( m_sAssault.GaeaId );
			if ( pChar )
				dwID = pChar->CharDbNum();
		}

		if ( CROW_SUMMON == m_sAssault.emCrow )
		{
			GLSummonField* pSummon =
				pLand->GetSummon( m_sAssault.GaeaId );
			if ( pSummon )
			{
				ServerActor* pOwner = pSummon->GetOwner();
				if ( pOwner )
				{
					GLChar* pChar = static_cast< GLChar* >( pOwner );
					if ( pChar )
						dwID = pChar->CharDbNum();
				}
			}
		}

		// 주의! : dwDamage는 reference로 인자를 넘기기때문에 값이 변경될수 있다;
		// ( 스크립트에서 데미지값 변경을 할 수 있게 하기 위함 );
		// return값이 false일 경우 기존의 데미지 처리를 하지 않겠다는 뜻;
		if ( false == pInstance->EventReceiveDamage(
			CROW_PC,
			CharDbNum(),
			emACrow,
			dwID,
			_dwActualDamage,
			dwDamageFlag ) )
		{
			return _dwActualDamage;
		}
	}

    // 쇼크판정;
    if ( dwDamageFlag & DAMAGE_TYPE_SHOCK ) 
    {
        TurnAction ( GLAT_SHOCK );
    }

    // 피격시 GLAT_GATHERING 동작 해제 (회피,블락 시에도 동작 해제되고 있음);
    else if ( isAction(GLAT_GATHERING) )
    {
        GLMSG::SNET_ACTION_BRD NetMsgBrd;
        NetMsgBrd.emCrow	= CROW_PC;
        NetMsgBrd.dwID		= m_dwGaeaID;
        NetMsgBrd.emAction	= GLAT_IDLE;

        //	Note : 주변의 Char 에게.
        SendMsgViewAround(&NetMsgBrd);

        SendToClient((NET_MSG_GENERIC*) &NetMsgBrd );		
        TurnAction ( GLAT_IDLE );
    }
    // 피격시 제작활동 해제 (회피,블락 시에도 동작 해제되고 있음);
    else if ( IsProducting() )
    {
        GLProductField* pProduct = GetProduct();
        if ( pProduct )
            pProduct->CancleProduct(this);
    }


    // 피격시 투명화 해제 (회피,블락 시에도 투명화 해제되고 있음);
	if ( GLHIDESET::CheckHideSet(EMHIDE_ON_HIT) )
	{
		RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		RestoreActStateByInvisible();
	}

    // Damage 적용.
	const DWORD wDxHP = (DWORD)GLCHARLOGIC::RECEIVE_DAMAGE ( dwDamage > USHRT_MAX ? USHRT_MAX : dwDamage );	
    // Damage 값이 있을때. 0 일때는 miss 발생;
	if ( wDxHP > 0 )
	{
        // 피격시 내구도 감소;
        if( emACrow == CROW_PC )
            DurabilityDropHit( wDxHP, TRUE );
        else
            DurabilityDropHit( wDxHP, FALSE );

		if ( emACrow == CROW_PC )
		{
			GLChar* pChar = GetChar(dwAID);
			AddDamageLog ( m_cDamageLog, dwAID, pChar->m_dwUserID, wDxHP );

            //
            //mjeon.CaptureTheField
            //
            if ( dwDamageFlag != DAMAGE_TYPE_GATHER && pChar->IsCTFPlayer() )
            {
                pChar->CTFDamageTo(wDxHP);
            }
            pChar->ToAllDamage_PVE(wDxHP);
		}
		else if ( emACrow == CROW_SUMMON )
		{
			if ( pLand == NULL )
			{
                sc::writeLogDebug("GLChar::ReceiveDamage_Proc 메소드 오류 m_pLandMan이 없습니다.");
				return 0;
            }
			// Note : 주인이 친걸로 등록
			//
			PGLSUMMONFIELD pSummon = pLand->GetSummon(dwAID);
			if (pSummon)
			{
				if ( pSummon->m_sOwnerID.actorType == CROW_PC )
				{
					GLChar* pSummonOwner = static_cast< GLChar* >(
						m_pGaeaServer->GetActor(
						pLand,
						pSummon->m_sOwnerID ) );
					if (pSummonOwner)
					{
						m_sAssault.emCrow = pSummonOwner->GetCrow();
						m_sAssault.GaeaId = pSummonOwner->GetGaeaID();
						AddDamageLog(m_cDamageLog, pSummonOwner->GetGaeaID(), pSummonOwner->m_dwUserID, wDxHP);

						//
						//mjeon.CaptureTheField
						//
						if ( dwDamageFlag != DAMAGE_TYPE_GATHER && pSummonOwner->IsCTFPlayer() )
						{
							pSummonOwner->CTFDamageTo(wDxHP);
						}
						pSummonOwner->ToAllDamage_PVE(wDxHP);
					}
				}
			}
		}				
	}

	if ( m_sHP.nNow > 0 )
	{
		Update_AccumulateHit( wDxHP, emACrow, dwAID );

		Faction::GLActorID sAttackerID( emACrow, dwAID );
		ServerActor* pAttacker =
			m_pGaeaServer->GetActor( pLand, sAttackerID );
		if ( pAttacker )
			SendToChildCrowAttacked( pAttacker );
	}

	return wDxHP;
}

// 회복 함수 통합 [9/26/2013 hsshin];
const DWORD GLChar::ReceiveHeal(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

    DWORD dwActualHeal(dwHeal);
    InstanceSystem::InstanceField* const _pInstance = pLand->GetScriptInstance();
    if ( _pInstance != NULL )  // 인던이 아닐 경우 NULL;
    {
        // dwID는 플레이어일경우 DBNUM, 몹,Summon등은 Land내에서 유효한 GloabalID를 사용;
        DWORD dwID(m_sAssault.GaeaId);
        if ( m_sAssault.emCrow == CROW_PC )
        {
            GLChar* pChar = m_pGaeaServer->GetChar(m_sAssault.GaeaId);
            if ( pChar )
                dwID = pChar->CharDbNum();
        }

        // 주의! : dwHeal는 reference로 인자를 넘기기때문에 값이 변경될수 있다. ( 스크립트에서 회복값 변경을 할 수 있게 하기 위함 );
        if ( _pInstance->EventReceiveHeal(emACrow, dwID, CROW_PC, CharDbNum(), dwActualHeal, dwHealFlag) == false )  // return값이 false일 경우 기존의 회복 처리를 하지 않겠다는 뜻;
            return dwActualHeal;
    }

    return ReceiveHeal_Procedure(emACrow, dwAID, dwActualHeal, dwHealFlag);
}
const DWORD GLChar::ReceiveHeal_Procedure(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag)
{
	if ( !IsValidBody() )
		return 0;

	if ( m_sHP.nNow == 0 )
		return 0;

	m_sHeal.emCrow = emACrow;
	m_sHeal.GaeaId = dwAID;

	const WORD wDxHP = GLCHARLOGIC::RECEIVE_HEAL( (WORD) dwHeal );	

    if ( emACrow == CROW_PC )
    {
        GLChar* pChar = GetChar(dwAID);
        //
        //mjeon.CaptureTheField
        //
        if ( dwHealFlag != HEAL_TYPE_GATHER && pChar->IsCTFPlayer() )
        {
            pChar->CTFHealTo(wDxHP);
        }
        pChar->ToAllHeal_PVE(wDxHP);
    }
    else if ( emACrow == CROW_SUMMON )
    {
		GLLandMan* const pLand( GetLandMan() );
		if ( pLand == NULL )
        {
            sc::writeLogDebug("GLChar::ReceiveHeal 메소드 오류 m_pLandMan이 없습니다.");
			return 0;
        }
        // Note : 주인이 친걸로 등록
        //
        PGLSUMMONFIELD pSummon = pLand->GetSummon(dwAID);
        if (pSummon)
        {
			if ( pSummon->m_sOwnerID.actorType == CROW_PC )
			{
				GLChar* pSummonOwner = static_cast< GLChar* >( m_pGaeaServer->GetActor(
					pLand,
					pSummon->m_sOwnerID ) );
				if (pSummonOwner)
				{
					//
					//mjeon.CaptureTheField
					//
					if ( dwHealFlag != HEAL_TYPE_GATHER && pSummonOwner->IsCTFPlayer() )
					{
						pSummonOwner->CTFHealTo(wDxHP);
					}
					pSummonOwner->ToAllHeal_PVE(wDxHP);
				}
			}
        }
    }

	return wDxHP;
}

void GLChar::SendPushPull( const D3DXVECTOR3 &vMovePos, const float fPUSH_SPEED, const DWORD dwAniType, const bool bReActor /*= true*/ )
{
	if( !IsValidBody() )
		return;

	// 밀려날 위치로 이동 시도.
	
	D3DXVECTOR3			vP0 = D3DXVECTOR3( vMovePos.x, vMovePos.y+5, vMovePos.z );
	D3DXVECTOR3			vP1 = D3DXVECTOR3( vMovePos.x, vMovePos.y-5, vMovePos.z );
	if( m_actorMove.GotoLocation( vP0, vP1 ) )
	{
        if ( bReActor )
            ActiveSkillClear();

		// 밀려나는 엑션 시작.
		m_TargetID.vPos = vMovePos;
		TurnAction( GLAT_PUSHPULL );

		// 밀리는 속도 설정.
		m_actorMove.SetMaxSpeed( fPUSH_SPEED );

		// 메시지 내용 설정.
		GLMSG::SNET_PUSHPULL_BRD		NetMsgBRD;
		NetMsgBRD.emCrow		= GETCROW();
		NetMsgBRD.dwID			= m_dwGaeaID;
		NetMsgBRD.vMovePos		= vMovePos;
		NetMsgBRD.fPUSH_SPEED	= fPUSH_SPEED;
		NetMsgBRD.dwAniType		= dwAniType;

		// [자신에게] Msg를 전달.
		SendToClient( &NetMsgBRD );

		// [자신의 주변 Char] 에게 Msg를 전달.
		SendMsgViewAround( &NetMsgBRD );
	} //if
} //GLChar::SendPushPull

// 이벤트나 탐닉 방지 시스템에 의해 변경된 경험치를 구한다.
int GLChar::CalculateReceveExp ( int nGenExp )
{
	float fGenExp = (float)nGenExp;
	int	  nCalculateExp = 0;
//#if defined(VN_PARAM) //vietnamtest%%%

	if (m_ServiceProvider == SP_VIETNAM)
	{
		if( m_dwVietnamGainType == GAINTYPE_HALF )
		{
			fGenExp /= 2.0f;
		}
		if( m_dwVietnamGainType == GAINTYPE_EMPTY )
		{
			fGenExp = 0.0f;
		}

		nCalculateExp = (int)fGenExp;

		if( m_dwVietnamGainType == GAINTYPE_EMPTY || m_dwVietnamGainType == GAINTYPE_HALF )
		{
			m_lVNGainSysExp += (nGenExp - nCalculateExp);
		}
	}
//#endif

#ifdef CH_PARAM_USEGAIN //chinaTest%%%
	if( m_ChinaGainType == GAINTYPE_HALF )
	{
		fGenExp /= 2.0f;
	}
	if( m_ChinaGainType == GAINTYPE_EMPTY )
	{
		fGenExp = 0;
	}

	nCalculateExp = (int)fGenExp;
#endif

#ifndef CH_PARAM_USEGAIN //** Add EventTime
	if( m_bEventStart && m_bEventApply )
	{
		SEventState sEventState = m_pGaeaServer->m_sEventState;
		if( sEventState.bEventStart )
		{
			fGenExp *= sEventState.fExpGainRate;
		}
	}
	nCalculateExp = (int)fGenExp;
#endif

	return nCalculateExp;
}

// *****************************************************
// Desc: 경험치 획득
// *****************************************************
void GLChar::ReceiveExp ( float fGenExp, bool bExp_ShowUser, bool bExp_Addition /*= true*/  )
{
    // 상태가 올바르지 않으면 경험치를 받지 못한다.
	if ( !IsValidBody() )
		return;

    ReceiveExpApply(fGenExp, bExp_ShowUser, bExp_Addition);

}

void GLChar::ReceiveExpApply( float fGenExp, bool bExp_ShowUser, bool bExp_Addition /*= false */ )
{
    // 무조껀 경험치를 적용한다.

	DWORD AcquireEXP = 0;
    // 파티든 아니든 경험치 계산은 마지막으로 여기서한다
	if ( bExp_Addition == true )
	{
		const float fTempExp = static_cast< float >( fGenExp );
		const float fExpGainRate = m_pGaeaServer->GetExpGainRate(m_CHARINDEX,m_wLevel);

		int nMemExp = static_cast< int >( fTempExp * fExpGainRate );
		fGenExp = CalculateReceveExp(nMemExp);

		SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap().getBaseMapID()));
		if ( pMapNode != NULL )
		{
			fGenExp = static_cast<float>(fGenExp * pMapNode->GetRateExpDrop());
		}

		if ( m_sEVENTFACT.IsIPEvent() )
		{
			float IPEventExpRate = 1.0f + (ran_event::GLIPEvent::Instance()->GetExp() * 0.01f);

			fGenExp = static_cast<float>(fGenExp * IPEventExpRate);
		}	

		m_sExperience.lnNow += DWORD( fGenExp*m_fEXP_RATE );
		AcquireEXP = static_cast<DWORD>( fGenExp*m_fEXP_RATE );
	}
	else
	{
		AcquireEXP = fGenExp;
		m_sExperience.lnNow += DWORD( fGenExp );
	}

	/*if( fGenExp != 0 && m_bTracingUser )
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		TracingMsg.SetAccount(m_szUID);

		CString strTemp;
		strTemp.Format(
			"Receive Exp, [%s][%s], Receive exp amount[%d]",
			m_szUID, m_szName, fGenExp);

		TracingMsg.SetLog(strTemp.GetString());
		SendToAgent(&TracingMsg);
	}*/

    
    if( (bExp_ShowUser || m_bAcquireEXP) && AcquireEXP > 0 && m_dwUserLvl >= USER_GM1 )
    {
        GLMSG::SNETPC_ACQUIRE_EXP NetMsg;
        NetMsg.EXP = AcquireEXP;
        SendToClient( &NetMsg );
    }
}

// *****************************************************
// Desc: 경험치 획득(파티)
// *****************************************************
void GLChar::ReceivePartyExp( const float fGenExp, const STARGETID &TargetID )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if ( !IsValidBody() )
		return;

    float fTempExp = fGenExp;

    ServerActor* pActor = m_pGaeaServer->GetTarget(pLand, TargetID);
    if (!pActor)
        return;

    int nAttackerLev = static_cast<int>( GetLevel() );
    int nDefenserLev = static_cast<int>( pActor->GetLevel() );

    float fExpRate = GLCONST_CHAR::GETEXP_RATE( nAttackerLev, nDefenserLev );
    if ( fExpRate <= 0.f )
        fTempExp = 0;

	ReceiveExp( fTempExp, true );
}

void GLChar::ReceiveKillExpParty(const STARGETID &TargetID, const bool bConftSchool, bool bBoss/* =false */)
{	
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	GLPartyField* const pParty = GetMyParty();		
	if ( pParty == NULL )
		return;

	PartyMemberGaeaIDs dwPartyMemberGaeaIDs;	
	const DWORD nMember = pParty->GetPartyMemberGaeaIDs(dwPartyMemberGaeaIDs);
	GASSERT(!(nMember > MAXPARTY));	

	if ( nMember == 0 )
		return;
	
	WORD wLEVEL_TOTAL(0);
	WORD wLEVEL_LOWER(USHRT_MAX);
	DWORD dwNearMemberGaeaIDs[MAXPARTY];
	DWORD nNearMember = 0;
	for ( DWORD _i = 0; _i < nMember; ++_i )
	{
		const DWORD dwGaeaID = dwPartyMemberGaeaIDs[_i];
		GLChar* const pChar = GetChar(dwGaeaID);
		if (pChar == NULL )
			continue;

		if ( pChar->GetCurrentMap() != GetCurrentMap() )
			continue;

		if ( pChar->IsValidBody() == false )
			continue;

		const D3DXVECTOR3 vDistance = GetPosition() - pChar->GetPosition();
		const float fDistance = D3DXVec3Length ( &vDistance );
		if ( fDistance > GLCONST_CHAR::fPARTYEXPRANGE )
			continue;

		dwNearMemberGaeaIDs[nNearMember++] = dwGaeaID;
		wLEVEL_TOTAL += pChar->m_wLevel;
		if ( wLEVEL_LOWER > pChar->m_wLevel )
			wLEVEL_LOWER = pChar->m_wLevel;
	}

	// 경험치 계산;
	const float fCalcKillExp = float(GLOGICEX::CALCKILLEXP(m_pGaeaServer, wLEVEL_LOWER, TargetID, pLand));
	const float fEXP_ONE = float(GLCONST_CHAR::aPARTYKILLEXP_ONE[nMember - 1] * 0.01f);

	// '경험치'를 파티 경험치로 환산
	const float fGenExpRate = fCalcKillExp * fEXP_ONE * (bConftSchool ? GLCONST_CHAR::fCONFRONT_SCHOOL_EXP_SCALE : 1.0f);

	const float fDecreaseExpRate = GLCONST_CHAR::fPARTYEXP_S * (isValidExpedition() ? GLCONST_CHAR::fExpeditionDecreaseExpRate : 1.0f);
	for ( DWORD _i = 0; _i < nNearMember; ++_i )
	{		
		GLChar* pChar = GetChar(dwNearMemberGaeaIDs[_i]);
		if (!pChar)
			continue;

		const WORD wLevel = pChar->m_wLevel;
		int nMemExp = int(fCalcKillExp * float(wLevel) / float(wLEVEL_TOTAL));

		//	레벨차이(수신자렙-최저렙)에 감소율을 반영하여 경험치를 감소시킴.
		nMemExp -= int(float(nMemExp * (wLevel - wLEVEL_LOWER)) * fDecreaseExpRate);

		// 아이템 에디트에서 입력한 경험치 배율 적용
		float fMultipleEXP = pChar->GetExpMultiple();
		if ( false == bBoss )
		{
			fMultipleEXP += pChar->GetIncR_ExpAddEffect();
		}

		nMemExp = int((float)nMemExp * fMultipleEXP);

		if (nMemExp < 0 )
			nMemExp = 0;

		pChar->ReceivePartyExp(nMemExp, TargetID);
	}
}

// *****************************************************
// Desc: 상대를 죽여서 발생하는 경험치 (파티 or 싱글)
// *****************************************************
void GLChar::ReceiveKillExp( const STARGETID &cTargetID, bool bConftSchool/* =false */, bool bSummonReceive/* =false */, bool bBoss/* =false */ )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if ( !IsValidBody() )
		return;

	//
	//mjeon.instance
	//
	if (IsInInstance())
	{
		GASSERT(bConftSchool == false);

		GLCHARNODE* pCharNode = pLand->m_GlobPCList.m_pHead;
		GLChar* pChar = NULL;
		D3DXVECTOR3 vDis;
		int idxExpRate = 0;
		WORD wTotalLevel = 0;
		WORD wLowestLevel = USHRT_MAX;

		std::vector<GLChar *> vecChars;

		for (; pCharNode; pCharNode = pCharNode->pNext)
		{
			pChar = pCharNode->Data;

			if (!pChar)
				continue;

			if (!pChar->IsValidBody())
				continue;

			vDis = GetPosition() - pChar->GetPosition();
			float fDis = D3DXVec3Length(&vDis);
			if (fDis > GLCONST_CHAR::fPARTYEXPRANGE)
				continue;

			wTotalLevel += pChar->m_wLevel;
			if (wLowestLevel > pChar->m_wLevel)
                wLowestLevel = pChar->m_wLevel;

			vecChars.push_back(pChar);
		}

		idxExpRate = vecChars.size() - 1;
		idxExpRate = idxExpRate >= 0 ? idxExpRate : 0;

		
		//경험치 계산
		int nGenExp = GLOGICEX::CALCKILLEXP(m_pGaeaServer, wLowestLevel, cTargetID, pLand);		
		const float fDecreaseExpRate = GLCONST_CHAR::fPARTYEXP_S * (isValidExpedition() ? GLCONST_CHAR::fExpeditionDecreaseExpRate : 1.0f);
		
		//float fEXP_ONE = float(GLCONST_CHAR::aPARTYKILLEXP_ONE[idxExpRate] * 0.01f);	//8인을 초과하는 파티를 디자인한다면 이 배열도 확장해야 한다. 하지만 현재 모두 100이므로 사용되지 않는듯...
		//nGenExp = int(nGenExp * fEXP_ONE);	//파티 경험치로 환산

		DWORD dwSize = vecChars.size();
		for (DWORD i=0; i < dwSize ; ++i)
		{
			GLChar* pChar = vecChars[i];
			
			if (!pChar)
                continue;

			int nMemExp = int ( nGenExp * pChar->m_wLevel / float(wTotalLevel) );
			
			// 레벨차이(수신자렙-최저렙)에 감소율을 반영하여 경험치를 감소시킴
			nMemExp -= int ( nMemExp * ( pChar->m_wLevel - wLowestLevel ) * fDecreaseExpRate );

			// 아이템 에디트에서 입력한 경험치 배율 적용
			float fMultipleEXP = pChar->GetExpMultiple();
			if ( false == bBoss )
			{
				fMultipleEXP += pChar->GetIncR_ExpAddEffect();
			}
			nMemExp = int( (float)nMemExp * fMultipleEXP );

			if (nMemExp < 0)
				nMemExp = 0;

			pChar->ReceivePartyExp(nMemExp, cTargetID);
		}

		return;
	}


	//파티일 경우	
	if ( isValidParty() )
		ReceiveKillExpParty(cTargetID, bConftSchool, bBoss);
	else
	{
		//*/*/*/*//	Note : 경험치 계산.
		int nGenExp = GLOGICEX::CALCKILLEXP(m_pGaeaServer, GETLEVEL(), cTargetID, pLand);
		if (bConftSchool)
            nGenExp = int (nGenExp * GLCONST_CHAR::fCONFRONT_SCHOOL_EXP_SCALE);

		// 아이템 에디트에서 입력한 경험치 배율 적용
		float fMultipleEXP = GetExpMultiple();
		if ( false == bBoss )
		{
			fMultipleEXP += GetIncR_ExpAddEffect();
		}

		int nMyGenExp = int((float) nGenExp * fMultipleEXP);

		ReceiveExp ( nMyGenExp, true );
	}

	// quest와 연결된 처리
	if ( cTargetID.emCrow != CROW_MOB )
		return;

	const GLCrow* const pCROW = pLand->GetCrow( cTargetID.GaeaId );		
	if ( pCROW == NULL )
		return;

	const SNATIVEID& NativeID = pCROW->GetNativeId();

	GLPartyField* const pParty = GetMyParty();
	if ( pParty != NULL )		
	{
		PartyMemberGaeaIDs arPartyMemberGaeaIDs;
		const DWORD nMember = pParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);    
		for ( DWORD _i = 0; _i < nMember; ++_i )
		{
			GLChar* pChar = GetChar(arPartyMemberGaeaIDs[_i]);

			if( !pChar )
				continue;

			if( m_ClientSlot == pChar->m_ClientSlot )
				DoQuestMobKill ( this, NativeID, true );
			else
				pChar->DoQuestMobKill( this, NativeID, false );

			//
			//mjeon.activity
			//
			pChar->ActivityCheckMobKill(this, NativeID.dwID);
		}
	}
	else
	{
		DoQuestMobKill( this, NativeID, true );

		//
		//mjeon.activity
		//
		ActivityCheckMobKill(this, NativeID.dwID);
	}
}

// *****************************************************
// Desc: 상대를 때려서 발생하는 경험치 (파티 or 싱글)
// *****************************************************
void GLChar::ReceiveAttackExp( const STARGETID &cTargetID, DWORD dwDamage, BOOL bPartySkill, bool bConftSchool/* =false */, 
							   bool bSummonReceive/* =false */, bool bBoss/* =false */ )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if ( !IsValidBody() )
		return;

	//mjeon.instance
	if (IsInInstance())
	{
		GASSERT(bConftSchool == false);

		GLCHARNODE* pCharNode = pLand->m_GlobPCList.m_pHead;
		GLChar* pChar = NULL;
		D3DXVECTOR3 vDis;
		int idxExpRate = 0;
		WORD wTotalLevel = 0;
		WORD wLowestLevel = 0;

		std::vector<GLChar *> vecChars;

		for (; pCharNode; pCharNode = pCharNode->pNext)
		{
			pChar = pCharNode->Data;

			if (!pChar)
				continue;

			if (!pChar->IsValidBody())
				continue;

			vDis = GetPosition() - pChar->GetPosition();
			float fDis = D3DXVec3Length(&vDis);
			if (fDis > GLCONST_CHAR::fPARTYEXPRANGE)
				continue;

			wTotalLevel += pChar->m_wLevel;
			if (wLowestLevel > pChar->m_wLevel)
                wLowestLevel = pChar->m_wLevel;

			vecChars.push_back(pChar);
		}

		idxExpRate = vecChars.size() - 1;
		idxExpRate = idxExpRate >= 0 ? idxExpRate : 0;

		
		//경험치 계산
		float fGenExp = GLOGICEX::CALCATTACKEXP(m_pGaeaServer, wLowestLevel, cTargetID, pLand, dwDamage);		
		const float fDecreaseExpRate = GLCONST_CHAR::fPARTYEXP_S * (isValidExpedition() ? GLCONST_CHAR::fExpeditionDecreaseExpRate : 1.0f);
		//float fEXP_ONE = float(GLCONST_CHAR::aPARTYKILLEXP_ONE[idxExpRate] * 0.01f);	//8인을 초과하는 파티를 디자인한다면 이 배열도 확장해야 한다. 하지만 현재 모두 100이므로 사용되지 않는듯...
		//nGenExp = int(nGenExp * fEXP_ONE);	//파티 경험치로 환산

		DWORD dwSize = vecChars.size();
		for (DWORD i=0; i < dwSize ; ++i)
		{
			GLChar* pChar = vecChars[i];
			
			if (!pChar)
                continue;

			float fMemExp = float ( fGenExp * pChar->m_wLevel / float(wTotalLevel) );
			
			// 레벨차이(수신자렙-최저렙)에 감소율을 반영하여 경험치를 감소시킴.
			fMemExp -= float ( fMemExp * ( pChar->m_wLevel - wLowestLevel ) * fDecreaseExpRate );

			// 아이템 에디트에서 입력한 경험치 배율 적용
			float fMultipleEXP = pChar->GetExpMultiple();
			if ( false == bBoss )
			{
				fMultipleEXP += pChar->GetIncR_ExpAddEffect();
			}

			fMemExp = fMemExp * fMultipleEXP;

			if (fMemExp < 0)
				fMemExp = 0;

			if (m_ServiceProvider == SP_THAILAND || m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_MALAYSIA_EN || m_ServiceProvider == SP_PHILIPPINES)
			{
				// 파티일 경우의 사이버 카페 경험치
				if( pChar->m_emIPBonus == EMIP_BONUS_A )		// A등급
				{
					float fTemp = (float)fMemExp;
					fTemp *= 1.5f;
					fMemExp = (float)fTemp;
				}
				else if( pChar->m_emIPBonus == EMIP_BONUS_B )	// B등급
				{
					float fTemp = (float)fMemExp;
					fTemp *= 1.3f;
					fMemExp = (float)fTemp;
				}else if( pChar->m_emIPBonus == EMIP_BONUS_C )	// C등급
				{
					float fTemp = (float)fMemExp;
					fTemp *= 1.2f;
					fMemExp = (float)fTemp;
				}else if( pChar->m_emIPBonus == EMIP_BONUS_D )	// D등급
				{
					float fTemp = (float)fMemExp;
					fTemp *= 1.5f;
					fMemExp = (float)fTemp;
				}
			}

			pChar->ReceivePartyExp(fMemExp, cTargetID);
		}

		return;
	}


	//	Note : 파티일 경우
	if (isValidParty())
	{
		GLPartyField* const pParty = GetMyParty();
		if ( pParty )
		{
			//	Note : 경험치 수신 가능한 파티 인원.
			std::vector<DWORD> vecPARTY;
			vecPARTY.reserve ( MAXPARTY );

			//	Note : 경험치 수신 가능한 파티인원. ( 자신 포함. )
			//
			WORD wLEVEL_TOTAL(0);
			WORD wLEVEL_LOWER(USHRT_MAX);
			PartyMemberGaeaIDs arPartyMemberGaeaIDs;
			const DWORD nMember = pParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);
			
			for ( DWORD _i = 0; _i < nMember; ++_i )
			{
				const DWORD dwPARTY_MBR = arPartyMemberGaeaIDs[_i];
				GLChar* pChar = GetChar(dwPARTY_MBR);
				if (!pChar || pChar->GetCurrentMap() != GetCurrentMap() )
                    continue;
				if (!pChar->IsValidBody())
                    continue;
			
				D3DXVECTOR3 vDis = GetPosition() - pChar->GetPosition();
				float fDis = D3DXVec3Length ( &vDis );
				if (fDis > GLCONST_CHAR::fPARTYEXPRANGE)
                    continue;
		
				wLEVEL_TOTAL += pChar->m_wLevel;
				if (wLEVEL_LOWER>pChar->m_wLevel)
                    wLEVEL_LOWER = pChar->m_wLevel;

				vecPARTY.push_back ( dwPARTY_MBR );
			}

			DWORD dwINDEX = DWORD(vecPARTY.size());
			if ( dwINDEX>0 )
				--dwINDEX;
			GASSERT(dwINDEX<MAXPARTY);

			float fEXP_ONE(0);
			fEXP_ONE = float ( GLCONST_CHAR::aPARTYEXP_ONE[dwINDEX] * 0.01f );

			//	Note : 경험치량 산출.
			float fGenExp = GLOGICEX::CALCATTACKEXP(m_pGaeaServer, wLEVEL_LOWER, cTargetID, pLand, dwDamage);
			if (bConftSchool)
                fGenExp = fGenExp * GLCONST_CHAR::fCONFRONT_SCHOOL_EXP_SCALE;

			//	'경험치'를 직접 발생시킨 당사자의 경험치량 계산.
			fGenExp = fGenExp * fEXP_ONE;
			const float fDecreaseExpRate = GLCONST_CHAR::fPARTYEXP_S * (isValidExpedition() ? GLCONST_CHAR::fExpeditionDecreaseExpRate : 1.0f);

			//	Note : 각각 파티원에게 맞게 경험치 발생.
			//
			DWORD dwSIZE = (DWORD) vecPARTY.size();
			for ( DWORD i=0; i<dwSIZE; ++i )
			{
				DWORD dwPARTY_MBR = vecPARTY[i];
				GLChar* pChar = GetChar(dwPARTY_MBR);
				if (!pChar)
                    continue;

				//	파티 분배 경험치.
				float fMemExp = float ( fGenExp * pChar->m_wLevel / float(wLEVEL_TOTAL) );
				//	레벨차이(수신자렙-최저렙)에 감소율을 반영하여 경험치를 감소시킴.
				fMemExp -= float ( fMemExp * ( pChar->m_wLevel - wLEVEL_LOWER ) * fDecreaseExpRate );

				// 아이템 에디트에서 입력한 경험치 배율 적용
				float fMultipleEXP = pChar->GetExpMultiple();
				if ( false == bBoss )
				{
					fMultipleEXP += pChar->GetIncR_ExpAddEffect();
				}

				fMemExp = (float)fMemExp * fMultipleEXP;

				if (fMemExp < 0)
					fMemExp = 0;
				
				//IP보너스
				if (m_ServiceProvider == SP_THAILAND || m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_MALAYSIA_EN || m_ServiceProvider == SP_PHILIPPINES)
				{
					// 파티일 경우의 사이버 카페 경험치
					if( pChar->m_emIPBonus == EMIP_BONUS_A )		// A등급
					{
						float fTemp = (float)fMemExp;
						fTemp *= 1.5f;
						fMemExp = (float)fTemp;
					}
					else if( pChar->m_emIPBonus == EMIP_BONUS_B )	// B등급
					{
						float fTemp = (float)fMemExp;
						fTemp *= 1.3f;
						fMemExp = (float)fTemp;
					}else if( pChar->m_emIPBonus == EMIP_BONUS_C )	// C등급
					{
						float fTemp = (float)fMemExp;
						fTemp *= 1.2f;
						fMemExp = (float)fTemp;
					}else if( pChar->m_emIPBonus == EMIP_BONUS_D )	// D등급
					{
						float fTemp = (float)fMemExp;
						fTemp *= 1.5f;
						fMemExp = (float)fTemp;
					}
				}

				pChar->ReceivePartyExp( fMemExp, cTargetID );

			}
		}
	}
	else
	{
		float fGenExp = GLOGICEX::CALCATTACKEXP(m_pGaeaServer, GETLEVEL(), cTargetID, pLand, dwDamage);
		if (bConftSchool)
            fGenExp = fGenExp * GLCONST_CHAR::fCONFRONT_SCHOOL_EXP_SCALE;

		//아이템 에디트에서 입력한 경험치 배율 적용
		float fMultipleEXP = GetExpMultiple();
		if ( false == bBoss )
		{
			fMultipleEXP += GetIncR_ExpAddEffect();
		}

		float fMyGenExp = (float)fGenExp * fMultipleEXP;

		//IP보너스
		if (m_ServiceProvider == SP_THAILAND || m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_MALAYSIA_EN || m_ServiceProvider == SP_PHILIPPINES)
		{
			// 솔로일 경우의 사이버 카페 경험치
			if( m_emIPBonus == EMIP_BONUS_A )		// A등급
			{
				float fTemp = (float)fMyGenExp;
				fTemp *= 1.5f;
				fMyGenExp = (float)fTemp;
			}
			else if( m_emIPBonus == EMIP_BONUS_B )	// B등급
			{
				float fTemp = (float)fMyGenExp;
				fTemp *= 1.3f;
				fMyGenExp = (float)fTemp;
			}else if( m_emIPBonus == EMIP_BONUS_C )	// C등급
			{
				float fTemp = (float)fMyGenExp;
				fTemp *= 1.2f;
				fMyGenExp = (float)fTemp;
			}else if( m_emIPBonus == EMIP_BONUS_D )	//	D등급
			{
				float fTemp = (float)fMyGenExp;
				fTemp *= 1.5f;
				fMyGenExp = (float)fTemp;
			}
		}

		ReceiveExp ( fMyGenExp, true );
	}
}

// *****************************************************
// Desc: 상대를 죽여서 발생하는 Combat Point (파티 or 싱글)
// *****************************************************
void GLChar::ReceiveKillCP(const STARGETID &cTargetID)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	// CP를 사용하지 않음
	if( !GLCONST_CHAR::bCOMBAT_POINT )
		return;

	WORD wTargetLevel = 0;
	WORD wBoundCP = 0;

	if( cTargetID.emCrow == CROW_PC )
	{
		GLChar* pCHAR = GetChar(cTargetID.GaeaId);
		if (!pCHAR)
            return;

		wTargetLevel = pCHAR->GETLEVEL();

	}
	else
	{
		PGLCROW pCROW = pLand->GetCrow( cTargetID.GaeaId );
		if( !pCROW )
			return;

		wTargetLevel = pCROW->GETLEVEL();
		wBoundCP	 = pCROW->GetBonusCP();
	}
	

	WORD wMaxLevel;
	WORD wRecvCP = 0;
	float fPartyCP = 0.0f;

	if (IsInInstance())
	{
		GLCHARNODE* pCharNode = pLand->m_GlobPCList.m_pHead;
		DWORD dwPartySize = pLand->m_GlobPCList.m_dwAmount;
		GLChar* pChar = NULL;		

		for (; pCharNode; pCharNode = pCharNode->pNext)
		{
			pChar = pCharNode->Data;

			if (!pChar)
				continue;

			if (!pChar->IsValidBody())
				continue;

			if (pChar->m_emClass & GLCC_EXTREME_M || pChar->m_emClass & GLCC_EXTREME_W)
			{
				wMaxLevel = GLCONST_CHAR::wMAX_EXTREME_LEVEL;
			}
			else
			{
				wMaxLevel = GLCONST_CHAR::wMAX_LEVEL;
			}

			fPartyCP += (float)(wMaxLevel - (pChar->m_wLevel - wTargetLevel)) / 100.0f;
			fPartyCP += wBoundCP;
		}

		float fTempRecvCP = (float)fPartyCP / (float)dwPartySize;
		wRecvCP = (WORD)(fTempRecvCP + 0.5f);

		
		pCharNode = pLand->m_GlobPCList.m_pHead;
		pChar = NULL;
		
		for (; pCharNode; pCharNode = pCharNode->pNext)
		{
			pChar = pCharNode->Data;

			if (!pChar)
				continue;

			if (!pChar->IsValidBody())
				continue;

			pChar->ReceiveCP(cTargetID, wRecvCP);
		}

		return;
	}


	// 파티인 경우
	if (isValidParty())
	{
		GLPartyField* const pParty = GetMyParty();
		if ( pParty )
		{
			PartyMemberGaeaIDs arPartyMemberGaeaIDs;
			const DWORD nMember = pParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);

			for ( DWORD _i = 0; _i < nMember; ++_i )
			{
				const DWORD dwPARTY_MBR = arPartyMemberGaeaIDs[_i];
				GLChar* pChar = GetChar(dwPARTY_MBR);
				if ( !pChar || pChar->GetCurrentMap() != GetCurrentMap() )
                    continue;
				if ( !pChar->IsValidBody() )
                    continue;

				if( pChar->m_emClass & GLCC_EXTREME_M || pChar->m_emClass & GLCC_EXTREME_W )
				{
					wMaxLevel = GLCONST_CHAR::wMAX_EXTREME_LEVEL;
				}else{
					wMaxLevel = GLCONST_CHAR::wMAX_LEVEL;
				}

				fPartyCP += (float)(wMaxLevel - (pChar->m_wLevel - wTargetLevel)) / 100.0f ;
				fPartyCP += wBoundCP;
			}

			const float fTempRecvCP = (float)fPartyCP / (float)nMember;
			wRecvCP = (WORD)(fTempRecvCP + 0.5f);

			for ( DWORD _i = 0; _i < nMember; ++_i )
			{
				const DWORD dwPARTY_MBR = arPartyMemberGaeaIDs[_i];
				GLChar* pChar = GetChar(dwPARTY_MBR);
				if (!pChar || pChar->GetCurrentMap() != GetCurrentMap() )
                    continue;
				if (!pChar->IsValidBody())
                    continue;

				pChar->ReceiveCP( cTargetID, wRecvCP );

				if (cTargetID.emCrow == CROW_PC)
				{
					//
					//mjeon.activity.PK
					//
					const SNATIVEID idMap = GetCurrentMap().getBaseMapID();
					pChar->ActivityCheckPK();
					pChar->DoQuestMobKill( this,NATIVEID_NULL(),false,true);
				}
			}

		}
	}
	else
	{
		// 극강부인 경우
		if( m_emClass & GLCC_EXTREME_M || m_emClass & GLCC_EXTREME_W )
		{
			wMaxLevel = GLCONST_CHAR::wMAX_EXTREME_LEVEL;
		}
		else
		{
			wMaxLevel = GLCONST_CHAR::wMAX_LEVEL;
		}

		// 반올림을 위해 0.5f를 더한다.
		float fTempRecvCP = (float)(wMaxLevel - (m_wLevel - wTargetLevel)) / 100.0f;
		wRecvCP = (WORD)( fTempRecvCP + 0.5f );
		wRecvCP += wBoundCP;

		ReceiveCP( cTargetID, wRecvCP );

		//
		//mjeon.activity.PK
		//

		GLChar* pChar = GetChar(cTargetID.GaeaId);		
		if (!pChar)
			return;
		
		//
		// 서로 같은 학원이거나 같은 클럽이면 해당 특활을 수행할 수 없다. (어뷰징 방지).
		//
		DWORD dwClub1 = pChar->GetCharData2().m_ClubDbNum;	// target's club
		DWORD dwClub2 = GetCharData2().m_ClubDbNum;			// my club

		if ( (dwClub1 == CLUB_NULL || dwClub2 == CLUB_NULL || (dwClub1 != dwClub2) ) &&
			 (pChar->GetSchool() != GetSchool()) )
		{
			const SNATIVEID &idMap = GetCurrentMap().getBaseMapID();
			ActivityCheckPK();
			DoQuestMobKill( this,NATIVEID_NULL(),true,true);
		}
	}
}

void GLChar::ReceiveCP(const STARGETID &cTargetID, const WORD wRecvCP)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	// CP를 사용하지 않음
	if( !GLCONST_CHAR::bCOMBAT_POINT )
		return;

	WORD wIncreaseCP = wRecvCP;
	
	bool bClubBattle = false;
	bool bClubDeathMatch = false;
	bool bGuidBattleMap = false;

	if (cTargetID.emCrow == CROW_PC)
	{
		GLChar* pCHAR = GetChar(cTargetID.GaeaId);
		if (!pCHAR)
            return;

		bGuidBattleMap = (m_pGaeaServer->IsGuidanceBattle() && pLand->m_bGuidBattleMap);		

		if (pLand->IsClubBattleZone())
		{
			std::tr1::shared_ptr<GLClubField> pMyClub = GetMyClub();
			std::tr1::shared_ptr<GLClubField> pTarClub = GetClub(pCHAR->m_ClubDbNum);

			if ( pMyClub && pTarClub )
			{
				bool bClub = pMyClub->IsBattle( pTarClub->m_DbNum );
				bool bAlliance = pMyClub->IsBattleAlliance ( pTarClub->m_dwAlliance );
				bClubBattle = (bClub || bAlliance);
			}				
		}		

		if (pLand->m_bClubDeathMatchMap)
		{
            bClubDeathMatch = m_pGaeaServer->IsClubDeathMatch(pLand->m_dwClubMapID);
		}

		// 클럽배틀, 선도클럽결정전, 클럽데스매치로 PC를 Kill 했을 때 추가 점수를 받는다.
		if( bGuidBattleMap || bClubBattle || bClubDeathMatch )
		{
			wIncreaseCP += GLCONST_CHAR::wPK_KILL_Combat_Point;
		}
	}

	const int nCP_Add(m_nCP_ADD + m_sSUM_PASSIVE.m_nCP_ADD);
	m_sCP.INCREASE((wIncreaseCP + m_nGET_CP_VALUE), FALSE, nCP_Add);
	m_sCP.VARIATION(static_cast<int>(m_sSUMITEM.nReceive_CP_Inc));
}

HRESULT GLChar::CheckInstanceItem ()
{
	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	SITEM* pRHAND = GET_SLOT_ITEMDATA ( emRHand );
	SITEM* pLHAND = GET_SLOT_ITEMDATA ( emLHand );

	//	사용가능 횟수 많큼 사용시에 모두 소진되어 아이탬 사라지게 만듬.
	if ( pLHAND && pLHAND->sDrugOp.bInstance )
	{
		if ( GET_SLOT_ITEM(emLHand).wTurnNum==0 )
		{
			// 화살/부적/총알이 소모되었을 때 아이템이 정상적으로 삭제되도록 수정함
			//	아이탬 제거.
			RELEASE_SLOT_ITEM_SERVER( emLHand, true );

				//	[자신에게] SLOT에 있었던 아이탬 제거.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(emLHand);
			NetMsg_PutOn_Release.emType = EMPUTONRELEASE_TYPE_REFRESH;
			SendToClient(&NetMsg_PutOn_Release);
		}
		else
		{
			//	[자신에게] 인밴 아이탬 소모됨.
			GLMSG::SNETPC_PUTON_DRUG_UPDATE NetMsg_PutOn_Update;
			NetMsg_PutOn_Update.emSlot = emLHand;
			NetMsg_PutOn_Update.wTurnNum = GET_SLOT_ITEM(emLHand).wTurnNum;
			SendToClient(&NetMsg_PutOn_Update);
		}
	}

	// 이부분 코드 이상함, 1이상이면 삭제하고 0이면 아이템 소모? 오른손 아이템이도 소모성 아이템이 없어서 그렇나?
	// 바로 위에꺼 복사해서 쓴거 같은데 ==0 이 빠진 것같음, 수정함, RELEASE_SLOT_ITEM 이 아닌 RELEASE_SLOT_ITEM_SERVER 로 변경
	// 디비에서도 삭제되도록 변경함
	if ( pRHAND && pRHAND->sDrugOp.bInstance )
	{
		if ( GET_SLOT_ITEM(emRHand).wTurnNum == 0 )
		{
			//	아이탬 제거.
			RELEASE_SLOT_ITEM_SERVER( emRHand, true );

				//	[자신에게] SLOT에 있었던 아이탬 제거.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(emRHand);
			NetMsg_PutOn_Release.emType = EMPUTONRELEASE_TYPE_REFRESH;
			SendToClient(&NetMsg_PutOn_Release);
		}
		else
		{
			//	[자신에게] 인밴 아이탬 소모됨.
			GLMSG::SNETPC_PUTON_DRUG_UPDATE NetMsg_PutOn_Update;
			NetMsg_PutOn_Update.emSlot = emRHand;
			NetMsg_PutOn_Update.wTurnNum = GET_SLOT_ITEM(emRHand).wTurnNum;
			SendToClient(&NetMsg_PutOn_Update);
		}
	}

	return S_OK;
}

void GLChar::ReceiveLivingPoint(const int nVerLP)
{
	if ( !IsValidBody() )
		return;

	//	Note : 생활 점수 변경.
	//
	m_nLiving += nVerLP;
	
	//	Note : 생활점수 클라이언트에 반영.
	//
	GLMSG::SNETPC_UPDATE_LP NetMsg;
	NetMsg.nLP = m_nLiving;
	SendToClient( &NetMsg );
}

BOOL GLChar::ReceiveQItemFact ( const SNATIVEID& nidITEM )
{
	RECEIVE_QITEMFACT ( nidITEM );

	GLMSG::SNETPC_QITEMFACT_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.sFACT = m_sQITEMFACT;
	SendToClient(&NetMsgBrd);

	SendMsgViewAround(&NetMsgBrd);

	return TRUE;
}

void GLChar::ReceiveEventEx( const EMGM_EVENT_TYPE emType, const WORD wValue )
{
	//	Memo :	wValue 값이 0이면 이벤트 종료를 의미한다.
	if( wValue > 0 )
	{
		RECEIVE_EVENTFACT_BEGIN( emType, wValue );

		GLMSG::SNETPC_EVENTFACT_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;
		NetMsgBrd.emType = emType;
		NetMsgBrd.wValue = wValue;

		SendToClient(&NetMsgBrd);
		SendMsgViewAround(&NetMsgBrd);
	}
	else
	{
		RECEIVE_EVENTFACT_END( emType );
		
		GLMSG::SNETPC_EVENTFACT_END_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;
		NetMsgBrd.emType = emType;

		SendToClient(&NetMsgBrd);
		SendMsgViewAround(&NetMsgBrd);
	}
}

void GLChar::ReceiveBeginEvent ( const SEventState emType )
{
	//** Add EventTime
	// 이벤트 시간을 다시 셋팅한다.
	CTime	  crtTime      = CTime::GetCurrentTime();
	m_sEventTime.loginTime = crtTime.GetTime();
	m_bEventStart		   = FALSE;
	m_bEventApply		   = FALSE;

	GLMSG::SNET_GM_LIMIT_EVENT_BEGIN_FB NetMsgEventBeginFb;
	NetMsgEventBeginFb.start_Lv		= emType.MinEventLevel;
	NetMsgEventBeginFb.end_Lv	    = emType.MaxEventLevel;
	NetMsgEventBeginFb.play_Time     = emType.EventPlayTime;
	NetMsgEventBeginFb.buster_Time   = emType.EventBusterTime;
	NetMsgEventBeginFb.expGain_Rate  = emType.fExpGainRate;
	NetMsgEventBeginFb.itemGain_Rate = emType.fItemGainRate;

	if( emType.MinEventLevel <= GETLEVEL() && emType.MaxEventLevel >= GETLEVEL() )
	{
		m_bEventApply = TRUE;
	}


	SendToClient(&NetMsgEventBeginFb);
}

void GLChar::ReceiveVietnam( const __time64_t initTime )
{
	m_dwVietnamGainType = GAINTYPE_MAX;
	m_sVietnamSystem.Init();
	m_sVietnamSystem.loginTime = initTime;
	m_sVietnamSystem.gameTime  = 0;

	GLMSG::SNETPC_VIETNAM_ALLINITTIME NetMsg;
	NetMsg.initTime = initTime;
	SendToClient( &NetMsg );
}

void GLChar::ReceiveIPEvent( bool _Enable )
{
    m_sEVENTFACT.bIPEvent = _Enable;

    // Setting
    GLMSG::SNETPC_IPEVENT_BRD NetMsgBrd( _Enable );
    NetMsgBrd.dwGaeaID = m_dwGaeaID;

    NetMsgBrd.m_AttackValue = ran_event::GLIPEvent::Instance()->GetAttackValue();
    NetMsgBrd.m_AttackSpeed = ran_event::GLIPEvent::Instance()->GetAttackSpeed();
    NetMsgBrd.m_MoveSpeed = ran_event::GLIPEvent::Instance()->GetMoveSpeed();
    NetMsgBrd.m_ItemDrop = ran_event::GLIPEvent::Instance()->GetItempDrop();
    NetMsgBrd.m_MoneyDrop = ran_event::GLIPEvent::Instance()->GetMoneyDrop();
    NetMsgBrd.m_Exp = ran_event::GLIPEvent::Instance()->GetExp();
    NetMsgBrd.m_HPRecovery = ran_event::GLIPEvent::Instance()->GetHPRecovery();
    NetMsgBrd.m_MPRecovery = ran_event::GLIPEvent::Instance()->GetMPRecovery();
    NetMsgBrd.m_SPRecovery = ran_event::GLIPEvent::Instance()->GetSPRecovery();
    NetMsgBrd.m_AllRecovery = ran_event::GLIPEvent::Instance()->GetAllRecovery();
    NetMsgBrd.m_CriticalRate = ran_event::GLIPEvent::Instance()->GetCriticalRate();
    //

    SendToClient(&NetMsgBrd);
    SendMsgViewAround(&NetMsgBrd);
}

bool GLChar::IsCHATBLOCK ()
{
	if ( m_tCHATBLOCK==0 )
		return false;

	CTime cBLOCK(m_tCHATBLOCK);
	CTime cCUR(CTime::GetCurrentTime());

	// 채팅금지 시간인지 검사	
	if ( cBLOCK > cCUR )
	{
		GLMSG::SNETPC_CHAT_BLOCK NetChatBlock;
		NetChatBlock.tChatBlock = m_tCHATBLOCK;

		SendToClient ( &NetChatBlock );
		return true;
	}
	// 채팅금지이 초과 되었을 경우는 시간을 리셋, 채팅금지 해제
	m_tCHATBLOCK = 0;


	// 채팅금지 유저에게 채팅금지 해제 알림
	GLMSG::SNET_GM_CHAT_BLOCK_NOTIFY NetChatBlockNotify;
	NetChatBlockNotify.tChatBlock = m_tCHATBLOCK;
	SendToClient ( &NetChatBlockNotify );

	return false;
}


void GLChar::PostSetFlagTakenAttach(LONGLONG llPostID)
{
	m_setAttachTakenPostID.insert(llPostID);
}

BOOL GLChar::PostGetFlagTakenAttach(LONGLONG llPostID)
{
	std::set<LONGLONG>::iterator iter = m_setAttachTakenPostID.find(llPostID);
	
	return (iter != m_setAttachTakenPostID.end());
}


bool GLChar::IsPOSTBOXUSEABLE ( const EMPOSTBOXID_TYPE emIDType, const SNATIVEID& sPostBoxID )
{
	// Note : 우편함 사용 불가능이면
	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return false;
	}

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return false;

	if ( !IsValidBody() )
	{
		return false;
	}

	if ( emIDType == EMPOSTBOXID_TYPE_NONE || sPostBoxID == NATIVEID_NULL() )
	{
		return false;
	}

	// Note : 우편함 아이디가 있다면
	switch ( emIDType )
	{
	default:
		return false;

	case EMPOSTBOXID_TYPE_NPC:
		{
			// 우편함 거리 유효성 체크
			PGLCROW pCrow = pLand->GetCrow( sPostBoxID.dwID );
			if ( !pCrow || !pCrow->m_pCrowData )
			{
				return false;
			}

			if (   pCrow->m_pCrowData->m_emCrow		!= CROW_NPC 
				|| pCrow->m_pCrowData->m_emNPCType	!= NPC_TYPE_POSTBOX )
			{
				return false;
			}

			float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPosition-pCrow->GetPosition()) );
			float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
			float fTalkableDis = fTalkRange + GLCONST_POST::fPOSTBOX_DISTANCE;

			if ( fDist>fTalkableDis ) 
			{
				return false;
			}
		}
		break;

	case EMPOSTBOXID_TYPE_ITEM_CARD:
		{
            // 아이템 수량이 0이되었을때 재검사를 하기때문에 삭제하였습니다.
			//SITEM* pITEM = GLogicData::GetInstance().GetItem( sPostBoxID );
			//if ( !pITEM )
			//{
			//	return false;
			//}

			//SINVENITEM* pInvenItem = m_cInventory.FindItem( ITEM_POSTBOX_CARD );
			//if ( !pInvenItem )
			//{
			//	return false;
			//}
		}
		break;
	}

	return true;
}

void GLChar::Online()
{
    if ( m_pGaeaServer == NULL )
		return;

	m_pGaeaServer->Online(m_dwGaeaID);
	//m_pGaeaServer->ChaNameDbNumAdd(m_szName, m_CharDbNum, m_dwUserID, static_cast<EMUSERTYPE> (m_dwUserLvl));		
	m_pGaeaServer->ChaNameDbNumAdd(m_szName, m_CharDbNum, m_dwUserID, static_cast<EMUSERTYPE> (m_dwUserLvl), static_cast<int>(m_emClass), static_cast<int>(m_wLevel) );
}

void GLChar::Offline()
{
	if ( m_pGaeaServer == NULL )
		return;

	// khkim, 16.5.16285.1031 버전 필드서버 종료할 때 crash 수정, CFieldServer CloseClient 에는 worker 들이 수시로 아무데나 접근하기 때문에 로직 처리는 하면 안됨
	// Trigger System 퇴출메시지 전송, CFieldServer CloseClient 에서 호출되면 안됨, 거기에서 여기로 옮김
	TriggerSystem::SServerExitMessage sMsg;
	sMsg.dwCharDbNum = m_CharDbNum;
	SendTriggerMessage( &sMsg );

	// khkim, 
	// CFieldServer CloseClient 에서 호출되면 안됨, 거기에서 여기로 옮김
	// CloseClient의 경우 접속 종료이므로 Offline 이벤트 발생
	InstanceSystem::ManagerField* _pInstanceMan = m_pGaeaServer->GetInstanceManagerField();
	if ( _pInstanceMan )
	{
		_pInstanceMan->offline( m_CharDbNum );
	}

	m_pGaeaServer->Offline(m_dwGaeaID);
    m_pPrivateMarket->DoMarketClose();
}

void GLChar::SetOffence ( int _nOffence )
{
	m_nOffence = _nOffence;
}

void GLChar::SetDefence ( int _nDefence )
{
	m_nDefence = _nDefence;
}

void GLChar::SetFireRegist ( int _nFireRegist )
{
	m_nFireRegist = _nFireRegist;
}

void GLChar::SetIceRegist ( int _nIceRegist )
{
	m_nIceRegist = _nIceRegist;
}

void GLChar::SetElectricRegist ( int _nElectricRegist )
{
	m_nElectricRegist = _nElectricRegist;
}

void GLChar::SetPoisonRegist ( int _nPoisonRegist )
{
	m_nPoisonRegist = _nPoisonRegist;
}

void GLChar::SetSpiritRegist ( int _nSpiritRegist )
{
	m_nSpiritRegist = _nSpiritRegist;
}

void GLChar::SetAccuracyRate ( int _nAccuracyRate )
{
	m_nAccuracyRate = _nAccuracyRate;
}

void GLChar::SetAvoidanceRate ( int _nAvoidanceRate )
{
	m_nAvoidanceRate = _nAvoidanceRate;
}

void GLChar::SetHPRestorationRate ( float _fHPRestorationRate )
{
}

void GLChar::SetMPRestorationRate ( float _fMPRestorationRate )
{
}

void GLChar::SetSPRestorationRate ( float _fSPRestorationRate )
{
}
