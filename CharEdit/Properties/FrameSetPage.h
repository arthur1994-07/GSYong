#if !defined(AFX_FRAMESETPAGE_H__A8094E70_C2AA_40E2_B18C_D0094DDAC44B__INCLUDED_)
#define AFX_FRAMESETPAGE_H__A8094E70_C2AA_40E2_B18C_D0094DDAC44B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameSetPage.h : header file
//

class CsheetWithTabFrame;
/////////////////////////////////////////////////////////////////////////////
// CFrameSetPage dialog

class CFrameSetPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFrameSetPage)

protected:
	CsheetWithTabFrame	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabFrame *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	CString	m_sFrameName;
	DWORD	m_rpType;

public:
	void PieceListing();

public:
	void SetFrameName ( CString sName );

// Construction
public:
	CFrameSetPage();
	~CFrameSetPage();

// Dialog Data
	//{{AFX_DATA(CFrameSetPage)
	enum { IDD = IDD_DIALOG_FRAMESET };
	CListBox	m_ListPiece;
	CComboBox	m_ComboType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFrameSetPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFrameSetPage)
	afx_msg void OnButtonOk();
	afx_msg void OnSelchangeComboType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMESETPAGE_H__A8094E70_C2AA_40E2_B18C_D0094DDAC44B__INCLUDED_)
