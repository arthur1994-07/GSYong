#ifndef CClientStageInstance_H
#define CClientStageInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	���� Ŭ���� ���¸� ó�� �Ѵ�.
	
 */
class CClientStageInstance
    : public CClientTriggerInstance
{
public:
    CClientStageInstance();
    ~CClientStageInstance();

    /**
        �������� ���� ���¸� ���� �Ѵ�.
        �̰��� ���������� ���°� ����Ǿ��� �� ������ ���� ���۵Ǿ�
        Ŭ���̾�Ʈ�� ������ UI �� ���� �Ѵ�.

     */
    void    Stage(SMessage* msg);

	/**
        �츮�� �ƹ�Ÿ�� ������
		��� ��� UI�� ��� �Ѵ�.

     */
	void    Action(SMessage* msg);

	/**
		�츮�� �ƹ�Ÿ�� ��Ƴ���
		��� ��� UI�� ���� �Ѵ�.

	 */
	void    Rebirth(SMessage* msg);

	/**
		������ ó���� ���� �Ѵ�.
		�ð��� �帧�� ó�� �Ѵ�.
		
	 */
	void	FrameMove(float fAccTime, float fElapsedTime);

protected:
    /**
        ����� ���¸� �ݿ� �Ѵ�.

     */
    void    Update(bool bEnable);

	/**
		�������� ���¸� ��� �Ѵ�.

	 */
	void	StageState(int nStage, bool bEnable);

    int     m_stage;    //@ ���� ��������
    float   m_time;     //@ �������� ���� �ð�
    bool    m_visible;  //@ ���� ���� ���� �Ǹ� UI�� ������ �ʰ� ó�� 
};

} // end namespace

#endif