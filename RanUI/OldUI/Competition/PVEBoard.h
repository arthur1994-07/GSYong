#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include <deque>

class CBasicTextBox;
class EngineDeviceMan;
class CUIOrdinal;
class CSwapImage;

/**
	PVE 시에 정보를 출력하는 기능적 UI를 관리 한다.

 */
class CPVEBoard
	: public CUIGroup
{
public:
		CPVEBoard(EngineDeviceMan* pEngineDevice);
virtual ~CPVEBoard();

        enum ERESULT_PVE
        {
            ERESULT_SUCCEED,        //@ 성공
            ERESULT_FAILED,         //@ 실패
            ERESULT_WAITAFTERDEATH, //@ 죽음 대기
        };

		/**
			서브 컨트롤들을 생성 한다.
			우리는 2개 텍스트박스를 생성 한다.
			하나는 상단 텍스트 박스로 스테이지 등을 표시 한다.
			하나는 하단 텍스트 박스로 유저들에게 플레이시 출력할 메시지를 표시 한다.

		 */
virtual bool				CreateSubControl();

		/**
			상단 보드에 스테이지를 출력 한다.

		 */
        void                SetOrdinalToUpBoard(const int nNum, bool bShow);

		/**
			하단 보드에 텍스트를 출력 한다.

		 */
		void				SetTextToDownBoard(const TCHAR* pText, const D3DCOLOR& TextColor, bool bShow);

        /**
            결과를 출력 한다.
            우리는 성공, 실패, 죽음 대기 의 결과를 출력할 수 있다.

         */
        void                SetResult(ERESULT_PVE eResult, bool bShow);

		/**
			시간 경과를 처리 한다.

		 */
virtual void				Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);

protected:
		/**
			텍스트 박스를 만든다.
			중복 코드를 방지 한다.

			\param dwFontSize 폰트 사이즈
			\param dwFlags 폰트 생성 플래그들
			\param pControl 컨트롤명
			\param wUIAlign 컨트롤 정렬 정보
			\param nTextAlign 텍스트 정렬 정보
			\return 생성시 텍스트 박스, 실패시 NULL
		 */
		CBasicTextBox*		CreateTextBox(DWORD dwFontSize, DWORD dwFlags, const TCHAR* pControl, WORD wUIAlign, int nTextAlign);

		/**
			서브 컨트롤들

		 */
        CUIOrdinal*         m_pOrdinal;         //@ 영어 서수 표시
		CBasicTextBox*		m_pDownBoard;		//@ 하위 텍스트 박스
        CSwapImage*			m_pResultImage;     //@ 결과(성공, 실패, 죽음 대기) 이미지
        CUIControl*         m_pWaitAfterDeath;  //@ 죽음 대기 상태 이미지
		float				m_fResultTimer;		//@ 결과 UI 출력 시간
};
