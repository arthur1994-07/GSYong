#ifndef CPieceReplaceEditState_H
#define CPieceReplaceEditState_H

#include "LightEditState.h"

/** 
    Piece Object Manager
    
 */
class CPieceReplaceEditState
	: public CLightEditState
{
public:
		CPieceReplaceEditState();

		int				GetState();

        void			SetFocus(int nOld) {}
        void			Reload(CParam& kParam) {}
        void			Insert(CParam& kParam) {}
        void            Archive(CParam& kParam) {}

        void            Asset(CParam& kParam) {}
        void            EditAsset(CParam& kParam);
        void            SelectAsset(CParam& kParam);

protected:
        /**
            탭 페이지의 피스 리플레이스(Piece Replace) 와 
            어셋 뷰의 피스 리플레이스를 동기화(같게) 맞춘다.

         */
        void            SyncPieceReplace();

        /**
        	리스트 컨트롤의 선택된 아이템을 모두 제거 한다.

        	\param kControl 제거할 리스트 컨트롤
         */
        void            RemoveAllSelected(CListCtrl& kControl);
};

#endif