#ifndef CClientTrigger_H
#define CClientTrigger_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
    클라이언트 트리거
    클라이언트에서 사용하는 모든 트리거의 공용 인터페이스
    모든 클라이언트 트리거 공용 인터페이스 임에 주의 한다.
	
 */
template<class T>
class CClientTrigger
	: public T
{
public:
        /**
        	클라이언트 트리거는 기본적으로 클라이언트 인스턴스를 생성하도록 한다.
	        
         */
virtual CTriggerInstance*   CreateInstance() const { return new CClientTriggerInstance; }
};

} // end namespace

#endif