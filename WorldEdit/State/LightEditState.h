#ifndef CLightEditState_H
#define CLightEditState_H

#include "EditState.h"

/** 
    Light Edit (빛)
    
 */
class CLightEditState
	: public CEditState
{
public:
		CLightEditState(CEditState* pNext = 0);

virtual int				GetState();

virtual void			SetFocus(int nOld) {}
virtual void			Reload(CParam& kParam) {}
virtual void			Insert(CParam& kParam) {}
virtual void            Archive(CParam& kParam) {}

virtual void            Asset(CParam& kParam) {}
virtual void            EditAsset(CParam& kParam);
virtual void            SelectAsset(CParam& kParam);

protected:

        /**
        	입력된 리스트 박스의 모든 선택된 사항을 제거 한다.
            우리는 멀티 선택이 가능하므로 어셋 뷰에서 멀티 뷰를 지원하지 않으므로
            처음 선택된 것을 모두 지우는 용도로 사용 한다.

         */
        void            RemovSelection(CListBox& kListBox);
};

#endif