#ifndef CCameraEditState_H
#define CCameraEditState_H

#include "EditState.h"

/** 
    Camera Edit (카메라 설정)
    
 */
class CCameraEditState
	: public CEditState
{
public:
		CCameraEditState();

		int				GetState();

        void			SetFocus(int nOld) {}
        void			Reload(CParam& kParam) {}
        void			Insert(CParam& kParam) {}
        void            Archive(CParam& kParam) {}

        void            Asset(CParam& kParam) {}
        void            EditAsset(CParam& kParam);
        void            SelectAsset(CParam& kParam);
};

#endif