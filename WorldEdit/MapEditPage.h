#pragma once

#include "afxwin.h"

#include "../MfcExLib/ExLibs/Separator.h"

#include "ClrButton.h"

// CMapEditPage 대화 상자입니다.
class CsheetWithTab;
class CsheetWithTabMapEdit;

class CMapEditPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMapEditPage)

protected:
	CClrButton	cClrQurd;
	CClrButton	cClrCircle;
	CClrButton	cClrEdit;
	CClrButton	cClrGame;

protected:
	CsheetWithTabMapEdit	*m_psheetWithTab;

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }


public:
	void UpdateContrl ();

protected:
	BOOL			m_bDlgInit;

protected:
	void	ReSetCheckBox ();
public:
	CMapEditPage();
	virtual ~CMapEditPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MAPEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
//	virtual BOOL OnSetActive();
//	virtual BOOL OnKillActive();
	CComboBox m_comboEditType;
	afx_msg void OnCbnSelchangeComboEdittype();
	afx_msg void OnEnChangeEditBlushScale();
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnBnClickedButtonQurd();
	afx_msg void OnBnClickedButtonCircle();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonGame();
};
