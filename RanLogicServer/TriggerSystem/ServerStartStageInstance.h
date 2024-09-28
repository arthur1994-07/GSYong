#ifndef CServerStartStageInstance_H
#define CServerStartStageInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	���������� ���۵� ���� ������ ó�� �Ѵ�.
    ����� Ŭ�� ���� �޽����� ���� �Ѵ�.

	������������ ó���ص� �Ǵµ� ���� �������� ������ �ִ� ������
	���������� �ٷ� ���۵Ǵ°� �ƴ϶� ���� ��(�ð�)�� �ֱ� ������
	�������� �����̶�� Ʈ���Ű� ���� �Ѵ�.

 */
class CServerStartStageInstance
    : public CServerTriggerInstance
{
public:
        /**
            �ν��Ͻ��� ������ �� ȣ�� �ȴ�.

         */
        void	Start();

        /**
            ���� ���¸� ���� �Ѵ�.

            \param dwCharDbNum ���¸� ������ ĳ���� DB ID, 0 �ϰ�� �� ��ü�� ����.
         */
        void    Update(DWORD dwCharDbNum = 0);

        /**
        	������ �������� ��� ȣ�� �ȴ�.

         */
        void    EntryPC(SMessage* msg);
};

} // end namespace

#endif