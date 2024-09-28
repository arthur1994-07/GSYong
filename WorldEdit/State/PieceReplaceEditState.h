#ifndef CPieceReplaceEditState_H
#define CPieceReplaceEditState_H

#include "LightEditState.h"

/** 
    Piece Object Manager
    
 */
class CPieceReplaceEditState
	: public CLightEditState
{
public:
		CPieceReplaceEditState();

		int				GetState();

        void			SetFocus(int nOld) {}
        void			Reload(CParam& kParam) {}
        void			Insert(CParam& kParam) {}
        void            Archive(CParam& kParam) {}

        void            Asset(CParam& kParam) {}
        void            EditAsset(CParam& kParam);
        void            SelectAsset(CParam& kParam);

protected:
        /**
            �� �������� �ǽ� ���÷��̽�(Piece Replace) �� 
            ��� ���� �ǽ� ���÷��̽��� ����ȭ(����) �����.

         */
        void            SyncPieceReplace();

        /**
        	����Ʈ ��Ʈ���� ���õ� �������� ��� ���� �Ѵ�.

        	\param kControl ������ ����Ʈ ��Ʈ��
         */
        void            RemoveAllSelected(CListCtrl& kControl);
};

#endif