//mjeon.instance.match

#pragma once

#ifdef _USE_TBB
#include "../../=TBB/include/tbb/concurrent_queue.h"
#endif

#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/gassert.h"
#include "../../enginelib/ServiceProviderDefine.h"
#include "../../enginelib/G-Logic/TypeDefine.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/MatchSystem/MatchDefine.h"

#include <time.h>
#include <set>
#include <map>
#include <queue>
#include <list>

#include <boost/tr1/memory.hpp>


struct NET_MSG_GENERIC;

class MatchServer;
class MatchingRoom;
class MatchSystem;
class MatchScriptMan;

struct INSTANCE_PLAYER;
struct INSTANCE_PARTY;

#ifdef _USE_TBB
	typedef tbb::concurrent_queue<MATCH_REQUEST *>			CQUEUE_MATCH_REQUEST;
	typedef tbb::concurrent_queue<DWORD _KEY_>				CQUEUE_INSTANCEKEY;
#else
	typedef std::queue<MATCH_REQUEST *>			CQUEUE_MATCH_REQUEST;
	typedef std::queue<DWORD _KEY_>				CQUEUE_INSTANCEKEY;
#endif

typedef std::set<DWORD _KEY_>							SET_INSTANCEKEY;
typedef std::set<DWORD _KEY_>::iterator					SET_INSTANCEKEY_ITER;

typedef std::list<INSTANCE_PLAYER>						LIST_INSTANCE_PLAYER;
typedef std::list<INSTANCE_PLAYER>::iterator			LIST_INSTANCE_PLAYER_ITER;

//<Key, Room>
typedef std::map<DWORD _KEY_, MatchingRoom>						MAP_INSTANCE_ROOM;
typedef std::map<DWORD _KEY_, MatchingRoom>::iterator			MAP_INSTANCE_ROOM_ITER;
typedef std::map<DWORD _KEY_, MatchingRoom>::value_type			MAP_INSTANCE_ROOM_VALUETYPE;

//<SvgGroup+ChaNum, set<Key>>
typedef std::map<INSTANCE_PLAYER, SET_INSTANCEKEY>				MAP_BOUND;
typedef std::map<INSTANCE_PLAYER, SET_INSTANCEKEY>::iterator	MAP_BOUND_ITER;
typedef std::map<INSTANCE_PLAYER, SET_INSTANCEKEY>::value_type	MAP_BOUND_VALUETYPE;

//<SvgGroup+ChaNum, Key>
typedef std::map<INSTANCE_PLAYER, DWORD _KEY_>					MAP_PLAYER_TO_KEY;
typedef std::map<INSTANCE_PLAYER, DWORD _KEY_>::iterator		MAP_PLAYER_TO_KEY_ITER;
typedef std::map<INSTANCE_PLAYER, DWORD _KEY_>::value_type		MAP_PLAYER_TO_KEY_VALUETYPE;

//request of match cancel
typedef std::set<DWORD64>								SET_CANCEL;
typedef std::set<DWORD64>::iterator						SET_CANCEL_ITER;

typedef std::vector<DWORD>								VEC_INSTANCESVRNUM_STATS;
typedef std::vector<DWORD>::iterator					VEC_INSTANCESVRNUM_STATS_ITER;

typedef std::multimap<DWORD, DWORD>						MMAP_MAP_2_KEY;		//<mapID, Key>
typedef std::multimap<DWORD, DWORD>::iterator			MMAP_MAP_2_KEY_ITER;
typedef std::multimap<DWORD, DWORD>::value_type			MMAP_MAP_2_KEY_VALUETYPE;


enum MatchingRoomState
{
	ROOM_STATE_READY		= 0,	//joinable
	ROOM_STATE_CONSTRUCT	= 1,	//playable & joinable
	ROOM_STATE_DESTRUCT		= 2,	//not joinable & not playable
	ROOM_STATE_SIZE			= 3,
};


//
//room: virtual area that makes matches for join requests.
//
class MatchingRoom
{
public:
	MatchingRoom();
	MatchingRoom(MatchSystem *pMatchSystem, DWORD dwKey, DWORD dwMapId, DWORD dwTO, BOOL bBound, DWORD dwExpireM);
	virtual ~MatchingRoom();

	BOOL	IsValid()
	{
		GASSERT(m_pMatchSystem != NULL);
		GASSERT(m_dwKey != INVALID_INSTANCE_KEY);
		GASSERT(m_dwMapID != INVALID_MAP_ID);

		if (m_pMatchSystem == NULL)
		{
			sc::writeLogError(stringformat("MatchingRoom::IsValid() - Room(%1%: %2%, %3%)'s MatchSystem is NULL!!", m_dwKey, LOWORD(m_dwMapID), HIWORD(m_dwMapID)));
		}

		if (m_dwKey == INVALID_INSTANCE_KEY)
		{
			sc::writeLogError(stringformat("MatchingRoom::IsValid() - Key of the room(%1%, %2%) is invalid!!", LOWORD(m_dwMapID), HIWORD(m_dwMapID)));
		}

		if (m_dwMapID == INVALID_MAP_ID)
		{
			sc::writeLogError(stringformat("MatchingRoom::IsValid() - MapID of the room(%1%) is invalid!!", m_dwKey));
		}

		return ((m_dwKey != INVALID_INSTANCE_KEY) && (m_dwMapID != INVALID_MAP_ID) && (m_pMatchSystem != NULL));
	}

	BOOL	IsExpired();

	DWORD _KEY_ Join(DWORD dwKey, INSTANCE_PLAYER &who, DWORD dwMapID);
	DWORD _KEY_ Quit(INSTANCE_PLAYER &who);
	void	QuitAll();

	BOOL	IsReady() {return (m_state == ROOM_STATE_READY);}

	BOOL	IsDestructing() {return (m_state == ROOM_STATE_DESTRUCT);}

	BOOL	IsFull() {return (m_dwMaxTO == size());}
	BOOL	IsEmpty() {return (size() == 0);}
	DWORD	size() {return (DWORD)m_lstPlayers.size();}
	DWORD	TO() {return m_dwMaxTO - size();}

	DWORD	GetMapID() {return m_dwMapID;}
	DWORD	GetKey() {return m_dwKey;}

	BOOL	IsBoundType() {return m_bBound;}

	DWORD _KEY_	Bind(INSTANCE_PLAYER &who);	//all members of the party will be bound by this instance.
	void		BindAll();

	MatchingRoomState GetState() {return m_state;}
	void SetState(MatchingRoomState state) {m_state = state;}
	void MoveState()
	{
		GASSERT(m_state < ROOM_STATE_DESTRUCT);

		DWORD dwState = (DWORD)m_state;

		++dwState;

		dwState = dwState%ROOM_STATE_SIZE;

		m_state = (MatchingRoomState)dwState;

		GASSERT(m_state < ROOM_STATE_SIZE);
	}

	void SetInstanceServerNum(DWORD dwInstanceServerNum)
	{
		m_dwInstanceServerNum = dwInstanceServerNum;
	}

	DWORD GetInstanceServerNum()
	{
		return m_dwInstanceServerNum;
	}

	void BringParty();
	void BringPlayer(INSTANCE_PLAYER &who);

	void KickOutParty();
	void KickOutPlayer(INSTANCE_PLAYER &who);

	void CheckCancel();


protected:
	DWORD		m_dwKey;		//Instance Key	= Unique Identity
	DWORD		m_dwMapID;		//InstanceID	= MapID(MID + SID)
	DWORD		m_dwMaxTO;
	BOOL		m_bBound;		//귀속인던인가?
	DWORD		m_dwInstanceServerNum;	//이 방이 배정받은 InstaceServer 번호
	DWORD		m_dwExpireM;

	MatchingRoomState	m_state;

	CTime		m_ctCreateTime;
	CTime		m_ctFilledTime;	//room이 full된 시점

	LIST_INSTANCE_PLAYER	m_lstPlayers;	//List of the characters playing in this instance

	MatchSystem *m_pMatchSystem;
};


//
//MatchSystem does not depend on the game logic like GLChar, GLCrow and etc...
//

class MatchSystem
{
public:
	MatchSystem(MatchServer *pMatchServer);
	virtual ~MatchSystem();

	BOOL Initialize();

	void FrameMove();	//FrameMove에서 조건에 맞는 Full or Empty된 방의 생성/파괴를 수행한다.
	DWORD _KEY_ Process(MATCH_REQUEST &request);	//request 처리
	void RunStateMachine();							//room state process

	void Request(MATCH_REQUEST *pRequest);			//request 등록

	void Cancel(DWORD64& id);						//matching 취소

	//UnMatch는 Transfer 이후에 전달받은 메시지에 의해 이루어지므로, msg handler에서 Quit을 호출하기 위해 public method 제공
	void FinishUnmatch(DWORD dwKey, INSTANCE_PLAYER &who)	//UnMatch 마무리
	{
		Quit(dwKey, who);
	}

	void TransferToInstance(DWORD dwKey);
	void TransferFromInstance(DWORD dwKey);

	DWORD _KEY_ FreeRoom(DWORD dwKey);

	void SetKey(INSTANCE_PLAYER &who, DWORD dwKey);
	void ResetKey(INSTANCE_PLAYER &who);
	DWORD _KEY_ GetKey(INSTANCE_PLAYER &who);

	DWORD GetInstanceServerNum(DWORD dwSvrGroupNum, DWORD dwChaNum);
	DWORD GetInstanceServerNum(DWORD dwKey);

	BOOL IsCanceled(DWORD64& dw64ID);


public:
	//
	// InstanceServer management
	//
	void NewInstanceServer(DWORD dwSvrNum)
	{
		GASSERT(dwSvrNum < MAXNUM_OF_INSTANCESERVER_TO_MATCH);

		m_vecInstanceSvrStats[dwSvrNum] = 0;
	}

	void ClosedInstanceServer(DWORD dwSvrNum)
	{
		GASSERT(dwSvrNum < MAXNUM_OF_INSTANCESERVER_TO_MATCH);

		m_vecInstanceSvrStats[dwSvrNum] = MAXDWORD;
	}

	DWORD PickInstanceServer()
	{
		VEC_INSTANCESVRNUM_STATS_ITER iter = std::min_element(m_vecInstanceSvrStats.begin(), m_vecInstanceSvrStats.end());

		if (*iter == MAXDWORD)
		{
			sc::writeLogError(std::string("PickInstanceServer - There's no available InstanceServer"));

			return MAXNUM_OF_INSTANCESERVER_TO_MATCH;
		}
		
		return (DWORD)std::distance(m_vecInstanceSvrStats.begin(), iter);
	}

	DWORD PickInstanceServerExcept(DWORD dwExceptSvrNum)
	{
		DWORD dwMinIdx	= MAXNUM_OF_INSTANCESERVER_TO_MATCH;
		DWORD dwMin		= MAXDWORD;

		for (DWORD i=0; i<MAXNUM_OF_INSTANCESERVER_TO_MATCH; i++)
		{
			if (i == dwExceptSvrNum)
				continue;

			if (m_vecInstanceSvrStats[i] < dwMin)
			{
				dwMinIdx = i;
				dwMin = m_vecInstanceSvrStats[i];
			}
		}

		if (dwMin == MAXDWORD)
		{
			sc::writeLogError(stringformat("PickInstanceServerExcept - There's no more InstanceServer except %1%", dwExceptSvrNum));
			
			return MAXNUM_OF_INSTANCESERVER_TO_MATCH;
		}

		return dwMinIdx;
	}

	void StateInstanceConstruction(DWORD dwSvrNum)
	{
		GASSERT(dwSvrNum < MAXNUM_OF_INSTANCESERVER_TO_MATCH);

		m_vecInstanceSvrStats[dwSvrNum]++;
	}

	void StateInstanceDestruction(DWORD dwSvrNum)
	{
		GASSERT(dwSvrNum < MAXNUM_OF_INSTANCESERVER_TO_MATCH);

		m_vecInstanceSvrStats[dwSvrNum]--;

		if (m_vecInstanceSvrStats[dwSvrNum] < 0) // always false
			m_vecInstanceSvrStats[dwSvrNum] = 0;
	}

	DWORD GetGlobalExpireM() {return m_dwGlobalExpireM;}

	void PrintMatchSystemInfo();

public:
	// network methods
	void SendClient(UNIQUEID &who, NET_MSG_CHARACTER *nmc);
	void SendClient(NET_MSG_UNIQUEKEY *nmu);

	BOOL SendInstance(DWORD dwInstanceNum, NET_MSG_GENERIC *nmg);
	BOOL SendInstance(NET_MSG_GENERIC *nmg, DWORD dwKey);

	void SendAgent(DWORD dwSvrGroupNum, NET_MSG_GENERIC *nmg);
	void SendAgent(NET_MSG_UNIQUEKEY *nmu);
	
		
protected:
	// match methods
	DWORD _KEY_ Match(INSTANCE_PLAYER &who, DWORD dwMapID, DWORD dwPartySize = 1);	//Match = Finding a room + Enter
	DWORD _KEY_ Enter(DWORD dwKey, INSTANCE_PLAYER &who, DWORD dwMapID);			//Enter = Enter to the room with a specific key

	DWORD _KEY_ Join(DWORD dwkey, INSTANCE_PLAYER &who, DWORD dwMapID);				//Match or Enter가 성공일 경우 지정된 방으로 Join을 위해 호출
	DWORD _KEY_ Quit(DWORD dwKey, INSTANCE_PLAYER &who);							//UnMatch가 성공일 경우 matching room으로 부터 Quit을 위해 호출

	DWORD _KEY_ UnMatch(INSTANCE_PLAYER &who);										//UnMatch = Transfer-back the character to FieldServer

	DWORD _KEY_ Bind(DWORD dwKey, INSTANCE_PLAYER &who);
	DWORD _KEY_ UnBind(INSTANCE_PLAYER &who, DWORD dwMapID);


protected:
	// room methods
	DWORD _KEY_ GetBoundRoom(INSTANCE_PLAYER &who, DWORD dwMapID, DWORD dwPartySize = 1);
	DWORD _KEY_ GetAvailableRoom(DWORD dwMapID, DWORD dwPartySize);

	DWORD _KEY_	ConstructRoom(DWORD dwMapID, DWORD dwTO, BOOL bBound);
	DWORD/*MapID*/	DestructRoom(DWORD dwKey);


protected:
	// key management methods
	DWORD	GetFreeInstanceKey();
	void	ReleaseInstanceKey(DWORD dwKey);
	DWORD	GetRunningInstanceNumber();


protected:
	//
	//Join Requests
	//
	CQUEUE_MATCH_REQUEST	m_cqRequests;	//queue for joiners who're waiting for matching for an instance	

	//
	//Room management
	//
	MAP_INSTANCE_ROOM		m_mapRooms;			//<Key, MatchingRoom> - virtual instance rooms / real rooms are made by InstanceServer.
	
	MAP_BOUND				m_mapCharBound;
	
	MMAP_MAP_2_KEY			m_mmMapid2Key;		//<mapID, Key>	

	MAP_PLAYER_TO_KEY		m_mapPlayer2Key;	//<SvgGroup+ChaNum, Key>

	SET_CANCEL				m_setCancel;		//players who requested to cancel match

	//
	//Key management
	//
	CQUEUE_INSTANCEKEY	m_cqKey_Consumer;	//only pop
	CQUEUE_INSTANCEKEY	m_cqKey_Producer;	//only push
	SET_INSTANCEKEY		m_setKey_Filter;	//filter for same keys

	VEC_INSTANCESVRNUM_STATS	m_vecInstanceSvrStats;	//InstanceServer statistics(connections)

	DWORD					m_dwDailyResetH;
	DWORD					m_dwDailyResetM;

	DWORD					m_dwGlobalExpireM;

private:
	MatchServer *m_pMatchServer;
	MatchScriptMan *m_pScriptMan;

	CRITICAL_SECTION	m_CS_Key;

	friend class MatchingRoom;
};

