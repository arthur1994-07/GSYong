#pragma once

#include "../MfcExLib/ExLibs/reportctrl.h"
#include "../EngineLib/DxEffect/EffKeep/DxEffKeepCTColor.h"

class CsheetWithTabChar;

// CEffKeepCTColorPage ��ȭ �����Դϴ�.

class CEffKeepCTColorPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffKeepCTColorPage)

protected:
	CsheetWithTabChar	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTab = pSheetTab; }

private:
	DxEffKeepData*				m_pEff;
	EFFKEEP_PROPERTY_CTCOLOR*	m_rProperty;
	BOOL						m_bCreateEff;

private:
	void UpdateColorList();

public:
	void SetInstance( DxEffKeepData* pEff );

public:
	CEffKeepCTColorPage();
	virtual ~CEffKeepCTColorPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFFKEEP_CTCOLOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CReportCtrl	m_wndList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonColorInsert();
	afx_msg void OnBnClickedButtonColorState();
	afx_msg void OnBnClickedButtonColorDelete();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMDblclkListColor(NMHDR *pNMHDR, LRESULT *pResult);
};
