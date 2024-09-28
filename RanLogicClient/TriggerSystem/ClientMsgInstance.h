#ifndef CClientMsgInstance_H
#define CClientMsgInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	�������� �޽��� �ν��Ͻ�
    �������������� �����鿡�� �˸� �޽����� ��� �Ѵ�.
	
 */
class CClientMsgInstance
    : public CClientTriggerInstance
{
public:
    CClientMsgInstance() : m_time(0.f) {}
	~CClientMsgInstance();

    /**
        ��� �ؾ��� �޽����� ���� �� ȣ�� �ȴ�. 
        �츮�� UI �� ���޵� �޽����� ��� �Ѵ�.

    */
    void    Message(SMessage* msg);

	/**
        �������� ���� �޽����� ó�� �Ѵ�.
        �츮�� ���� ���� ������ ���� �޽��� ����� ���´�.

     */
    void    Stage(SMessage* msg);

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

    float   m_time; //@ �޽��� ��� ���� �ð�
    TSTRING m_msg;  //@ ��� �޽���
};

} // end namespace

#endif