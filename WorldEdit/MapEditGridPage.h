#pragma once

#include "afxwin.h"

#include "../MfcExLib/ExLibs/Separator.h"

#include "../EngineLib/DxEffect/MapEdit/DxMapEditMan.h"

// CMapEditGridPage 대화 상자입니다.
class CsheetWithTabMapEdit;

class CMapEditGridPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMapEditGridPage)

protected:
	CsheetWithTabMapEdit	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabMapEdit *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void UpdateContrl ();
	void InitDialog ();

protected:
	BOOL			m_bDlgInit;

protected:
	MAPEDIT_PROPERTY m_sProperty;

protected:
	void	ReSetCheckBox ();
public:
	CMapEditGridPage();
	virtual ~CMapEditGridPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MAPEDIT_GRID };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedCheckLength();
	afx_msg void OnBnClickedButtonCreate();
	CComboBox m_comboGRIDNUM;
};
