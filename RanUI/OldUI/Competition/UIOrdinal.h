#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include <deque>

class CBasicTextBox;
class EngineDeviceMan;
class CSwapImage;

/**
	�̹��� ������� ���� ������ ǥ�� �Ѵ�.    
    �̹����� ���� 0 - 9 ������
    ���� ǥ�ø� ���� ���ĺ� th, st, nd, rd�� �̷�����.

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
			���� ��Ʈ�ѵ��� ���� �Ѵ�.
			
		 */
virtual bool			CreateSubControl();

		/**
			���ڸ� ���� ������ ��� �Ѵ�.

		 */
        void            SetOrdinal(const int nNum, bool bShow);

		/**
			�̹��� ���ڸ� ȿ�������� ������ �ϱ� ���� ���� �������� �Ѵ�.
			
		 */
virtual HRESULT			Render(LPDIRECT3DDEVICEQ pd3dDevice);

protected:
		/**
			����� ���� ��ġ�� ��´�.
            �츮�� ��� ������ ���� �ǹ� ó���� ���� �Ѵ�.
			
		 */
		const UIRECT    GetPivot();

        /**
            �ش� ��ġ�� ���ڿ� ������ ��� �Ѵ�.

         */
        const UIRECT	RenderNum(const int nNum, const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
        const UIRECT	RenderOridinal(const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);

		/**
			���� ��Ʈ�ѵ�

		 */
        CSwapImage*     m_pDigitImg;        //@ ���� �̹���
        CSwapImage*     m_pOrdinalImg;      //@ ���� ����(th, st, nd, rd) ���ĺ� �̹���

        int             m_nUnitsDigit;      //@ ���� 1�ڸ���
        int             m_nTensDigit;       //@ ���� 10�ڸ���
        int             m_nOrdinal;         //@ ���� ����
};
