#pragma once


// CToolBarCombo
static const UINT wm_ChangeSelCombo = RegisterWindowMessage("WM_CHANGE_SELCOMBO");

class CToolBarCombo : public CComboBox
{
	DECLARE_DYNAMIC(CToolBarCombo)

public:
	CToolBarCombo();
	virtual ~CToolBarCombo();

	CWnd* m_pWndMsgParent;

	void SetWndMsgParent(CWnd *pWnd);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchange();
};


