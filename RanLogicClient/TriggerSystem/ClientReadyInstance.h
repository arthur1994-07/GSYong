#ifndef CClientReadyInstance_H
#define CClientReadyInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	�δ� ������ ó�� �Ѵ�.
    ���� ���� ī��Ʈ �ϱ� ���� ��� �Ѵ�.
	
 */
class CClientReadyInstance
    : public CClientTriggerInstance
{
public:
    CClientReadyInstance();
    ~CClientReadyInstance();

	/**
		��� ���°� ���۽� ȣ�� �ȴ�.
		�츮�� ���۵ɶ� ShowTip �� ���¸� ��� �صд�.

	 */
	void	Start();

    /**
        ���� ��� ���¸� �˸��� �޽����� ���� ȣ�� �ȴ�.
        �������� ���ۿ� �ռ� �������� ���� ���¸� ���� �Ѵ�.

    */
    void    Ready(SMessage* msg);

	/**
        �������� ���� �޽����� ó�� �Ѵ�.
        �츮�� ���� ���� ������ ���� �޽��� ����� ���´�.

     */
    void    Stage(SMessage* msg);

	/**
		������ ó���� ���� �Ѵ�.
		�츮�� ��� ���� �ð��� �帣�� �Ѵ�.
        �� ��� �ð��� �������� �������� �Ϸ� ���°� ���޵��� ������
        ���°Ϳ� ���� �Ѵ�.
		
	 */
	void	FrameMove(float fAccTime, float fElapsedTime);

protected:
    /**
        ����� ���¸� �ݿ� �Ѵ�.

     */
    void    Update(bool bEnable);

	/**
		Tip �� ������ ������ ���� �Ѵ�.
		Tip �� ȭ�� �߾� ��ܿ� ǥ�õǴ� ���� ���� �̴�.

	 */
	void	ShowTip(bool bEnable);

    int     m_cur;  	//@ ���� �ο�
    int     m_all;  	//@ �� �ο�
    float   m_time; 	//@ ��� ���� �ð�
	bool	m_showtip;	//@ ������ �� ���⸦ ���� ���� ���¸� ����� �д�.
};

} // end namespace

#endif