#if !defined(AFX_ANIMPAGE_H__3E42965E_2E10_4D4D_8E07_4BEAA758F6E1__INCLUDED_)
#define AFX_ANIMPAGE_H__3E42965E_2E10_4D4D_8E07_4BEAA758F6E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimPage.h : header file
//


class CsheetWithTabAnim;
/////////////////////////////////////////////////////////////////////////////
// CAnimPage dialog
#include "../Util/ListBoxAni.h"

#define WM_ANIMATECONT	(WM_USER+100)


class CAnimPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAnimPage)

public:
	void AnimationListing ();

protected:
	CsheetWithTabAnim	*m_pSheetTab;

	CComboBox m_ComboMType;
	CComboBox m_ComboSType;

public:
	void SetSheetTab ( CsheetWithTabAnim *pSheetTab ) { m_pSheetTab = pSheetTab; }
	BOOL LoadAnimation ( const char *pszFileName );



// Construction
public:
	CAnimPage();
	~CAnimPage();

// Dialog Data
	//{{AFX_DATA(CAnimPage)
	enum { IDD = IDD_DIALOG_ANIM };
	CListBoxAni	m_ListAnimation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAnimPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	afx_msg LRESULT OnAnimationCont(WPARAM wParam, LPARAM lParam);
	//{{AFX_MSG(CAnimPage)
	afx_msg void OnButtonAnimNew();
	afx_msg void OnButtonAnimDelete();
	afx_msg void OnButtonAnimSelect();
	afx_msg void OnButtonAnimSetting();
	afx_msg void OnButtonAddMultiAnim();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonAnimMix();
	afx_msg void OnBnClickedButtonAnimConversion();
	afx_msg void OnBnClickedMoveTestcheck();
	afx_msg void OnBnClickedApplyButton();
	afx_msg void OnLbnDblclkListAnimation();
	afx_msg void OnBnClickedButtonResetPos();
	afx_msg void OnBnClickedButtonRemakeAllBin();
	afx_msg void OnBnClickedButtonRemakeSelBin();
	afx_msg void OnBnClickedButtonCreateAniSequence();
	afx_msg void OnBnClickedButtonAnimListExport();
	afx_msg void OnBnClickedTransApply();
	afx_msg void OnCbnSelchangeComboMType();
	afx_msg void OnCbnSelchangeComboSType();
	afx_msg void OnDeactiveEdit();

public:
	static void SubAnimComboInit(CComboBox& comboBox, const int nMType);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMPAGE_H__3E42965E_2E10_4D4D_8E07_4BEAA758F6E1__INCLUDED_)
