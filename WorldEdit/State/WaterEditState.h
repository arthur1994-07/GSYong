#ifndef CWaterEditState_H
#define CWaterEditState_H

#include "EditState.h"

/** 
    Water Surf (�ĵ� ����)
    
 */
class CWaterEditState
	: public CEditState
{
public:
		CWaterEditState();

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