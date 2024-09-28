#if !defined(AFX_LISTBOXANI_H__0B06DB00_E2A2_48EA_93EC_CEACBCA08A15__INCLUDED_)
#define AFX_LISTBOXANI_H__0B06DB00_E2A2_48EA_93EC_CEACBCA08A15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListBoxAni.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListBoxAni window

class CListBoxAni : public CListBox
{
private:
	DWORD m_dwIDRPopup;
	DWORD m_dwNotifyMsg;

// Construction
public:
	CListBoxAni();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBoxAni)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListBoxAni();

	void SetPopupID(DWORD dwIDRPopup, DWORD dwNotifyMsg) { m_dwIDRPopup = dwIDRPopup; m_dwNotifyMsg = dwNotifyMsg; }

	// Generated message map functions
protected:
	//{{AFX_MSG(CListBoxAni)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTBOXANI_H__0B06DB00_E2A2_48EA_93EC_CEACBCA08A15__INCLUDED_)
