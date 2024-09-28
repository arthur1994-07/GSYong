#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CSwapImage;
class CBasicTextBox;
class CBasicProgressBar;
class EngineDeviceMan;
class CBasicLineBoxSmart;

/**
	타이머를 표시하는 UI 를 정의 한다.
	시간의 진행을 프로그래스바를 통해 표현하는 UI 이다.
	10초라면 10초일때 프로그래스바가 100% 이고 0초 일때 0 % 이다.

	타이머가 완료되면 부모에 완료 이벤트 메시지가 전달 됨에 주의 한다.

 */
class CUIProgressTimer
	: public CUIGroup
{
public:
		CUIProgressTimer(EngineDeviceMan* pEngineDevice, CUIControl* pParent);
virtual ~CUIProgressTimer();

        // 타이머 시간 오버시 메시지
        static const DWORD  PT_TIMEOVER = 0x01000000;

		/**
			타이머를 생성 한다.
			타이머를 생성 위치를 입력 받는데 로컬 기반임에 주의

		 */
virtual bool				Create(float x, float y);

		/**
			타이머를 시작 시킨다.
			재시작하고 시플 때도 호출 하면 다시 초기 시간으로 초기화 된다.

			\param nTimer 총 타이머 시간을 입력 한다. 단위는 sec 단위, 1 = 1초
		 */
		void				Start(int nTimer = 0);

		/**
			시간 경과를 처리 한다.

		 */
virtual void				Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);

protected:
		/**
			서브 컨트롤들

		 */		
		CBasicLineBoxSmart*	m_pGauge;	//@ 타이머 게이지
        CBasicLineBoxSmart*	m_pBlank;	//@ 타이머 게이지 배경
		CBasicTextBox*		m_pText;	//@ 남은 시간 표시 텍스트 박스

		/**
			시간 제어

		 */
		int					m_nTimer;	//@ 총 시간
		float				m_fCur;		//@ 남은 시간
};
