#pragma once

#include "../../RanLogic/Msg/PVEMsg.h"
#include "../../RanLogicClient/TriggerSystem/ClientMapInfoMessage.h"

class GLGaeaClient;
struct NET_MSG_GENERIC;

typedef TriggerSystem::SClientTriggerMapInfo TriggerMapInfo;
typedef TriggerSystem::TriggerMapInfoList TriggerMapInfoList;
typedef TriggerSystem::TriggerMapInfoListIt TriggerMapInfoListIt;

/**
	PVE 관련 패킷 및 클라이언트 로직을 처리 한다.
    
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
			맵에 진입시 호출 된다.
			우리는 맵에 진입시 에이전트에 PVE 맵 테이블을 요청 한다.

		 */
		void			        ReqMapTable();

		/**
			PVE 인던 관련 테이블 정보를 전달 한다.
			
		 */
		void			        AckPVEMapTable(NET_MSG_GENERIC* pMsg);

		/**
			PVE 맵 진입 요청

		 */
		void			        ReqEntrance(const DWORD dwCharDbNum, const DWORD dwMapID, const DWORD dwGateID, const int nLevel);
        bool                    GetEntranceMapInfo(TriggerMapInfo*& info);

		/**
			PVE 맵 진입 요청 응답
            응답이 오면 확인 요청을 처리 한다.

		 */
        void                    AckPVEEntrance(NET_MSG_GENERIC* pMsg);

        /**
        	PVE 맵 진입 확인 요청

        	\param bOk true : 진입 확인 OK, false : 진입 확인 no
         */
        void                    ReqPVEEntranceConfirm(bool bOk);

        /**
        	PVE 재도전 결과 처리

        	\param bOk true : 재도전 OK, false : 재도전 no
         */
        void                    ReqPVERetry(bool bOk);

        /**
        	PVE 인던 탈출을 요청 한다.

         */
        void                    ReqPVEExit();

        /**
        	PVE 맵 진입 확인 요청 응답
            실패시에만 응답이 온다. 

         */
        void                    AckPVEEntranceConfirm(NET_MSG_GENERIC* pMsg);

	    // PVE 맵의 귀속 해제 요청
		void			        ReqClearOfBelonging(DWORD dwCharDbNum, DWORD dwMapID, int nLevel);
		void					AckPVEClearOfBelonging(NET_MSG_GENERIC* pMsg);

		// GM 일 경우 리로딩에 대한 결과를 받는다.
		void					AckPVEAckReload(NET_MSG_GENERIC* pMsg);

		// 재도전에 대한 결과를 받는다.
		void					AckPVERetry(NET_MSG_GENERIC* pMsg);

        // 유저 입장 상태를 처리 한다.
        void                    MsgPVEEntranceState(NET_MSG_GENERIC* pMsg);

        // 몬스터 클리어 상태를 처리 한다.
        void                    MsgPVEMonsterClearState(NET_MSG_GENERIC* pMsg);

        // 이동 클리어 상태를 처리 한다.
        void                    MsgPVEPositioningClearState(NET_MSG_GENERIC* pMsg);

        // 스테이지 상태를 처리 한다.
        void                    MsgPVEStageState(NET_MSG_GENERIC* pMsg);

        // 스테이지 결과(보상)를 처리 한다.
        void                    MsgPVEStageResult(NET_MSG_GENERIC* pMsg);

        // 포탈(게이트) On, Off 처리를 수행 한다.
        void                    MsgPVEPortalState(NET_MSG_GENERIC* pMsg);

        // 스테이지를 시작 한다.
        void                    MsgPVEStartStage(NET_MSG_GENERIC* pMsg);

        // 스테이지를 시작 메시지를 전달 한다.
        void                    MsgPVEMsgStartStage(NET_MSG_GENERIC* pMsg);
        
        // 스테이지 일반 메시지를 전달 한다.
        void                    MsgPVEMsg(NET_MSG_GENERIC* pMsg);

		// 인던이 소멸 되었음을 알린다.
		void					MsgPVERemoveIndun(NET_MSG_GENERIC* pMsg);

		// 인던에서 부활을 요청 한다.
		void					MsgPVERebirth(NET_MSG_GENERIC* pMsg);

        // 스테이지 결과를 출력 한다.
        void                    GetStageResult(GLMSG::StageResult& results, int& nStage, bool& bLast, int& nTimer);

        // 맵 입장 가능 유무를 계산 한다.
        void                    UpdatePossible(double dTimer = 0.01f);		

        void                    FrameMove(float fElapsedTime);
		void                    FrameMoveTable(_TIME_TYPE nElapsedTime);

		void					SendMessage(TriggerSystem::SMessage* msg);

		lua_tinker::table GetResult(void) const;

protected:
		void					ClearTable();

		/**
			현재 게이트로 포탈을 요청 한다.
            현재 게이트 처리가 프레임 기반이라 프레임 기반으로 처리 한다.

		 */
		void			        ReqPortal();

        /**
            현재 게이트ID 를 통해 게이트 클래스를 얻는다.
            중복 코드 방지용

         */
        class DxLandGate*       GetLandGate(DWORD dwGateID) const;

        /**
            외부 인터페이스 변수

         */
		GLGaeaClient*	        m_pGaeaClient;

        /**
            내부 데이터 저장 변수

         */
        MapTableContainer       m_tables;   //@ 서버에서 받은 전체 PVE 맵 정보		
        TriggerMapInfoList      m_kMapInfo; //@ 클라에서 가지는 트리거 맵 정보
		TriggerMapInfoListIt	m_kUpdateit;//@ 마지막으로 업데이트한 Iterator

		/**
			현재 소속된 인던 정보

		 */
		MAP_TABLE				m_table;	//@ 현재 입장 인던 정보
		bool					m_update;	//@ DB 업데이트 유무

        /**
            입장 요청한 맵 ID

         */
        SPVEInDunID             m_indun;    //@ 인던 구분 고유 ID

        /**
            스테이지 결과 정보

         */
        GLMSG::StageResult      m_results;
        int                     m_stage;
        bool                    m_last;
        int                     m_timer;

        /**
            포탈 요청을 하기 위해
            포탈에 들어갔을 경우 1번만 패킷을 날리기 위한 변수
            기존에 게이트를 FrameMove 기반으로 처리 되어 있어 필요

         */
        bool                    m_portal;

		/**
			현재 스테이지
         */
		int						m_nThisStage;
};
