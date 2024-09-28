#ifndef CPortalCloseTrigger_H
#define CPortalCloseTrigger_H

#include "Trigger.h"

namespace TriggerSystem
{

/**
	포탈 닫기 트리거
	
	서버와 클라에서 포탈을 닫고 클리어 후 포탈을 열 수 있다. 
	툴에서 포탈을 설정 하므로 이를 제어하기 위해서는 서버, 클라 모두 필요하다.
	오픈은 언제 어느 시점에 할 지 클라는 알수 없으므로 클라는 오픈은 서버만 있음에 주의 한다.

 */
class CPortalCloseTrigger
	: public CTrigger
{
public:
		CPortalCloseTrigger();

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		Bind(CTriggerScript* pScript);

		/**
			바인딩 인터페이스 함수들

		 */
		void		SetFromGateID(DWORD id) { m_fromGateID = id; }
		const DWORD	GetFromGateID() const { return m_fromGateID; }

protected:
		int			m_fromGateID;	//@ 현재 맵에서 타켓 게이트 ID
};

} // end namespace

#endif