#ifndef CPVEInDun_H
#define CPVEInDun_H

#include <map>
#include "PVEInDunParty.h"
#include "PVETimer.h"

#include "../../SigmaCore/String/MinGuid.h"

typedef TriggerSystem::SServerTriggerMapInfo MAP_INFO;

namespace PVESystem
{

/**
	PVE 귀속 인던을 관리 한다.
	
	PVE 인던 이전에 인던 시스템이 존재 하였다.
	이 인던 시스템을 활용해 PVE 인던이 만들었으며
	PVE 인던은 실제 맵과는 다른 가상의 관리자 임에 주의해야 한다.

	즉 PVE 인던이 3번 스테이지라 하더라도 3번 스테이지의 맵(인던)을 직접적으로 관리하지는 않는다.
	직접적인 관리는 필드 서버의 인던 시스템에 의해 관리 되어 진다.

 */
class CPVEInDun
{
public:
    CPVEInDun();
	~CPVEInDun();

    enum EIndunState
    {
        INDUNSTATE_NONE,        //@ 아무런 상태 아님, 초기값
        INDUNSTATE_CONFIRM,     //@ 인던 입장 확인 상태, 모든 파티원이 OK 해야 인던 생성 처리
        INDUNSTATE_CREATING,    //@ 인던 생성 중, 필드를 통해야 하므로 시간이 소요됨
        INDUNSTATE_CREATED,     //@ 인던 생성 완료 상태, 스테이지 진행 중 상태
    };

    /**
        인던 고유 구분자를 설정한다.
        따른 무엇보다도 최우선 설정 되어야 한다.

     */
    void                SetID(const SPVEInDunID& id) { m_id = id; }
    const SPVEInDunID&  GetID() const { return m_id; }

    /**
    	에이전트 서버를 설정 한다.
        서버를 통해 패킷을 보낸다.

     */
    void                SetServer(GLAgentServer* sever) { m_server = sever; }

    /**
        파티마스터에 기반한 DB 정보와 맵 설정 정보를 입력 한다.
        이를 바탕으로 입장 확인 등의 로직을 처리 한다.
        값에 의한 복사임에 주의 한다.

     */
    void                SetInfo(const MAP_INFO& mapinfo) { m_mapinfo = mapinfo; }
	const MAP_INFO&		GetInfo() const { return m_mapinfo; }

    /**
        입장 관련 인스턴스 맵 정보를 출력 한다.		

     */
    const DWORD         GetEntranceMapID() const { return m_indunid; }	
    const DWORD         GetEntranceGateID() const { return m_gateid; }
	const DWORD         GetPreEntranceMapID() const { return m_preid; }

	/**
		현재 스테이지 번호를 가져온다.

	 */
	int					GetStage() { return m_stage; };

    /**
    	개인 or 모든 파티원들의 입장 처리를 수행 한다.
		우리는 입장 전 입장 가능 한지를 체크 하고 가능할 경우 입장 시킨다.

     */
    bool                Entrance(GLPartyAgent* pParty);
	bool                Entrance(DWORD dwCharDbNum);

	/**
		인던 상태를 출력 한다.

		IsIdle : 인던 초기 상태
		IsConfirming : 인던 입장 확인 상태
		IsCreating : 인던 생성 중
		IsRunning : 인던 진행 중
	 */	
	bool				IsIdle() const { return IsState(INDUNSTATE_NONE); }
	bool				IsConfirming() const { return IsState(INDUNSTATE_CONFIRM); }
	bool				IsCreating() const { return IsState(INDUNSTATE_CREATING); }
	bool                IsRunning() const { return IsState(INDUNSTATE_CREATED); }

    /**
    	입장 확인 한 파티 멤버를 처리 한다.
        모든 파티원이 확인을 해야지만 입던이 된다.

     */
    void                PassMember(DWORD member);

    void                Impossible( DWORD member);
    /**
    	파티원 중에 입장을 취소하면 발생 한다.
        만약 파티원이 무응답이라면 응답 대기 시간이 있으므로 
        이를 통해 처리 한다.
        
     */
    void                Cancel(DWORD member);

    /**
        귀속 해제를 처리 한다.
        인던은 처음 생성할떄 파티마스터에 종속되고
        마스터가 귀속 해제 요청을 하면 완전 인던은 사라지고
		파티원이 귀속 해제를 요청하면 해당 인던에 종속이 사라 진다.

     */
    bool                ClearOfBelonging(DWORD member);

    /**
        재도전을 처리 한다.
        재도전은 모두 죽거나 시간 오버시 할 수 있다.
        모든 파티원들이 재도전에 OK 해야 재도전 된다.

     */
    void                Retry();

    /**
    	모든 파티원들이 응답에 성공했을 때 인던을 생성 한다.
        인던은 에이전트와 필드에 각각 있는데 일딴 에이전트에 먼저 생성한 후
        필드에 생성을 요청한 후 대기 한다.

     */
    bool                CreateInstanceMap(DWORD dwBaseMapID, DWORD dwGateID, int nStage);
    void				DeleteInstanceMap(DWORD dwGaeaMapID);

    /**        
        이 함수는 필드에서 정상적으로 던전이 생성 되었다는 메시지에 의해 호출 된다.
        우리는 멤버들을 인던에 실제 입장 시킨다.

     */
    bool                PostCreateInstanceMap(DWORD dwGaeaMapID, bool bResult);

    /**
    	파티원들을 입력되는 맵의 게이트로 모두 이동 시킨다.

     */
    void                MoveParty(DWORD dwMapID, DWORD dwGateID, int nStage);

    /**        
        PostCreateInstanceMap 에 의해 필드에 던전 입장을 요청한 결과에 의해 호출 된다.
        우리는 입장이 완료되면 그에 따른 입장 카운트 등의 입장 최종 처리를 수행 한다.

     */
    void                PostEntranceInstanceMap(DWORD dwCharDbNum, bool bResult);

    /**
    	시간을 진행 시킨다.
        우리는 입장 확인 시간이 정해져 있으므로
        이 시간내에 모두 이뤄져야 한다.

    	\param fElpaseTime 초단위 진행 시간
     */
    void                FrameMove(float fElpaseTime);

    /**
        스테이지 결과를 저장 한다.
        매 스테이지를 클리어 하면 전송 되어 지며
        PVE 가 종료되기 전까지 누적되어 최종 보상을 처리 한다.

     */
    void                SetStageResult(const StageResult& results);

    /**
        멤버를 귀환 시킨다.
        우리는 던전 완료 후 나가기를 클릭하면 사용 한다.
        즉시 귀환 시키지 않고 필드에 귀환을 알린 후 결과를 받아 귀환(Return) 시킨다.

     */    
    void                RestoreAll(int nWhy);

    
    // 파티 아이디를 반환한다.
    const GLPartyID     GetPartyID() const { return m_party.GetPartyID(); }

protected:
    /**
    	인던 상태를 초기화 한다.
		내부적으로만 사용하도록 한다.

    	\param bSucceed 정상적인 게임 종료에 의한 종료인지 유무
     */
    void                Init(bool bSucceed = false);

    /**
    	파티 상태가 파괴 되면 호출 된다.
		우리는 파티가 파괴될때(DestroyParty) 인던을 종료 시킨다.

     */    
    void                DestroyParty(GLPartyAgent* pPartyAgent);

    /**
        파티 콜백을 설정, 해제 한다.		

     */
    void                SetPartyCallback();
    void                RemovePartyCallback();

    /**
    	입장 가능성 유무를 체크 한다.

     */    
    bool                IsEntrance();
	bool				IsEntrance(DWORD dwCharDbNum);

    /**
        상태를 처리 한다.
        우리는 이 상태를 기반으로 현재 인던의 상태를 알 수 있다.        

     */
    void                SetState(const EIndunState state);
	bool                IsState(const EIndunState state) const { return m_state == state; }
    void                ChangeState(const EIndunState from, const EIndunState to);

    /**
    	입장 대기 시간이 완료되었을 경우 호출 된다.
        유저가 응답이 없을 때 발생 할 수 있다.
        우리는 입장을 취소 시킨다.

     */
    void                Timerover();

	/**
		인던 생성이 실패했을 경우 호출 된다.
		어떤 경우에 의해 인던 생성이 실패했을 때를 처리 한다.

	 */
	void				FailedCreate();

    /**
        인던의 생명이 완료되었을 때 호출 된다.
        완료에 따른 여러 처리를 수행 한다.

     */
    void                Terminate();

	/**
		던전 입장 후 DB 관련 처리를 수행 한다.
		우리는 입장원들의 입장 카운트 및 기타 DB 관련 처리를 수행 한다.

	 */
	void				ProcessDB();

	/**
		생성 시간을 초기화 한다.
		인던을 실패하면 귀속 시간동안 제도전 가능하지만
		인던을 성공하면 귀속이 소멸 된다. 귀속은 생성 시간으로 부터 발생 되므로
		생성 시간을 소멸 시킴으로써 귀속이 사라 지게 된다.

	 */
	bool				ClearGenTime();

	/**
		입장에 대한 응답을 클라이언트에 전달 한다.	
		중복 코드를 제거 한다.

	 */
	void				EntranceAck(GLMSG::ENTRANCE_ACK eAck, DWORD dwCharDbNum = 0);

	/**
		입장 확인에 대한 응답을 클라이언트에 전달 한다.
		중복 코드를 제거 한다.

	 */
	void				EntranceConfirmAck(GLMSG::ENTRANCE_CONFIRM_ACK eAck, DWORD dwChaNum=0);

	/**
		인던의 소속 시간이 지나면 인던을 클리어 한다.

	 */
	void				AutoClearOfBelonging();

    /**
        외부 인터페이스

     */
    GLAgentServer*      m_server;       //@ 연결된 에이전트 서버

    /**
        인던 정보

     */
    SPVEInDunID         m_id;           //@ PVE 인던을 구분하는 고유 ID
    MAP_INFO            m_mapinfo;      //@ PVE 인던에 대한 기획 설정 데이터
	DWORD				m_dwChannelNum;	//@ PVE 인던이 생성된 채널 정보

    /**
        입장 한 맵 정보

     */
	DWORD				m_preid;		//@ 이전 입장 한 실제(Base가 아님) 인던 맵 ID
    DWORD               m_indunid;      //@ 현재 입장 한 실제(Base가 아님) 인던 맵 ID
    DWORD               m_gateid;       //@ 현재 입장 한 인던의 게이트 ID
	DWORD				m_RealIndunid;	//@ 현재 입장 한 인던 맵 ID (Lev 파일에 적혀있는 ID)

    /**
        파티 정보

     */
    CPVEInDunParty      m_party;        //@ 인던 파티 관리자

    /**
        인던 상태

     */
    EIndunState         m_state;        //@ 현재 인던 상태
    int                 m_stage;        //@ 현재 인던의 스테이지 단계
    float               m_lifetime;     //@ 인던 귀속 시간

    /**
        기타

     */    
    CCbTimer            m_timers;       //@ 타이머 관리자

public:
    const sc::MGUID&    GetGUID() { return m_Guid; }

private:
    sc::MGUID           m_Guid;         //@ 로그를 남기기위한 인던 고유ID
};

typedef std::map<SPVEInDunID, CPVEInDun> PVEInDunContainer;
typedef PVEInDunContainer::iterator PVEInDunContainerIt;
typedef PVEInDunContainer::const_iterator PVEInDunContainerCIt;

} // end namespace

#endif