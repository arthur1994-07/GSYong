#ifndef CClientMonsterClearInstance_H
#define CClientMonsterClearInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	���� Ŭ���� ���¸� ó�� �Ѵ�.
	
 */
class CClientMonsterClearInstance
    : public CClientTriggerInstance
{
public:
    CClientMonsterClearInstance();
    ~CClientMonsterClearInstance();

    /**
        �������� ���� ���¸� ���� �Ѵ�.
        �̰��� ���������� ���°� ����Ǿ��� �� ������ ���� ���۵Ǿ�
        Ŭ���̾�Ʈ�� ������ UI �� ���� �Ѵ�.

     */
    void    Stage(SMessage* msg);

    /**
        ���Ͱ� óġ�Ǿ��� ��� ȣ�� �ȴ�.
        �츮�� ���� ���¸� UI �� �ѷ� �ش�.

    */
    void    MonsterClear(SMessage* msg);

protected:
    /**
        ����� ���¸� �ݿ� �Ѵ�.

     */
    void    Update(bool bEnable = true);

    int     m_cur;  //@ ���� ���� ���ͼ�
    int     m_all;  //@ �� ��ƾ��� ���ͼ�
};

} // end namespace

#endif