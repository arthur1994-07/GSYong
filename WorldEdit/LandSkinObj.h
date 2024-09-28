#pragma once
#include "afxwin.h"


// CLandSkinObj 대화 상자입니다.
class CsheetWithTab;
class DXLANDSKINOBJ;

class CLandSkinObj : public CPropertyPage
{
	DECLARE_DYNAMIC(CLandSkinObj)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	BOOL			m_bDlgInit;
	DXLANDSKINOBJ*	m_pLandSkinObj;

public:
	void ListtingSkinObj ();
	void UpdateContrl ();
	void ViewContrls ( BOOL bView, BOOL bClear=FALSE );

public:
	CLandSkinObj();
	virtual ~CLandSkinObj();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LANDSKINOBJ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listSkinObj;
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonSkinobjapply();
	afx_msg void OnBnClickedButtonAdd();
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedCheckRenderName();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonCurPos1();
	afx_msg void OnBnClickedButtonCurPos2();
	afx_msg void OnBnClickedButtonResetFade();
	afx_msg void OnEnChangeEditVisibleRadius();
	afx_msg void OnEnChangeEditInvisibleRadius();
};
