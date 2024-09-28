#ifndef SMessage_H
#define SMessage_H

#include "TriggerSystemType.h"

namespace TriggerSystem
{

/**
	트리거 인스턴스 메시지
	작동기를 통해 메시지를 전달 할 떄 사용 한다.
	인던에서 트리거 인스턴스에 무언가 메시지를 보낼 때 사용 한다.

 */
struct SMessage
{
virtual ~SMessage() {}
virtual	void Command(CTriggerInstance* pInstance) = 0;
template<class T>
inline  bool Cast(T*& ref) { return (ref = dynamic_cast<T*>(this)) != 0; }
};

/**
	트리거 메시지
	트리거 시스템을 통해 메시지를 전달 할 때 사용 한다.
	인스턴스 메시지와는 다름에 주의 한다.
	사용예는 현재 사용되는 트리거들에 정보를 추출할 떄 등에 사용될 수 있다.

 */
struct SSystemMessage
{
virtual ~SSystemMessage() {}
virtual	void Command(CTrigger* pTrigger) = 0;
};

} // end namespace

#endif