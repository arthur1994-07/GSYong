
#pragma once

class CUIGroup;
class CBasicTextBox;
class RnButton;

class FlyCameraControlKeySlot : public CUIGroupHelper
{
private:
	enum /*UIID*/{
		IDD_FCCKEY_NAMEBOX = NO_ID + 1,
	};
	enum /**/{

	};
	DWORD				m_dwSlotIndex;
	CBasicTextBox*		m_pTxtName;	
	CUIControl*			m_pHighlight;

public:
	FlyCameraControlKeySlot(EngineDeviceMan* pEngineDevice, const DWORD nSlotIndex);
	virtual ~FlyCameraControlKeySlot(void) {}

public:
	void CreateSubControl(void);
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

public:
	void SetKeySlotInfo( const DWORD dwSlot, const BOOL bSelect = FALSE );
	const DWORD GetSlotIndex(void) const	{ return m_dwSlotIndex; }
};