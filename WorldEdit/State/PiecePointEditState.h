#ifndef CPiecePointEditState_H
#define CPiecePointEditState_H

#include "LightEditState.h"

/** 
    Piece Point Edit
    
 */
class CPiecePointEditState
	: public CLightEditState
{
public:
		CPiecePointEditState();

		int				GetState();

        void			SetFocus(int nOld) {}
        void			Reload(CParam& kParam) {}
        void			Insert(CParam& kParam) {}
        void            Archive(CParam& kParam) {}

        void            Asset(CParam& kParam) {}
        void            EditAsset(CParam& kParam);
        void            SelectAsset(CParam& kParam);

        /**
            업데이트량이 많을 때 분산 갱신 한다.

         */
        void            Update(CParam& kParam);

protected:
        /**
            탭 페이지의 피스 포인트(Piece Point) 와 
            어셋 뷰의 피스 포인트를 동기화(같게) 맞춘다.

            \param dwTimer 업데이트 할 최대 시간(1 = 1초)
         */
        void            SyncPiecePoint(double dTimer = 0.010f);

        /**
        	리스트 컨트롤의 선택된 아이템을 모두 제거 한다.

        	\param kControl 제거할 리스트 컨트롤
         */
        void            RemoveAllSelected(CListCtrl& kControl);

        /**
            업데이트 갯수가 너무 많을 때
            업데이트 하는데 시간을 너무 많이 잡아 먹으므로
            업데이트를 분산시키기 위해 추가한 변수.

         */
        bool            m_bUpdate;      //@ 업데이트 할지 유무
        DWORD           m_dwGreatest;   //@ 업데이트를 해야 하는 컨트롤 갯수
        HTREEITEM       m_hResumeChild; //@ 이어할 아이템
};

#endif