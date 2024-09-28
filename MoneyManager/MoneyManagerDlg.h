// MoneyManagerDlg.h : ��� ����
//

#pragma once

#include <set>
#include "afxcmn.h"
#include "../RanLogic/Util/s_CCfg.h"

// CMoneyManagerDlg ��ȭ ����
class CMoneyManagerDlg : public CDialog
{
// ����
public:
	CMoneyManagerDlg(CWnd* pParent = NULL);	// ǥ�� ������

// ��ȭ ���� ������
	enum { IDD = IDD_MONEYMANAGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

	BOOL LoadCfg();
	BOOL OpenDatabase();

// ����
protected:
	HICON m_hIcon;
    std::set<int> m_setSTOUserNum;
    CCfg* m_pCfg;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	CListCtrl m_list_FieldInfo;
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedCheckMultiply();
	CProgressCtrl m_prog_Complete;
	afx_msg void OnBnClickedButtonMoneycheck();	
	afx_msg void OnBnClickedButtonReencrypt();
	afx_msg void OnBnClickedButtonNewencrypt();
};
