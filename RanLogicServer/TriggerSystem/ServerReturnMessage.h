#ifndef SServerReturnMessage_H
#define SServerReturnMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	인던이 종료 되고 원래 있던 맵으로
    리턴시 호출 된다.
	
 */
struct SServerReturnMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum;  //@ 캐릭터 DB ID
    int nWhy;           //@ 귀환 이유
};

/**
	즉각적으로 인던의 모든 인원들을 
	리턴 시킨다. 이는 특별한 경우를 위해 사용 되어 진다.
	현재는 트리거리스트를 다시 로딩 할때 사용 되어 진다.

 */
struct SServerReturnAllMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);
};

} // end namespace

#endif