#ifndef CClientStageInstance_H
#define CClientStageInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	다시 도전을 제어 한다.
    우리는 모두 죽었을 때 재도전을 할 수 있다.
    	
 */
class CClientRetryInstance
    : public CClientTriggerInstance
{
public:
    CClientRetryInstance();
    ~CClientRetryInstance();

    /**
        스테이지 상태 메시지를 처리 한다.
        자신이 죽었거나 모두 죽었을 상태를 캐치해서
        해당 상태일때 UI 처리를 수행 한다.

     */
    void    Stage(SMessage* msg);

protected:
	/**
		재도전 UI 를 On Off 한다.

	 */
	void	Retry(bool bEnable);

    int		m_time; //@ 재도전 대기 시간
};

} // end namespace

#endif