#ifndef CPVEInDunParty_H
#define CPVEInDunParty_H

#include <map>
#include "../../RanLogic/Msg/PVEMsg.h"
#include "../RanLogicServer/Party/GLPartyAgent.h"
#include "../TriggerSystem/ServerMapInfoMessage.h"
#include "../TriggerSystem/ServerRewardInstance.h"

typedef GLPartyAgent::MEMBER PartyMember;
typedef PartyMember::iterator PartyMemberIt;

typedef std::map<DWORD, MAP_TABLE> PVEDBContainer;
typedef PVEDBContainer::iterator PVEDBContainerIt;
typedef PVEDBContainer::const_iterator PVEDBContainerCIt;

typedef GLMSG::StageResult StageResult;

namespace PVESystem
{

class CPVEInDun;

/**
	PVE 귀속 인던의 입장 한
    파티 멤버들을 관리 한다.

 */
class CPVEInDunParty
{
public:
    CPVEInDunParty();

    /**
        멤버 정보들을 설정 한다.

     */
    bool            Init(GLPartyAgent* pPartyAgent, GLAgentServer* pServer, const SPVEInDunID& id);

	/**
		PVE 시스템에 파티원들을 등록 한다.
		이렇게 등록 된 인원은 인던에 입장한 인원이 된다.

	 */
	void			Register();

	/**
		PVE 시스템에 파티원들을 해제 한다.
		해제 된 인원만이 다시 인던에 입장할 수 있음에 주의 한다.

		\param bOwner 인던 오너 등록 해제 유무
	 */
	void			UnRegister(bool bOwner);	

    /**
        내부 자료를 초기화 한다.

     */
    void            Reset();

    /**
        멤버들의 DB 정보를 설정, 추출 한다.
        이것은 자주하면 부하가 걸리므로
        모든 일반적인 검사가 끝난 후 최종적으로 호출 해야 한다.

     */    
    void            SetDB(MAP_TABLE& tbl);
    bool            GetDB(const DWORD dwCharDbNum, MAP_TABLE& tbl);
	void			ProcessDB(const int nOwnTime, float& fLifeTime);

    /**
        파티 고유 ID 를 출력 한다.
        이를 통해 인던을 생성 한다.

     */
    const GLPartyID GetPartyID() const { return m_partyID; }

	/**
		현재 파티 멤버수를 리턴 한다.

	 */
	const DWORD 	GetCount() const;

    /**
        모든 멤버들이 입장 카운트를 만족하는지 체크 한다.        

     */
    bool            IsCount(int nCount, int nOwntime);

	/**
		소속 인던이 있는 멤버가 있는지 체크 한다.

	 */
	bool			IsIndun() const;

	/**
		멤버들이 CTF 에 신청중이거나 참여중인지 확인 한다.

	 */
	bool			IsCTF() const;
	bool			IsCTF(DWORD dwCharDbNum) const;

    /**
		멤버들이 매칭시스템 컨텐츠 에 신청중이거나 참여중인지 확인 한다.

	 */
	bool			IsJoinMatchingContents() const;
	bool			IsJoinMatchingContents(DWORD dwCharDbNum) const;

	/**
		모든 멤버들이 레벨 범위내에 포함되는지 살핀다.

	 */
	bool			IsLevel(const WORD wMin, const WORD wMax) const;

	/**
		현재 파티의 파티장인지 체크 한다.

	 */
	bool			IsMaster(const DWORD dwCharDbNum) const;

	/**
		신청시 멤버와 현재 멤버가 동일한지 확인 한다.
		신청 후 입장 확인 사이에 멤버가 변경 될 수 있다.

	 */
	bool			ChkMember() const;

	/**
		멤버를 삭제 한다.

	 */
	void			RemoveMember(DWORD dwCharDbNum);

    /**
    	입장 확인 파티 멤버를 처리 한다.
        모든 파티원이 확인을 해야지만 입던이 된다.

     */
	bool			IsPassMember(DWORD member);
    bool            PassMember(DWORD member);

    /**
    	파티원들을 입력되는 맵의 게이트로 모두 이동 시킨다.

     */
    void            MoveParty(CPVEInDun* pIndun, DWORD dwMapID, DWORD dwGateID);
	void            Move(CPVEInDun* pIndun, DWORD dwMapID, DWORD dwGateID, DWORD dwCharDbNum);

    /**
        모든 멤버들을 인던 전 위치로 되돌린다.
        이것은 인던 귀속시간이 끝나거나 게임 포기를 할 경우 사용 된다.

        nWhy 리턴대는 이유
     */
    void            RestoreAll(int nWhy);    

    /**
        모든 파티원의 현재 위치를 저장 한다.
        이것은 인던에 입장전에 수행 된다.

     */
    void            SavePositionAll();
	void            SavePosition(DWORD dwCharDbNum);

    /**
        스테이지 결과를 저장 한다.
        매 스테이지를 클리어 하면 전송 되어 지며
        PVE 가 종료되기 전까지 누적되어 최종 보상을 처리 한다.

     */
    void            SetStageResult(const StageResult& results);
	
    /**
        파티원에 메시지를 전송을 위한 편의 함수들을 제공 한다.

     */
    template<class T>
    void            SendParty(const T& NetMsg, const EMNET_MSG& eType)
    {
        if (m_server)
        {
            msgpack::sbuffer buf;
            msgpack::pack(buf, NetMsg);
            m_server->SENDTOPARTY(m_partyID, eType, buf);
        }
    }

	void            SendParty(NET_MSG_GENERIC* nmg);

	template<class T>
	void            SendPartyMember(DWORD dwCharDbNum, const T& NetMsg, const EMNET_MSG& eType)
	{
		if (m_server)
		{
			typename GLCharAG* pChar = m_server->GetCharByDbNum(dwCharDbNum);

			if (pChar)
			{
				msgpack::sbuffer buf;
				msgpack::pack(buf, NetMsg);
				m_server->SENDTOCLIENT(pChar->ClientSlot(), eType, buf);
			}
		}
	}

	template<class T>
	void            SendFieldPartyMember(DWORD dwCharDbNum, const T& NetMsg, const EMNET_MSG& eType)
	{
		if (m_server)
		{
			typename GLCharAG* pChar = m_server->GetCharByDbNum(dwCharDbNum);

			if (pChar)
			{
				msgpack::sbuffer buf;
				msgpack::pack(buf, NetMsg);
				m_server->SENDTOFIELD(pChar->GetChannel(), pChar->GetCurrentFieldServer(), eType, buf);
			}
		}
	}

protected:
	/**
		파티원들을 추출 한다.

	 */
	PartyMember		CollectMember();

	/**
		파티원을 귀환 시킨다.

	 */
	void            Restore(int nWhy, DWORD dwCharDbNum);

    GLAgentServer*  m_server;   //@ 연결된 에이전트 서버

    GLPartyID       m_partyID;  //@ 파티 고유 ID
    SPVEInDunID     m_indunID;  //@ 인던 고유 ID

    PartyMember     m_member;   //@ 신청 한 멤버들
    PartyMember     m_pass;     //@ 신청 확인 된 멤버들
	PartyMember     m_register; //@ PVE 인던에 등록된 멤버

    StageResult     m_results;   //@ 멤버들 보상 정보
};

} // end namespace

#endif