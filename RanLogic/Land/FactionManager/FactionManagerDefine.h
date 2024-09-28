#pragma once

#include <map>
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../Character/GLCharDefine.h"

#include "../../Actor/GLActorDefine.h"

namespace Faction
{
	enum EMRELATION_TYPE
	{ // ���� UI���� �� �Ʊ� �߸� ����� ǥ�� �� �� �� �ֱ⿡ bool ���·� ���� ����;

// #ifdef _SKILLTARGET_RENEWAL
// 		EMRELATION_DAMAGE           = 0x0001,  // �ش� �������� ���ظ� �� �� �ִ°�?;
//         EMRELATION_HEAL             = 0x0002,  // �ش� �������� ȸ���� ��ų �� �ִ°�?;
//         EMRELATION_BUFF             = 0x0004,  // �ش� �������� �̷ο� ȿ���� �� �� �ִ°�?;
//         EMRELATION_DEBUFF           = 0x0008,  // �ش� �������� �طο� ȿ���� �� �� �ִ°�?;
// 
//         EMRELATION_FORCED_DAMAGE    = 0x0010,  // �ش� �������� ������ ���ظ� �� �� �ִ°�?;
//         EMRELATION_FORCED_HEAL      = 0x0020,  // �ش� �������� ������ ȸ���� ��ų �� �ִ°�?;
//         EMRELATION_FORCED_BUFF      = 0x0040,  // �ش� �������� ������ �̷ο� ȿ���� �� �� �ִ°�?;
//         EMRELATION_FORCED_DEBUFF    = 0x0080,  // �ش� �������� ������ �طο� ȿ���� �� �� �ִ°�?;
// 
//         EMRELATION_INVISIBLE        = 0x0100,  // �ش� �������� �Ⱥ������°�?;
// 
//         EMRELATION_ENEMY = (EMRELATION_DAMAGE | EMRELATION_DEBUFF),  // �� �������Դ� ���ؿ� ������� ����;
// 		EMRELATION_ALLY = (EMRELATION_HEAL | EMRELATION_BUFF),  // �� �������Դ� ȸ���� ������ ����;
//         EMRELATION_NEUTRAL = (EMRELATION_FORCED_DAMAGE | EMRELATION_FORCED_HEAL | EMRELATION_FORCED_BUFF | EMRELATION_FORCED_DEBUFF), // ������ ��� �ൿ�̵� ����;
// #else
        EMRELATION_ENEMY,				// �������			: ��� �鿡�� �������� ����;
        EMRELATION_ALLY,				// ���Ͱ���			: ��� �鿡�� ��ȣ���� ����;
        EMRELATION_NEUTRAL_ENEMY,		// �߸��������	: ���������δ� ������ ��ĥ �� �ִ� ������ �߸�����;
		EMRELATION_NEUTRAL_ALLY,		// �߸����Ͱ���	: �ƹ��� ���⵵ ��ĥ �� ���� ��ȣ�� �߸�����;

        EMRELATION_ERROR,				// �߸��Ȱ���;
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

		// ���ο� ������ ���鶧�� �̰����� �����;
		EMFACTION_TEAM		= 5,		// ���� �δ��� ���� �������� ����Ѵ�;
		EMFACTION_SAFE_TEAM	= 6,		// ���� �δ��� ���� ������������ ����Ѵ�;
		EMFACTION_BASIC		= 7,		// ���� �δ��� �Ϲ��������� ����Ѵ� ( NPC );
		EMFACTION_TEAM2		= 8,
		EMFACTION_TEAM3		= 9,
    };

    enum EMMESSAGE_TYPE
    {  // Faction �� ���Ǵ� ��Ŷ�� ���� �޽��� Ÿ��;
        EMMESSAGE_REQ_FACTION_INFO_CF,
        EMMESSAGE_FACTION_INFO_FC,
        EMMESSAGE_FACTION_CHANGE_FCB,

		EMMESSAGE_REQ_FACTOIN_SET_FACTION, ///< Faction ����.
        EMMESSAGE_FACTION_RELATION_INFO_FC,
        EMMESSAGE_FACTION_RELATION_CHANGE_FCB,

        EMMESSAGE_FACTION_HP_VISIBLE_INFO_FC, // HP ���� ���̱� ����;
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

        EMFACTION_TYPE factionType;  // ���� Ÿ��;
        DWORD factionID_Num;  // ���� �ĺ��� (ex. PartyID);
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
		const DWORD actorID_Num; // �� : GaeaID, �÷��̾� : dbNum;
	};*/

    /*
	typedef std::map<GLActorID, DWORD> ActorMap;  // ������ ���� ��ä�� �� ( ��ü ID�� Ű�� �ش� ������ �迭��ȣ�� ���� );
	typedef ActorMap::const_iterator ActorMapCIter;
    typedef ActorMap::iterator ActorMapIter;
	typedef std::pair<GLActorID, DWORD> ActorPair;

    typedef std::map<FactionID, DWORD> FactionIDMap;  // ������ ID �� ( ID�� Ű�� �ش� ������ �迭��ȣ�� ���� );
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
        FactionID factionID;  // ���� ����;
        FactionMemberVector factionMemberVector;  // ������ ���� ��ü ����;
    };

    typedef EMRELATION_TYPE FactionRelationTable[MAX_FACTION][MAX_FACTION];  // ������ ���� ���̺�;

    typedef FactionGroup FactionGroupArray[MAX_FACTION];  // ���� ���� �迭 ( ����ID,������ ���� ��ü�� ���� );
	typedef std::queue<DWORD> FactionIDQueue;  // ���� ������ ����� ID ť;
    */

    // hashmap, map�� �ٸ� �ڷᱸ������ �����غ�����. �Ϲ����� ��Ȳ���δ� faction�� ������ ���ƺ��� 10�� �����ϰ����� �����Ǽ� ���������� vector�� ���� ��������;

    typedef std::pair<GLActorID, FactionID> ActorFactionPair;
    typedef std::vector<ActorFactionPair>	ActorVector;

    typedef std::pair<FactionID, DWORD>		RelationVectorPair;
    typedef std::vector<RelationVectorPair> RelationVector;
	typedef RelationVector::const_iterator	RelationVectorCIter;
	typedef RelationVector::iterator		RelationVectorIter;

}

#include "FactionManagerDefine.hpp"