
#pragma once

class GLGaeaClient;
class CUIListBox;
class CBasicScrollBarEx;
class FlyCameraControlKeySlot;

class FlyCameraControlKeyframeList : public CUIGroupHelper
{
private:
	enum /*control*/{
		IDD_FLYCAMERACONTROLLIST_LIST_BOX = NO_ID + 1,
		IDD_FLYCAMERACONTROLLIST_SCROLLBAR,
		IDD_FLYCAMERACONTROLLIST_LIST_SLOT_0,
		IDD_FLYCAMERACONTROLLIST_LIST_SLOT_1,
		IDD_FLYCAMERACONTROLLIST_LIST_SLOT_2,
		IDD_FLYCAMERACONTROLLIST_LIST_SLOT_3,
		IDD_FLYCAMERACONTROLLIST_LIST_SLOT_4,
		IDD_FLYCAMERACONTROLLIST_LIST_SLOT_5,
		IDD_FLYCAMERACONTROLLIST_LIST_SLOT_6,
		IDD_FLYCAMERACONTROLLIST_LIST_SLOT_7,
		IDD_FLYCAMERACONTROLLIST_LIST_SLOT_8,
		IDD_FLYCAMERACONTROLLIST_LIST_SLOT_9,
	};

	enum /*flag*/{
		LIST_VISIBLE_LINE = 10,
	};

	GLGaeaClient*		m_pGaeaClient;
	CUIListBox*			m_pKeyFrameListBox;
	CBasicScrollBarEx*	m_pListScrollBar;

	int m_nSelectIndex;
	
	FlyCameraControlKeySlot* m_pKeySlot[LIST_VISIBLE_LINE];

public:
	FlyCameraControlKeyframeList(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual ~FlyCameraControlKeyframeList() {}

public:
	void CreateSubControl(void);
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

	const unsigned int GetSelectIndex(void)	const {return m_nSelectIndex;}

private:
	void UpdateList(void);
	void DelSlot(void);
};