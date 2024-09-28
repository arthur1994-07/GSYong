//	�⺻ ���� ǥ��
//
//	���� �ۼ��� : ���⿱
//	���� ������ : �ֺ���
//	�α�
//		[2003.11.31]
//			@ �ۼ�
//		[2010.05.04]
//			@  Color ���� �߰�

#pragma	once

#include "../EngineLib/GUInterface/UIGroup.h"
#include "../../Enginelib/GUInterface/UITextControl.h"
#include "UITextControl.h"

////////////////////////////////////////////////////////////////////
//	����� �޽��� ����
const DWORD UI_MSG_COMBOBOX_ROLLOVER = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class	CBasicTextBox;

class	CBasicComboBox : public CUIGroup
{
protected:
	enum
	{
		BASIC_COMBOBOX_BUTTON_DOWN = 1,
		BASIC_COMBOBOX_TEXTBOX
	};

public:
	CBasicComboBox (EngineDeviceMan* pEngineDevice);
	virtual	~CBasicComboBox ();

public:
	void	SetText		 (const CString& str, const D3DCOLOR& cColor=NS_UITEXTCOLOR::WHITE );
	void	SetTextColor ( const D3DCOLOR& cColor );

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	CreateBaseComboBox ( char* szComboBoxControl );	

private:
	void	CreateComboImage ();
	void	CreateTextBox ();

protected:
	CBasicTextBox*	m_pTextBox;
};
