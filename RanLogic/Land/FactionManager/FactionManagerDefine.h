#pragma once

#include <map>
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../Character/GLCharDefine.h"

#include "../../Actor/GLActorDefine.h"

namespace Faction
{
	enum EMRELATION_TYPE
	{ // 추후 UI에서 적 아군 중립 등등을 표현 해 줄 수 있기에 bool 형태로 하지 않음;

// #ifdef _SKILLTARGET_RENEWAL
// 		EMRELATION_DAMAGE           = 0x0001,  // 해당 진영에게 피해를 줄 수 있는가?;
//         EMRELATION_HEAL             = 0x0002,  // 해당 진영에게 회복을 시킬 수 있는가?;
//         EMRELATION_BUFF             = 0x0004,  // 해당 진영에게 이로운 효과를 줄 수 있는가?;
//         EMRELATION_DEBUFF           = 0x0008,  // 해당 진영에게 해로운 효과를 걸 수 있는가?;
// 
//         EMRELATION_FORCED_DAMAGE    = 0x0010,  // 해당 진영에게 강제로 피해를 줄 수 있는가?;
//         EMRELATION_FORCED_HEAL      = 0x0020,  // 해당 진영에게 강제로 회복을 시킬 수 있는가?;
//         EMRELATION_FORCED_BUFF      = 0x0040,  // 해당 진영에게 강제로 이로운 효과를 걸 수 있는가?;
//         EMRELATION_FORCED_DEBUFF    = 0x0080,  // 해당 진영에게 강제로 해로운 효과를 걸 수 있는가?;
// 
//         EMRELATION_INVISIBLE        = 0x0100,  // 해당 진영에게 안보여지는가?;
// 
//         EMRELATION_ENEMY = (EMRELATION_DAMAGE | EMRELATION_DEBUFF),  // 적 진영에게는 피해와 디버프만 가능;
// 		EMRELATION_ALLY = (EMRELATION_HEAL | EMRELATION_BUFF),  // 적 진영에게는 회복과 버프만 가능;
//         EMRELATION_NEUTRAL = (EMRELATION_FORCED_DAMAGE | EMRELATION_FORCED_HEAL | EMRELATION_FORCED_BUFF | EMRELATION_FORCED_DEBUFF), // 강제로 어떠한 행동이든 가능;
// #else
        EMRELATION_ENEMY,				// 적대관계			: 모든 면에서 적대적인 관계;
        EMRELATION_ALLY,				// 동맹관계			: 모든 면에서 우호적인 관계;
        EMRELATION_NEUTRAL_ENEMY,		// 중립적대관계	: 강제적으로는 영향을 끼칠 수 있는 적대적 중립관계;
		EMRELATION_NEUTRAL_ALLY,		// 중립동맹관계	: 아무런 영향도 끼칠 수 없는 우호적 중립관계;

        EMRELATION_ERROR,				// 잘못된관계;
		EMRELATION_NSIZE = EMRELATION_ERROR,
//#endif
	};

    enum EMFACTION_TYPE
    {
		EMFACTION_ERROR		= -1,
		EMFACTION_NONE		= 0,
        EMFACTION_PARTY		= 1,
        EMFACTION_CLUB		= 2,
        EMFACTION_COUNTRY	= 3,
		EMFACTION_SCHOOL	= 4,

		// 새로운 진영을 만들때는 이것으로 만든다;
		EMFACTION_TEAM		= 5,		// 보통 인던의 유저 진영으로 사용한다;
		EMFACTION_SAFE_TEAM	= 6,		// 보통 인던의 유저 안전진영으로 사용한다;
		EMFACTION_BASIC		= 7,		// 보통 인던의 일반진영으로 사용한다 ( NPC );
		EMFACTION_TEAM2		= 8,
		EMFACTION_TEAM3		= 9,
    };

    enum EMMESSAGE_TYPE
    {  // Faction 에 사용되는 패킷의 내부 메시지 타입;
        EMMESSAGE_REQ_FACTION_INFO_CF,
        EMMESSAGE_FACTION_INFO_FC,
        EMMESSAGE_FACTION_CHANGE_FCB,

		EMMESSAGE_REQ_FACTOIN_SET_FACTION, ///< Faction 설정.
        EMMESSAGE_FACTION_RELATION_INFO_FC,
        EMMESSAGE_FACTION_RELATION_CHANGE_FCB,

        EMMESSAGE_FACTION_HP_VISIBLE_INFO_FC, // HP 정보 보이기 설정;
    };
    
    enum
    {
        MAX_FACTION = 32,
    };

    enum EMRELATION_FLAG_TYPE
    {
        EM_RELATION_FLAG_NONE        = 0x0000,
        EM_RELATION_FLAG_ENEMY      = 0x0001,
        EM_RELATION_FLAG_NEUTRAL    = 0x0002,
        EM_RELATION_FLAG_ALLY       = 0x0004,
        EM_RELATION_FLAG_ALL        = EM_RELATION_FLAG_ENEMY + EM_RELATION_FLAG_NEUTRAL + EM_RELATION_FLAG_ALLY,

        EMRELATION_FLAG_ERROR       = 0xFFFF,
    };

	class FactionID
    {
    public:
        __forceinline const FactionID& operator =(const FactionID& _rhs);
        __forceinline const bool operator <(const FactionID& _rhs) const;
        __forceinline const bool operator ==(const FactionID& _rhs) const;
        __forceinline const bool operator !=(const FactionID& _rhs) const;

        FactionID(void);
        FactionID(const DWORD _factionType, const DWORD _factionID_Num);
        FactionID(const FactionID& _rhs);

        __forceinline void reset();

        EMFACTION_TYPE factionType;  // 진영 타입;
        DWORD factionID_Num;  // 진영 식별자 (ex. PartyID);
    };

    /*class GLActorID
	{
	public:
		__forceinline const GLActorID& operator =(const GLActorID& _rhs);
		__forceinline const bool operator <(const GLActorID& _rhs) const;
		__forceinline const bool operator ==(const GLActorID& _rhs) const;
        __forceinline const bool operator !=(const GLActorID& _rhs) const;

		GLActorID(void);
		GLActorID(const DWORD _actorType, const DWORD _actorID_Num);
		GLActorID(const GLActorID& _rhs);
	
		const EMCROW actorType;
		const DWORD actorID_Num; // 몹 : GaeaID, 플레이어 : dbNum;
	};*/

    /*
	typedef std::map<GLActorID, DWORD> ActorMap;  // 진영에 속한 객채의 맵 ( 객체 ID를 키로 해당 진영의 배열번호를 저장 );
	typedef ActorMap::const_iterator ActorMapCIter;
    typedef ActorMap::iterator ActorMapIter;
	typedef std::pair<GLActorID, DWORD> ActorPair;

    typedef std::map<FactionID, DWORD> FactionIDMap;  // 진영의 ID 맵 ( ID를 키로 해당 진영의 배열번호를 저장 );
    typedef FactionIDMap::const_iterator FactionIDMapCIter;
    typedef FactionIDMap::iterator FactionIDMapIter;
    typedef std::pair<FactionID, DWORD> FactionIDPair;
    */

    typedef std::vector<GLActorID> FactionMemberVector;
    typedef FactionMemberVector::const_iterator FactionMemberVectorCIter;
    typedef FactionMemberVector::iterator FactionMemberVectorIter;

    /*
    class FactionGroup
    {
    public:
        FactionID factionID;  // 진영 정보;
        FactionMemberVector factionMemberVector;  // 진영내 속한 객체 벡터;
    };

    typedef EMRELATION_TYPE FactionRelationTable[MAX_FACTION][MAX_FACTION];  // 진영간 관계 테이블;

    typedef FactionGroup FactionGroupArray[MAX_FACTION];  // 진영 정보 배열 ( 진영ID,진영에 속한 객체를 지님 );
	typedef std::queue<DWORD> FactionIDQueue;  // 진영 생성에 사용할 ID 큐;
    */

    // hashmap, map등 다른 자료구조들을 적용해봤으나. 일반적인 상황으로는 faction의 개수가 많아봐야 10개 이하일것으로 생각되서 무식하지만 vector가 가장 빠를것임;

    typedef std::pair<GLActorID, FactionID> ActorFactionPair;
    typedef std::vector<ActorFactionPair>	ActorVector;

    typedef std::pair<FactionID, DWORD>		RelationVectorPair;
    typedef std::vector<RelationVectorPair> RelationVector;
	typedef RelationVector::const_iterator	RelationVectorCIter;
	typedef RelationVector::iterator		RelationVectorIter;

}

#include "FactionManagerDefine.hpp"