#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include <deque>

class CBasicTextBox;
class EngineDeviceMan;
class CUIOrdinal;
class CSwapImage;

/**
	PVE �ÿ� ������ ����ϴ� ����� UI�� ���� �Ѵ�.

 */
class CPVEBoard
	: public CUIGroup
{
public:
		CPVEBoard(EngineDeviceMan* pEngineDevice);
virtual ~CPVEBoard();

        enum ERESULT_PVE
        {
            ERESULT_SUCCEED,        //@ ����
            ERESULT_FAILED,         //@ ����
            ERESULT_WAITAFTERDEATH, //@ ���� ���
        };

		/**
			���� ��Ʈ�ѵ��� ���� �Ѵ�.
			�츮�� 2�� �ؽ�Ʈ�ڽ��� ���� �Ѵ�.
			�ϳ��� ��� �ؽ�Ʈ �ڽ��� �������� ���� ǥ�� �Ѵ�.
			�ϳ��� �ϴ� �ؽ�Ʈ �ڽ��� �����鿡�� �÷��̽� ����� �޽����� ǥ�� �Ѵ�.

		 */
virtual bool				CreateSubControl();

		/**
			��� ���忡 ���������� ��� �Ѵ�.

		 */
        void                SetOrdinalToUpBoard(const int nNum, bool bShow);

		/**
			�ϴ� ���忡 �ؽ�Ʈ�� ��� �Ѵ�.

		 */
		void				SetTextToDownBoard(const TCHAR* pText, const D3DCOLOR& TextColor, bool bShow);

        /**
            ����� ��� �Ѵ�.
            �츮�� ����, ����, ���� ��� �� ����� ����� �� �ִ�.

         */
        void                SetResult(ERESULT_PVE eResult, bool bShow);

		/**
			�ð� ����� ó�� �Ѵ�.

		 */
virtual void				Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);

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
			���� ��Ʈ�ѵ�

		 */
        CUIOrdinal*         m_pOrdinal;         //@ ���� ���� ǥ��
		CBasicTextBox*		m_pDownBoard;		//@ ���� �ؽ�Ʈ �ڽ�
        CSwapImage*			m_pResultImage;     //@ ���(����, ����, ���� ���) �̹���
        CUIControl*         m_pWaitAfterDeath;  //@ ���� ��� ���� �̹���
		float				m_fResultTimer;		//@ ��� UI ��� �ð�
};
