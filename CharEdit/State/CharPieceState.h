#ifndef CharPieceState_H
#define CharPieceState_H

#include "EditState.h"

/**
    조각 상태 처리기
    조각에는 캐릭터 조각(cps), 어태치 조각(aps), 비어클 조각(vps) 가 존재한다.
    내부적으로는 piece 로 동일하게 사용되며 이름만 다를 뿐이다.

 */
class CCharPieceState
	: public CEditState
{
public:
		CCharPieceState();

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