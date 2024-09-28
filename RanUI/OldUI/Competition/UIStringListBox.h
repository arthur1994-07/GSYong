#pragma once

#include "UIListBox.h"

class CBasicTextBoxEx;

/**
	문자열을 사용하는 리스트 박스를 정의 한다.

	처음에는 하나의 텍스트 박스에 라인별로 사용하였는데
	이렇게 하면 줄간격이나 라인 시작, 끝 간격 등 커스텀마이즈가 어려워
	라인별 텍스트 박스 1개로 수정 했다.

 */
class CUIStringListBox
	: public CUIListBox
{
public:
        /**
            생성 인자로 문자열 박스의 문자열 정렬을 받는 것에 주의 한다.
            이 정렬은 모든 라인에 적용 된다.
            
         */
		CUIStringListBox(EngineDeviceMan* pEngineDevice, const DWORD dwFont = 9, const int nTextAlign = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y);
virtual ~CUIStringListBox();

		/**
			문자열 아이템을 추가 한다.
			추가 인자로 우리는 문자열 색상을 받는다.

			\param item 추가할 문자열
			\param ... 문자열 색상
		 */
virtual void				AddItem(const TCHAR* item, ...);

        /**
        	인덱스의 문자열을 수정 한다.

	        \param nIndex 수정할 문자열 리스트 인덱스
            \param item 추가할 문자열
            \param ... 문자열 색상
         */
virtual void				SetItem(int nIndex, const TCHAR* item, ...);

protected:
		/**
			텍스트박스를 생성 한다.
			우리는 하나의 텍스트 박스는 하나의 라인을 의미 한다.

		 */
		CBasicTextBoxEx*	CreateTextBox(DWORD font = 9, WORD align = UI_FLAG_DEFAULT);

		DWORD				m_dwDefaultFont;	//@ 텍스트박스 디폴트 폰트 크기
		int					m_nDefaultAlign;	//@ 텍스트박스 디폴트 문자열 정렬 방법
};
