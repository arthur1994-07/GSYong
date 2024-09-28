#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CSwapImage;
class CBasicTextBox;
class CBasicProgressBar;
class EngineDeviceMan;
class CBasicLineBoxSmart;

/**
	Ÿ�̸Ӹ� ǥ���ϴ� UI �� ���� �Ѵ�.
	�ð��� ������ ���α׷����ٸ� ���� ǥ���ϴ� UI �̴�.
	10�ʶ�� 10���϶� ���α׷����ٰ� 100% �̰� 0�� �϶� 0 % �̴�.

	Ÿ�̸Ӱ� �Ϸ�Ǹ� �θ� �Ϸ� �̺�Ʈ �޽����� ���� �ʿ� ���� �Ѵ�.

 */
class CUIProgressTimer
	: public CUIGroup
{
public:
		CUIProgressTimer(EngineDeviceMan* pEngineDevice, CUIControl* pParent);
virtual ~CUIProgressTimer();

        // Ÿ�̸� �ð� ������ �޽���
        static const DWORD  PT_TIMEOVER = 0x01000000;

		/**
			Ÿ�̸Ӹ� ���� �Ѵ�.
			Ÿ�̸Ӹ� ���� ��ġ�� �Է� �޴µ� ���� ����ӿ� ����

		 */
virtual bool				Create(float x, float y);

		/**
			Ÿ�̸Ӹ� ���� ��Ų��.
			������ϰ� ���� ���� ȣ�� �ϸ� �ٽ� �ʱ� �ð����� �ʱ�ȭ �ȴ�.

			\param nTimer �� Ÿ�̸� �ð��� �Է� �Ѵ�. ������ sec ����, 1 = 1��
		 */
		void				Start(int nTimer = 0);

		/**
			�ð� ����� ó�� �Ѵ�.

		 */
virtual void				Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);

protected:
		/**
			���� ��Ʈ�ѵ�

		 */		
		CBasicLineBoxSmart*	m_pGauge;	//@ Ÿ�̸� ������
        CBasicLineBoxSmart*	m_pBlank;	//@ Ÿ�̸� ������ ���
		CBasicTextBox*		m_pText;	//@ ���� �ð� ǥ�� �ؽ�Ʈ �ڽ�

		/**
			�ð� ����

		 */
		int					m_nTimer;	//@ �� �ð�
		float				m_fCur;		//@ ���� �ð�
};
