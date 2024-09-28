#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class EngineDeviceMan;
class CSwapImage;

/**
	�������� ǥ�ø� ���� ���� UI �� ���� �Ѵ�.
	2���� ����(����, ���)�� ���ڰ� �ִ�.
    0 ~ 99 ������ ǥ���� �� �ִ�.

 */
class CUIStageNumber
	: public CUIGroup
{
public:
		CUIStageNumber(EngineDeviceMan* pEngineDevice);
virtual ~CUIStageNumber();

		/**
			����� ��Ʈ�ѵ��� ���� �Ѵ�.

		 */
virtual bool				CreateSubControl();

		/**
			��� ���ڸ� �Է� �Ѵ�.

			\param nNum ��� ����
            \param bWhiteNBlack true : ȭ��Ʈ, false : ��
		 */
		void				SetNumber(int nNum, bool bWhiteNBlack);

		/**
			�̹��� ���ڸ� ȿ�������� ������ �ϱ� ���� ���� �������� �Ѵ�.
			
		 */
virtual HRESULT				Render(LPDIRECT3DDEVICEQ pd3dDevice);

protected:
    	/**
			����� ���� ��ġ�� ��´�.
            �츮�� ��� ������ ���� �ǹ� ó���� ���� �Ѵ�.
			
		 */
		const UIRECT		GetPivot();

        /**
            �Էµ� ��ġ�� ���ڿ� �ݷ��� ��� �Ѵ�.

         */
        const UIRECT		RenderNum(int nNum, D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);

		/**
			���� ��Ʈ�ѵ�

		 */
        CSwapImage*         m_pWhiteImg;    //@ �� ���� �̹���
        CSwapImage*         m_pBlackImg;    //@ ���� ���� �̹���

        bool                m_bWhiteNBlack; //@ �� �������� ���� �������� ����
        int                 m_nUnitsDigit;  //@ 1 �ڸ�
        int                 m_nTensDigit;   //@ 10�ڸ�
};
