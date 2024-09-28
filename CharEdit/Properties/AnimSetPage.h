#include "afxwin.h"
#include "afxcmn.h"
#if !defined(AFX_ANIMSETPAGE_H__80CDDE64_F6C4_4AF1_96B5_C17613E29436__INCLUDED_)
#define AFX_ANIMSETPAGE_H__80CDDE64_F6C4_4AF1_96B5_C17613E29436__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimSetPage.h : header file
//

class CsheetWithTabAnim;
struct SAttachBoneAnimInfo;
/////////////////////////////////////////////////////////////////////////////
// CAnimSetPage dialog

class CAnimSetPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAnimSetPage)

protected:
	CsheetWithTabAnim	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabAnim *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	CString m_sAnimName;

public:
	void SetAnimName ( CString sName );
	void UpdateAttachAnim( const SAttachBoneAnimInfo *pAttachBoneAnimInfo );

// Construction
public:
	CAnimSetPage();
	~CAnimSetPage();

// Dialog Data
	//{{AFX_DATA(CAnimSetPage)
	enum { IDD = IDD_DIALOG_ANIMSET };
	CComboBox	m_ComboType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAnimSetPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAnimSetPage)
	afx_msg void OnButtonOk();
	afx_msg void OnBnClickedDontSaveOK();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedCheckLoop();
	afx_msg void OnBnClickedCheckMove();
	afx_msg void OnBnClickedButtonSound();
	afx_msg void OnBnClickedButtonAnidiv();
	CComboBox m_ComboSubType;
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnBnClickedButtonStrike();
	afx_msg void OnBnClickedButtonAnieff();
	afx_msg void OnBnClickedCheckUpbody();
	afx_msg void OnBnClickedButtonAniScale();
	CListCtrl m_List_AttAni;
	afx_msg void OnNMDblclkListAttanimation(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonImportCfg();
	afx_msg void OnBnClickedButtonXfileChange();
	afx_msg void OnBnClickedButtonAniJiggleBlend();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMSETPAGE_H__80CDDE64_F6C4_4AF1_96B5_C17613E29436__INCLUDED_)
