
#pragma once

#include "MainPropertySheet.h"
#include "afxdtctl.h"
#include "afxwin.h"
#include "atltime.h"


class CRanStatisticsDlg : public CDialog
{
public:
	CRanStatisticsDlg(CWnd* pParent = NULL);	// 표준 생성자
	~CRanStatisticsDlg();

	CTime&	GetFrom()	{ UpdateData(TRUE); return m_ctFrom; }
	CTime&	GetTo()		{ UpdateData(TRUE); return m_ctTo; }
	BOOL	IsExported() { return m_bExport; }

	enum { IDD = IDD_RANSTATISTICS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원
	virtual BOOL OnInitDialog();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	HICON m_hIcon;

	DECLARE_MESSAGE_MAP()

public:	
	CMainPropertySheet *m_pPS; 

	CTime m_ctFrom;
	CTime m_ctTo;

	BOOL m_bExport;
};
