//	기본 정보 표시
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 주병진
//	로그
//		[2003.11.31]
//			@ 작성
//		[2010.05.04]
//			@  Color 세팅 추가

#pragma	once

#include "../EngineLib/GUInterface/UIGroup.h"
#include "../../Enginelib/GUInterface/UITextControl.h"
#include "UITextControl.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
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
