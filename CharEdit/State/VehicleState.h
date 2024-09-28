#ifndef VehicleState_H
#define VehicleState_H

#include "EditState.h"

/**
    비어클(탈것) 상태

 */
class CVehicleState
	: public CEditState
{
public:
		CVehicleState();

		int				GetState();

        /**
            아래 함수들의 설명은 CEditState 를 참조 한다.

         */
        void			SetFocus(CParam& kParam) {}
		void			Reload(CParam& kParam);
		void			Insert(CParam& kParam);
        void            Archive(CParam& kParam);
        void			Asset(CParam& kParam);
};

#endif