// RanSNSDlg.h : ��� ����
//

#pragma once

#include "PropSheet.h"


// CRanSNSDlg ��ȭ ����
class CRanSNSDlg : public CDialog
{
// ����
public:
	CRanSNSDlg(CWnd* pParent = NULL);	// ǥ�� ������
	~CRanSNSDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_RANSNS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����


// ����
protected:
	HICON m_hIcon;

	CPropSht *m_pPS;	

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
