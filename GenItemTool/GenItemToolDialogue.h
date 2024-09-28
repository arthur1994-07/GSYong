// SpecialToolDlg.h : ��� ����
//

#pragma once

#include "afxwin.h"
#include "./LinkLostDatainfo.h"


class CsheetWithTab;
class EngineDeviceMan;

// CSpecialToolDlg ��ȭ ����
class CGenItemToolDilogue : public CDialog
{
// ����
public:
	CGenItemToolDilogue(CWnd* pParent = NULL);	// ǥ�� ������
    virtual ~CGenItemToolDilogue();

// ��ȭ ���� ������
	enum { IDD = IDD_GENITEMTOOL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

protected:
    EngineDeviceMan* m_pEngineDevice;

	LPDIRECT3D9			m_pD3D;
	LPDIRECT3DDEVICE9	m_pD3DDevice;

	LinkLostDatainfo m_LinkLostData;

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
	LPDIRECT3DDEVICEQ GetD3DDevice();

// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnClose();
	afx_msg void OnFileExit();
	afx_msg void OnBnClickedButtonCsvload();
	afx_msg void OnBnClickedButtonCsvsave();
	afx_msg void OnBnClickedButtonLinklostdata();
	afx_msg void OnBnClickedButtonSelecReload();
	afx_msg void OnBnClickedButtonFileedit();
};
