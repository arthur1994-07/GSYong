#ifndef CServerPortalCloseTrigger_H
#define CServerPortalCloseTrigger_H

#include "../../RanLogic/TriggerSystem/PortalCloseTrigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	서버 포탈 닫기 트리거
	스테이지 시작시 포탈을 닫을 때 사용 한다.
	닫기와 오픈은 동일한 구조이지만 기능적으로 다르므로
    따로 분리 한다. 이렇게 함으로써 스크립트를 명확하게 할 수 있다.

    아래 자세히 보면 m_toMapID 와 m_toGateID 입력을 처리 하지 않는다.
    포탈을 닫는 방법으로 쓰지 않는(ID_NULL) 값을 줌으로써 작동되지 않게 하는 방법이다.

 */
class CServerPortalCloseTrigger
	: public CServerTrigger<CPortalCloseTrigger>
{
public:
		CServerPortalCloseTrigger();

		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_PORTALCLOSE; }

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
        const SNATIVEID	    GetToMapID() const { return m_toMapID; }
        const DWORD	        GetToGateID() const { return m_toGateID; }

protected:
        SNATIVEID           m_toMapID;      //@ 이동할 맵 ID
        DWORD               m_toGateID;     //@ 이동할 게이트 ID 
};

} // end namespace

#endif