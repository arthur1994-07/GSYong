// MobNpcEditDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CsheetWithTab;
class EngineDeviceMan;

// CMobNpcEditDlg 대화 상자
class CMobNpcEditDlg : public CDialog
{
// 생성
public:
	CMobNpcEditDlg(CWnd* pParent = NULL);	// 표준 생성자
    virtual ~CMobNpcEditDlg();

// 대화 상자 데이터
	enum { IDD = IDD_MOBNPCEDIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

	char	m_szAppPath[MAX_PATH];
	CString m_strAppPath;

public:
	CsheetWithTab	*m_pSheetTab;

private:
	CToolBar    m_wndToolBar;
    EngineDeviceMan* m_pEngineDevice;

private:
	void	MakeToolBar ();

private:
	bool	DoSave ();

public:	
	void	EnableMenu ( UINT uIDEnableItem, BOOL bEnable );
	void	EnableToobar ( UINT uIDEnableItem, BOOL bEnable );

	void	SetReadMode();

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	virtual void PostNcDestroy();
public:		
	afx_msg void OnMenuitemLoad();
	afx_msg void OnMenuitemSave();
	afx_msg void OnMenuitemExit();
	afx_msg void OnMenuitemNew();
	afx_msg void OnClose();
};
