#pragma once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CUIProgressTimer;
class CBasicTextBox;
class CBasicTextButton;
class EngineDeviceMan;

/**
	�絵�� Ȯ�� �޽��� �ڽ��� ���� �Ѵ�.	

 */
class CPVERetry
	: public CUIWindowEx
{
public:
		CPVERetry(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CPVERetry();

		enum
		{
			CONTROL_ID_NONE = CUIWindowEx::ET_CONTROL_NEXT + 1,
            CONTROL_ID_TIMER,
			CONTROL_ID_BUTTON_OK,
			CONTROL_ID_BUTTON_CANCEL,
		};

		/**
			�絵�� Ȯ�� �޽��� �ڽ��� ���� �Ѵ�.
			ȭ�� �߾ӿ� �ֻ��� ��� �ڽ��� ���� �Ѵ�.
			
			\return ���� ���� ����
		 */
virtual bool				CreateSubControl();

		/**
			��ư Ŭ��, Ÿ�� ���� ���� �޽����� ó�� �Ѵ�.

		 */
virtual	void				TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);

		/**
			�����츦 ���� �Ѵ�.
            �絵�� �ʿ��� �ڱݰ� ��� �ð��� �Է� �� �־�� �Ѵ�.

		 */
		void				Open(LONGLONG nMoney, int nTime);

		/**
			�ű��δ������� Ʈ���Žý��ۿ����� �����Ѵ�;
		 */
		void				SetNewInstance ( bool bNewInstance, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID );

		/**
			��ư�� Ŭ������ �� �̺�Ʈ�� ó�� �Ѵ�.

		 */
		void				OnOk();
		void				OnCancel();

		/**
        	â ���� ó���� ���� �Ѵ�.

         */
        void                Close();

protected:
        /**
        	���������� ���� �Ѵ�.
            �츮�� �������� ��޷� ó���ϰ� �������� ����� ���� �Ѵ�.

         */
        void                SetVisibleSingle(BOOL bVisible);

		/**
			��ư�� ���� �Ѵ�.
			������ �ڵ� ������ ���� ��� �Ѵ�.
			
		 */
		CBasicTextButton*	CreateButton(const TCHAR* name, const TCHAR* text, UIGUID id);

		/**
			���� ��Ʈ�ѵ�

		 */
		CUIProgressTimer*	m_pTimer;	//@ Ÿ�̸� ��Ʈ��
		CBasicTextBox*		m_pText;	//@ �ؽ�Ʈ ǥ�� ��Ʈ��
		CBasicTextButton*	m_pOk;		//@ ���� ��ư
		CBasicTextButton*	m_pCancel;	//@ �׸� ��ư
        CUIControl*			m_pClock;	//@ �ð� �̹���

        /**
            �ܺ� �������̽�

         */
        GLGaeaClient*       m_pGaeaClient;

		/**
			�ű��δ����� Ʈ���Žý������� �����ϱ� ���� �����Ѵ�;
		 */
		bool								m_bNewInstance;
		InstanceSystem::InstanceMapID		m_nInstanceID;
		SNATIVEID							m_sKeyMapID;
};
