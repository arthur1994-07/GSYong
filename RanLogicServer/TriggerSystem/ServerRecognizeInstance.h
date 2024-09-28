#ifndef CServerRecognizeInstance_H
#define CServerRecognizeInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	ĳ������ ���� �ð��� ���� �Ѵ�.
	ĳ���Ͱ� ��Ȱ�ϰų� �̵� �� ���� �����ϴ� �ð��� ���� �Ѵ�.
	
 */
class CServerRecognizeInstance
    : public CServerTriggerInstance
{
public:
        CServerRecognizeInstance();

		/**
			�ν��Ͻ��� ���۵� �� ȣ�� �ȴ�.
			�츮�� �̸� ���� �ð��� ������ ���´�.

		 */
		void		Start();

        /**
        	������ �������� ��� ȣ�� �ȴ�.
			�츮�� ������ ���� ���� ��(��Ȱ ����) ���� �ð��� ���� �Ѵ�.

         */
        void        EntryPC(SMessage* msg);

protected:
		float		m_time;	//@ ���� �ð�
};

} // end namespace

#endif