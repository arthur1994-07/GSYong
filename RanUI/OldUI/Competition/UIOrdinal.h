#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include <deque>

class CBasicTextBox;
class EngineDeviceMan;
class CSwapImage;

/**
	이미지 기반으로 영어 서수로 표현 한다.    
    이미지는 숫자 0 - 9 까지와
    서수 표시를 위한 알파벳 th, st, nd, rd로 이뤄진다.

 */
class CUIOrdinal
	: public CUIGroup
{
public:
		CUIOrdinal(EngineDeviceMan* pEngineDevice);
virtual ~CUIOrdinal();

        enum EORDINAL
        {
            ORDI_NONE,            
            ORDI_ST,
            ORDI_ND,
            ORDI_RD,
            ORDI_TH,
        };

		/**
			서브 컨트롤들을 생성 한다.
			
		 */
virtual bool			CreateSubControl();

		/**
			숫자를 영어 서수로 출력 한다.

		 */
        void            SetOrdinal(const int nNum, bool bShow);

		/**
			이미지 숫자를 효율적으로 렌더링 하기 위해 수동 랜더링을 한다.
			
		 */
virtual HRESULT			Render(LPDIRECT3DDEVICEQ pd3dDevice);

protected:
		/**
			출력할 시작 위치를 얻는다.
            우리는 가운데 정렬을 위한 피벗 처리를 수행 한다.
			
		 */
		const UIRECT    GetPivot();

        /**
            해당 위치에 숫자와 서수를 출력 한다.

         */
        const UIRECT	RenderNum(const int nNum, const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
        const UIRECT	RenderOridinal(const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);

		/**
			서브 컨트롤들

		 */
        CSwapImage*     m_pDigitImg;        //@ 숫자 이미지
        CSwapImage*     m_pOrdinalImg;      //@ 영어 서수(th, st, nd, rd) 알파벳 이미지

        int             m_nUnitsDigit;      //@ 숫자 1자릿수
        int             m_nTensDigit;       //@ 숫자 10자릿수
        int             m_nOrdinal;         //@ 서수 문자
};
