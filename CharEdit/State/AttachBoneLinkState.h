#ifndef AttachBoneLinkState_H
#define AttachBoneLinkState_H

#include "EditState.h"

/** 
    ����ġ �� ��ũ ����

 */
class CAttachBoneLinkState
	: public CEditState
{
public:
		CAttachBoneLinkState();

		int				GetState();

        /**
            �Ʒ� �Լ����� ������ CEditState �� ���� �Ѵ�.

         */
        void			SetFocus(CParam& kParam) {}
		void			Reload(CParam& kParam);
		void			Insert(CParam& kParam);
        void            Archive(CParam& kParam);
        void			Asset(CParam& kParam);
};

#endif