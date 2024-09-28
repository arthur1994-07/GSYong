#pragma once

#include "../../enginelib/G-Logic/TypeDefine.h"
#include "../../RanLogic/Party/GLPartyDefine.h"
#include "../../RanLogic/InstanceSystem/GLInstanceSystem.h"
#include <map>
#include <vector>
#include <list>
#include <queue>

#define DEFINSTANCE_DO_OUT_TIMER 0.5f
#define DEFINSTANCE_REQUEST_JOIN_TIMER  0.5f
#define DEFINSTANCE_SET_POSITION_TIMER  0.5f
#define DEFINSTANCE_UPDATE_TIMER        1.0f
#define DEFINSTANCE_TIME_TO_DESTROY	3.0f

class GLGaeaServer;
class GLAgentServer;
class GLCharAG;

struct NET_MSG_GENERIC;
namespace InstanceSystem
{	
	class ScriptManager;

	enum EMMEMBER_STATE
	{
		EMMEMBER_NULL								= 0x00000000L,
		EMMEMBER_NOT_INITIALIZE				= 0x00000001L, // 갓 등록 되었을 때의 상태;
		EMMEMBER_REQUEST_JOIN_FIELD		= 0x00000002L, // 필드에 입장 가능한지 요청 보낸 상태;
		EMMEMBER_CONFIRM_JOIN_FILED		= 0x00000004L, // 입장을 허가 받은 상태;
		EMMEMBER_IN_GUARD						= 0x00000008L, // 이동 처리 중;
		EMMEMBER_SET_TO_REMOVE				= 0x00000010L, // 삭제 대기 중;
	};

	enum EMINSTANCE_STATE
	{
		EMINSTANCE_NULL							= 0x00000000L,
		EMINSTANCE_JOIN_LOCK					= 0x00000001L, // 입장 불가;
		EMINSTANCE_JOIN_SUSPEND				= 0x00000002L, // 입장 지연;
		EMINSTANCE_CREATE_COMPLETE		= 0x00000004L, // 생성 완료;
		EMINSTANCE_RESERVE_DESTROY		= 0x00000008L, // 삭제 예약;
		EMINSTANCE_ALLOW_WATCHING		= 0x00000010L, // 관전 허용;
	};
	

	class MemberField;
	class MemberAgent;
	class InstanceField;
	class InstanceAgent;
    class InstanceBaseAgent;
	class ReloadState;
	class InformationPosition;

	// 인던에 속한 플레이어 목록;
	typedef std::map<DBNUM, MemberField> MemberFieldMap;
	typedef MemberFieldMap::const_iterator MemberFieldMapCIter;
	typedef MemberFieldMap::iterator MemberFieldMapIter;
    typedef MemberFieldMap::value_type MemberFieldValue;

	typedef std::queue<InformationPosition> InformationPositionQueue;

	typedef std::map<DBNUM, MemberAgent> MemberAgentMap;
	typedef MemberAgentMap::const_iterator MemberAgentMapCIter;
	typedef MemberAgentMap::iterator MemberAgentMapIter;
	typedef MemberAgentMap::value_type MemberAgentValue;


	// 인던 목록;
	typedef std::map<InstanceMapID, InstanceField> InstanceFieldMap;
	typedef InstanceFieldMap::const_iterator InstanceFieldMapCIter;
	typedef InstanceFieldMap::iterator InstanceFieldMapIter;
	typedef InstanceFieldMap::value_type InstanceFieldValue;

	typedef std::map<InstanceMapID, InstanceAgent> InstanceAgentMap;
	typedef InstanceAgentMap::const_iterator InstanceAgentMapCIter;
	typedef InstanceAgentMap::iterator InstanceAgentMapIter;
	typedef InstanceAgentMap::value_type InstanceAgentValue;	

    typedef std::map<SNATIVEID, InstanceBaseAgent> InstanceBaseAgentMap;
    typedef InstanceBaseAgentMap::const_iterator InstanceBaseAgentMapCIter;
    typedef InstanceBaseAgentMap::iterator InstanceBaseAgentMapIter;
    typedef InstanceBaseAgentMap::value_type InstanceBaseAgentValue;	

	typedef std::map<SNATIVEID, InstanceLog> InstanceLogMap;
	typedef InstanceLogMap::const_iterator InstanceLogMapCIter;
	typedef InstanceLogMap::iterator InstanceLogMapIter;
	typedef InstanceLogMap::value_type InstanceLogValue;

    typedef std::vector<SNATIVEID> MapIDVector;
    typedef MapIDVector::const_iterator MapIDVectorCIter;
    typedef MapIDVector::iterator MapIDVectorIter;

    typedef std::vector<DBNUM> DBNUMVector;
    typedef DBNUMVector::const_iterator DBNUMVectorCIter;
    typedef DBNUMVector::iterator DBNUMVectorIter;



    // 타이머 목록 (맵);
    typedef std::map<TimerHandle, InstanceTimer> InstanceTimerMap;
    typedef InstanceTimerMap::const_iterator InstanceTimerMapCIter;
    typedef InstanceTimerMap::iterator InstanceTimerMapIter;
    typedef InstanceTimerMap::value_type InstanceTimerValue;

	// 타이머 핸들;
	typedef std::queue<TimerHandle> TimerHandleQueue;

    // 특정 시간 타이머 목록 (맵);
    typedef std::map<TimerHandle, InstanceAbsTimer> InstanceAbsTimerMap;
    typedef InstanceAbsTimerMap::const_iterator InstanceAbsTimerMapCIter;
    typedef InstanceAbsTimerMap::iterator InstanceAbsTimerMapIter;
    typedef InstanceAbsTimerMap::value_type InstanceAbsTimerValue;    
	
	// search(dbNum) -> instanceMapID 검색 목록;
    typedef std::map<DBNUM, InstanceIDSet> MemberInstanceAgentMap;
    typedef MemberInstanceAgentMap::const_iterator MemberInstanceAgentMapCIter;
    typedef MemberInstanceAgentMap::iterator MemberInstanceAgentMapIter;	
    typedef MemberInstanceAgentMap::value_type MemberInstanceAgentValue;

    typedef std::map<DBNUM, SNATIVEID> MemberInstanceFieldMap;
    typedef MemberInstanceFieldMap::const_iterator MemberInstanceFieldMapCIter;
    typedef MemberInstanceFieldMap::iterator MemberInstanceFieldMapIter;
    typedef MemberInstanceFieldMap::value_type MemberInstanceFieldValue;

    // 인던 내 맵 생성으로 인한 맵 이동 대기자 목록;
    typedef std::map<Faction::GLActorID, InformationPosition> ReserveSetPositionMap;
    typedef ReserveSetPositionMap::const_iterator ReserveSetPositionMapCIter;
    typedef ReserveSetPositionMap::iterator ReserveSetPositionMapIter;
    typedef ReserveSetPositionMap::value_type ReserveSetPositionValue;

	// reload에 따른 필드 서버군 상태;
	typedef std::vector<ReloadState> ReloadStateVector;
	typedef ReloadStateVector::const_iterator ReloadStateVectorCIter;
	typedef ReloadStateVector::iterator ReloadStateVectorIter;

    typedef std::queue<InstanceMapID> InstanceMapIDQueue;	


	

};
