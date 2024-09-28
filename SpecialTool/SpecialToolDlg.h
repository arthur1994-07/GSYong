// SpecialToolDlg.h : 헤더 파일
//

#pragma once

#include "afxwin.h"


class CsheetWithTab;
class EngineDeviceMan;

// CSpecialToolDlg 대화 상자
class CSpecialToolDlg : public CDialog
{
// 생성
public:
	CSpecialToolDlg(CWnd* pParent = NULL);	// 표준 생성자
    virtual ~CSpecialToolDlg();

// 대화 상자 데이터
	enum { IDD = IDD_SPECIALTOOL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

protected:
    EngineDeviceMan* m_pEngineDevice;

	LPDIRECT3D9			m_pD3D;
	LPDIRECT3DDEVICE9	m_pD3DDevice;

public:
	char			m_szAppPath[MAX_PATH];
	CString			m_strAppPath;

	CsheetWithTab	*m_pSheetTab;

	int				m_nPage;

	CToolBar    m_wndToolBar;

public:
	void	UpdateItems();
	void	InitCombo();
	bool	DoSave();
	void	MakeToolBar();
	LPDIRECT3DDEVICEQ GetD3DDevice();


// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonOldload();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnClose();
	afx_msg void OnCbnSelchangeComboToolpage();
	CComboBox m_cComboPage;
	afx_msg void OnFileNewfile();
	afx_msg void OnFileLoadfile();
	afx_msg void OnFileLoadfileOld();
	afx_msg void OnFileSavefile();
	afx_msg void OnFileExit();
	afx_msg void OnBnClickedButtonCsvload();
	afx_msg void OnBnClickedButtonCsvsave();
};
