#ifndef AttachBoneState_H
#define AttachBoneState_H

#include "EditState.h"

/** 
    ����ġ �� ����

 */
class CAttachBoneState
	: public CEditState
{
public:
		CAttachBoneState();

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