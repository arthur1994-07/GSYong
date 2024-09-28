#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CSwapImage;
class EngineDeviceMan;
class CUIStageNumber;

/**
	게임 진행중에 스테이지를 표시 한다.	
	게임 끝까지 표시되어 유저에게 스테이지 정보를 알린다.

 */
class CUIStageIndicator
	: public CUIGroup
{
public:
		CUIStageIndicator(EngineDeviceMan* pEngineDevice);
virtual ~CUIStageIndicator();

		/**
			타이머를 생성 한다.
			타이머를 생성 위치를 입력 받는데 로컬 기반임에 주의

		 */
virtual bool				CreateSubControl();

		/**
			현재 스테이지를 입력 한다.
			
		 */
		void				SetStage(int nCur) { m_nCur = nCur; }

		/**
			스테이지 범위 정보를 설정 한다.

			마지막 범위 인자에 주의 해야 한다.
			이 지정된 범위 만큼 스테이지가 표시 되며 마지막에는 끝 스테이지가 표시 된다.

			만약 범위를 3으로 입력하고 마지막 스테이지가 40이고 현재 1 스테이지 라면 아래처럼 표시된다.
			< 1, 2, 3, 40 >

			\param nStart 시작 스테이지
			\param nEnd 마지막 스테이지
			\param nCount 스테이지를 보여줄 범위
		 */
		void				SetRange(int nStart, int nEnd, int nRange)  { m_nStart = nStart; m_nEnd = nEnd; m_nRange = nRange; }

		/**
			이미지 숫자를 효율적으로 렌더링 하기 위해 수동 랜더링을 한다.
			
		 */
virtual HRESULT				Render(LPDIRECT3DDEVICEQ pd3dDevice);

protected:
		/**
			스테이지를 출력할 시작 위치를 얻는다.
            우리는 가운데 정렬을 위한 피벗 처리를 수행 한다.
			
		 */
		const UIRECT		GetPivot();

		/**
			컨트롤을 만든다.
			중복 코드를 방지 한다.

			\param pControl 컨트롤 XML
			\return 성공시 컨트롤 포인트, 실패시 NULL
		 */
		CUIControl*			CreateControl(const TCHAR* pControl);

		/**
			이미지를 렌더링 한다.

		 */
		const UIRECT		RenderBG(const UIRECT sRt, LPDIRECT3DDEVICEQ pd3dDevice);
		const UIRECT		RenderPreStage(const int nNum, const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice, bool bClear);
		const UIRECT		RenderCurStage(const int nNum, const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
        const UIRECT		RenderPostStage(const int nNum, const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
		const UIRECT		RenderClear(const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
        void                RenderNumber(const int nNum, bool bWhiteNBlack, const UIRECT sRt, LPDIRECT3DDEVICEQ pd3dDevice);

		/**
			서브 컨트롤들

		 */
		CUIControl*			m_pBg;		    //@ 배경 이미지
		CUIControl*			m_pPreActive;	//@ 이전 스테이지 배경 이미지
        CUIControl*			m_pActive;	    //@ 현재 스테이지 배경 이미지
        CUIControl*			m_pPostActive;	//@ 다음 스테이지 배경 이미지
		CUIControl*			m_pClear;	    //@ 스테이지 클리어 이미지
        CUIStageNumber*     m_pNubmer;      //@ 스테이지 표시 숫자

		/**
			범위 정보

		 */
		int					m_nStart;	    //@ 시작 스테이지
		int					m_nEnd;		    //@ 마지막 스테이지
		int					m_nRange;	    //@ 스테이지를 보여줄 범위
		int					m_nCur;		    //@ 현재 스테이지
};
