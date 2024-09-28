#ifndef CServerPositioningClearTrigger_H
#define CServerPositioningClearTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	특정 위치 도달시 완료 트리거
	특정 위치에 인원 수 만큼 도달시 완료 된다.
    이를 위한 정보를 가진다.
	
 */
class CServerPositioningClearTrigger
	: public CServerTrigger<CTrigger>
{
public:
		CServerPositioningClearTrigger();

		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_POSITIONINGCLEAR; }

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
		void		        SetMin(float x, float y) { m_min = D3DXVECTOR2(x, y); }
		const D3DXVECTOR2	GetMin() const { return m_min; }

		void		        SetMax(float x, float y) { m_max = D3DXVECTOR2(x, y); }
		const D3DXVECTOR2	GetMax() const { return m_max; }

        void		        SetCount(int count) { m_count = count; }
        const int	        GetCount() const { return m_count; }

private:
		D3DXVECTOR2	        m_min;
		D3DXVECTOR2	        m_max;
        int                 m_count;
};

} // end namespace

#endif