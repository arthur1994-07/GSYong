// HelpEditDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"

struct	SHELPNODE;

// CHelpEditDlg 대화 상자
class CHelpEditDlg : public CDialog
{
// 생성
public:
	CHelpEditDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_HELPEDIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

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

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
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
