#ifndef CServerLayerTrigger_H
#define CServerLayerTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	���� �� ���̾� Ʈ����
	�ʿ� ��Ƽ ���̾�� ���͸� ���� �� ���Ѵٸ�
	�� Ʈ���Ÿ� ���� ���̾ ���������ν� ���͵��� ���Ӱ� ���� �� �� �ִ�.
	�̴� ���������� ���� �ؾ� �Ѵ�.
	
 */
class CServerLayerTrigger
	: public CServerTrigger<CTrigger>
{
public:
		CServerLayerTrigger();

		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	GetType() const { return TRIGGER_TYPE_LAYER; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		Bind(CTriggerScript* pScript);

		/**
			���ε� �������̽� �Լ���

		 */
		void		SetLayer(int layer) { m_layer = layer; }
		const int	GetLayer() const { return m_layer; }

        /**
        	���� ���ڸ��� ���̾ ���� ���� �ְ� ���� ��Ų��.
	        
         */
virtual void        Start(CTriggerInstance* pInstance) const;

private:
		int			m_layer;	//@ ������ �� ���̾�
};

} // end namespace

#endif