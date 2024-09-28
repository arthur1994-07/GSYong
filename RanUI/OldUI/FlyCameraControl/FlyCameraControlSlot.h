
#pragma once

class CBasicTextBox;
class RnEditBox;
class RnButton;
class GLGaeaClient;

class FlyCameraControlSlot : public CUIGroupHelper
{
private:
	enum /*UIID*/{
		IDD_FCC_EDITBOX_VALUE = NO_ID + 1,
		IDD_FCC_BUTTON_VALUEUP,
		IDD_FCC_BUTTON_VALUEDOWN,
	};
	const DWORD m_dwSlotIndex;
	float		m_fValue;

	CBasicTextBox*	m_pTxtName;
	RnEditBox*		m_pEdtValue;
	RnButton*		m_pBtnValueUp;
	RnButton*		m_pBtnValueDown;

public:
	FlyCameraControlSlot(EngineDeviceMan* pEngineDevice, const DWORD nSlotIndex);
	virtual ~FlyCameraControlSlot(void) {}

public:
	void CreateSubControl( const std::string& strText = "FLYCAMERACONTROL_SLOT_NAME");
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
	
	void SetValue( const float fValue );
	const float GetValue() { return m_fValue; }
};