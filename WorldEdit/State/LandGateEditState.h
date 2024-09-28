#ifndef CLandGateEditState_H
#define CLandGateEditState_H

#include "EditState.h"

/** 
    Land-Gate Edit (√‚¿‘±∏)
    
 */
class CLandGateEditState
	: public CEditState
{
public:
		CLandGateEditState();

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