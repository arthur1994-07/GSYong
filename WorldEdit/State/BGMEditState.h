#ifndef CBGMEditState_H
#define CBGMEditState_H

#include "EditState.h"

/** 
    BGM Edit (πË∞Ê¿Ωæ«)
    
 */
class CBGMEditState
	: public CEditState
{
public:
		CBGMEditState();

		int				GetState();

        void			SetFocus(int nOld) {}
        void			Reload(CParam& kParam) {}
        void			Insert(CParam& kParam) {}
        void            Archive(CParam& kParam) {}

        void            Asset(CParam& kParam) {}
        void            EditAsset(CParam& kParam);
        void            SelectAsset(CParam& kParam) {}
};

#endif