#pragma once

#include "../MfcExLib/ExLibs/reportctrl.h"
#include "../EngineLib/DxEffect/EffKeep/DxEffKeepCTColor.h"

class CsheetWithTabChar;

// CEffKeepCTColorPage 대화 상자입니다.

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

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFFKEEP_CTCOLOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
