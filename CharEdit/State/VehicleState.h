#ifndef VehicleState_H
#define VehicleState_H

#include "EditState.h"

/**
    ���Ŭ(Ż��) ����

 */
class CVehicleState
	: public CEditState
{
public:
		CVehicleState();

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