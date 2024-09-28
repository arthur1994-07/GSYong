// MobNpcEditDlg.h : ��� ����
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CsheetWithTab;
class EngineDeviceMan;

// CMobNpcEditDlg ��ȭ ����
class CMobNpcEditDlg : public CDialog
{
// ����
public:
	CMobNpcEditDlg(CWnd* pParent = NULL);	// ǥ�� ������
    virtual ~CMobNpcEditDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_MOBNPCEDIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

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

// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
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
