#pragma once

#include "afxwin.h"

#include "../MfcExLib/ExLibs/Separator.h"

#include "ClrButton.h"

#include "afxcmn.h"

// CMapEditTilePage 대화 상자입니다.
class CsheetWithTabMapEdit;

class CMapEditTilePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMapEditTilePage)

protected:
	CsheetWithTabMapEdit	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabMapEdit *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void UpdateContrl ();

protected:
	BOOL			m_bDlgInit;

protected:
	CClrButton	cClrCOLOR[14];

protected:
	void		ReSetListCtrl ();
	void		ReSetAllEditBox ();
	void		ReSetColorButton ();

protected:
	void	ReSetCheckBox ();
public:
	CMapEditTilePage();
	virtual ~CMapEditTilePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MAPEDIT_TILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonApply();
//	virtual BOOL OnSetActive();
//	virtual BOOL OnKillActive();
	CListCtrl m_listData;
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonFileSave();
	afx_msg void OnBnClickedButtonFileLoad();
	afx_msg void OnBnClickedButtonColor1();
	afx_msg void OnBnClickedButtonColor2();
	afx_msg void OnBnClickedButtonColor3();
	afx_msg void OnBnClickedButtonColor4();
	afx_msg void OnBnClickedButtonColor5();
	afx_msg void OnBnClickedButtonColor6();
	afx_msg void OnBnClickedButtonColor7();
	afx_msg void OnBnClickedButtonColor8();
	afx_msg void OnBnClickedButtonColor9();
	afx_msg void OnBnClickedButtonColor10();
	afx_msg void OnBnClickedButtonColor11();
	afx_msg void OnBnClickedButtonColor12();
	afx_msg void OnBnClickedButtonColor13();
	afx_msg void OnBnClickedButtonColor14();
	afx_msg void OnNMClickListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditDetailScale();
	afx_msg void OnBnClickedButtonDetailTex();
	afx_msg void OnBnClickedButtonColorChange();
	afx_msg void OnBnClickedCheckTile();
	afx_msg void OnBnClickedCheckColor();
};
