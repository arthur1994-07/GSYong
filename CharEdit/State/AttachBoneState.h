#ifndef AttachBoneState_H
#define AttachBoneState_H

#include "EditState.h"

/** 
    어태치 본 상태

 */
class CAttachBoneState
	: public CEditState
{
public:
		CAttachBoneState();

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