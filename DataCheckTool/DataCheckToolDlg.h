// DataCheckToolDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"

class SUBPATH;

// CDataCheckToolDlg 대화 상자
class CDataCheckToolDlg : public CDialog
{
// 생성
public:
	CDataCheckToolDlg(CWnd* pParent = NULL);	// 표준 생성자
    virtual ~CDataCheckToolDlg();

// 대화 상자 데이터
	enum { IDD = IDD_DATACHECKTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

// 구현
protected:
	HICON m_hIcon;
    SUBPATH* m_pPath;

	// 메시지 맵 함수를 생성했습니다.
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

