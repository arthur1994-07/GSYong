#ifndef CServerPortalOpenTrigger_H
#define CServerPortalOpenTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	서버 포탈 오픈 트리거
	스테이지가 클리어 되면 포탈을 열어 유저들을 이동 시킬 수 있다.
	이를 작동 하는 트리거, 서버에서만 제어 한다.
	
 */
class CServerPortalOpenTrigger
	: public CServerTrigger<CTrigger>
{
public:
		CServerPortalOpenTrigger();

		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_PORTALOPEN; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

        /**
        	트리거와 짝이 되는 인스턴스를 생성 한다.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

		/**
			바인딩 인터페이스 함수들

		 */
		void		        SetFromGateID(DWORD id) { m_fromGateID = id; }
		const DWORD		    GetFromGateID() const { return m_fromGateID; }

        void		        SetToMapID(WORD mid, WORD sid) { m_toMapID = SNATIVEID(mid, sid); }
        const SNATIVEID	    GetToMapID() const { return m_toMapID; }

        void		        SetToGateID(DWORD gateid) { m_toGateID = gateid; }
        const DWORD	        GetToGateID() const { return m_toGateID; }

private:
		DWORD		        m_fromGateID;   //@ 현재 맵에서 타켓 게이트 ID
        SNATIVEID           m_toMapID;      //@ 이동할 맵 ID
        DWORD               m_toGateID;     //@ 이동할 게이트 ID        
};

} // end namespace

#endif