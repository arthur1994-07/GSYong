#pragma once

#include "../../RanLogic/Msg/PVEMsg.h"
#include "../../RanLogicClient/TriggerSystem/ClientMapInfoMessage.h"

class GLGaeaClient;
struct NET_MSG_GENERIC;

typedef TriggerSystem::SClientTriggerMapInfo TriggerMapInfo;
typedef TriggerSystem::TriggerMapInfoList TriggerMapInfoList;
typedef TriggerSystem::TriggerMapInfoListIt TriggerMapInfoListIt;

/**
	PVE ���� ��Ŷ �� Ŭ���̾�Ʈ ������ ó�� �Ѵ�.
    
 */
class GLPVEClient
{
public:
		GLPVEClient(GLGaeaClient* pGaeaClent);
virtual ~GLPVEClient();

        void                    Init();

		int						GetThisStage() { return m_nThisStage; }
		MapTableContainer&		GetMapTable() { return m_tables; }
		TriggerMapInfoList&		GetMapInfo() { return m_kMapInfo; }

        bool                    FindTable(const SNATIVEID mid, const int nLevel, MAP_TABLE*& table);
		bool                    FindTable(const DWORD dwChaDbNum, const SNATIVEID mid, const int nLevel, MAP_TABLE*& table);
        bool                    FindInfo(const DWORD dwMapID, TriggerMapInfo*& info);

		bool					IsUpdateDB() { return m_update; }
		void					ToggleUpdateDB() { m_update = !m_update; }

		/**
			�ʿ� ���Խ� ȣ�� �ȴ�.
			�츮�� �ʿ� ���Խ� ������Ʈ�� PVE �� ���̺��� ��û �Ѵ�.

		 */
		void			        ReqMapTable();

		/**
			PVE �δ� ���� ���̺� ������ ���� �Ѵ�.
			
		 */
		void			        AckPVEMapTable(NET_MSG_GENERIC* pMsg);

		/**
			PVE �� ���� ��û

		 */
		void			        ReqEntrance(const DWORD dwCharDbNum, const DWORD dwMapID, const DWORD dwGateID, const int nLevel);
        bool                    GetEntranceMapInfo(TriggerMapInfo*& info);

		/**
			PVE �� ���� ��û ����
            ������ ���� Ȯ�� ��û�� ó�� �Ѵ�.

		 */
        void                    AckPVEEntrance(NET_MSG_GENERIC* pMsg);

        /**
        	PVE �� ���� Ȯ�� ��û

        	\param bOk true : ���� Ȯ�� OK, false : ���� Ȯ�� no
         */
        void                    ReqPVEEntranceConfirm(bool bOk);

        /**
        	PVE �絵�� ��� ó��

        	\param bOk true : �絵�� OK, false : �絵�� no
         */
        void                    ReqPVERetry(bool bOk);

        /**
        	PVE �δ� Ż���� ��û �Ѵ�.

         */
        void                    ReqPVEExit();

        /**
        	PVE �� ���� Ȯ�� ��û ����
            ���нÿ��� ������ �´�. 

         */
        void                    AckPVEEntranceConfirm(NET_MSG_GENERIC* pMsg);

	    // PVE ���� �ͼ� ���� ��û
		void			        ReqClearOfBelonging(DWORD dwCharDbNum, DWORD dwMapID, int nLevel);
		void					AckPVEClearOfBelonging(NET_MSG_GENERIC* pMsg);

		// GM �� ��� ���ε��� ���� ����� �޴´�.
		void					AckPVEAckReload(NET_MSG_GENERIC* pMsg);

		// �絵���� ���� ����� �޴´�.
		void					AckPVERetry(NET_MSG_GENERIC* pMsg);

        // ���� ���� ���¸� ó�� �Ѵ�.
        void                    MsgPVEEntranceState(NET_MSG_GENERIC* pMsg);

        // ���� Ŭ���� ���¸� ó�� �Ѵ�.
        void                    MsgPVEMonsterClearState(NET_MSG_GENERIC* pMsg);

        // �̵� Ŭ���� ���¸� ó�� �Ѵ�.
        void                    MsgPVEPositioningClearState(NET_MSG_GENERIC* pMsg);

        // �������� ���¸� ó�� �Ѵ�.
        void                    MsgPVEStageState(NET_MSG_GENERIC* pMsg);

        // �������� ���(����)�� ó�� �Ѵ�.
        void                    MsgPVEStageResult(NET_MSG_GENERIC* pMsg);

        // ��Ż(����Ʈ) On, Off ó���� ���� �Ѵ�.
        void                    MsgPVEPortalState(NET_MSG_GENERIC* pMsg);

        // ���������� ���� �Ѵ�.
        void                    MsgPVEStartStage(NET_MSG_GENERIC* pMsg);

        // ���������� ���� �޽����� ���� �Ѵ�.
        void                    MsgPVEMsgStartStage(NET_MSG_GENERIC* pMsg);
        
        // �������� �Ϲ� �޽����� ���� �Ѵ�.
        void                    MsgPVEMsg(NET_MSG_GENERIC* pMsg);

		// �δ��� �Ҹ� �Ǿ����� �˸���.
		void					MsgPVERemoveIndun(NET_MSG_GENERIC* pMsg);

		// �δ����� ��Ȱ�� ��û �Ѵ�.
		void					MsgPVERebirth(NET_MSG_GENERIC* pMsg);

        // �������� ����� ��� �Ѵ�.
        void                    GetStageResult(GLMSG::StageResult& results, int& nStage, bool& bLast, int& nTimer);

        // �� ���� ���� ������ ��� �Ѵ�.
        void                    UpdatePossible(double dTimer = 0.01f);		

        void                    FrameMove(float fElapsedTime);
		void                    FrameMoveTable(_TIME_TYPE nElapsedTime);

		void					SendMessage(TriggerSystem::SMessage* msg);

		lua_tinker::table GetResult(void) const;

protected:
		void					ClearTable();

		/**
			���� ����Ʈ�� ��Ż�� ��û �Ѵ�.
            ���� ����Ʈ ó���� ������ ����̶� ������ ������� ó�� �Ѵ�.

		 */
		void			        ReqPortal();

        /**
            ���� ����ƮID �� ���� ����Ʈ Ŭ������ ��´�.
            �ߺ� �ڵ� ������

         */
        class DxLandGate*       GetLandGate(DWORD dwGateID) const;

        /**
            �ܺ� �������̽� ����

         */
		GLGaeaClient*	        m_pGaeaClient;

        /**
            ���� ������ ���� ����

         */
        MapTableContainer       m_tables;   //@ �������� ���� ��ü PVE �� ����		
        TriggerMapInfoList      m_kMapInfo; //@ Ŭ�󿡼� ������ Ʈ���� �� ����
		TriggerMapInfoListIt	m_kUpdateit;//@ ���������� ������Ʈ�� Iterator

		/**
			���� �Ҽӵ� �δ� ����

		 */
		MAP_TABLE				m_table;	//@ ���� ���� �δ� ����
		bool					m_update;	//@ DB ������Ʈ ����

        /**
            ���� ��û�� �� ID

         */
        SPVEInDunID             m_indun;    //@ �δ� ���� ���� ID

        /**
            �������� ��� ����

         */
        GLMSG::StageResult      m_results;
        int                     m_stage;
        bool                    m_last;
        int                     m_timer;

        /**
            ��Ż ��û�� �ϱ� ����
            ��Ż�� ���� ��� 1���� ��Ŷ�� ������ ���� ����
            ������ ����Ʈ�� FrameMove ������� ó�� �Ǿ� �־� �ʿ�

         */
        bool                    m_portal;

		/**
			���� ��������
         */
		int						m_nThisStage;
};
