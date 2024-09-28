#pragma once

#include "UIListBox.h"

class CBasicTextBoxEx;

/**
	���ڿ��� ����ϴ� ����Ʈ �ڽ��� ���� �Ѵ�.

	ó������ �ϳ��� �ؽ�Ʈ �ڽ��� ���κ��� ����Ͽ��µ�
	�̷��� �ϸ� �ٰ����̳� ���� ����, �� ���� �� Ŀ���Ҹ���� �����
	���κ� �ؽ�Ʈ �ڽ� 1���� ���� �ߴ�.

 */
class CUIStringListBox
	: public CUIListBox
{
public:
        /**
            ���� ���ڷ� ���ڿ� �ڽ��� ���ڿ� ������ �޴� �Ϳ� ���� �Ѵ�.
            �� ������ ��� ���ο� ���� �ȴ�.
            
         */
		CUIStringListBox(EngineDeviceMan* pEngineDevice, const DWORD dwFont = 9, const int nTextAlign = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y);
virtual ~CUIStringListBox();

		/**
			���ڿ� �������� �߰� �Ѵ�.
			�߰� ���ڷ� �츮�� ���ڿ� ������ �޴´�.

			\param item �߰��� ���ڿ�
			\param ... ���ڿ� ����
		 */
virtual void				AddItem(const TCHAR* item, ...);

        /**
        	�ε����� ���ڿ��� ���� �Ѵ�.

	        \param nIndex ������ ���ڿ� ����Ʈ �ε���
            \param item �߰��� ���ڿ�
            \param ... ���ڿ� ����
         */
virtual void				SetItem(int nIndex, const TCHAR* item, ...);

protected:
		/**
			�ؽ�Ʈ�ڽ��� ���� �Ѵ�.
			�츮�� �ϳ��� �ؽ�Ʈ �ڽ��� �ϳ��� ������ �ǹ� �Ѵ�.

		 */
		CBasicTextBoxEx*	CreateTextBox(DWORD font = 9, WORD align = UI_FLAG_DEFAULT);

		DWORD				m_dwDefaultFont;	//@ �ؽ�Ʈ�ڽ� ����Ʈ ��Ʈ ũ��
		int					m_nDefaultAlign;	//@ �ؽ�Ʈ�ڽ� ����Ʈ ���ڿ� ���� ���
};
