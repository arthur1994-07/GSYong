#pragma once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CUIListBox;
class CBasicTextButton;
class EngineDeviceMan;

/**
	PVE �������� ��� ������ ȭ�鿡 ��� �Ѵ�.

 */
class CPVEResult
	: public CUIWindowEx
{
public:
		CPVEResult(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CPVEResult();

		enum
		{
			CONTROL_ID_NONE = NO_ID + 1,
			CONTROL_ID_BUTTON_EXIT,
		};

		/**
			���� ��Ʈ�ѵ��� ���� �Ѵ�.
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
			�ð� ����� ó�� �Ѵ�.

		 */
virtual void				Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);

		/**
			������ ��ư�� Ŭ������ �� ȣ�� �ȴ�.

		 */
		void				OnExit();

		/**
			�ð��� �Ϸ�Ǹ� ȣ�� �ȴ�.

		 */
		void				OnCompleted();

        /**
            ��� â�� ��� �Ѵ�.

         */
        void                SetStageResult();

        /**
			��Ÿ ������ ���� �Ѵ�.

		 */
		void				SetStage(int nStage, bool bLast);
		void				SetTimer(int nTime);

protected:
		/**
			��ư�� ���� �Ѵ�.
			������ �ڵ� ������ ���� ��� �Ѵ�.
			
		 */
		CBasicTextButton*	CreateButton(const TCHAR* name, const TCHAR* text, UIGUID id);

		/**
			����Ʈ �ڽ��� ���� �Ѵ�.
			�츮�� ����Ʈ �ڽ��� ���ð� ��ũ���� ������� �ʴ´�.
			������ �ڵ� ������ ���� ��� �Ѵ�.
			
		 */
		bool			    CreateListBox(const int nIndex, const TCHAR* pColumn, const TCHAR* pColumnWord, 
                                          const TCHAR* pListBox, const TCHAR* pTextBoxInListBox, float fLine, float fFront, float fBack);

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
			����Ʈ�� ���� �߰�
			
		 */
		void				AddClass(DWORD dwClass);
		void				AddCharName(const TCHAR* pCharName);
		void				AddDamage(int nDamage);
		void				AddHeal(int nHeal);
		void				AddBonus(bool bExp, bool bMoney);
        void                AddMVP(bool bMVP);

		/**
			��� ����Ʈ �ڽ��� �ʱ�ȭ ��Ų��.

		 */
        void                Clear();

		/**
			���� ��Ʈ�ѵ�

		 */
		CBasicTextButton*	m_pExit;	//@ ������ ��ư
		CBasicTextBox*		m_pTime;	//@ ���������� Ÿ�̸� �ؽ�Ʈ �ڽ�
        CUIOrdinal*         m_pOrdinal; //@ ���� ���� �̹���(���������� ǥ��)
        CUIControl*         m_pClear;   //@ all Ŭ���� �̹���

        enum
        {            
            LB_CLASS,                   //@ Ŭ���� �̹���
            LB_CLASSNAME,               //@ Ŭ������
            LB_DAMAGE,                  //@ ��������
            LB_HEAL,                    //@ ����
            LB_BONUS,                   //@ �߰� ���ʽ� ����
            LB_MVP,                     //@ MVP �̹���
            LB_QUANTITY,
        };

        CUIListBox*         m_pListBox[LB_QUANTITY];
        CBasicTextBox*		m_pColumn[LB_QUANTITY];

		/**
			�ð� ����

		 */
		int					m_nTimer;	//@ �� �ð�
		float				m_fCur;		//@ ���� �ð�

        /**
            �ܺ� �������̽�

         */
		GLGaeaClient*       m_pGaeaClient;
};
