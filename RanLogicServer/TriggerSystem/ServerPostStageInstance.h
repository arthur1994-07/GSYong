#ifndef CServerPostStageInstance_H
#define CServerPostStageInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	�������� �� ó��
    �츮�� �������� ����(���� or ����) �� �Ǹ� ���� ���� UI �� ������ �ð� ���� ��� ��
    ���� �������� ���Ḧ ó�� �Ѵ�. �� �����̸� ���� ���������
    ���� ���������� �� ��� ó���� ������ ���⵵�� ���� ������ �и��ϸ� ������ ���ؼ� ����

 */
class CServerPostStageInstance
    : public CServerTriggerInstance
{
public:
        CServerPostStageInstance();

		/**
			���� ó���� ���� �Ѵ�.
			�츮�� ���� ���� ó���� ���� �ʾƾ� �Ѵ�.
			�ֳ��ϸ� ���� �������� ���� �� �� �δ� �ð��� �����ų� ��Ƽ�� ������ ����
			���� ���°� �ȴٸ� �̸� ó�� �ϱ� ���� �� ó���� �ʿ��ϱ� ���� �̴�.

		 */
		void		Exit();

        /**
            �޽����� ���� �������� ���� ������ ���� �Ѵ�.
            �������� ��� �޽����� �޾ƾ� �ϹǷ�
            CServerPostStageInstance �� �ݵ�� ���������� ���۵� ��
            ���� ���۵ǵ��� ���� �� ���ƾ� �Կ� ���� �Ѵ�.

         */
        void        StageClear(SMessage* msg);

        /**
            ���� ��ȯ�� �ͼ� �ð� ���ῡ ���� ��ȯ �̶��
            �츮�� �������� �� ó���� ���� �Ѵ�.

         */
        void        ReturnPC(SMessage* msg);

        /**
            ������ ó���� ���� �Ѵ�.

         */
        void		FrameMove(float fAccTime, float fElapsedTime);

protected:
        /**
            ���� or ���� �޽����� ���� �Ǹ�
            ����Ʈ���������� ���� ��Ų��.

         */
        void        Run();

        
        /**
            �츮�� ����Ʈ ���������� ������ �Ǹ�
            �������� ���� ���������� ���Ḧ ó�� �Ѵ�.

         */
        void        PostStage();
        
        float       m_fTime;        //@ ���� ��� �ð�
        bool        m_bCompleted;   //@ �������� ���� ����
        bool        m_bLast;        //@ ���� �������� ����
		bool        m_bTimeOver;    //@ �ͼ� �ð� ���� ����
		bool        m_bFireParty;   //@ ��Ƽ �ı��� ���� ���� ����
};

} // end namespace

#endif