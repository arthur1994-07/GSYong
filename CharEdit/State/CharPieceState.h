#ifndef CharPieceState_H
#define CharPieceState_H

#include "EditState.h"

/**
    ���� ���� ó����
    �������� ĳ���� ����(cps), ����ġ ����(aps), ���Ŭ ����(vps) �� �����Ѵ�.
    ���������δ� piece �� �����ϰ� ���Ǹ� �̸��� �ٸ� ���̴�.

 */
class CCharPieceState
	: public CEditState
{
public:
		CCharPieceState();

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