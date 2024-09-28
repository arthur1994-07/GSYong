
#pragma once

class GLGaeaClient;
class CUIListBox;
class CBasicScrollBarEx;
struct SSETITEM;
class CSetitemPreviewlistSlot;

class CSetitemPreviewWindow : public CUIWindowEx, ISetitemPreviewWindow
{
private:
	enum {
		// UI Identify decode
		IDD_SETITEM_PREVIEW_LISTBOX = ET_CONTROL_NEXT,
		IDD_SETITEM_PREVIEW_SCROLLBAR,
		IDD_SETITEM_PREVIEW_LIST_SLOT_0,
		IDD_SETITEM_PREVIEW_LIST_SLOT_1,
		IDD_SETITEM_PREVIEW_LIST_SLOT_2,
		IDD_SETITEM_PREVIEW_LIST_SLOT_3,
		IDD_SETITEM_PREVIEW_LIST_SLOT_4,
		IDD_SETITEM_PREVIEW_LIST_SLOT_5,
		IDD_SETITEM_PREVIEW_LIST_SLOT_6,
		IDD_SETITEM_PREVIEW_LIST_SLOT_7,
		IDD_SETITEM_PREVIEW_LIST_SLOT_8,
		IDD_SETITEM_PREVIEW_LIST_SLOT_9,

		IDD_SETITEM_PREVIEW_ITEMNAME,

		// flag
		LIST_VISIBLE_LINE = 10,
	};
	GLGaeaClient*	m_pGaeaClient;

	// Setitem List
	CUIListBox*					m_pSetitemListBox;
	CBasicScrollBarEx*			m_pListScrollBar;

	SSETITEM*					m_pSetitem;
	CSetitemPreviewlistSlot*	m_pListSlot[LIST_VISIBLE_LINE];

	VNATIVEID					m_vecItemList;
	STARGETID					m_sTargetID;
	SNATIVEID					m_sPutonItem[SLOT_NSIZE_S_2];
public:
	CSetitemPreviewWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	~CSetitemPreviewWindow(void);

public:
	virtual void CreateUIWindowAndRegisterOwnership(void) override;
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

	virtual void SetitemGroupComposition(SSETITEM* pSetitem, STARGETID sTargetID = STARGETID()) override;
	virtual void ResetSetitemComposition(void) override;
private:
	void CreateSubControl(void);
	void UpdateList(void);

	BOOL IsWoreItem(EMSUIT emSuit, const SNATIVEID& sNID, const SNATIVEID* sPutOnItemCollect) const;
	BOOL ComposePutonItem(const STARGETID& sTargetID, DWORD& dwCharclass, SNATIVEID* sPutonItem);
};