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
	���� Ʈ���� �ν��Ͻ� ó����
    �������� ����ϴ� �ν��Ͻ��� ��� �� Ŭ������ ��� �޾� ���� �ϵ��� �Ѵ�.
    ������������ϴ� �ν��Ͻ��� ���� �������̽� �Լ����� ���� �Ѵ�.
	
 */
class CServerTriggerInstance
    : public CTriggerInstance
{
public:
virtual ~CServerTriggerInstance() {}

        /**
            PC ���� �޽��� ó��

         */
virtual void        EntryPC(SMessage* msg) {}
virtual void        ExitPC(SMessage* msg) {}
virtual void        DiePC(SMessage* msg) {}
virtual void        ReturnPC(SMessage* msg);
virtual void        ReturnAllPC(SMessage* msg) {}
virtual void		PartyOutPC(SMessage* msg) {}
virtual void		ActionPC(SMessage* msg) {}

        /**
            Crow ���� �޽��� ó��

         */
virtual void        DieCrow(SMessage* msg) {}

        /**
            Ÿ�� ���� �޽��� ó��

         */
virtual void        Damage(SMessage* msg) {}
virtual void        Heal(SMessage* msg) {}

        /**
            �̼� ���� �޽��� ó��

         */
virtual void        MissionCompleted(SMessage* msg) {}
virtual void        StageClear(SMessage* msg) {}
virtual void        StageExit(SMessage* msg) {}
virtual void        StageFail(SMessage* msg) {}

        /**
            �̵� ���� �޽��� ó��

         */
virtual void        DoPortal(SMessage* msg) {}
virtual void        SavePos(SMessage* msg) {}

        /**
            ���� �� ó�� ���� �޽��� ó��

         */
virtual void        RetryReq(SMessage* msg) {}
virtual void        RetryAns(SMessage* msg) {}
virtual void        RewardAdd(SMessage* msg) {}
virtual void        RewardReq(SMessage* msg) {}

        /**
            ������� ���� �Ѵ�.
            ���� ���� �Լ��̹Ƿ� ���� �Լ��� ����.

         */
        GLLandMan*  GetLandMan();
     
        // ���۷α״� ������Ʈ���� �����.
        //void WriteLogStart( const int& nPersonnel, const DWORD& dwChaDbNum, const int& nChrLevel );
        
        void WriteLogPlay( const int& nPersonnel, const DWORD& dwChaDbNum, const int& nChrLevel, const int& nStage );
        void WriteLogEnd( const int& nPersonnel, const DWORD& dwChaDbNum, const int& nChrLevel, const int& nStage, const bool& bMVP, const LONGLONG& llMoney, const DWORD& dwExp );

protected:
        /**
            ��Ŷ ���� ���� �Լ�

         */
        template<class T>
        void        SendMsgMySelf(const DWORD dwCharDbNum, const EMNET_MSG dwMsgType, const T& NetMsg);        
        template<class T>
        void        SendMsgClient(const DWORD dwCharDbNum, const EMNET_MSG dwMsgType, const T& NetMsg);
        template<class T>
        void        SendMsgAgent(const EMNET_MSG dwMsgType, const T& NetMsg);

        /**
            �޽����� ���� �Ѵ�.

         */
        void		SendMessage(SMessage* msg);
};

#include "ServerTriggerInstance.inl"

} // end namespace

#endif