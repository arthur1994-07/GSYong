#ifndef CServerTriggerInstance_H
#define CServerTriggerInstance_H

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"
#include "../../enginelib/G-Logic/GLDefine.h"
#include "../Land/GLLandMan.h"
#include "../FieldServer/GLGaeaServer.h"

class GLLandMan;
class GLGaeaServer;
struct NET_MSG_GENERIC;

namespace TriggerSystem
{

/**
	서버 트리거 인스턴스 처리기
    서버에서 사용하는 인스턴스는 모두 이 클래스를 상속 받아 구현 하도록 한다.
    서버에서사용하는 인스턴스의 공용 인터페이스 함수들을 관리 한다.
	
 */
class CServerTriggerInstance
    : public CTriggerInstance
{
public:
virtual ~CServerTriggerInstance() {}

        /**
            PC 관련 메시지 처리

         */
virtual void        EntryPC(SMessage* msg) {}
virtual void        ExitPC(SMessage* msg) {}
virtual void        DiePC(SMessage* msg) {}
virtual void        ReturnPC(SMessage* msg);
virtual void        ReturnAllPC(SMessage* msg) {}
virtual void		PartyOutPC(SMessage* msg) {}
virtual void		ActionPC(SMessage* msg) {}

        /**
            Crow 관련 메시지 처리

         */
virtual void        DieCrow(SMessage* msg) {}

        /**
            타격 관련 메시지 처리

         */
virtual void        Damage(SMessage* msg) {}
virtual void        Heal(SMessage* msg) {}

        /**
            미션 관련 메시지 처리

         */
virtual void        MissionCompleted(SMessage* msg) {}
virtual void        StageClear(SMessage* msg) {}
virtual void        StageExit(SMessage* msg) {}
virtual void        StageFail(SMessage* msg) {}

        /**
            이동 관련 메시지 처리

         */
virtual void        DoPortal(SMessage* msg) {}
virtual void        SavePos(SMessage* msg) {}

        /**
            종료 후 처리 관련 메시지 처리

         */
virtual void        RetryReq(SMessage* msg) {}
virtual void        RetryAns(SMessage* msg) {}
virtual void        RewardAdd(SMessage* msg) {}
virtual void        RewardReq(SMessage* msg) {}

        /**
            랜드맨을 추출 한다.
            자주 쓰는 함수이므로 공용 함수로 뺀다.

         */
        GLLandMan*  GetLandMan();
     
        // 시작로그는 에이전트에서 남긴다.
        //void WriteLogStart( const int& nPersonnel, const DWORD& dwChaDbNum, const int& nChrLevel );
        
        void WriteLogPlay( const int& nPersonnel, const DWORD& dwChaDbNum, const int& nChrLevel, const int& nStage );
        void WriteLogEnd( const int& nPersonnel, const DWORD& dwChaDbNum, const int& nChrLevel, const int& nStage, const bool& bMVP, const LONGLONG& llMoney, const DWORD& dwExp );

protected:
        /**
            패킷 전송 편의 함수

         */
        template<class T>
        void        SendMsgMySelf(const DWORD dwCharDbNum, const EMNET_MSG dwMsgType, const T& NetMsg);        
        template<class T>
        void        SendMsgClient(const DWORD dwCharDbNum, const EMNET_MSG dwMsgType, const T& NetMsg);
        template<class T>
        void        SendMsgAgent(const EMNET_MSG dwMsgType, const T& NetMsg);

        /**
            메시지를 전송 한다.

         */
        void		SendMessage(SMessage* msg);
};

#include "ServerTriggerInstance.inl"

} // end namespace

#endif