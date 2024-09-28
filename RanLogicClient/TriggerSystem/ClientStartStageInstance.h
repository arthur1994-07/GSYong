#ifndef CClientStartStageInstance_H
#define CClientStartStageInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	�������� ���� Ʈ������ �ν��Ͻ�
    ���������� ���۵Ǹ� �޽����� ��� �Ѵ�.
	
 */
class CClientStartStageInstance
    : public CClientTriggerInstance
{
public:
    CClientStartStageInstance() : m_time(0.f) {}
    ~CClientStartStageInstance();

    /**
        �������� ���� �޽����� ���� ȣ�� �ȴ�.
        �츮�� �̶��������� ������ ��� �Ѵ�.

    */
    void    StartStage(SMessage* msg);

    /**
		������ ó���� ���� �Ѵ�.
		�ð��� �帧�� ó�� �Ѵ�.
		
	 */
	void	FrameMove(float fAccTime, float fElapsedTime);

protected:
    /**
        ����� ���¸� �ݿ� �Ѵ�.

     */
    void    Update(bool bEnable = true);

    float   m_time; //@ ��� ���� �ð�    
};

} // end namespace

#endif