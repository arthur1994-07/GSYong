#ifndef CTriggerFinger_H
#define CTriggerFinger_H

#include "TriggerSystemType.h"

namespace TriggerSystem
{

/**
	Ʈ���� �۵���
	Ʈ���� ������ ���� �Ѵ�.
	�ܺο����� �� �۵��⸦ ������ Ʈ���Ÿ� �۵� ��Ų��.
	
 */
class CTriggerFinger
{
public:
		CTriggerFinger();
virtual ~CTriggerFinger();

		/**
			Ʈ���Ÿ� �۵� ��Ų��.
          
		 */
virtual	void 						Start();

		/**
			Ʈ���Ÿ� ���� ��Ų��.
			Pause �� �ʿ伺�� ������ ���� �������� �ʴ´�.

		 */
virtual void 						Stop();

		/**
			Ʈ������ �������� ó�� �Ѵ�.
			Ʈ���Ŵ� Child �� Sibling �� �����µ� Sibling �� ���ÿ� ���� �Ǿ�� �� Ʈ���� �̹Ƿ�
			�̵� ��� ������ ó�� �� �־�� �Ѵ�.

			\param fAccTime ��ü ���� �ð�(sec ����, 1 -> 1��)
			\param fElapsedTime ������ ���� �ð�(sec ����, 1 -> 1��)
		 */
virtual void 						FrameMove(float fAccTime, float fElapsedTime);

		/**
			Ʈ���ŵ��� �ܺο��� �ǵ���� ����� ���� ó�� �Ѵ�.
			����� �޽����� ���ؼ� ó���Ǹ� �޽����� Send �� Post �� ���� �Ѵ�.
			Send �� ȣ�� �Ǵ� ������ �ٷ� �����ϸ�, Post �� ���� �� �� FrameMove ���� ȣ�� ó�� �ȴ�.

			Send �� �ﰢ������ ó���Ǳ� ������ �����ͷ� �ѱ��
			Post �� �����ӿ� ó���ǹǷ� �޸� �Ҵ��� �����͸� �Ѱܾ� �Ѵ�.
		 */
virtual void						SendMessage(SMessage* msg);
virtual void						PostMessage(SpMessage msg);

		/**
			Ʈ���Ÿ� �߰� �Ѵ�.
            �츮 ���� Trigger �� ���� Sibling �� ��� ������ 
            ���� ���� Trigger �� �߰� �� �ش�.
			
		 */
		void						AddTrigger(const CTrigger* pTrigger);

protected:
        /**
        	Ʈ���� �۵� ��Ų��.

        	\param start �۵��� ���� Ʈ����
         */
        void 						Start(CTrigger* start);

		TriggerInstanceContainer	m_cur;      //@ ���� �������� Ʈ�����ν��Ͻ���
        TriggerInstanceContainer	m_new;      //@ ���� ������ Ʈ�����ν��Ͻ���        
        float                       m_fTime;    //@ ���� �ð�
};

} // end namespace

#endif