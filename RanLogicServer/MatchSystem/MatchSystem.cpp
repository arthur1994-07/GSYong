#include "pch.h"
#include "./MatchSystem.h"
#include "./MatchScriptMan.h"
#include "../Server/MatchServer.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/gassert.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLogicFile.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

INSTANCE_PLAYER MATCH_REQUEST::invalid_player(false);

MatchSystem::MatchSystem(MatchServer *pMatchServer)
:m_pMatchServer(pMatchServer)
,m_dwDailyResetH(3)	//default 3:00:00
,m_dwDailyResetM(0)
,m_dwGlobalExpireM(14400)
{
	InitializeCriticalSectionAndSpinCount(&m_CS_Key, sc::CRITICAL_SECTION_SPIN_COUNT);

#ifdef _USE_TBB
	
	for (DWORD i=0; i<MAXNUM_OF_INSTANCE_PER_MATCHSERVER; ++i)
	{
		m_cqKey_Consumer.push(i);
	}
	
	m_cqKey_Producer.clear();
	m_setKey_Filter.clear();

	m_vecInstanceSvrStats.reserve(MAXNUM_OF_INSTANCESERVER_TO_MATCH);

	for (DWORD j=0; j<MAXNUM_OF_INSTANCESERVER_TO_MATCH; j++)
	{
		m_vecInstanceSvrStats.push_back(MAXDWORD);
	}
	
#else

	sc::writeLogError(std::string("MatchSystem requires TBB! Turn on TBB for MatchServer!"));
	exit(0);

#endif

	m_pScriptMan = new MatchScriptMan();
}


MatchSystem::~MatchSystem()
{
	SAFE_DELETE(m_pScriptMan);

	DeleteCriticalSection(&m_CS_Key);
}


BOOL MatchSystem::Initialize()
{
	//
	//Load Script
	//
	std::string strScriptPath(sc::getAppPath());
	strScriptPath.append("\\Data\\GLogicServer\\");
	strScriptPath.append("scripts\\");

	std::string strMatchSystemScriptPath(strScriptPath);    
    strMatchSystemScriptPath.append(GLogicData::GetInstance().GetMatchSystemScriptName());

	m_pScriptMan->LoadScript(strMatchSystemScriptPath.c_str());
	
	if (!m_pScriptMan->IsLoaded())
	{
		sc::writeLogError(std::string("MatchSystem::Initialize() failure. Script MUST be loaded to run this feature."));

		return FALSE;
	}

	//
	//initialize Configurations
	//
	m_dwDailyResetH = m_pScriptMan->GetConfigUINT("DailyResetH");
	m_dwDailyResetM = m_pScriptMan->GetConfigUINT("DailyResetM");

	m_dwGlobalExpireM = m_pScriptMan->GetConfigUINT("GlobalExpireM");

	return TRUE;
}

void MatchSystem::FrameMove()
{
	MATCH_REQUEST *pRequest = NULL;

#ifdef _USE_TBB
	//
	//1.Process queued requests
	//
	while( m_cqRequests.try_pop(pRequest) )
	{
		if (pRequest != NULL)
		{
			Process(*pRequest);
			SAFE_DELETE(pRequest);
		}
	}
#else
	//
	//1.Process queued requests
	//
	while( pRequest = m_cqRequests.front() )
	{
		if (pRequest != NULL)
		{
			Process(*pRequest);
			SAFE_DELETE(pRequest);

			m_cqRequests.pop();
		}
	}
#endif

	//
	//2.Manage each rooms
	//
	RunStateMachine();
	

	//
	//3.Print MatchSystem Info
	//
	PrintMatchSystemInfo();
}

void MatchSystem::RunStateMachine()
{
	DWORD dwKey = INVALID_INSTANCE_KEY;
	MAP_INSTANCE_ROOM_ITER iter = m_mapRooms.begin();

	for (; iter != m_mapRooms.end();)
	{
		dwKey = iter->first;
		MatchingRoom &room = iter->second;
		

		switch (room.GetState())
		{
		case ROOM_STATE_READY:			
			{
				room.CheckCancel();

				//ready 상태에서 empty인 경우는, Cancel에 의해 방이 빈 경우이다. 바로 방 제거
				if (room.IsEmpty())
				{
					iter = m_mapRooms.erase(iter);
					continue;
				}

				if (room.IsFull())
				{
					//
					//real Instance 생성
					//
					GLMSG::SNET_CREATE_INSTANCE_MI msg;
					msg.dwKey	= dwKey;
					msg.dwMapID = room.GetMapID();
					
					//real instance가 생성된 이후에는 cancel처리 불가 - 대신 퇴장(UnMatch) 요청을 보내야 한다

					
					DWORD dwInstanceServerNum = PickInstanceServer();

					if (dwInstanceServerNum == MAXNUM_OF_INSTANCESERVER_TO_MATCH)
					{
						continue;
					}						
					
					if (SendInstance(dwInstanceServerNum, &msg) == FALSE)
					{
						//해당 InstanceServer에 문제가 있을 경우, 다른 InstanceServer에 대해 한번 더 시도해본다

						dwInstanceServerNum = PickInstanceServerExcept(dwInstanceServerNum);

						if (dwInstanceServerNum == MAXNUM_OF_INSTANCESERVER_TO_MATCH)
						{
							continue;
						}
						else
						{
							if (SendInstance(dwInstanceServerNum, &msg) == FALSE)
							{
								continue;
							}
						}
					}

					room.SetInstanceServerNum(dwInstanceServerNum);
					
					GASSERT(room.GetState() == ROOM_STATE_READY);
					room.MoveState();	//Ready -> Construct
					GASSERT(room.GetState() == ROOM_STATE_CONSTRUCT);
				}
			}
			break;

		case ROOM_STATE_CONSTRUCT:
			{
				if (room.IsBoundType())
				{
					if (room.IsExpired())
					{
						room.KickOutParty();		//request to transfer the party in the room to their FieldServers

						GASSERT(room.GetState() == ROOM_STATE_CONSTRUCT);
						room.MoveState();	//Construct -> Destruct
						GASSERT(room.GetState() == ROOM_STATE_DESTRUCT);
					}
				}
				else	//귀속 인던이 아닌 경우에는 방이 비었을 때 파괴해 버린다
				{
					if (room.IsEmpty())
					{
						GASSERT(room.GetState() == ROOM_STATE_CONSTRUCT);
						room.MoveState();	//Construct -> Destruct
						GASSERT(room.GetState() == ROOM_STATE_DESTRUCT);
					}
				}
			}
			break;

		case ROOM_STATE_DESTRUCT:
			{
				if (room.IsEmpty())
				{
					//
					//real Instance 파괴
					//
					GLMSG::SNET_DELETE_INSTANCE_MI msg;
					msg.dwKey	= dwKey;
					msg.dwMapID = room.GetMapID();

					SendInstance(room.GetInstanceServerNum(), &msg);

					StateInstanceDestruction(room.GetInstanceServerNum());

					FreeRoom(dwKey);
				}
			}
			break;

		default:
			sc::writeLogError(stringformat("(CRITICAL) MatchSystem::FrameMove() - Room(%1%) is in wrong state(%2%) !!", room.GetKey(), room.GetState()));
			break;
		}

		++iter;
	}

	m_setCancel.clear();
}

void MatchSystem::Request(MATCH_REQUEST *request)
{
	m_cqRequests.push(request);
}

void MatchSystem::Cancel(DWORD64& id)
{
	m_setCancel.insert(id);
}


/*
When newly constructed (새로운 instance를 생성하는 경우)
	1.Construct a room (on MatchServer)
	2.Create an instance (on InstanceServer)
	3.Transfer party to Instance (from FieldServer)


When expired (해당 instance가 사라져야 할 경우)
	1.Transfer party to FieldServer (from InstanceServer)
	2.Destruct the room (on MatchServer)
	3.Delete the instance (on InstanceServer)

그 외에 개별적인 matching 요청이나 unmatching 요청은 방/instance를 생성하는 부분이 필요하지 않다.
*/
DWORD _KEY_ MatchSystem::Process(MATCH_REQUEST &request)
{
	DWORD dwMapID = request.GetMapID();	
	
	GASSERT(dwMapID != INVALID_MAP_ID);
	if (dwMapID == INVALID_MAP_ID)
	{
		sc::writeLogError(std::string("MatchSystem::Process() - Wrong Request. MapID is invalid."));

		return INVALID_INSTANCE_KEY;
	}

	DWORD dwKey = INVALID_INSTANCE_KEY;	

	GLMSG::NET_MATCH_REQUEST_RESULT_MAC msg;
	msg.dwMapID = dwMapID;


	switch(request.GetType())
	{
	case REQUEST_MATCH:
		{
			//
			// Match
			//
			DWORD dwPartySize = request.size();
			
			GASSERT(dwPartySize > 0);
			if (dwPartySize < 1)
			{
				sc::writeLogError(std::string("MatchSystem::Process() - Wrong Request. PartySize is wrong."));

				return INVALID_INSTANCE_KEY;
			}

			for (DWORD i=0; i < dwPartySize; i++)
			{
				INSTANCE_PLAYER& who = request.GetMember(i);

				if (!who.IsValid())
				{
					continue;
				}

				msg.dwSvrGroupNum = who.dwSvrGroup;

				if (i == 0)	//파티장 or 개인 / Release는 개인으로써만 신청이 가능하다.
				{
					//
					//Check Cancel
					//
					if (IsCanceled(who.dw64id))
					{
						sc::writeLogDebug(stringformat("Match request of the char(%1%, %2%) has been canceled", who.dwSvrGroup, who.dwChaNum));

						GLMSG::NET_MATCH_CANCEL_CAM_FB msgCancel;
						msgCancel.dwSvrGroupNum = who.dwSvrGroup;
						msgCancel.dwChaNum		= who.dwChaNum;

						SendAgent(&msgCancel);

						return INVALID_INSTANCE_KEY;
					}
					

					REQUEST_TYPE type = request.GetType();

					dwKey = Match(who, dwMapID, dwPartySize);

					if (dwKey == INVALID_INSTANCE_KEY)
					{
						msg.bDone = FALSE;
						SendAgent(who.dwSvrGroup, &msg);

						sc::writeLogError(stringformat("MatchSystem::Process() - PartyMember(%1%, %2%) cannot match to the instance(%3%, %4%).", who.dwSvrGroup, who.dwChaNum, LOWORD(dwMapID), HIWORD(dwMapID)));

						return INVALID_INSTANCE_KEY;
					}
					else
					{
						msg.bDone = TRUE;
						SendAgent(who.dwSvrGroup, &msg);
					}
				}
				else		//나머지 파티원들
				{
					GASSERT(dwKey != INVALID_INSTANCE_KEY);
					if (dwKey == INVALID_INSTANCE_KEY)
					{
						msg.bDone = FALSE;
						SendAgent(who.dwSvrGroup, &msg);

						return INVALID_INSTANCE_KEY;
					}

					if (Enter(dwKey, who, dwMapID) == INVALID_INSTANCE_KEY)
					{
						msg.bDone = FALSE;
						SendAgent(who.dwSvrGroup, &msg);

						sc::writeLogError(stringformat("MatchSystem::Process() - PartyMember(%1%, %2%) cannot join to the instance(%3%, %4%) with the key(%5%).", who.dwSvrGroup, who.dwChaNum, LOWORD(dwMapID), HIWORD(dwMapID), dwKey));

						continue;
					}
					else
					{
						msg.bDone = TRUE;
						SendAgent(who.dwSvrGroup, &msg);
					}
				}
			}
		}
		break;

	case REQUEST_UNMATCH:
		{
			//
			// UnMatch
			//
			if (request.GetType() == REQUEST_UNMATCH)	//인던 플레이 중에 퇴장하는 경우
			{
				dwKey = INVALID_INSTANCE_KEY;

				INSTANCE_PLAYER& who = request.GetMember(0);

				dwKey = UnMatch(who);

				msg.dwSvrGroupNum = who.dwSvrGroup;
				msg.bRelease = TRUE;
				msg.bDone = (dwKey != INVALID_INSTANCE_KEY);
				SendAgent(who.dwSvrGroup, &msg);
			}
			break;
		}

	default:
		sc::writeLogError(stringformat("MatchSystem::Process() - Wrong request type(%1%) has come", request.GetType()));
		return dwKey;
	}

	return dwKey;
}
/*
						 Process

						  calls

							|
							|
							|
							v

			   one of the following two methods
					 (Match or Enter)
*/
//
//Request a matching with a specific MapID
//
DWORD _KEY_ MatchSystem::Match(INSTANCE_PLAYER &who, DWORD dwMapID, DWORD dwPartySize)
{
	//
	//0.Is the character already playing in another instance that has same mapID ?
	//
	MAP_PLAYER_TO_KEY_ITER iter = m_mapPlayer2Key.find(who);
	if (iter != m_mapPlayer2Key.end())
	{
		sc::writeLogError(stringformat("MatchSystem::Match() - Char(%1%, %2%) already plays in other instance with the key(%3%).", who.dwSvrGroup, who.dwChaNum, iter->second));

		return INVALID_INSTANCE_KEY;
	}

	DWORD dwType = m_pScriptMan->IsInstanceMap(LOWORD(dwMapID), HIWORD(dwMapID));

	BOOL  bBound	= (HIWORD(dwType) > 0);
	DWORD dwTO		= LOWORD(dwType);

	DWORD dwKey = INVALID_INSTANCE_KEY;

	//
	// 1.Is this mapID for an instance ?
	//
	GASSERT(dwTO > 0);
	if (dwTO <= 0)
	{
		sc::writeLogError(stringformat("MatchSystem::Match() - Char(%1%, %2%) is trying to match to WRONG instance(%3%, %4%).", who.dwSvrGroup, who.dwChaNum, LOWORD(dwMapID), HIWORD(dwMapID)));

		return INVALID_INSTANCE_KEY;
	}

	//GASSERT(dwTO >= dwPartySize);
	if (dwTO < dwPartySize)
	{
		sc::writeLogError(stringformat("MatchSystem::Match() - Char(%1%, %2%)'s party size(%3%) is bigger than room size(%4%) of the instance(%5%, %6%).", who.dwSvrGroup, who.dwChaNum, dwPartySize, dwTO, LOWORD(dwMapID), HIWORD(dwMapID)));

		return INVALID_INSTANCE_KEY;
	}


	//
	// 2.Check the Bound-type of the instance
	//
	if (bBound)
	{
		dwKey = GetBoundRoom(who, dwMapID, dwPartySize);

		if (dwKey != INVALID_INSTANCE_KEY)
		{
			return Join(dwKey, who, dwMapID);
		}
	}

	//
	// 3. Check other instances that has T.O.
	//
	dwKey = GetAvailableRoom(dwMapID, dwPartySize);

	if (dwKey != INVALID_INSTANCE_KEY)
	{
		MatchingRoom &room = m_mapRooms[dwKey];

 		dwKey = Join(dwKey, who, dwMapID);

		return dwKey;
	}	

	//
	// 4. Construct a new room
	//
	dwKey = ConstructRoom(dwMapID, dwTO, bBound);
	if (dwKey == INVALID_INSTANCE_KEY)
	{
		sc::writeLogError(stringformat("MatchSystem::Join() - Failed to construct a room(%3%, %4%).", who.dwSvrGroup, who.dwChaNum, LOWORD(dwMapID), HIWORD(dwMapID)));

		return dwKey;
	}
	else
	{
		return Join(dwKey, who, dwMapID);
	}
}


//
//Request to enter with a specific Key (follow the party-leader)
//
DWORD _KEY_ MatchSystem::Enter(DWORD dwKey, INSTANCE_PLAYER &who, DWORD dwMapID)
{
	DWORD dwType = m_pScriptMan->IsInstanceMap(LOWORD(dwMapID), HIWORD(dwMapID));

	BOOL  bBound	= (HIWORD(dwType) > 0);
	DWORD dwTO		= LOWORD(dwType);

	//
	// 1.Is this mapID for an instance ?
	//
	if (dwTO <= 0)
	{
		sc::writeLogError(stringformat("MatchSystem::Enter() - Char(%1%, %2%) is trying to match to WRONG instance(%3%, %4%).", who.dwSvrGroup, who.dwChaNum, LOWORD(dwMapID), HIWORD(dwMapID)));

		return INVALID_INSTANCE_KEY;
	}
	
	//
	// 2.Check the bound instance
	//
	if (bBound)
	{
		DWORD dwBoundKey = GetBoundRoom(who, dwMapID);

		//파티 신청인 경우, 본인이 귀속된 인던이 있다면 방장과 다른 (자신이 귀속된) 인던으로 들어가게 된다.
		if (dwBoundKey != INVALID_INSTANCE_KEY)
		{
			return Join(dwBoundKey, who, dwMapID);
		}

		//본인이 귀속된 동일한 인던이 없다면, 방장의 귀속 인던으로 들어간다.
	}
	

	//
	// 3.Join the room with the key (follow the party-leader)
	//
	GASSERT(dwKey != INVALID_INSTANCE_KEY);
	if (dwKey != INVALID_INSTANCE_KEY)
	{
		MatchingRoom &room = m_mapRooms[dwKey];
			
		return Join(dwKey, who, dwMapID);
	}
	else
	{
		sc::writeLogError(stringformat("MatchSystem::Enter() - Char(%1%, %2%) is trying to enter to instance(%3%, %4%) with WRONG key.", who.dwSvrGroup, who.dwChaNum, LOWORD(dwMapID), HIWORD(dwMapID), dwKey));
	}

	return INVALID_INSTANCE_KEY;
}
/*
					 Match and Enter

						  calls

							|
							|
							|
							v

						  Join
							
*/
//
//Join specific room with a Key
//
DWORD _KEY_ MatchSystem::Join(DWORD dwKey, INSTANCE_PLAYER &who, DWORD dwMapID)
{
	DWORD dwRet = m_mapRooms[dwKey].Join(dwKey, who, dwMapID);

	if (dwRet != INVALID_INSTANCE_KEY)
	{
		m_mapPlayer2Key.insert(MAP_PLAYER_TO_KEY_VALUETYPE(who, dwKey));
	}

	return dwRet;
}


//
//search for a bound room
//
DWORD _KEY_ MatchSystem::GetBoundRoom(INSTANCE_PLAYER &who, DWORD dwMapID, DWORD dwPartySize)
{
	DWORD dwKey = INVALID_INSTANCE_KEY;

	MAP_BOUND_ITER iterBound;
	MAP_INSTANCE_ROOM_ITER iterRoom;

	iterBound = m_mapCharBound.find(who);

	if (iterBound == m_mapCharBound.end())
	{
		return INVALID_INSTANCE_KEY;
	}

	SET_INSTANCEKEY &setKey = iterBound->second;

	for (SET_INSTANCEKEY_ITER iterKey = setKey.begin(); iterKey != setKey.end(); ++iterKey)
	{
		dwKey = *iterKey;

		iterRoom = m_mapRooms.find(dwKey);

		GASSERT(iterRoom != m_mapRooms.end());
		if (iterRoom == m_mapRooms.end())
		{
			//캐릭터의 귀속인던 목록에는 존재하나, 실제 room이 존재하지 않으면 로직 오류다!!

			sc::writeLogError(stringformat("MatchSystem::Match() - Char(%1%, %2%) couldn't match to the bound instance(%3%, %4%) with the Key(%5%). The room does not exist.", who.dwSvrGroup, who.dwChaNum, LOWORD(dwMapID), HIWORD(dwMapID), dwKey));
			
			continue;
		}

		MatchingRoom &room = iterRoom->second;

		if (room.GetMapID() == dwMapID)
		{
			if (room.TO() < dwPartySize)
			{
				sc::writeLogWarn(stringformat("MatchSystem::Match() - Char(%1%, %2%) cannot match to the bound instance(%3%, %4%). It has not enough T.O.(%5%/%6%)", who.dwSvrGroup, who.dwChaNum, LOWORD(dwMapID), HIWORD(dwMapID), dwPartySize, room.TO()));

				return INVALID_INSTANCE_KEY;
			}
			else
			{
				return dwKey;
			}
		}
	}

	return INVALID_INSTANCE_KEY;
}

DWORD _KEY_ MatchSystem::GetAvailableRoom(DWORD dwMapID, DWORD dwPartySize)
{
	DWORD dwKey = INVALID_INSTANCE_KEY;

	MMAP_MAP_2_KEY_ITER iterMap2Key;

	for (iterMap2Key = m_mmMapid2Key.lower_bound(dwMapID); iterMap2Key != m_mmMapid2Key.upper_bound(dwMapID); ++iterMap2Key)
	{
		dwKey = iterMap2Key->second;

		MAP_INSTANCE_ROOM_ITER iterRoom = m_mapRooms.find(dwKey);
		MatchingRoom &room = iterRoom->second;

		if (room.GetState() > ROOM_STATE_CONSTRUCT)	//not available room
		{
			continue;
		}

		if (room.IsBoundType())	//본인의 귀속 인던은 GetBoundRoom()에 먼저 확인되므로
		{
			continue;
		}
		
		if (!room.IsEmpty() && room.TO() >= dwPartySize)
		{
			return dwKey;
		}
	}

	return INVALID_INSTANCE_KEY;
}


DWORD _KEY_ MatchSystem::FreeRoom(DWORD dwKey)
{
	return DestructRoom(dwKey);
}


DWORD _KEY_ MatchSystem::ConstructRoom(DWORD dwMapID, DWORD dwTO, BOOL bBound)
{
	DWORD dwKey = GetFreeInstanceKey();

	if (dwKey == INVALID_INSTANCE_KEY)
	{
		sc::writeLogError(std::string("MatchSystem::ConstructRoom() - cannot get free Key!"));
		return dwKey;
	}

	DWORD dwExpireM = m_pScriptMan->GetExpireM(LOWORD(dwMapID), HIWORD(dwMapID));

	if (dwExpireM == 0)
	{
		dwExpireM = m_dwGlobalExpireM;
	}

	GASSERT(m_mapRooms.count(dwKey) <= 0);
	if (m_mapRooms.count(dwKey) > 0)
	{
		sc::writeLogError(stringformat("MatchSystem::ConstructRoom() - Key(%1%) is weird.", dwKey));

		m_mapRooms.erase(dwKey);
	}

	m_mapRooms.insert(MAP_INSTANCE_ROOM_VALUETYPE(dwKey, MatchingRoom(this, dwKey, dwMapID, dwTO, bBound, dwExpireM)));
	m_mmMapid2Key.insert(MMAP_MAP_2_KEY_VALUETYPE(dwMapID, dwKey));

	return dwKey;
}

DWORD _KEY_ MatchSystem::DestructRoom(DWORD dwKey)
{
	if (dwKey == INVALID_INSTANCE_KEY)
	{
		sc::writeLogError(std::string("MatchSystem::DestructRoom() - Key is invalid!"));
		return dwKey;
	}

	MAP_INSTANCE_ROOM_ITER iter = m_mapRooms.find(dwKey);

	GASSERT(iter != m_mapRooms.end());
	if (iter == m_mapRooms.end())
	{
		sc::writeLogError(stringformat("MatchSystem::DestructRoom() - Key(%1%) is weird.", dwKey));
	}

	DWORD dwMapID = (iter->second).GetMapID();

	m_mapRooms.erase(dwKey);
	

	MMAP_MAP_2_KEY_ITER iterMap2Key;
	for (iterMap2Key = m_mmMapid2Key.lower_bound(dwMapID); iterMap2Key != m_mmMapid2Key.upper_bound(dwMapID); ++iterMap2Key)
	{
		if (dwKey == iterMap2Key->second)
		{
			m_mmMapid2Key.erase(iterMap2Key);
			break;
		}
	}

	MAP_BOUND_ITER iterBound = m_mapCharBound.begin();

	for (; iterBound != m_mapCharBound.end(); ++iterBound)
	{
		SET_INSTANCEKEY &setKey = iterBound->second;

		DWORD dwKeyBound = INVALID_INSTANCE_KEY;
		for (SET_INSTANCEKEY_ITER iterSet = setKey.begin(); iterSet != setKey.end();)
		{
			dwKeyBound = *iterSet;

			if (dwKeyBound == dwKey)
			{
				iterSet = setKey.erase(iterSet);
			}
			else
			{
				++iterSet;
			}
		}
	}

	ReleaseInstanceKey(dwKey);

	return dwKey;
}

DWORD _KEY_ MatchSystem::Quit(DWORD dwKey, INSTANCE_PLAYER &who)
{
	MAP_INSTANCE_ROOM_ITER iter = m_mapRooms.find(dwKey);
	if (iter != m_mapRooms.end())
	{
		MatchingRoom &room = iter->second;
		
		DWORD dwRet = room.Quit(who);
	}
	else
	{
		sc::writeLogError(stringformat("MatchSystem::Quit() - The room(%1%) does not exist", dwKey));
	}

	return INVALID_INSTANCE_KEY;
}

//current key that the character has
void MatchSystem::SetKey(INSTANCE_PLAYER &who, DWORD dwKey)
{
	GASSERT(dwKey != INVALID_INSTANCE_KEY);

	MAP_PLAYER_TO_KEY_ITER iter = m_mapPlayer2Key.find(who);

	if (iter != m_mapPlayer2Key.end())
	{
		m_mapPlayer2Key.erase(iter);
	}

	m_mapPlayer2Key.insert(MAP_PLAYER_TO_KEY_VALUETYPE(who, dwKey));
}

void MatchSystem::ResetKey(INSTANCE_PLAYER &who)
{
	GASSERT(who.IsValid());

	MAP_PLAYER_TO_KEY_ITER iter = m_mapPlayer2Key.find(who);

	if (iter != m_mapPlayer2Key.end())
	{
		m_mapPlayer2Key.erase(iter);
	}
}

DWORD _KEY_ MatchSystem::GetKey(INSTANCE_PLAYER &who)
{
	GASSERT(who.IsValid());

	MAP_PLAYER_TO_KEY_ITER iter = m_mapPlayer2Key.find(who);

	if (iter != m_mapPlayer2Key.end())
	{
		return (iter->second);
	}

	return INVALID_INSTANCE_KEY;
}

DWORD MatchSystem::GetInstanceServerNum(DWORD dwSvrGroupNum, DWORD dwChaNum)
{
	GASSERT(dwSvrGroupNum <= MAX_SERVER_GROUP);
	GASSERT(dwChaNum != INVALID_CHANUM);

	if (dwSvrGroupNum > MAX_SERVER_GROUP || dwChaNum == INVALID_CHANUM)
		return MAXNUM_OF_INSTANCESERVER_TO_MATCH;

	INSTANCE_PLAYER who(dwSvrGroupNum, dwChaNum);
	DWORD dwKey = INVALID_INSTANCE_KEY;

	MAP_PLAYER_TO_KEY_ITER iter = m_mapPlayer2Key.find(who);

	if (iter != m_mapPlayer2Key.end())
	{
		dwKey = iter->second;

		return GetInstanceServerNum(dwKey);
	}

	return MAXNUM_OF_INSTANCESERVER_TO_MATCH;
}

DWORD MatchSystem::GetInstanceServerNum(DWORD dwKey)
{
	GASSERT(dwKey != INVALID_INSTANCE_KEY);

	if (dwKey == INVALID_INSTANCE_KEY)
		return MAXNUM_OF_INSTANCESERVER_TO_MATCH;

	MAP_INSTANCE_ROOM_ITER iter = m_mapRooms.find(dwKey);
	if (iter != m_mapRooms.end())
	{
		MatchingRoom &room = iter->second;

		return room.GetInstanceServerNum();
	}
	else
	{
		sc::writeLogError(stringformat("MatchSystem::GetInstanceServerNum() - The room(%1%) does not exist", dwKey));
	}

	return MAXNUM_OF_INSTANCESERVER_TO_MATCH;
}

DWORD _KEY_ MatchSystem::Bind(DWORD dwKey, INSTANCE_PLAYER &who)
{
	if (dwKey == INVALID_INSTANCE_KEY)
	{
		sc::writeLogError(stringformat("MatchSystem::Bind() - Char(%1%, %2%) cannot be bound by the room with a invalid Key.", who.SvrGroup(), who.ChaDbNum()));
		return INVALID_INSTANCE_KEY;
	}
	
	if (!who.IsValid())
	{
		sc::writeLogError(stringformat("MatchSystem::Bind() - Invalid character cannot be bound by a room with a Key(%1%).", dwKey));
		return INVALID_INSTANCE_KEY;
	}

	MAP_BOUND_ITER iterBound = m_mapCharBound.find(who);

	if (iterBound == m_mapCharBound.end())
	{
		SET_INSTANCEKEY setKey;
		setKey.insert(dwKey);

		bool bRet = m_mapCharBound.insert(MAP_BOUND_VALUETYPE(who, setKey)).second;

		if (bRet == false)
		{
			sc::writeLogError(stringformat("MatchSystem::Bind(map) - Char(%1%, %2%) cannot be bound by the room with a Key(%3%).", who.SvrGroup(), who.ChaDbNum(), dwKey));
			
			return INVALID_INSTANCE_KEY;
		}
	}
	else
	{
		SET_INSTANCEKEY &setKey = iterBound->second;

		setKey.insert(dwKey);
	}

	return dwKey;
}

DWORD _KEY_ MatchSystem::UnMatch(INSTANCE_PLAYER &who)
{
	DWORD dwKey = GetKey(who);

	if (dwKey == INVALID_INSTANCE_KEY)
	{
		sc::writeLogError(stringformat("MatchSystem::UnMatch() - Char(%1%, %2%) is not in a room.", who.SvrGroup(), who.ChaDbNum()));

		return dwKey;
	}

	MAP_INSTANCE_ROOM_ITER iter = m_mapRooms.find(dwKey);

	if (iter == m_mapRooms.end())
	{
		sc::writeLogError(stringformat("MatchSystem::UnMatch - The room(%1%) does not exist.", dwKey));
		return INVALID_INSTANCE_KEY;
	}

	MatchingRoom &room = iter->second;

	room.KickOutPlayer(who);

	return dwKey;
	
	//return Quit(dwKey, who);
}


BOOL MatchSystem::IsCanceled(DWORD64& dw64ID)
{
	if (m_setCancel.count(dw64ID) > 0)
	{
		//this request is canceled

		return TRUE;
	}

	return FALSE;
}


//해당 맵의 귀속을 풀기
DWORD _KEY_ MatchSystem::UnBind(INSTANCE_PLAYER &who, DWORD dwMapID)
{
	if (dwMapID == INVALID_MAP_ID)
	{
		sc::writeLogError(stringformat("MatchSystem::UnBind() - Char(%1%, %2%) cannot UnBind with a mapID.", who.SvrGroup(), who.ChaDbNum()));
		return INVALID_INSTANCE_KEY;
	}
	
	if (!who.IsValid())
	{
		sc::writeLogError(stringformat("MatchSystem::UnBind() - Invalid character cannot UnBind with mapID(%1%, %2%).", LOWORD(dwMapID), HIWORD(dwMapID)));
		return INVALID_INSTANCE_KEY;
	}

	MAP_BOUND_ITER iterBound = m_mapCharBound.find(who);

	if (iterBound == m_mapCharBound.end())
	{
		sc::writeLogError(stringformat("MatchSystem::UnBind() - Char(%1%, %2%) does not exist in bound list to the room with mapID(%3%, %4%).", who.SvrGroup(), who.ChaDbNum(), LOWORD(dwMapID), HIWORD(dwMapID)));
		return INVALID_INSTANCE_KEY;
	}
	else
	{
		SET_INSTANCEKEY &setKey = iterBound->second;

		DWORD dwKey = INVALID_INSTANCE_KEY;
		for (SET_INSTANCEKEY_ITER iter = setKey.begin(); iter != setKey.end(); ++iter)
		{
			dwKey = *iter;

			MAP_INSTANCE_ROOM_ITER iterRoom = m_mapRooms.find(dwKey);
			if (iterRoom != m_mapRooms.end())
			{
				MatchingRoom &room = iterRoom->second;
				
				if (room.GetMapID() == dwMapID)
				{
					setKey.erase(iter);

					return dwKey;
				}
			}
		}

		sc::writeLogError(stringformat("MatchSystem::UnBind() - Char(%1%, %2%) is not bound to the room with a Key(%3%).", who.SvrGroup(), who.ChaDbNum(), dwKey));
	}

	return INVALID_INSTANCE_KEY;
}


void MatchSystem::PrintMatchSystemInfo()
{
	static DWORD dwTickPrev = 0;

	DWORD dwTickNow = GetTickCount();

	if ((dwTickNow - dwTickPrev) >=	FREQUENCY_UPDATE_MATCHINFO)
	{
		dwTickPrev = dwTickNow;

		sc::writeLogInfo(stringformat("-Total running instance(s): %1%", GetRunningInstanceNumber()));
		sc::writeLogInfo(stringformat("-Total instance player(s): %1%", m_mapPlayer2Key.size()));
		sc::writeLogInfo(std::string("-InstancesServer status:"));

		for (DWORD i=0; i<MAXNUM_OF_INSTANCESERVER_TO_MATCH; i++)
		{
			if (m_vecInstanceSvrStats[i] != MAXDWORD)
			{
				sc::writeLogInfo(stringformat("  +InstanceSvr[%1%] is running %2% instance(s)", i, m_vecInstanceSvrStats[i]));
			}
		}
	}
}


DWORD _KEY_ MatchSystem::GetFreeInstanceKey()
{
#ifdef _USE_TBB

	DWORD dwKey = 0;
	
	if( m_cqKey_Consumer.try_pop(dwKey) == false)	//empty
	{
		{	//for the scope of the scoped lock
			sc::CriticalSectionOwnerLock OwnerLock(m_CS_Key);
			
			if( m_cqKey_Consumer.empty() ) //double-check for empty 
			{
				if( m_cqKey_Producer.empty() )
				{
					sc::writeLogError(std::string("MatchSystem::GetFreeInstanceKey() - There is not enough Key."));
					return INVALID_INSTANCE_KEY;
				}

				DWORD nFreedKey = 0;
				//DWORD cnt = 0;
				
				while( m_cqKey_Producer.try_pop(nFreedKey) )
				{
					if( m_setKey_Filter.insert(nFreedKey).second )
					{
						m_cqKey_Consumer.push(nFreedKey);
					}
				}

				//cnt = (DWORD)m_setKey_Filter.size();
				m_setKey_Filter.clear();
			}
		}
		
		if( m_cqKey_Consumer.try_pop(dwKey) == false)
		{
			sc::writeLogError(std::string("(CRITICAL!) MatchSystem::GetFreeInstanceKey() - Cannot get a Key!"));
			return INVALID_INSTANCE_KEY;
		}
	}

	return dwKey;

#else

	sc::writeLogError(std::string("MatchSystem requires TBB! Turn on TBB for MatchServer!"));
    return 0;
	
#endif
}

void MatchSystem::ReleaseInstanceKey(DWORD dwKey)
{
#ifdef _USE_TBB
	
	m_cqKey_Producer.push(dwKey);

#else

	sc::writeLogError(std::string("MatchSystem requires TBB! Turn on TBB for MatchServer!"));

#endif

}


DWORD MatchSystem::GetRunningInstanceNumber()
{
#ifdef _USE_TBB
	/*
	unsafe_size()

	Returns number of items in queue. May return incorrect value 
	if any push or try_pop operations are concurrently in flight.
	*/

	DWORD dwHowMany = MAXNUM_OF_INSTANCE_PER_MATCHSERVER - (DWORD)( m_cqKey_Consumer.unsafe_size() + m_cqKey_Producer.unsafe_size() );

	return dwHowMany;
	
#else

	sc::writeLogError(std::string("MatchSystem requires TBB! Turn on TBB for MatchServer!"));
	return 0;

#endif
}

void MatchSystem::TransferToInstance(DWORD dwKey)
{
	MAP_INSTANCE_ROOM_ITER iter = m_mapRooms.find(dwKey);

	if (iter == m_mapRooms.end())
	{
		sc::writeLogError(stringformat("MatchSystem::TransferToInstance - The room(%1%) does not exist.", dwKey));
		return;
	}

	MatchingRoom &room = iter->second;

	room.BringParty();
}

void MatchSystem::TransferFromInstance(DWORD dwKey)
{
	MAP_INSTANCE_ROOM_ITER iter = m_mapRooms.find(dwKey);

	if (iter == m_mapRooms.end())
	{
		sc::writeLogError(stringformat("MatchSystem::TransferFromInstance - The room(%1%) does not exist.", dwKey));
		return;
	}

	MatchingRoom &room = iter->second;

	room.KickOutParty();
}

BOOL MatchSystem::SendInstance(DWORD dwInstanceNum, NET_MSG_GENERIC *nmg)
{
	return m_pMatchServer->SendInstance(dwInstanceNum, nmg);
}

BOOL MatchSystem::SendInstance(NET_MSG_GENERIC *nmg, DWORD dwKey)
{
	DWORD dwInstanceNum = GetInstanceServerNum(dwKey);

	return m_pMatchServer->SendInstance(dwInstanceNum, nmg);
}

void MatchSystem::SendAgent(DWORD dwSvrGroupNum, NET_MSG_GENERIC *nmg)
{
	m_pMatchServer->SendAgent(dwSvrGroupNum, nmg);
}

void MatchSystem::SendAgent(NET_MSG_UNIQUEKEY *nmu)
{
	m_pMatchServer->SendAgent(nmu->dwSvrGroupNum, nmu);
}

void MatchSystem::SendClient(UNIQUEID &who, NET_MSG_CHARACTER *nmc)
{
	GASSERT(m_pMatchServer);
	GASSERT(who.IsValid());

	nmc->dwChaNum = who.ChaDbNum();
	m_pMatchServer->SendClient(who.dwSvrGroup, nmc);
}


void MatchSystem::SendClient(NET_MSG_UNIQUEKEY *nmu)
{
	GASSERT(m_pMatchServer);

	m_pMatchServer->SendClient(nmu);
}
