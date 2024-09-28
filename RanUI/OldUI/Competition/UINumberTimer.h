#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class EngineDeviceMan;
class CSwapImage;

/**
	타이머를 표시하는 UI 를 정의 한다.
	시간의 진행을 숫자로 표현하는 UI 이다.
	분과 초로 표현 된다. 시는 표현되지 않는다.	
    이미지 기반으로 숫자를 표현 한다.

 */
class CUINumberTimer
	: public CUIGroup
{
public:
		CUINumberTimer(EngineDeviceMan* pEngineDevice);
virtual ~CUINumberTimer();

		/**
			타이머를 생성 한다.
			타이머를 생성 위치를 입력 받는데 로컬 기반임에 주의

		 */
virtual bool				CreateSubControl();

		/**
			현재 시간을 입력 한다.

			\param nTime 현재 시간을 입력 한다. 단위는 sec 단위, 1 = 1초
		 */
		void				SetTime(int nTime);

		int					GetTime();

		void				SetTimerImage(bool isVisible);

		/**
			시간을 깜박이도록 처리 한다.

			\param bBlank 깜박임 유무
			\param dwDuringTick 얼마 간격으로 깜박이게 할지, 1000 = 1초, 틱단위
		 */
		void				SetBlank(bool bBlank, DWORD dwDuringTick = 250);

		/**
			시간 완료 처리를 수행 한다.

		 */
		void				OnCompleted();
        
		/**
			이미지 숫자를 효율적으로 렌더링 하기 위해 수동 랜더링을 한다.
			
		 */
virtual HRESULT				Render(LPDIRECT3DDEVICEQ pd3dDevice);

protected:
    	/**
			출력할 시작 위치를 얻는다.
            우리는 가운데 정렬을 위한 피벗 처리를 수행 한다.
			
		 */
		const UIRECT		GetPivot();

		/**
			컨트롤을 만든다.
			중복 코드를 방지 한다.

		 */
		CUIControl*			CreateControl(const TCHAR* pControl, CUIControl* pParentControl);

        /**
            입력된 위치에 숫자와 콜론을 출력 한다.
			마이너스는 시간이 00:00 이 되면 00:00 보다 --:-- 을 보여주기 위한 -- 이다.

         */
        const UIRECT		RenderClock(D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
        const UIRECT		RenderNum(int nNum, D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
		const UIRECT		RenderMinus(D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
        const UIRECT		RenderColon(D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);

		/**
			현재 블랭크(깜박임) 해야 하는 상황인지를 출력 한다.

		 */
		bool				IsBlank();

		/**
			서브 컨트롤들

		 */
        CSwapImage*         m_pDigitImg;        //@ 숫자 이미지
        CSwapImage*         m_pColonImg;        //@ : 이미지
		CSwapImage*         m_pMinusImg;        //@ -- 이미지
        CUIControl*         m_pClock;           //@ 시계 이미지

		/**
			시간 정보

		 */
        int                 m_nUnitsDigitMin;   //@ 분 1 자리
        int                 m_nTensDigitMin;    //@ 분 10자리
        int                 m_nUnitsDigitSec;   //@ 초 1 자리
        int                 m_nTensDigitSec;    //@ 초 10자리

		int					m_SavedTime;		//저장된 시간 (초)

		/**
		    Time 안보이게 하기
		 */
		bool				m_isTimerImage;		//@ Time 이미지 나올꺼니?

		/**
			깜박임 제어 변수

		 */
		DWORD				m_dwDuringTick;		//@ 깜박임 간격
		bool				m_bBlank;			//@ 깜박임 유무
		DWORD				m_dwPreTick;		//@ 깜박임 true 시에 이전 깜박임 했던 틱
};
