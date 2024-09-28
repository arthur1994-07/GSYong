#ifndef CSoundEditState_H
#define CSoundEditState_H

#include "EditState.h"

/** 
    Sound Edit (���� ����)
    
 */
class CSoundEditState
	: public CEditState
{
public:
		CSoundEditState();

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