#pragma once

#include <string>

#include "../../enginelib/g-logic/TypeDefine.h"

// CGrindingSimulatorDlg 대화 상자입니다.
struct SITEM;

class CGrindingSimulatorDlg : public CDialog
{
	DECLARE_DYNAMIC(CGrindingSimulatorDlg)

public:
	CGrindingSimulatorDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CGrindingSimulatorDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_GRINDING_SIMULATOR };

public:
	void SetGrinder( SITEM* pGrinder ) { m_pGrinder = pGrinder; }

private:
	void DoSimulate( const SNATIVEID dstID, const UINT nGrinderNum, const UINT nGrindingCount, const UINT nGrindingOption );
	void DoLog( const std::string& strLog );
	void ClearLog();

private:
	SITEM*		m_pGrinder;
	CListBox	m_LogList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonStart();
};
