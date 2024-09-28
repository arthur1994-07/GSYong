#ifndef CEffectEditState_H
#define CEffectEditState_H

#include "LightEditState.h"

/** 
    Effect Edit (효과 설정)
    
 */
class CEffectEditState
	: public CLightEditState
{
public:
		CEffectEditState();

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