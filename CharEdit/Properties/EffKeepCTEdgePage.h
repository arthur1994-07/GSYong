#pragma once

#include "../EngineLib/DxEffect/EffKeep/DxEffKeepCTEdge.h"

class CsheetWithTabChar;

// CEffKeepCTEdgePage ��ȭ �����Դϴ�.

#include "../MfcExLib/ExLibs/reportctrl.h"

class CEffKeepCTEdgePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffKeepCTEdgePage)

protected:
	CsheetWithTabChar	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTab = pSheetTab; }

private:
	DxEffKeepData*				m_pEff;
	EFFKEEP_PROPERTY_CTEDGE*	m_rProperty;
	BOOL						m_bCreateEff;

private:
	void UpdateColorList();

public:
	void SetInstance( DxEffKeepData* pEff );

public:
	CEffKeepCTEdgePage();
	virtual ~CEffKeepCTEdgePage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFFKEEP_CTEDGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CReportCtrl	m_wndList;
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonColorInsert();
	afx_msg void OnBnClickedButtonColorDelete();
	afx_msg void OnBnClickedButtonColorState();
	afx_msg void OnNMDblclkListColor(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckShadecolor();
};
