#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CUINumberTimer;
class CUIStageIndicator;
class EngineDeviceMan;

/**
	PVE ���� ���࿡ �ʿ��� UI �� ��� �Ѵ�.

 */
class CPVEIndicator
	: public CUIGroup
{
public:
		CPVEIndicator(EngineDeviceMan* pEngineDevice);
virtual ~CPVEIndicator();

		/**
			���� ��Ʈ�ѵ��� ���� �Ѵ�.
			�츮�� 2�� �ؽ�Ʈ�ڽ��� ���� �Ѵ�.
			�ϳ��� ��� �ؽ�Ʈ �ڽ��� �������� ���� ǥ�� �Ѵ�.
			�ϳ��� �ϴ� �ؽ�Ʈ �ڽ��� �����鿡�� �÷��̽� ����� �޽����� ǥ�� �Ѵ�.

		 */
virtual bool				CreateSubControl();

		/**
			�������� ���� �ð��� �Է� �Ѵ�.

			\param nSec �ʴ��� �Է�
		 */
		void				SetTime(int nSec, bool bBlank, bool bShow);

		/**
			���������� ���� �Ѵ�.
			ȭ�鿡 ��� �������� ������ �����ִ� ���� �ƴ϶� �ԷµǴ� ���ݸ�ŭ�� ���� �ܿ� ����	
			�ڼ��� ������ ��ȹ���� ���� �ٶ�

			\param nStage ���� ��������			
			\param nEnd �� ������ �� ��������, ���ݿ� �� �ȸ��� ��쵵 ���� �Ѵ�.
			\param nRange ȭ�鿡 ������ �������� ����
		 */
		void				SetStage(int nStage, int nEnd, int nRange, bool bShow);

		/**
			���� ���͸� ���� �Ѵ�.

			\param nRem ���� ����
			\param nAll ��ƾ��� ��� ����
		 */
		void				SetMonster(int nRem, int nAll, bool bShow);

		/**
			�̵� ����Ʈ�� ���� �Ѵ�.
            �̵� ����Ʈ�� �������� �̵� �ؾ��� ������ �ǹ��Ѵ�.
            �̸� ���� ��ֹ� ��� ���� �̺�Ʈ�� �� �� �ִ�.

		 */
		void				SetPositioning(int nX, int nY, int nNow, int nNeed, bool bShow);

protected:
		/**
			�ؽ�Ʈ �ڽ��� �����.
			�ߺ� �ڵ带 ���� �Ѵ�.

			\param dwFontSize ��Ʈ ������
			\param dwFlags ��Ʈ ���� �÷��׵�
			\param pControl ��Ʈ�Ѹ�
			\param wUIAlign ��Ʈ�� ���� ����
			\param nTextAlign �ؽ�Ʈ ���� ����
			\return ������ �ؽ�Ʈ �ڽ�, ���н� NULL
		 */
		CBasicTextBox*		CreateTextBox(DWORD dwFontSize, DWORD dwFlags, const TCHAR* pControl, WORD wUIAlign, int nTextAlign);

		/**
			���� �� ��°� �̵� ����Ʈ ����� ���� �Ѵ�.
			
		 */
		void				Align();

		CUINumberTimer*		m_pTimerIndicator;	//@ ���ڱ�� Ÿ�̸�
		CUIStageIndicator*	m_pStageIndicator;	//@ �������� ǥ�ñ�
		CBasicTextBox*		m_pMobIndicator;	//@ ���� �� ��ġ ǥ��
        CBasicTextBox*		m_pMoveIndicator;	//@ �̵� ����Ʈ ���� ǥ��

};
