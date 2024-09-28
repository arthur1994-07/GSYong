#pragma	once

#include "../../../EngineLib/GUInterface/BasicComboBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UI_MSG_COMBOBOX_ROLLOVER_CHANGE = UIMSG_USER8;
////////////////////////////////////////////////////////////////////

class CBasicComboBoxRollOver;

class CSmartComboBoxBasicEx : public CBasicComboBox
{
public:
	enum
	{
		NULLENUM = 1 + BASIC_COMBOBOX_TEXTBOX,
		BASIC_COMBOBOX_ROLLOVER,
		BASIC_COMBOBOX_LINE_TEXTURE,
		BASIC_COMBOBOX,
	};
public:
	CUIControl* m_pControl;
	BOOL m_fRollOverIN;
	CBasicLineBoxSmart* m_pComboBox;
	int m_nSelectIndex;
private:
	CBasicComboBoxRollOver * m_pComboBox_RollOver;

public:
	CSmartComboBoxBasicEx(EngineDeviceMan* pEngineDevice);
	virtual	~CSmartComboBoxBasicEx();

public:
	void CreateSmartComboBox(const char* szComboBoxControl, const char* szTextureInfo);
	void CreateSmartComboBoxCustom ( const char* szComboBoxControl,
		const char* szTextureInfo,
		const char* szButtonPos, 
		const char* szButtonNormal, 
		const char* szButtonOver,
		const char* szButtonDown);
	void SetVisibleSingleRollOver(BOOL fBool);
	BOOL IsVisibleComboxRoolOver();
	void ClearText();
	void SetRollOverAddText( const CString strText, int nIndex );
	CString GetRollOverText( int nIndex );
	int GetCount();

public:
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);

protected:
	void CreateSmartComboImage(const char* szTextureInfo);
	void CreateSmartComboImageCustom ( const char* szTextureInfo,
		const char* szButtonPos, 
		const char* szButtonNormal, 
		const char* szButtonOver,
		const char* szButtonDown);
	public:
	void CreateComboBoxRllOver();
	void CreateSmartTextBox();
	void SetSelectIndex ( int nIndex );
	int GetSelectIndex ();

};
