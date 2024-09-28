#pragma once

#include "afxwin.h"

#include "../MfcExLib/ExLibs/Separator.h"

#include "../EngineLib/DxEffect/MapEdit/DxMapEditMan.h"

// CMapEditGridPage ��ȭ �����Դϴ�.
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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_MAPEDIT_GRID };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedCheckLength();
	afx_msg void OnBnClickedButtonCreate();
	CComboBox m_comboGRIDNUM;
};
