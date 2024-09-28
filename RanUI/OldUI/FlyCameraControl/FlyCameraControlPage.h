
#pragma once

class GLGaeaClient;
class CUIEditBoxMan;
class FlyCameraControlSlot;

class FlyCameraControlPage : public CUIGroupHelper
{
private:
	enum /*UIID*/{
		IDD_FLYCAMERACONTROLSLOT_MOVE_SLOT = NO_ID + 1,
		IDD_FLYCAMERACONTROLSLOT_ROTATE_SLOT,

		IDD_FLYCAMERACONTROLPAGE_CAMERAPOS_INFO_EDIT,
		IDD_FLYCAMERACONTROLPAGE_HELP_BUTTON,
		IDD_FLYCAMERACONTROLPAGE_HELP_TEXTBOX,
	};
	enum /*UIINDEX*/{
		FLYCAMERACONTROLSLOT_MOVE_SLOT_INDEX = 0,
		FLYCAMERACONTROLSLOT_ROTATE_SLOT_INDEX,
		FLYCAMERACONTROLSLOT_SIZE,

		FLYCAMERACONTROLPAGE_EDITSIZE = 1,
	};
	GLGaeaClient*		m_pGaeaClient;
	CUIEditBoxMan*		m_pEditMan;
	CBasicVarTextBox*	m_pInfoDisplay;
	FlyCameraControlSlot*	m_pFlyCameraControlSlot[FLYCAMERACONTROLSLOT_SIZE];

	D3DXVECTOR3 m_vCamPos;

public:
	FlyCameraControlPage(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual ~FlyCameraControlPage(void) {}

public:
	void CreateSubControl(void);
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

private:
	void CreateInfoDisplayBox();
};