#ifndef CServerLayerTrigger_H
#define CServerLayerTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	서버 맵 레이어 트리거
	맵에 멀티 레이어로 몬스터를 설정 해 놓앗다면
	이 트리거를 통해 레이어를 변경함으로써 몬스터들을 새롭게 설정 할 수 있다.
	이는 서버에서만 제어 해야 한다.
	
 */
class CServerLayerTrigger
	: public CServerTrigger<CTrigger>
{
public:
		CServerLayerTrigger();

		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	GetType() const { return TRIGGER_TYPE_LAYER; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		Bind(CTriggerScript* pScript);

		/**
			바인딩 인터페이스 함수들

		 */
		void		SetLayer(int layer) { m_layer = layer; }
		const int	GetLayer() const { return m_layer; }

        /**
        	실행 하자마자 레이어를 변경 시켜 주고 종료 시킨다.
	        
         */
virtual void        Start(CTriggerInstance* pInstance) const;

private:
		int			m_layer;	//@ 변경할 맵 레이어
};

} // end namespace

#endif