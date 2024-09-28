#ifndef CClientPositioningClearInstance_H
#define CClientPositioningClearInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	��ġ Ŭ���� ���¸� ó�� �Ѵ�.
	
 */
class CClientPositioningClearInstance
    : public CClientTriggerInstance
{
public:
    CClientPositioningClearInstance();
    ~CClientPositioningClearInstance();

    /**
        �������� ���� ���¸� ���� �Ѵ�.
        �̰��� ���������� ���°� ����Ǿ��� �� ������ ���� ���۵Ǿ�
        Ŭ���̾�Ʈ�� ������ UI �� ���� �Ѵ�.

     */
    void    Stage(SMessage* msg);

    /**
        ��ġ�� ��ƾ� �� ��� ȣ�� �ȴ�.
        �츮�� ���� ���¸� UI �� �ѷ� �ش�.

    */
    void    PositioningClear(SMessage* msg);

protected:
    /**
        ����� ���¸� �ݿ� �Ѵ�.

     */
    void    Update(bool bEnable);

    int     m_x;  //@ ���� ��� X ��ǥ
    int     m_y;  //@ ���� ��� Y ��ǥ

    int     m_nNow;           //@ ���� �Ϸ��� ���
    int     m_nNeed;          //@ �̼� �Ϸῡ �ʿ��� ���
};

} // end namespace

#endif