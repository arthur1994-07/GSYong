#ifndef CClientTriggerFinger_H
#define CClientTriggerFinger_H

#include "../../RanLogic/TriggerSystem/TriggerFinger.h"

class GLGaeaClient;

namespace TriggerSystem
{

/**
	Ŭ���̾�Ʈ �����Ϳ� ����� Ʈ���� �۵���	
	
 */
class CClientTriggerFinger
	: public CTriggerFinger
{
public:
        CClientTriggerFinger();
        ~CClientTriggerFinger();

        /**
        	Ʈ���� �ν��Ͻ��� �ܺο� ����� ���� �Ű��� ���� �Ѵ�.
            Ŭ���̾�Ʈ �ڵ忡�� GLGaeaClient �� �ȴ�.

         */
        void            SetGaeaClient(GLGaeaClient* pGaeaClient) { m_pGaeaClient = pGaeaClient; }
        GLGaeaClient*   GetGaeaClient() { return m_pGaeaClient; }

        /**
        	���� �� ���������� ���� �Ѵ�.

         */
        void            SetStage(int stage) { m_stage = stage; }
        int             GetStage() { return m_stage; }

        /**
        	������ ������ ���� �Ѵ�.
            ������ ���̵��� ǥ���ϱ� ���� ��� �Ǿ���.
            ���ڰ� ���� ���� ���̵��� ������.

         */
        void            SetLevel(int nLevel) { m_level = nLevel; }
        int             GetLevel() { return m_level; }

        /**
            Ʈ���Ÿ� �ߵ� ��Ų��.
            ������ ������(Set����) �����͸� �������� �ߵ� ��Ų��.

         */
        void            Start();

protected:
        GLGaeaClient*   m_pGaeaClient;  //@ Ŭ���̾�Ʈ ��ü ���� ������
        int             m_stage;        //@ �� �������� ����
        int             m_level;        //@ ����
};

} // end namespace

#endif