// DataCheckToolDlg.h : ��� ����
//

#pragma once
#include "afxcmn.h"

class SUBPATH;

// CDataCheckToolDlg ��ȭ ����
class CDataCheckToolDlg : public CDialog
{
// ����
public:
	CDataCheckToolDlg(CWnd* pParent = NULL);	// ǥ�� ������
    virtual ~CDataCheckToolDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_DATACHECKTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

// ����
protected:
	HICON m_hIcon;
    SUBPATH* m_pPath;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	void DrawItem(LPDRAWITEMSTRUCT lpDS, char parm_push_flag);
	void DrawCursor(LPDRAWITEMSTRUCT lpDS, char parm_push_flag);
	HRESULT SearchWldData();
	HRESULT SearchItemData();
	HRESULT SearchMobNpcData();

	afx_msg void OnNMCustomdrawListSearchResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	CListCtrl m_list_SearchResult;
	afx_msg void OnBnClickedButtonSearch();

	CString	GetAppPath();

};

