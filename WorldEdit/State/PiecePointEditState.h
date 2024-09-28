#ifndef CPiecePointEditState_H
#define CPiecePointEditState_H

#include "LightEditState.h"

/** 
    Piece Point Edit
    
 */
class CPiecePointEditState
	: public CLightEditState
{
public:
		CPiecePointEditState();

		int				GetState();

        void			SetFocus(int nOld) {}
        void			Reload(CParam& kParam) {}
        void			Insert(CParam& kParam) {}
        void            Archive(CParam& kParam) {}

        void            Asset(CParam& kParam) {}
        void            EditAsset(CParam& kParam);
        void            SelectAsset(CParam& kParam);

        /**
            ������Ʈ���� ���� �� �л� ���� �Ѵ�.

         */
        void            Update(CParam& kParam);

protected:
        /**
            �� �������� �ǽ� ����Ʈ(Piece Point) �� 
            ��� ���� �ǽ� ����Ʈ�� ����ȭ(����) �����.

            \param dwTimer ������Ʈ �� �ִ� �ð�(1 = 1��)
         */
        void            SyncPiecePoint(double dTimer = 0.010f);

        /**
        	����Ʈ ��Ʈ���� ���õ� �������� ��� ���� �Ѵ�.

        	\param kControl ������ ����Ʈ ��Ʈ��
         */
        void            RemoveAllSelected(CListCtrl& kControl);

        /**
            ������Ʈ ������ �ʹ� ���� ��
            ������Ʈ �ϴµ� �ð��� �ʹ� ���� ��� �����Ƿ�
            ������Ʈ�� �л��Ű�� ���� �߰��� ����.

         */
        bool            m_bUpdate;      //@ ������Ʈ ���� ����
        DWORD           m_dwGreatest;   //@ ������Ʈ�� �ؾ� �ϴ� ��Ʈ�� ����
        HTREEITEM       m_hResumeChild; //@ �̾��� ������
};

#endif