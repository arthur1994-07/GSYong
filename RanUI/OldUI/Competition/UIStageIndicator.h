#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CSwapImage;
class EngineDeviceMan;
class CUIStageNumber;

/**
	���� �����߿� ���������� ǥ�� �Ѵ�.	
	���� ������ ǥ�õǾ� �������� �������� ������ �˸���.

 */
class CUIStageIndicator
	: public CUIGroup
{
public:
		CUIStageIndicator(EngineDeviceMan* pEngineDevice);
virtual ~CUIStageIndicator();

		/**
			Ÿ�̸Ӹ� ���� �Ѵ�.
			Ÿ�̸Ӹ� ���� ��ġ�� �Է� �޴µ� ���� ����ӿ� ����

		 */
virtual bool				CreateSubControl();

		/**
			���� ���������� �Է� �Ѵ�.
			
		 */
		void				SetStage(int nCur) { m_nCur = nCur; }

		/**
			�������� ���� ������ ���� �Ѵ�.

			������ ���� ���ڿ� ���� �ؾ� �Ѵ�.
			�� ������ ���� ��ŭ ���������� ǥ�� �Ǹ� ���������� �� ���������� ǥ�� �ȴ�.

			���� ������ 3���� �Է��ϰ� ������ ���������� 40�̰� ���� 1 �������� ��� �Ʒ�ó�� ǥ�õȴ�.
			< 1, 2, 3, 40 >

			\param nStart ���� ��������
			\param nEnd ������ ��������
			\param nCount ���������� ������ ����
		 */
		void				SetRange(int nStart, int nEnd, int nRange)  { m_nStart = nStart; m_nEnd = nEnd; m_nRange = nRange; }

		/**
			�̹��� ���ڸ� ȿ�������� ������ �ϱ� ���� ���� �������� �Ѵ�.
			
		 */
virtual HRESULT				Render(LPDIRECT3DDEVICEQ pd3dDevice);

protected:
		/**
			���������� ����� ���� ��ġ�� ��´�.
            �츮�� ��� ������ ���� �ǹ� ó���� ���� �Ѵ�.
			
		 */
		const UIRECT		GetPivot();

		/**
			��Ʈ���� �����.
			�ߺ� �ڵ带 ���� �Ѵ�.

			\param pControl ��Ʈ�� XML
			\return ������ ��Ʈ�� ����Ʈ, ���н� NULL
		 */
		CUIControl*			CreateControl(const TCHAR* pControl);

		/**
			�̹����� ������ �Ѵ�.

		 */
		const UIRECT		RenderBG(const UIRECT sRt, LPDIRECT3DDEVICEQ pd3dDevice);
		const UIRECT		RenderPreStage(const int nNum, const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice, bool bClear);
		const UIRECT		RenderCurStage(const int nNum, const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
        const UIRECT		RenderPostStage(const int nNum, const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
		const UIRECT		RenderClear(const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
        void                RenderNumber(const int nNum, bool bWhiteNBlack, const UIRECT sRt, LPDIRECT3DDEVICEQ pd3dDevice);

		/**
			���� ��Ʈ�ѵ�

		 */
		CUIControl*			m_pBg;		    //@ ��� �̹���
		CUIControl*			m_pPreActive;	//@ ���� �������� ��� �̹���
        CUIControl*			m_pActive;	    //@ ���� �������� ��� �̹���
        CUIControl*			m_pPostActive;	//@ ���� �������� ��� �̹���
		CUIControl*			m_pClear;	    //@ �������� Ŭ���� �̹���
        CUIStageNumber*     m_pNubmer;      //@ �������� ǥ�� ����

		/**
			���� ����

		 */
		int					m_nStart;	    //@ ���� ��������
		int					m_nEnd;		    //@ ������ ��������
		int					m_nRange;	    //@ ���������� ������ ����
		int					m_nCur;		    //@ ���� ��������
};
