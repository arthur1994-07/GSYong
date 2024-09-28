#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class EngineDeviceMan;

/**
	���� ���� UI �� ���� �Ѵ�.
	���� �� ��Ƽ���� ��� �ð��� ǥ�� �Ѵ�.

 */
class CPVEEntranceState
	: public CUIGroup
{
public:
		CPVEEntranceState(EngineDeviceMan* pEngineDevice);
virtual ~CPVEEntranceState();

		/**
			���� ���� ��Ʈ�ѵ��� ���� �Ѵ�.

		 */
virtual bool				CreateSubControl();

		/**
			���� ���¸� ��� �Ѵ�.

			\param nCur ���� ���� ��Ƽ����
			\param nAll ��ü �����ؾ��� ��Ƽ����
			\param nSec �ð� ���� ��� �ð�
		 */
		void				SetEntranceState(int nCur, int nAll, int nSec);

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
			�� ���¸� ���� �Ѵ�.

		 */
		void				SetTime(int nSec);
		void				SetPerson(int nNum, int nAll);

		/**
			�ð��� �Ϸ�ǰų� �ο��� ��� �����ϰų� ��
			���°� �Ϸ�� ���¿� ���� ó��

		 */
		void				OnBeCompleted();

		/**
			���� ��Ʈ�ѵ�

		 */
		CBasicTextBox*		m_pState;	//@ �ο� ���� ���
		CBasicTextBox*		m_pTime;	//@ �ð� ���

		/**
			�ð� ����

		 */		
		int					m_nTimer;	//@ �� �ð�
		float				m_fCur;		//@ ���� �ð�

		/**
			�ο� ����

		 */
		int					m_nAll;		//@ ��� �ο�
};
