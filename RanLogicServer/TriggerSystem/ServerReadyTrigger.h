#ifndef CServerReadyTrigger_H
#define CServerReadyTrigger_H

#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	서버 입장 대기 트리거
	설명은 CReadyTrigger 참조
	
 */
class CServerReadyTrigger	
    : public CServerTrigger<CTrigger>
{
public:
        CServerReadyTrigger() : m_time(0), m_count(0) {}

		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_READY; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

        /**
        	트리거와 짝이 되는 인스턴스를 생성 한다.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

		void		        SetCount(int count) { m_count = count; }
		const int			GetCount() const { return m_count; }

protected:
		int			        m_time;	//@ 대기 시간(sec)
		int			        m_count;//@ 대기 인원
};

} // end namespace

#endif