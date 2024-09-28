#if !defined(AFX_DLGEDIT_H__FAAB1C28_6108_4EFD_AF1C_CA048F8A5650__INCLUDED_)
#define AFX_DLGEDIT_H__FAAB1C28_6108_4EFD_AF1C_CA048F8A5650__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEdit.h : header file
//

#include "../Resource.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgEdit dialog
class CsheetWithTabChar;
class DxJiggleBone;
class DxJiggleBoneColl;
class DxClothColl;


class CDlgEdit : public CDialog
{
protected:
	CsheetWithTabChar	*m_psheetWithTab;

public:
	CsheetWithTabChar* GetSheetChar ()		{ return m_psheetWithTab; }
	void SetAcitveSheetPage ( int nPage );

	void SetActivePage_JiggleBone( int nPage, DxJiggleBone* pJiggleBone );
	void SetActivePage_JiggleBoneColl( int nPage, DxJiggleBoneColl* pJiggleBoneColl );
	void SetActivePage_ClothColl( int nPage, DxClothColl* pClothColl );

	// User 가 TimeLine 의 시간을 변경한다면 그와 연관된 작업을 한다.
	void ChangeTimeOfTimeLine( DWORD dwToTime );

	// User 가 CDlgTimeLine 의 OnButtonPlay 을 누른다면 그와 연관된 작업을 한다.
	void OnButtonPlayOfCDlgTimeLine();

	void SetSpinControl( int nRadius, int nHeight, int nSpin3, int nSpin4, int nColorHeight );
    void ResetDialog();
    void UpdateKeyBoardMouse();

	int GetActiveIndex();

public:
	void ReFlash ();
	void AnimationListing ();

	void SetActiveSheetCombo ( bool bActive );

// Construction
public:
	CDlgEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEdit)
	enum { IDD = IDD_DIALOG_EDIT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEdit)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboPage();
	CComboBox m_comboPage;
	afx_msg void OnDropFiles(HDROP hDropInfo);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDIT_H__FAAB1C28_6108_4EFD_AF1C_CA048F8A5650__INCLUDED_)
