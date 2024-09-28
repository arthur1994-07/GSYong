#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class EngineDeviceMan;
class CSwapImage;

/**
	스테이지 표시를 위한 숫자 UI 를 정의 한다.
	2가지 색상(검정, 흰색)의 숫자가 있다.
    0 ~ 99 까지를 표현할 수 있다.

 */
class CUIStageNumber
	: public CUIGroup
{
public:
		CUIStageNumber(EngineDeviceMan* pEngineDevice);
virtual ~CUIStageNumber();

		/**
			사용할 컨트롤들을 생성 한다.

		 */
virtual bool				CreateSubControl();

		/**
			출력 숫자를 입력 한다.

			\param nNum 출력 숫자
            \param bWhiteNBlack true : 화이트, false : 블랙
		 */
		void				SetNumber(int nNum, bool bWhiteNBlack);

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
            입력된 위치에 숫자와 콜론을 출력 한다.

         */
        const UIRECT		RenderNum(int nNum, D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);

		/**
			서브 컨트롤들

		 */
        CSwapImage*         m_pWhiteImg;    //@ 흰 숫자 이미지
        CSwapImage*         m_pBlackImg;    //@ 검정 숫자 이미지

        bool                m_bWhiteNBlack; //@ 흰 숫자인지 검은 숫자인지 유무
        int                 m_nUnitsDigit;  //@ 1 자리
        int                 m_nTensDigit;   //@ 10자리
};
