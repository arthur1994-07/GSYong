#ifndef CLightEditState_H
#define CLightEditState_H

#include "EditState.h"

/** 
    Light Edit (��)
    
 */
class CLightEditState
	: public CEditState
{
public:
		CLightEditState(CEditState* pNext = 0);

virtual int				GetState();

virtual void			SetFocus(int nOld) {}
virtual void			Reload(CParam& kParam) {}
virtual void			Insert(CParam& kParam) {}
virtual void            Archive(CParam& kParam) {}

virtual void            Asset(CParam& kParam) {}
virtual void            EditAsset(CParam& kParam);
virtual void            SelectAsset(CParam& kParam);

protected:

        /**
        	�Էµ� ����Ʈ �ڽ��� ��� ���õ� ������ ���� �Ѵ�.
            �츮�� ��Ƽ ������ �����ϹǷ� ��� �信�� ��Ƽ �並 �������� �����Ƿ�
            ó�� ���õ� ���� ��� ����� �뵵�� ��� �Ѵ�.

         */
        void            RemovSelection(CListBox& kListBox);
};

#endif