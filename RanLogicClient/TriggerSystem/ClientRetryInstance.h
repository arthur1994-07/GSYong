#ifndef CClientStageInstance_H
#define CClientStageInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	�ٽ� ������ ���� �Ѵ�.
    �츮�� ��� �׾��� �� �絵���� �� �� �ִ�.
    	
 */
class CClientRetryInstance
    : public CClientTriggerInstance
{
public:
    CClientRetryInstance();
    ~CClientRetryInstance();

    /**
        �������� ���� �޽����� ó�� �Ѵ�.
        �ڽ��� �׾��ų� ��� �׾��� ���¸� ĳġ�ؼ�
        �ش� �����϶� UI ó���� ���� �Ѵ�.

     */
    void    Stage(SMessage* msg);

protected:
	/**
		�絵�� UI �� On Off �Ѵ�.

	 */
	void	Retry(bool bEnable);

    int		m_time; //@ �絵�� ��� �ð�
};

} // end namespace

#endif