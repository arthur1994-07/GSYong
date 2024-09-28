// HelpEditDlg.h : ��� ����
//

#pragma once
#include "afxcmn.h"

struct	SHELPNODE;

// CHelpEditDlg ��ȭ ����
class CHelpEditDlg : public CDialog
{
// ����
public:
	CHelpEditDlg(CWnd* pParent = NULL);	// ǥ�� ������

// ��ȭ ���� ������
	enum { IDD = IDD_HELPEDIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

private:
	int		GetSelectMenu ( POINT pt );
	void	ForceSelectOnRightClick ( POINT pt );
	void	RunSelectMenu ( int nSelect );

private:
	BOOL	SelectItem ( HTREEITEM hItem, SHELPNODE* pNode );
	void	UpdateTree ();
	void	UpdateNode ( HTREEITEM hParent, SHELPNODE* pNode );

private:
	BOOL		m_bEdited;

// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrlHelpTree;

	afx_msg void OnNMRclickTreeHelp(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTvnEndlabeleditTreeHelp(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnTvnSelchangingTreeHelp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditContent();
	afx_msg void OnTvnSelchangedTreeHelp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonEdit();	
	afx_msg void OnMenuitemLoad();
	afx_msg void OnMenuitemSave();
	afx_msg void OnCsvSave();
	afx_msg void OnCsvLoad();
};
