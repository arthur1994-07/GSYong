#pragma once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CUIProgressTimer;
class CBasicTextBox;
class CBasicTextButton;
class EngineDeviceMan;

/**
	���� Ȯ�� �޽��� �ڽ��� ���� �Ѵ�.

 */
class CPVEEntranceConfirm
	: public CUIWindowEx
{
public:
		CPVEEntranceConfirm(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CPVEEntranceConfirm();

		enum
		{
            CONTROL_ID_NONE = CUIWindowEx::ET_CONTROL_NEXT + 1,
            CONTROL_ID_TIMER,
			CONTROL_ID_BUTTON_OK,
			CONTROL_ID_BUTTON_CANCEL,
		};

		/**
			���� Ȯ�� �޽��� �ڽ��� ���� �Ѵ�.
			ȭ�� �߾ӿ� �ֻ��� ��� �ڽ��� ���� �Ѵ�.
			
			\return ���� ���� ����
		 */
virtual bool				CreateSubControl();

		/**
			��ư Ŭ��, Ÿ�� ���� ���� �޽����� ó�� �Ѵ�.

		 */
virtual	void				TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);

		/**
			�� �̸��� �Է� �޴´�.

			\param pMapName Ȯ���� �� �� �̸��� �Է� �޴´�.
		 */
virtual void				SetMapName(const TCHAR* pMapName);

        /**
        	Ȯ�� �ð��� ���� �Ѵ�.

        	\param nTimer ��(1 = 1��)
         */
virtual void                SetTimer(int nTimer);

		/**
			��ư�� Ŭ������ �� �̺�Ʈ�� ó�� �Ѵ�.

		 */
virtual	void				OnOk();
virtual	void				OnCancel();

        /**
        	â ���� ó���� ���� �Ѵ�.

         */
virtual void                Close();

        /**
        	â ���� ó���� ���� �Ѵ�.

         */
virtual void                Open();

protected:
        /**
        	���������� ���� �Ѵ�.
            �츮�� �������� ��޷� ó���ϰ� �������� ����� ���� �Ѵ�.

         */
virtual	void                SetVisibleSingle(BOOL bVisible);

		/**
			��ư�� ���� �Ѵ�.
			������ �ڵ� ������ ���� ��� �Ѵ�.
			
		 */
virtual CBasicTextButton*	CreateButton(const TCHAR* name, const TCHAR* text, UIGUID id);

		/**
			���� ��Ʈ�ѵ�

		 */
		CUIProgressTimer*	m_pTimer;	//@ Ÿ�̸� ��Ʈ��
		CBasicTextBox*		m_pText;	//@ �ؽ�Ʈ ǥ�� ��Ʈ��
		CBasicTextButton*	m_pOk;		//@ ���� ��ư
		CBasicTextButton*	m_pCancel;	//@ ��� ��ư
        CUIControl*			m_pClock;	//@ �ð� �̹���

        /**
            �ܺ� �������̽�

         */
        GLGaeaClient*       m_pGaeaClient;
};
