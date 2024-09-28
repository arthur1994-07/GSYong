#ifndef CServerRecognizeInstance_H
#define CServerRecognizeInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	캐릭터의 인지 시간을 제어 한다.
	캐릭터가 부활하거나 이동 후 몹이 인지하는 시간을 제어 한다.
	
 */
class CServerRecognizeInstance
    : public CServerTriggerInstance
{
public:
        CServerRecognizeInstance();

		/**
			인스턴스가 시작될 떄 호출 된다.
			우리는 미리 인지 시간을 가져다 놓는다.

		 */
		void		Start();

        /**
        	유저가 진입했을 경우 호출 된다.
			우리는 유저가 진입 했을 때(부활 포함) 인지 시간을 설정 한다.

         */
        void        EntryPC(SMessage* msg);

protected:
		float		m_time;	//@ 인지 시간
};

} // end namespace

#endif