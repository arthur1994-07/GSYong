#ifndef CharFileState_H
#define CharFileState_H

#include "EditState.h"

/**
    캐릭터 상태
    chf 파일을 의미 한다.
    조각 들이 모여 하나의 캐릭터가 이뤄짐을 의미 한다.

 */
class CCharFileState
	: public CEditState
{
public:
		CCharFileState();

		int				GetState();

        /**
            아래 함수들의 설명은 CEditState 를 참조 한다.

         */
		void			SetFocus(CParam& kParam);
		void			Reload(CParam& kParam);
		void			Insert(CParam& kParam);
        void            Archive(CParam& kParam);
        void			Asset(CParam& kParam);
};

#endif