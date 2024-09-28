#pragma once

#include "afxwin.h"

#include "../MfcExLib/ExLibs/Separator.h"

#include "ClrButton.h"

// CMapEditHeightPage 대화 상자입니다.
class CsheetWithTabMapEdit;

class CMapEditHeightPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMapEditHeightPage)

protected:
	CsheetWithTabMapEdit	*m_pSheetTab;

protected:
	static const COLORREF CLOUDBLUE;
	static const COLORREF WHITE;
	static const COLORREF BLACK;
	static const COLORREF DKGRAY;
	static const COLORREF LLTGRAY;
	static const COLORREF LTGRAY;
	static const COLORREF YELLOW;
	static const COLORREF DKYELLOW;
	static const COLORREF COLORREF_RED;
	static const COLORREF DKRED;
	static const COLORREF COLORREF_BLUE;
	static const COLORREF LBLUE;
	static const COLORREF DKBLUE;
	static const COLORREF CYAN;
	static const COLORREF DKCYAN;
	static const COLORREF COLORREF_GREEN;
	static const COLORREF DKGREEN;
	static const COLORREF MAGENTA;
	static const COLORREF DKMAGENTA;

protected:
	CClrButton	cClrGround;
	CClrButton	cClrUneven;
	CClrButton	cClrUp;
	CClrButton	cClrDown;
	CClrButton	cClrRelativeHeight;
	CClrButton	cClrAbsoluteHeight;

protected:
	HICON m_hIcon;

public:
	void SetSheetTab ( CsheetWithTabMapEdit *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void UpdateContrl ();

protected:
	BOOL			m_bDlgInit;

protected:
	MAPEDIT_PROPERTY m_sProperty;

protected:
	void	ReSetCheckBox ();
public:
	CMapEditHeightPage();
	virtual ~CMapEditHeightPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MAPEDIT_HEIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonGround();
	afx_msg void OnBnClickedButtonUneven();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnBnClickedButtonRelativeHeight();
	afx_msg void OnBnClickedButtonAbsoluteHeight();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEnChangeEditHeightValue();
};
