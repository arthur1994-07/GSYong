#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class EngineDeviceMan;
class CSwapImage;

/**
	Ÿ�̸Ӹ� ǥ���ϴ� UI �� ���� �Ѵ�.
	�ð��� ������ ���ڷ� ǥ���ϴ� UI �̴�.
	�а� �ʷ� ǥ�� �ȴ�. �ô� ǥ������ �ʴ´�.	
    �̹��� ������� ���ڸ� ǥ�� �Ѵ�.

 */
class CUINumberTimer
	: public CUIGroup
{
public:
		CUINumberTimer(EngineDeviceMan* pEngineDevice);
virtual ~CUINumberTimer();

		/**
			Ÿ�̸Ӹ� ���� �Ѵ�.
			Ÿ�̸Ӹ� ���� ��ġ�� �Է� �޴µ� ���� ����ӿ� ����

		 */
virtual bool				CreateSubControl();

		/**
			���� �ð��� �Է� �Ѵ�.

			\param nTime ���� �ð��� �Է� �Ѵ�. ������ sec ����, 1 = 1��
		 */
		void				SetTime(int nTime);

		int					GetTime();

		void				SetTimerImage(bool isVisible);

		/**
			�ð��� �����̵��� ó�� �Ѵ�.

			\param bBlank ������ ����
			\param dwDuringTick �� �������� �����̰� ����, 1000 = 1��, ƽ����
		 */
		void				SetBlank(bool bBlank, DWORD dwDuringTick = 250);

		/**
			�ð� �Ϸ� ó���� ���� �Ѵ�.

		 */
		void				OnCompleted();
        
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
			��Ʈ���� �����.
			�ߺ� �ڵ带 ���� �Ѵ�.

		 */
		CUIControl*			CreateControl(const TCHAR* pControl, CUIControl* pParentControl);

        /**
            �Էµ� ��ġ�� ���ڿ� �ݷ��� ��� �Ѵ�.
			���̳ʽ��� �ð��� 00:00 �� �Ǹ� 00:00 ���� --:-- �� �����ֱ� ���� -- �̴�.

         */
        const UIRECT		RenderClock(D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
        const UIRECT		RenderNum(int nNum, D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
		const UIRECT		RenderMinus(D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);
        const UIRECT		RenderColon(D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice);

		/**
			���� ��ũ(������) �ؾ� �ϴ� ��Ȳ������ ��� �Ѵ�.

		 */
		bool				IsBlank();

		/**
			���� ��Ʈ�ѵ�

		 */
        CSwapImage*         m_pDigitImg;        //@ ���� �̹���
        CSwapImage*         m_pColonImg;        //@ : �̹���
		CSwapImage*         m_pMinusImg;        //@ -- �̹���
        CUIControl*         m_pClock;           //@ �ð� �̹���

		/**
			�ð� ����

		 */
        int                 m_nUnitsDigitMin;   //@ �� 1 �ڸ�
        int                 m_nTensDigitMin;    //@ �� 10�ڸ�
        int                 m_nUnitsDigitSec;   //@ �� 1 �ڸ�
        int                 m_nTensDigitSec;    //@ �� 10�ڸ�

		int					m_SavedTime;		//����� �ð� (��)

		/**
		    Time �Ⱥ��̰� �ϱ�
		 */
		bool				m_isTimerImage;		//@ Time �̹��� ���ò���?

		/**
			������ ���� ����

		 */
		DWORD				m_dwDuringTick;		//@ ������ ����
		bool				m_bBlank;			//@ ������ ����
		DWORD				m_dwPreTick;		//@ ������ true �ÿ� ���� ������ �ߴ� ƽ
};
