#ifndef CharFileState_H
#define CharFileState_H

#include "EditState.h"

/**
    ĳ���� ����
    chf ������ �ǹ� �Ѵ�.
    ���� ���� �� �ϳ��� ĳ���Ͱ� �̷����� �ǹ� �Ѵ�.

 */
class CCharFileState
	: public CEditState
{
public:
		CCharFileState();

		int				GetState();

        /**
            �Ʒ� �Լ����� ������ CEditState �� ���� �Ѵ�.

         */
		void			SetFocus(CParam& kParam);
		void			Reload(CParam& kParam);
		void			Insert(CParam& kParam);
        void            Archive(CParam& kParam);
        void			Asset(CParam& kParam);
};

#endif