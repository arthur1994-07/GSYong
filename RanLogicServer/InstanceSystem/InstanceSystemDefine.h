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
		EMMEMBER_NOT_INITIALIZE				= 0x00000001L, // �� ��� �Ǿ��� ���� ����;
		EMMEMBER_REQUEST_JOIN_FIELD		= 0x00000002L, // �ʵ忡 ���� �������� ��û ���� ����;
		EMMEMBER_CONFIRM_JOIN_FILED		= 0x00000004L, // ������ �㰡 ���� ����;
		EMMEMBER_IN_GUARD						= 0x00000008L, // �̵� ó�� ��;
		EMMEMBER_SET_TO_REMOVE				= 0x00000010L, // ���� ��� ��;
	};

	enum EMINSTANCE_STATE
	{
		EMINSTANCE_NULL							= 0x00000000L,
		EMINSTANCE_JOIN_LOCK					= 0x00000001L, // ���� �Ұ�;
		EMINSTANCE_JOIN_SUSPEND				= 0x00000002L, // ���� ����;
		EMINSTANCE_CREATE_COMPLETE		= 0x00000004L, // ���� �Ϸ�;
		EMINSTANCE_RESERVE_DESTROY		= 0x00000008L, // ���� ����;
		EMINSTANCE_ALLOW_WATCHING		= 0x00000010L, // ���� ���;
	};
	

	class MemberField;
	class MemberAgent;
	class InstanceField;
	class InstanceAgent;
    class InstanceBaseAgent;
	class ReloadState;
	class InformationPosition;

	// �δ��� ���� �÷��̾� ���;
	typedef std::map<DBNUM, MemberField> MemberFieldMap;
	typedef MemberFieldMap::const_iterator MemberFieldMapCIter;
	typedef MemberFieldMap::iterator MemberFieldMapIter;
    typedef MemberFieldMap::value_type MemberFieldValue;

	typedef std::queue<InformationPosition> InformationPositionQueue;

	typedef std::map<DBNUM, MemberAgent> MemberAgentMap;
	typedef MemberAgentMap::const_iterator MemberAgentMapCIter;
	typedef MemberAgentMap::iterator MemberAgentMapIter;
	typedef MemberAgentMap::value_type MemberAgentValue;


	// �δ� ���;
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



    // Ÿ�̸� ��� (��);
    typedef std::map<TimerHandle, InstanceTimer> InstanceTimerMap;
    typedef InstanceTimerMap::const_iterator InstanceTimerMapCIter;
    typedef InstanceTimerMap::iterator InstanceTimerMapIter;
    typedef InstanceTimerMap::value_type InstanceTimerValue;

	// Ÿ�̸� �ڵ�;
	typedef std::queue<TimerHandle> TimerHandleQueue;

    // Ư�� �ð� Ÿ�̸� ��� (��);
    typedef std::map<TimerHandle, InstanceAbsTimer> InstanceAbsTimerMap;
    typedef InstanceAbsTimerMap::const_iterator InstanceAbsTimerMapCIter;
    typedef InstanceAbsTimerMap::iterator InstanceAbsTimerMapIter;
    typedef InstanceAbsTimerMap::value_type InstanceAbsTimerValue;    
	
	// search(dbNum) -> instanceMapID �˻� ���;
    typedef std::map<DBNUM, InstanceIDSet> MemberInstanceAgentMap;
    typedef MemberInstanceAgentMap::const_iterator MemberInstanceAgentMapCIter;
    typedef MemberInstanceAgentMap::iterator MemberInstanceAgentMapIter;	
    typedef MemberInstanceAgentMap::value_type MemberInstanceAgentValue;

    typedef std::map<DBNUM, SNATIVEID> MemberInstanceFieldMap;
    typedef MemberInstanceFieldMap::const_iterator MemberInstanceFieldMapCIter;
    typedef MemberInstanceFieldMap::iterator MemberInstanceFieldMapIter;
    typedef MemberInstanceFieldMap::value_type MemberInstanceFieldValue;

    // �δ� �� �� �������� ���� �� �̵� ����� ���;
    typedef std::map<Faction::GLActorID, InformationPosition> ReserveSetPositionMap;
    typedef ReserveSetPositionMap::const_iterator ReserveSetPositionMapCIter;
    typedef ReserveSetPositionMap::iterator ReserveSetPositionMapIter;
    typedef ReserveSetPositionMap::value_type ReserveSetPositionValue;

	// reload�� ���� �ʵ� ������ ����;
	typedef std::vector<ReloadState> ReloadStateVector;
	typedef ReloadStateVector::const_iterator ReloadStateVectorCIter;
	typedef ReloadStateVector::iterator ReloadStateVectorIter;

    typedef std::queue<InstanceMapID> InstanceMapIDQueue;	


	

};
