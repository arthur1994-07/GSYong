#if !defined(AFX_DLGPIECEEDIT_H_)
#define AFX_DLGPIECEEDIT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEdit.h : header file
//

struct DxFrame;
class DxPiece;
class CsheetWithTab;

#include "../MfcExLib/ExLibs/CJFlatComboBox.h"
#include "../MfcExLib/ExLibs/flatedit.h"

#include "afxcmn.h"
#include "DlgPiecePreView.h"
#include "SettingIni.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgPieceEdit dialog

class CDlgPieceEdit : public CDialog, public CSettingIni
{
public:
	void UpdateLIST();


// Construction
public:
	CDlgPieceEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPieceEdit)
	enum { IDD = IDD_DIALOG_PIECE_DLG };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPieceEdit)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPieceEdit)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	virtual void PostNcDestroy();
public: 
	afx_msg void OnBnClickedButtonInsert();
public:
	afx_msg void OnBnClickedButtonDelete();
public:
	CListCtrl m_ctrlPIECE;
	CButton	  m_btnRandomRotate;
	int			m_nRandom;
    BOOL        m_bActive;
	BOOL		m_bSlope;
	float		m_fSlopeRate;

public:
	afx_msg void OnNMClickListPiece(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreInitDialog();
public:
	afx_msg void OnBnClickedButtonInsertSpt();


public:
	CDlgPiecePreView	m_cDlgPiecePreView;
public:
	afx_msg void OnBnClickedButtonPreviewLeft();
public:
	afx_msg void OnBnClickedButtonPreviewRight();
public:
	afx_msg void OnBnClickedButtonSetangle();
public:
	afx_msg void OnBnClickedCheckRandomrotate();
public:
	afx_msg void OnBnClickedButtonPreviewLeft2();
public:
	afx_msg void OnBnClickedButtonPreviewRight2();
public:
	afx_msg void OnBnClickedButtonDeleteAll();
    afx_msg void OnEnChangeEditRScaleMin();
    afx_msg void OnEnChangeEditRScaleMax();
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
    afx_msg void OnEnSetfocusEditAngle();
    afx_msg void OnEnSetfocusEditScale();
    afx_msg void OnEnSetfocusEditRScaleMin();
    afx_msg void OnEnSetfocusEditRScaleMax();
    afx_msg void OnParentNotify(UINT message, LPARAM lParam);
    afx_msg void OnEnUpdateEditRScaleMin();
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);    
	afx_msg void OnBnClickedCheckSlope();
	afx_msg void OnEnChangeEditSlopeRate();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPIECEEDIT_H_)
