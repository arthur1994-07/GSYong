#ifndef CSkinObjEditState_H
#define CSkinObjEditState_H

#include "EditState.h"

/** 
    SkinObj Edit (��Ų������Ʈ)
    
 */
class CSkinObjEditState
	: public CEditState
{
public:
		CSkinObjEditState();

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