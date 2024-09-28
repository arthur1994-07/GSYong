#ifndef CServerTrigger_H
#define CServerTrigger_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
    서버 트리거
    서버에서 사용하는 모든 트리거의 공용 인터페이스
    모든 서버 트리거 공용 인터페이스 임에 주의 한다.
	
 */
template<class T>
class CServerTrigger
	: public T
{
public:
        /**
        	서버 트리거는 기본적으로 서버 인스턴스를 생성하도록 한다.
	        
         */
virtual CTriggerInstance*   CreateInstance() const { return new CServerTriggerInstance; }
};

} // end namespace

#endif