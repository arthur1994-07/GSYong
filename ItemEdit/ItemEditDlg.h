// ItemEditDlg.h : 헤더 파일
//

#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include <boost/function.hpp>

#include "./Dialog/SearchDialog.h"

class CsheetWithTab;
class EngineDeviceMan;

// CItemEditDlg 대화 상자
class CItemEditDlg : public CDialog
{
// 생성
public:
	CItemEditDlg(CWnd* pParent = NULL);	// 표준 생성자
    virtual ~CItemEditDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ITEMEDIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

public:
	CSearchDialog	m_wndSearchDlg;
	CsheetWithTab	*m_pSheetTab;

	char			m_szAppPath[MAX_PATH];
	CString			m_strAppPath;

private:
	CToolBar    m_wndToolBar;
    EngineDeviceMan* m_pEngineDevice;

private:
	void	MakeToolBar ();

public:	
	void	EnableMenu ( UINT uIDEnableItem, BOOL bEnable );
	void	EnableToobar ( UINT uIDEnableItem, BOOL bEnable );
	void	SetReadMode();

protected:
	bool DoSave ();
	bool SaveSearchList(); // by luxes.

public:
	void ExportExcel ( boost::function<void (int)> PercentCallbackFunc=NULL );

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnMenuitemLoad();
	afx_msg void OnMenuitemSave();
	afx_msg void OnMenuitemExit();
	afx_msg void OnMenuitemNew();	
	afx_msg void OnBnClickedButtonUpdateSync();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonStrtbSync();
	afx_msg void OnBnClickedButtonReloadStrtbl();
	afx_msg void OnBnClickedBtnCsvSave();
	afx_msg void OnBnClickedBtnCsvLoad();
	afx_msg void OnBnClickedBtnExcelStrTblImport();
	afx_msg void OnBnClickedBtnExcelStrTblExport();
	afx_msg void OnBnClickedItemSearchButton();
	afx_msg void OnBnClickedSearchReportButton();
	afx_msg void OnBnClickedButtonStrtbSearch();
    afx_msg void OnBnClickedBtnSqlSave();
	afx_msg void OnBnClickedBtnXmlConvert();
	afx_msg void OnBnClickedBtnExcelExport();
};
